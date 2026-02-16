/**
 * @file integration_api.cpp
 * @brief Implementation of renderer integration utilities.
 */

#include "bh_collision/integration_api.h"
#include "bh_collision/simulation.h"
#include <algorithm>
#include <cmath>

namespace bh {

// ============================================================================
// Build a render timeline from simulation results
// ============================================================================

CollisionTimeline CollisionTimeline::build(const SimulationResult& result) {
    CollisionTimeline timeline;

    if (result.frames.empty()) {
        timeline.total_duration = 0;
        timeline.merger_time = 0;
        timeline.merger_frame_index = -1;
        return timeline;
    }

    timeline.merger_time = (float)result.merger_time;
    timeline.total_duration = (float)result.frames.back().time;
    timeline.merger_frame_index = -1;

    for (size_t i = 0; i < result.frames.size(); i++) {
        const auto& f = result.frames[i];

        CollisionRenderData rd = {};
        rd.time = (float)f.time;
        rd.phase = f.phase;

        // Determine number of active black holes
        if (f.phase <= 1) {
            rd.num_black_holes = 2;

            // BH1
            rd.black_holes[0].position = glm::vec3(f.bh1.position);
            rd.black_holes[0].mass = (float)f.bh1.mass;
            rd.black_holes[0].schwarzschild_radius = (float)f.bh1.schwarzschild_radius();
            rd.black_holes[0].spin = (float)f.bh1.chi;
            rd.black_holes[0].spin_axis = glm::vec3(f.bh1.spin_axis);
            rd.black_holes[0].isco_radius = (float)f.bh1.isco_radius();

            // BH2
            rd.black_holes[1].position = glm::vec3(f.bh2.position);
            rd.black_holes[1].mass = (float)f.bh2.mass;
            rd.black_holes[1].schwarzschild_radius = (float)f.bh2.schwarzschild_radius();
            rd.black_holes[1].spin = (float)f.bh2.chi;
            rd.black_holes[1].spin_axis = glm::vec3(f.bh2.spin_axis);
            rd.black_holes[1].isco_radius = (float)f.bh2.isco_radius();

            // Track merger frame
            if (f.phase == 1 && timeline.merger_frame_index < 0) {
                timeline.merger_frame_index = (int)i;
            }
        } else {
            rd.num_black_holes = 1;

            rd.black_holes[0].position = glm::vec3(f.bh1.position);
            rd.black_holes[0].mass = (float)f.bh1.mass;
            rd.black_holes[0].schwarzschild_radius = (float)(2.0 * f.bh1.mass);
            rd.black_holes[0].spin = (float)f.bh1.chi;
            rd.black_holes[0].spin_axis = glm::vec3(0, 1, 0);
            rd.black_holes[0].isco_radius = (float)f.bh1.isco_radius();
        }

        rd.gw_strain_plus = (float)f.gw.h_plus;
        rd.gw_strain_cross = (float)f.gw.h_cross;
        rd.gw_amplitude = (float)f.gw.amplitude;
        rd.gw_frequency = (float)f.gw.frequency;
        rd.orbital_phase = (float)f.orbital.orbital_phase;

        timeline.frames.push_back(rd);
    }

    return timeline;
}

// ============================================================================
// Interpolate between frames for smooth rendering
// ============================================================================

CollisionRenderData CollisionTimeline::interpolate(float t) const {
    if (frames.empty()) {
        return CollisionRenderData{};
    }

    // Clamp time
    t = std::max(0.0f, std::min(t, total_duration));

    // Binary search for the bounding frames
    size_t lo = 0, hi = frames.size() - 1;
    while (lo < hi - 1) {
        size_t mid = (lo + hi) / 2;
        if (frames[mid].time <= t) lo = mid;
        else hi = mid;
    }

    if (lo == hi || t <= frames[lo].time) {
        return frames[lo];
    }
    if (t >= frames[hi].time) {
        return frames[hi];
    }

    // Linear interpolation factor
    float alpha = (t - frames[lo].time) / (frames[hi].time - frames[lo].time);
    alpha = std::max(0.0f, std::min(1.0f, alpha));

    const auto& a = frames[lo];
    const auto& b = frames[hi];

    CollisionRenderData result = {};
    result.time = t;
    result.phase = (alpha < 0.5f) ? a.phase : b.phase;
    result.num_black_holes = (alpha < 0.5f) ? a.num_black_holes : b.num_black_holes;

    // Interpolate BH states
    int n = result.num_black_holes;
    for (int i = 0; i < n; i++) {
        result.black_holes[i].position =
            glm::mix(a.black_holes[i].position, b.black_holes[i].position, alpha);
        result.black_holes[i].mass =
            a.black_holes[i].mass * (1.0f - alpha) + b.black_holes[i].mass * alpha;
        result.black_holes[i].schwarzschild_radius =
            a.black_holes[i].schwarzschild_radius * (1.0f - alpha) +
            b.black_holes[i].schwarzschild_radius * alpha;
        result.black_holes[i].spin =
            a.black_holes[i].spin * (1.0f - alpha) + b.black_holes[i].spin * alpha;
        result.black_holes[i].spin_axis =
            glm::normalize(glm::mix(a.black_holes[i].spin_axis,
                                     b.black_holes[i].spin_axis, alpha));
        result.black_holes[i].isco_radius =
            a.black_holes[i].isco_radius * (1.0f - alpha) +
            b.black_holes[i].isco_radius * alpha;
    }

    // Interpolate GW data
    result.gw_strain_plus  = a.gw_strain_plus  * (1.0f - alpha) + b.gw_strain_plus  * alpha;
    result.gw_strain_cross = a.gw_strain_cross * (1.0f - alpha) + b.gw_strain_cross * alpha;
    result.gw_amplitude    = a.gw_amplitude    * (1.0f - alpha) + b.gw_amplitude    * alpha;
    result.gw_frequency    = a.gw_frequency    * (1.0f - alpha) + b.gw_frequency    * alpha;
    result.orbital_phase   = a.orbital_phase   * (1.0f - alpha) + b.orbital_phase   * alpha;

    return result;
}

} // namespace bh
