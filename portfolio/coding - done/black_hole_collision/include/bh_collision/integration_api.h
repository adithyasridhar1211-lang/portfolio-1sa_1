/**
 * @file integration_api.h
 * @brief Data structures for integrating the collision simulation with the
 *        black_hole_v2.2.0 visual renderer.
 *
 * Include this header in the renderer project to access simulation data.
 * The structures here map directly to OpenGL uniform values.
 */

#ifndef BH_COLLISION_INTEGRATION_API_H
#define BH_COLLISION_INTEGRATION_API_H

#include <glm/glm.hpp>
#include <vector>

namespace bh {

/// Render-ready state for a single black hole
/// Designed to map directly to shader uniforms
struct BHRenderState {
    glm::vec3 position;         // 3D position (float for GPU)
    float mass;                 // Mass (in units of total M)
    float schwarzschild_radius; // r_s = 2m
    float spin;                 // Dimensionless spin [0,1)
    glm::vec3 spin_axis;        // Spin direction (unit vector)
    float isco_radius;          // Innermost stable circular orbit radius
};

/// Complete render data for a single frame
struct CollisionRenderData {
    float time;
    int num_black_holes;         // 1 after merger, 2 during inspiral
    BHRenderState black_holes[4]; // Support up to 4 BHs
    float gw_strain_plus;        // GW h+ (for visual spacetime distortion)
    float gw_strain_cross;       // GW h× (for visual spacetime distortion)
    float gw_amplitude;          // |h| (for bloom/distortion intensity)
    float gw_frequency;          // Instantaneous GW frequency
    float orbital_phase;         // Current orbital phase
    int phase;                   // 0=inspiral, 1=merger, 2=ringdown
};

/// Timeline of render data for playback
struct CollisionTimeline {
    std::vector<CollisionRenderData> frames;
    float total_duration;
    float merger_time;
    int merger_frame_index;

    /// Get interpolated render data at arbitrary time t
    CollisionRenderData interpolate(float t) const;

    /// Build from simulation result (call after simulation completes)
    static CollisionTimeline build(const struct SimulationResult& result);
};

} // namespace bh

#endif // BH_COLLISION_INTEGRATION_API_H
