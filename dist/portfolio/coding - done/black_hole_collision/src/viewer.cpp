/**
 * @file viewer.cpp
 * @brief 3D visualization of binary black hole collision with advanced effects.
 *
 * Features:
 *   - Ray-marched metaball rendering for merging black holes
 *   - Gravitational Wave Ripple Grid (Vertex displacement shader)
 *   - Mouse drag to orbit camera, scroll to zoom
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bh_collision/simulation.h"
#include "bh_collision/integration_api.h"

#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// Globals
// ============================================================================

static int g_width = 1280, g_height = 720;

// Camera
static float g_cam_dist = 40.0f;
static float g_cam_yaw = 45.0f;     // degrees
static float g_cam_pitch = 30.0f;   // degrees
static glm::vec3 g_cam_target = {0, 0, 0};
static float g_cam_speed = 0.5f;

// Mouse
static bool g_mouse_dragging = false;
static double g_mouse_last_x = 0, g_mouse_last_y = 0;
static bool g_right_dragging = false;

// Playback
static bool g_paused = false;
static float g_playback_speed = 1.0f;
static float g_playback_time = 0.0f;

// ============================================================================
// Shaders
// ============================================================================

// --- Ray Marching (Black Holes) ---
static const char* raymarch_vert_src = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
out vec2 vUV;
void main() {
    vUV = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

static const char* raymarch_frag_src = R"(
#version 330 core
out vec4 fragColor;
in vec2 vUV;
uniform vec2 uResolution;
uniform vec3 uCamPos;
uniform vec3 uCamDir;
uniform vec3 uCamUp;
uniform vec3 uCamRight;
uniform float uFov;
#define MAX_BHS 2
uniform int uNumBH;
uniform vec3 uBHPos[MAX_BHS];
uniform float uBHMass[MAX_BHS];
uniform float uBHRadius[MAX_BHS]; 
uniform float uGlowIntensity;

float smin(float a, float b, float k) {
    float h = max(k - abs(a - b), 0.0) / k;
    return min(a, b) - h * h * k * 0.25;
}

float map(vec3 p) {
    float d = 1e9;
    for (int i = 0; i < uNumBH; i++) {
        float distSphere = length(p - uBHPos[i]) - uBHRadius[i];
        if (i == 0) d = distSphere;
        else {
             float k = 1.0 * (uBHRadius[0] + uBHRadius[i]); 
             d = smin(d, distSphere, k);
        }
    }
    return d;
}

vec3 calcNormal(vec3 p) {
    const float eps = 0.001;
    const vec2 h = vec2(eps, 0);
    return normalize(vec3(map(p+h.xyy) - map(p-h.xyy),
                          map(p+h.yxy) - map(p-h.yxy),
                          map(p+h.yyx) - map(p-h.yyx)));
}

void main() {
    float aspectRatio = uResolution.x / uResolution.y;
    vec2 uv = (vUV - 0.5) * vec2(aspectRatio, 1.0);
    vec3 rayDir = normalize(uCamDir + uv.x * uCamRight * uFov + uv.y * uCamUp * uFov);
    
    float t = 0.0;
    float tMax = 1000.0;
    int maxSteps = 128; // Standard steps
    
    // Bounding sphere optimization
    vec3 center = vec3(0.0);
    float maxR = 0.0;
    for(int i=0; i<uNumBH; i++) {
        center += uBHPos[i];
        maxR = max(maxR, length(uBHPos[i]) + uBHRadius[i] * 4.0);
    }
    center /= float(max(uNumBH, 1));
    float distToCenter = length(uCamPos - center);
    float sphereDist = distToCenter - maxR;
    if (sphereDist > 0.0) t = sphereDist;

    bool hit = false;
    vec3 p = uCamPos + t * rayDir;
    float glow = 0.0;

    for (int i = 0; i < maxSteps; i++) {
        p = uCamPos + t * rayDir;
        float d = map(p);
        float glowTerm = 1.0 / (d*d + 0.1);
        glow += glowTerm * 0.02 * uGlowIntensity;
        if (d < 0.001) { // Standard threshold
            hit = true;
            break;
        }
        if (t > tMax) break;
        t += d;
    }

    vec3 col = vec3(0.02, 0.02, 0.02); // Deep Void (#050505)
    col += vec3(1.0, 0.6, 0.2) * glow;

    if (hit) {
        vec3 n = calcNormal(p);
        float rim = 1.0 - max(dot(n, -rayDir), 0.0);
        rim = pow(rim, 4.0);
        col = mix(vec3(0.0), vec3(0.5, 0.2, 0.1), rim);
    }
    col = pow(col, vec3(1.0/2.2));
    fragColor = vec4(col, 1.0);
}
)";

// --- Sphere Geometry (COM Marker) ---
static const char* sphere_vert_src = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat3 uNormalMat;
out vec3 vNormal;
out vec3 vWorldPos;
void main() {
    vWorldPos = vec3(uModel * vec4(aPos, 1.0));
    vNormal = normalize(uNormalMat * aNormal);
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";
static const char* sphere_frag_src = R"(
#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
uniform vec3 uColor;
uniform vec3 uLightDir;
uniform float uGlow;
out vec4 fragColor;
void main() {
    float NdotL = max(dot(vNormal, uLightDir), 0.0);
    float ambient = 0.15;
    float diffuse = NdotL * 0.7;
    vec3 viewDir = normalize(-vWorldPos);
    float rim = 1.0 - max(dot(vNormal, viewDir), 0.0);
    rim = pow(rim, 3.0) * 0.4;
    vec3 color = uColor * (ambient + diffuse) + uColor * rim + vec3(uGlow);
    fragColor = vec4(color, 1.0);
}
)";

// --- Grid Ripple Shader ---
static const char* grid_vert_src = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 uMVP;
uniform float uTime;
uniform float uTotalTime;
uniform float uAmp;
uniform float uFreq;
out float vHeight;
out vec3 vPos;
out float vDist;

void main() {
    float r = length(aPos.xz);
    if (r < 1.0) r = 1.0; 
    
    // Dynamic amplitude scaling: 4x at start, 2x at end
    float progress = clamp(uTime / uTotalTime, 0.0, 1.0);
    float dynamic_scale = mix(4.0, 2.0, progress);
    
    // Wave ripple: h ~ (1/r) * sin(omega*(t-r))
    // Base multiplier 2e8 * dynamic_scale
    float disp = (uAmp * 2e8 * dynamic_scale / r) * sin(uFreq * 20.0 * (uTime - r * 0.2));
    
    // Dampen near origin to avoid mesh mess
    float fade = smoothstep(5.0, 20.0, r);
    disp *= fade;

    vec3 pos = aPos;
    pos.y += disp;
    vPos = pos;
    vHeight = disp;
    vDist = r;

    gl_Position = uMVP * vec4(pos, 1.0);
}
)";

static const char* grid_frag_src = R"(
#version 330 core
in float vHeight;
in vec3 vPos;
in float vDist;
uniform vec3 uColor;
out vec4 fragColor;
void main() {
    // Procedural grid lines
    vec2 coord = vPos.xz * 0.5; // spacing
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    float line = min(grid.x, grid.y);
    float alpha = 1.0 - min(line, 1.0);
    
    // Fade distant
    alpha *= smoothstep(150.0, 50.0, vDist);
    
    // Pulse color with height: higher points turn white
    float peak = smoothstep(0.0, 1.0, vHeight * 0.5);
    vec3 col = mix(uColor, vec3(1.0), peak);

    if (alpha <= 0.01) discard;
    fragColor = vec4(col, alpha * 0.6);
}
)";


// ============================================================================
// Objects
// ============================================================================

struct Mesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    int index_count = 0;
};

static Mesh g_sphere;
static Mesh g_grid;
static GLuint g_quad_vao = 0, g_quad_vbo = 0;

static void init_quad() {
    float verts[] = { -1,-1, 1,-1, -1,1, -1,1, 1,-1, 1,1 };
    glGenVertexArrays(1, &g_quad_vao);
    glBindVertexArray(g_quad_vao);
    glGenBuffers(1, &g_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

static void create_sphere(Mesh& mesh, int stacks = 16, int slices = 24) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    for (int i = 0; i <= stacks; i++) {
        float phi = (float)M_PI * i / stacks;
        for (int j = 0; j <= slices; j++) {
            float theta = 2.0f * (float)M_PI * j / slices;
            float x = sinf(phi) * cosf(theta);
            float y = cosf(phi);
            float z = sinf(phi) * sinf(theta);
            vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
            vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
        }
    }
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            int a = i * (slices + 1) + j;
            int b = a + slices + 1;
            indices.push_back(a); indices.push_back(b); indices.push_back(a + 1);
            indices.push_back(b); indices.push_back(b + 1); indices.push_back(a + 1);
        }
    }
    mesh.index_count = (int)indices.size();
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

static void create_grid_mesh(Mesh& mesh) {
    const int N = 150; // Divisions
    const float SIZE = 300.0f;
    std::vector<float> verts;
    std::vector<unsigned int> indices;
    
    for(int i=0; i<=N; ++i) {
        for(int j=0; j<=N; ++j) {
            float x = (float)i / N * SIZE - SIZE/2.0f;
            float z = (float)j / N * SIZE - SIZE/2.0f;
            verts.push_back(x);
            verts.push_back(0.0f);
            verts.push_back(z);
        }
    }
    
    for(int i=0; i<N; ++i) {
        for(int j=0; j<N; ++j) {
            int row1 = i * (N+1) + j;
            int row2 = (i+1) * (N+1) + j;
            indices.push_back(row1); indices.push_back(row1+1); indices.push_back(row2);
            indices.push_back(row2); indices.push_back(row1+1); indices.push_back(row2+1);
        }
    }
    mesh.index_count = (int)indices.size();
    
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// ============================================================================
// OpenGL Helpers
// ============================================================================
static GLuint compile_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    int ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        fprintf(stderr, "Shader error: %s\n", log);
    }
    return s;
}

static GLuint create_program(const char* vs, const char* fs) {
    GLuint v = compile_shader(GL_VERTEX_SHADER, vs);
    GLuint f = compile_shader(GL_FRAGMENT_SHADER, fs);
    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

static GLuint g_prog_raymarch = 0;
static GLuint g_prog_sphere = 0;
static GLuint g_prog_grid = 0;

// ============================================================================
// Draw Functions
// ============================================================================

static void draw_black_holes_raymarched(const bh::CollisionRenderData& frame,
                                        const glm::vec3& camPos,
                                        const glm::vec3& camTarget,
                                        float fovDegrees) {
    glUseProgram(g_prog_raymarch);
    
    glm::vec3 camDir = glm::normalize(camTarget - camPos);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 camRight = glm::normalize(glm::cross(camDir, up));
    glm::vec3 camUp = glm::cross(camRight, camDir);
    
    glUniform2f(glGetUniformLocation(g_prog_raymarch, "uResolution"), (float)g_width, (float)g_height);
    glUniform3fv(glGetUniformLocation(g_prog_raymarch, "uCamPos"), 1, glm::value_ptr(camPos));
    glUniform3fv(glGetUniformLocation(g_prog_raymarch, "uCamDir"), 1, glm::value_ptr(camDir));
    glUniform3fv(glGetUniformLocation(g_prog_raymarch, "uCamUp"), 1, glm::value_ptr(camUp));
    glUniform3fv(glGetUniformLocation(g_prog_raymarch, "uCamRight"), 1, glm::value_ptr(camRight));
    
    float tanFov = tanf(glm::radians(fovDegrees) * 0.5f);
    glUniform1f(glGetUniformLocation(g_prog_raymarch, "uFov"), tanFov);
    
    int numBH = std::min(frame.num_black_holes, 2);
    glUniform1i(glGetUniformLocation(g_prog_raymarch, "uNumBH"), numBH);
    
    glm::vec3 positions[2] = { glm::vec3(0), glm::vec3(0) };
    float masses[2] = { 0, 0 };
    float radii[2] = { 0, 0 };
    
    for(int i=0; i<numBH; i++) {
        positions[i] = frame.black_holes[i].position;
        masses[i] = frame.black_holes[i].mass;
        radii[i] = frame.black_holes[i].schwarzschild_radius;
    }
    
    glUniform3fv(glGetUniformLocation(g_prog_raymarch, "uBHPos"), 2, glm::value_ptr(positions[0]));
    glUniform1fv(glGetUniformLocation(g_prog_raymarch, "uBHMass"), 2, masses);
    glUniform1fv(glGetUniformLocation(g_prog_raymarch, "uBHRadius"), 2, radii);
    
    float glow = (frame.phase == 1) ? 2.0f : 1.0f; 
    glUniform1f(glGetUniformLocation(g_prog_raymarch, "uGlowIntensity"), glow);
    
    glBindVertexArray(g_quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static void draw_sphere(const glm::vec3& pos, float radius,
                         const glm::vec3& color, float glow,
                         const glm::mat4& view, const glm::mat4& proj) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::scale(model, glm::vec3(radius));
    glm::mat4 mvp = proj * view * model;
    glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(model)));
    glUseProgram(g_prog_sphere);
    glUniformMatrix4fv(glGetUniformLocation(g_prog_sphere, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(g_prog_sphere, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix3fv(glGetUniformLocation(g_prog_sphere, "uNormalMat"), 1, GL_FALSE, glm::value_ptr(normalMat));
    glUniform3fv(glGetUniformLocation(g_prog_sphere, "uColor"), 1, glm::value_ptr(color));
    glUniform3f(glGetUniformLocation(g_prog_sphere, "uLightDir"), 0.5f, 0.8f, 0.3f);
    glUniform1f(glGetUniformLocation(g_prog_sphere, "uGlow"), glow);
    glBindVertexArray(g_sphere.vao);
    glDrawElements(GL_TRIANGLES, g_sphere.index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

static void draw_grid_ripple(const glm::mat4& vp, float time, float total_time, float amp, float freq) {
    glUseProgram(g_prog_grid);
    glUniformMatrix4fv(glGetUniformLocation(g_prog_grid, "uMVP"), 1, GL_FALSE, glm::value_ptr(vp));
    glUniform1f(glGetUniformLocation(g_prog_grid, "uTime"), time);
    glUniform1f(glGetUniformLocation(g_prog_grid, "uTotalTime"), total_time);
    glUniform1f(glGetUniformLocation(g_prog_grid, "uAmp"), amp);
    glUniform1f(glGetUniformLocation(g_prog_grid, "uFreq"), freq);
    glUniform3f(glGetUniformLocation(g_prog_grid, "uColor"), 0.1f, 0.2f, 0.3f);
    
    // Draw grid
    glBindVertexArray(g_grid.vao);
    glDrawElements(GL_TRIANGLES, g_grid.index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

static void update_title(GLFWwindow* window, const bh::CollisionRenderData& frame, float total, float speed) {
    char title[256];
    const char* phase_names[] = {"INSPIRAL", "MERGER", "RINGDOWN", "POST-RINGDOWN"};
    const char* phase = (frame.phase >= 0 && frame.phase < 4) ? phase_names[frame.phase] : "?";
    snprintf(title, sizeof(title), "BH Collision (Raymarched+Ripple) | t=%.1f/%.1f M | %s | BHs=%d | speed=%.1fx%s",
        frame.time, total, phase, frame.num_black_holes, speed, g_paused ? " [PAUSED]" : "");
    glfwSetWindowTitle(window, title);
}

// Input callbacks
static void scroll_callback(GLFWwindow* w, double dx, double dy) {
    g_cam_dist = std::clamp(g_cam_dist - (float)dy * 2.0f, 3.0f, 200.0f);
}
static void mouse_button_callback(GLFWwindow* w, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        g_mouse_dragging = (action == GLFW_PRESS);
        if (g_mouse_dragging) glfwGetCursorPos(w, &g_mouse_last_x, &g_mouse_last_y);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        g_right_dragging = (action == GLFW_PRESS);
        if (g_right_dragging) glfwGetCursorPos(w, &g_mouse_last_x, &g_mouse_last_y);
    }
}
static void cursor_pos_callback(GLFWwindow* w, double xpos, double ypos) {
    double dx = xpos - g_mouse_last_x;
    double dy = ypos - g_mouse_last_y;
    g_mouse_last_x = xpos;
    g_mouse_last_y = ypos;
    if (g_mouse_dragging) {
        g_cam_yaw -= (float)dx * 0.3f;
        g_cam_pitch = std::clamp(g_cam_pitch + (float)dy * 0.3f, -89.0f, 89.0f);
    }
    if (g_right_dragging) {
        float yaw_rad = glm::radians(g_cam_yaw);
        glm::vec3 right = {-sinf(yaw_rad), 0, cosf(yaw_rad)};
        glm::vec3 up = {0, 1, 0};
        g_cam_target += (-right * (float)dx * 0.05f + up * (float)dy * 0.05f);
    }
}
static void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    g_width = width;
    g_height = height;
    glViewport(0, 0, width, height);
}
static void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;
    switch (key) {
        case GLFW_KEY_SPACE: g_paused = !g_paused; break;
        case GLFW_KEY_R: g_playback_time = 0.0f; break;
        case GLFW_KEY_EQUAL: case GLFW_KEY_KP_ADD: g_playback_speed = std::min(g_playback_speed*2.0f, 64.0f); break;
        case GLFW_KEY_MINUS: case GLFW_KEY_KP_SUBTRACT: g_playback_speed = std::max(g_playback_speed*0.5f, 0.0625f); break;
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(w, 1); break;
    }
}
static void process_held_keys(GLFWwindow* w, float dt) {
    float move_speed = g_cam_speed * g_cam_dist * dt;
    float yaw_rad = glm::radians(g_cam_yaw);
    float pitch_rad = glm::radians(g_cam_pitch);
    glm::vec3 forward = {cosf(pitch_rad) * cosf(yaw_rad), sinf(pitch_rad), cosf(pitch_rad) * sinf(yaw_rad)};
    glm::vec3 right = {-sinf(yaw_rad), 0, cosf(yaw_rad)};
    glm::vec3 up = {0, 1, 0};
    if (glfwGetKey(w, GLFW_KEY_W) || glfwGetKey(w, GLFW_KEY_UP)) g_cam_target += forward * move_speed;
    if (glfwGetKey(w, GLFW_KEY_S) || glfwGetKey(w, GLFW_KEY_DOWN)) g_cam_target -= forward * move_speed;
    if (glfwGetKey(w, GLFW_KEY_A) || glfwGetKey(w, GLFW_KEY_LEFT)) g_cam_target -= right * move_speed;
    if (glfwGetKey(w, GLFW_KEY_D) || glfwGetKey(w, GLFW_KEY_RIGHT)) g_cam_target += right * move_speed;
    if (glfwGetKey(w, GLFW_KEY_Q)) g_cam_target += up * move_speed;
    if (glfwGetKey(w, GLFW_KEY_E)) g_cam_target -= up * move_speed;
}

int main(int argc, char** argv) {
    printf("================================================================\n");
    printf("  BLACK HOLE COLLISION - 3D VIEWER (Final Polish)\n");
    printf("================================================================\n\n");

    bh::SimulationConfig sim_config;
    sim_config.record_interval = 1.0; 
    sim_config.binary.initial_separation = 16.0; // Reduced by 20% from default 20.0 
    
    // ULTRA EXTREME Fidelity Settings (400x more detailed)
    sim_config.integrator.safety_factor = 2.5e-7; 
    sim_config.integrator.dt_min = 1e-10;           
    sim_config.integrator.dt_max = 0.1; 
    
    // Post-merger extension: ~3 seconds of ringdown
    sim_config.ringdown_duration = 1400.0; 
    sim_config.ringdown_samples = 1500; 

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--m1") == 0 && i + 1 < argc) sim_config.binary.m1 = atof(argv[++i]);
        else if (strcmp(argv[i], "--m2") == 0 && i + 1 < argc) sim_config.binary.m2 = atof(argv[++i]);
        else if (strcmp(argv[i], "--sep") == 0 && i + 1 < argc) sim_config.binary.initial_separation = atof(argv[++i]);
    }
    double M_total = sim_config.binary.m1 + sim_config.binary.m2;
    sim_config.binary.m1 /= M_total; sim_config.binary.m2 /= M_total;

    printf("  Running simulation...\n");
    bh::SimulationResult result = bh::run_simulation(sim_config);
    bh::CollisionTimeline timeline = bh::CollisionTimeline::build(result);
    printf("  Timeline: %.1f M, %zu frames\n", timeline.total_duration, timeline.frames.size());

    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(g_width, g_height, "BH Collision Viewer", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    if (glewInit() != GLEW_OK) return 1;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable culling so we see grid from all angles
    glDisable(GL_CULL_FACE);

    // Init resources
    g_prog_raymarch = create_program(raymarch_vert_src, raymarch_frag_src);
    g_prog_sphere = create_program(sphere_vert_src, sphere_frag_src);
    g_prog_grid = create_program(grid_vert_src, grid_frag_src);

    init_quad();
    create_grid_mesh(g_grid);
    create_sphere(g_sphere, 16, 16);

    float last_time = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        float now = (float)glfwGetTime();
        float dt = std::min(now - last_time, 0.05f);
        last_time = now;
        
        process_held_keys(window, dt);
        if (!g_paused) {
            // Compute current separation for adaptive speed
            bh::CollisionRenderData current_frame = timeline.interpolate(g_playback_time);
            float separation = 100.0f;
            if (current_frame.num_black_holes == 2) {
                 separation = glm::length(current_frame.black_holes[0].position - current_frame.black_holes[1].position);
            }
            
            // Adaptive Speed Logic:
            // - Far (>15M): Slower overall (250 M/s - 50% of previous)
            // - Close (<10M): Adaptive 0.5x (125 M/s)
            // - Ringdown (Phase >= 2): Medium speed (100 M/s)
            
            float target_speed = 250.0f;
            
            if (current_frame.phase >= 2) {
                target_speed = 100.0f; // Ringdown
            } else {
                float t = std::clamp((separation - 10.0f) / 5.0f, 0.0f, 1.0f);
                target_speed = glm::mix(62.5f, 250.0f, t); // Slow down to 62.5 (0.25x) at merger
            }

            // Smooth transition
            static float current_speed_val = 250.0f;
            current_speed_val = glm::mix(current_speed_val, target_speed, dt * 5.0f);
            
            g_playback_time += dt * current_speed_val * g_playback_speed;
        }
        if (g_playback_time > timeline.total_duration) {
            g_playback_time = 0.0f; 
        }

        bh::CollisionRenderData frame = timeline.interpolate(g_playback_time);

        float yaw_rad = glm::radians(g_cam_yaw);
        float pitch_rad = glm::radians(g_cam_pitch);
        glm::vec3 cam_offset = { g_cam_dist * cosf(pitch_rad) * cosf(yaw_rad), g_cam_dist * sinf(pitch_rad), g_cam_dist * cosf(pitch_rad) * sinf(yaw_rad) };
        glm::vec3 cam_pos = g_cam_target + cam_offset;
        glm::mat4 view = glm::lookAt(cam_pos, g_cam_target, glm::vec3(0, 1, 0));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)g_width/g_height, 0.1f, 500.0f);
        glm::mat4 vp = proj * view;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        draw_black_holes_raymarched(frame, cam_pos, g_cam_target, 45.0f);
        glEnable(GL_DEPTH_TEST);

        // Draw Ripple Grid
        draw_grid_ripple(vp, g_playback_time, timeline.total_duration, frame.gw_amplitude, frame.gw_frequency);

        if (frame.num_black_holes == 2) {
             glm::vec3 com = (frame.black_holes[0].position * frame.black_holes[0].mass +
                             frame.black_holes[1].position * frame.black_holes[1].mass) /
                            (frame.black_holes[0].mass + frame.black_holes[1].mass);
             draw_sphere(com, 0.15f, {1.0f, 1.0f, 0.5f}, 0.3f, view, proj);
        }

        update_title(window, frame, timeline.total_duration, g_playback_speed);
        glfwSwapBuffers(window);
    }

    glDeleteProgram(g_prog_raymarch);
    glDeleteProgram(g_prog_sphere);
    glDeleteProgram(g_prog_grid);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
