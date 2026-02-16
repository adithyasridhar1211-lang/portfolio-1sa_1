/**
 * @file simulation.cpp
 * @brief Simulation manager: runs inspiral → merger → ringdown pipeline,
 *        records frame history, exports results.
 */

#include "bh_collision/simulation.h"
#include "bh_collision/physics.h"
#include "bh_collision/integrator.h"
#include "bh_collision/merger.h"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace bh {

// ============================================================================
// Initialize a binary system from configuration
// ============================================================================

static void init_binary(const BinaryConfig& config,
                        BlackHole& bh1, BlackHole& bh2)
{
    double M = config.m1 + config.m2;
    double r0 = config.initial_separation;

    bh1.mass = config.m1;
    bh1.chi = config.chi1;
    bh1.spin_axis = glm::normalize(config.spin_axis1);

    bh2.mass = config.m2;
    bh2.chi = config.chi2;
    bh2.spin_axis = glm::normalize(config.spin_axis2);

    // Place BH1 and BH2 in the orbital plane (x-z plane), offset from COM
    double r1 = r0 * config.m2 / M;  // distance of BH1 from COM
    double r2 = r0 * config.m1 / M;  // distance of BH2 from COM

    bh1.position = glm::dvec3(r1, 0.0, 0.0);
    bh2.position = glm::dvec3(-r2, 0.0, 0.0);

    // Circular orbit velocities (Keplerian + PN correction-ready)
    // v_circ = sqrt(M/r) for relative orbit
    double v_circ_rel = std::sqrt(M / r0);

    // For eccentric orbits: v_peri = v_circ * sqrt((1+e)/(1-e))
    double ecc = config.eccentricity;
    double v_factor = std::sqrt((1.0 + ecc) / (1.0 - ecc));
    double v_rel = v_circ_rel * v_factor;

    // Distribute relative velocity to individual BHs (COM frame)
    double v1 = v_rel * config.m2 / M;
    double v2 = v_rel * config.m1 / M;

    // Velocities perpendicular to position (circular orbit in x-z plane)
    bh1.velocity = glm::dvec3(0.0, 0.0, v1);
    bh2.velocity = glm::dvec3(0.0, 0.0, -v2);
}

// ============================================================================
// Build the derivative function for the integrator
// ============================================================================

static DerivativeFunc make_deriv_func(double m1, double m2,
                                       bool pn1, bool pn2, bool pn25)
{
    return [m1, m2, pn1, pn2, pn25](const BinaryState& state) -> BinaryStateDerivative {
        BinaryStateDerivative d;

        // Relative coordinate
        glm::dvec3 r = state.pos1 - state.pos2;
        glm::dvec3 v = state.vel1 - state.vel2;
        double M = m1 + m2;

        // Compute relative acceleration (PN)
        AccelerationResult acc = compute_relative_acceleration(
            r, v, m1, m2, pn1, pn2, pn25
        );
        glm::dvec3 a_rel = acc.total();

        // Convert to individual accelerations:
        // a1 = (m2/M) * a_rel
        // a2 = -(m1/M) * a_rel
        d.dpos1 = state.vel1;
        d.dvel1 = (m2 / M) * a_rel;
        d.dpos2 = state.vel2;
        d.dvel2 = -(m1 / M) * a_rel;

        return d;
    };
}

// ============================================================================
// Record a simulation frame
// ============================================================================

static SimulationFrame make_frame(
    double time, const BlackHole& bh1, const BlackHole& bh2,
    double obs_distance, double obs_inclination, int phase)
{
    SimulationFrame frame;
    frame.time = time;
    frame.bh1 = bh1;
    frame.bh2 = bh2;
    frame.orbital = compute_orbital_params(bh1, bh2);
    frame.gw = compute_gw_strain(bh1, bh2, obs_distance, obs_inclination);
    frame.phase = phase;
    return frame;
}

// ============================================================================
// Main simulation loop
// ============================================================================

SimulationResult run_simulation(const SimulationConfig& config)
{
    SimulationResult result = {};
    result.config = config.binary;
    result.merger_occurred = false;
    result.merger_time = 0.0;
    result.total_gw_cycles = 0.0;

    // Initialize the binary
    BlackHole bh1, bh2;
    init_binary(config.binary, bh1, bh2);

    // Get initial orbital parameters for time estimate
    OrbitalParams initial_orbit = compute_orbital_params(bh1, bh2);
    double estimated_merger_time = time_to_merger_estimate(
        initial_orbit.symmetric_mass_ratio,
        initial_orbit.total_mass,
        initial_orbit.separation
    );

    // Build integrator state
    BinaryState state;
    state.pos1 = bh1.position;
    state.vel1 = bh1.velocity;
    state.pos2 = bh2.position;
    state.vel2 = bh2.velocity;
    state.time = 0.0;

    auto deriv = make_deriv_func(
        bh1.mass, bh2.mass,
        config.enable_1pn, config.enable_2pn, config.enable_25pn
    );

    double total_mass = bh1.mass + bh2.mass;
    double last_record_time = -config.record_interval;
    double last_phase = 0.0;
    long long step_count = 0;

    // ========================================================================
    // PHASE 1: INSPIRAL
    // ========================================================================
    while (state.time < config.max_time) {
        // Update BH states from integrator state
        bh1.position = state.pos1;
        bh1.velocity = state.vel1;
        bh2.position = state.pos2;
        bh2.velocity = state.vel2;

        // Check merger condition
        if (should_merge(bh1, bh2)) {
            result.merger_occurred = true;
            result.merger_time = state.time;

            // Record the merger frame
            result.frames.push_back(
                make_frame(state.time, bh1, bh2,
                          config.observer_distance, config.observer_inclination, 1)
            );
            break;
        }

        // Adaptive recording interval: fast capture during plunge
        double current_sep = glm::length(state.pos1 - state.pos2);
        double M_tot = bh1.mass + bh2.mass;
        double effective_interval = config.record_interval;
        
        if (current_sep < 10.0 * M_tot) {
             effective_interval = config.record_interval / 4000.0; // High fidelity plunge (4000x)
        }

        // Record frame at intervals
        if (state.time - last_record_time >= effective_interval) {
            result.frames.push_back(
                make_frame(state.time, bh1, bh2,
                          config.observer_distance, config.observer_inclination, 0)
            );
            last_record_time = state.time;

            // Track GW cycles via phase
            OrbitalParams orb = compute_orbital_params(bh1, bh2);
            double phase_diff = orb.orbital_phase - last_phase;
            // Handle phase wrapping
            if (phase_diff < -M_PI) phase_diff += 2.0 * M_PI;
            if (phase_diff > M_PI) phase_diff -= 2.0 * M_PI;
            result.total_gw_cycles += std::abs(phase_diff) / M_PI;  // GW = 2x orbital
            last_phase = orb.orbital_phase;
        }

        // Progress callback
        if (config.progress_callback && step_count % 10000 == 0) {
            double frac = std::min(1.0, state.time / estimated_merger_time);
            config.progress_callback(state.time, frac, "inspiral");
        }

        // Adaptive time step
        double dt = adaptive_timestep(state, config.integrator, total_mass);

        // RK4 step
        state = rk4_step(state, dt, deriv);
        step_count++;

        // Safety: bail if we've done too many steps
        if (step_count > 2000000000) {
            break;
        }
    }

    result.num_inspiral_frames = (int)result.frames.size();

    // ========================================================================
    // PHASE 2: MERGER → REMNANT
    // ========================================================================
    if (result.merger_occurred) {
        result.remnant = compute_remnant(bh1, bh2);
        result.total_energy_radiated = result.remnant.energy_radiated;

        // Get GW amplitude at merger for ringdown matching
        GWStrain merger_gw = compute_gw_strain(
            bh1, bh2, config.observer_distance, config.observer_inclination
        );
        double merger_amplitude = merger_gw.amplitude * config.observer_distance;

        // Compute QNM parameters
        result.qnm = compute_qnm_222(
            result.remnant.mass, result.remnant.spin, merger_amplitude
        );

        // ====================================================================
        // PHASE 3: RINGDOWN
        // ====================================================================
        double ringdown_dt = config.ringdown_duration / config.ringdown_samples;

        for (int i = 0; i < config.ringdown_samples; i++) {
            double t_ring = i * ringdown_dt;

            GWStrain gw_ring = ringdown_strain(
                result.qnm, t_ring,
                config.observer_distance, config.observer_inclination
            );

            // Create a frame for the remnant
            SimulationFrame frame;
            frame.time = result.merger_time + t_ring;
            frame.bh1.mass = result.remnant.mass;
            frame.bh1.chi = result.remnant.spin;
            frame.bh1.position = result.remnant.position +
                                 result.remnant.velocity * t_ring;
            frame.bh1.velocity = result.remnant.velocity;
            frame.bh1.spin_axis = glm::dvec3(0, 1, 0);

            // BH2 doesn't exist in ringdown but we zero it out
            frame.bh2 = {};
            frame.bh2.mass = 0.0;

            frame.orbital = {};
            frame.orbital.separation = 0.0;
            frame.orbital.orbital_frequency = result.qnm.frequency;

            frame.gw = gw_ring;
            frame.phase = (gw_ring.amplitude > 1e-30) ? 2 : 3;

            result.frames.push_back(frame);

            if (config.progress_callback && i % 50 == 0) {
                double frac = (double)i / config.ringdown_samples;
                config.progress_callback(frame.time, frac, "ringdown");
            }
        }

        result.num_ringdown_frames = config.ringdown_samples;
    }

    return result;
}

// ============================================================================
// JSON export
// ============================================================================

bool export_to_json(const SimulationResult& result, const std::string& filename)
{
    std::ofstream out(filename);
    if (!out.is_open()) return false;

    out << std::setprecision(12);
    out << "{\n";

    // Metadata
    out << "  \"metadata\": {\n";
    out << "    \"units\": \"geometrized (G=c=1)\",\n";
    out << "    \"mass_unit\": \"total_mass_M\",\n";
    out << "    \"length_unit\": \"M\",\n";
    out << "    \"time_unit\": \"M\",\n";
    out << "    \"num_frames\": " << result.frames.size() << ",\n";
    out << "    \"merger_occurred\": " << (result.merger_occurred ? "true" : "false") << ",\n";
    out << "    \"merger_time\": " << result.merger_time << ",\n";
    out << "    \"total_gw_cycles\": " << result.total_gw_cycles << ",\n";
    out << "    \"energy_radiated_fraction\": " << result.total_energy_radiated << "\n";
    out << "  },\n";

    // Config
    out << "  \"config\": {\n";
    out << "    \"m1\": " << result.config.m1 << ",\n";
    out << "    \"m2\": " << result.config.m2 << ",\n";
    out << "    \"chi1\": " << result.config.chi1 << ",\n";
    out << "    \"chi2\": " << result.config.chi2 << ",\n";
    out << "    \"initial_separation\": " << result.config.initial_separation << ",\n";
    out << "    \"eccentricity\": " << result.config.eccentricity << "\n";
    out << "  },\n";

    // Remnant
    if (result.merger_occurred) {
        out << "  \"remnant\": {\n";
        out << "    \"mass\": " << result.remnant.mass << ",\n";
        out << "    \"spin\": " << result.remnant.spin << ",\n";
        out << "    \"kick_velocity\": " << result.remnant.kick_velocity << ",\n";
        out << "    \"energy_radiated\": " << result.remnant.energy_radiated << ",\n";
        out << "    \"position\": [" << result.remnant.position.x << ", "
            << result.remnant.position.y << ", " << result.remnant.position.z << "],\n";
        out << "    \"qnm_frequency\": " << result.qnm.frequency << ",\n";
        out << "    \"qnm_damping_time\": " << result.qnm.damping_time << "\n";
        out << "  },\n";
    }

    // Frames
    out << "  \"frames\": [\n";
    for (size_t i = 0; i < result.frames.size(); i++) {
        const auto& f = result.frames[i];
        out << "    {\n";
        out << "      \"time\": " << f.time << ",\n";
        out << "      \"phase\": " << f.phase << ",\n";

        out << "      \"bh1\": {\n";
        out << "        \"mass\": " << f.bh1.mass << ",\n";
        out << "        \"position\": [" << f.bh1.position.x << ", "
            << f.bh1.position.y << ", " << f.bh1.position.z << "],\n";
        out << "        \"velocity\": [" << f.bh1.velocity.x << ", "
            << f.bh1.velocity.y << ", " << f.bh1.velocity.z << "]\n";
        out << "      },\n";

        out << "      \"bh2\": {\n";
        out << "        \"mass\": " << f.bh2.mass << ",\n";
        out << "        \"position\": [" << f.bh2.position.x << ", "
            << f.bh2.position.y << ", " << f.bh2.position.z << "],\n";
        out << "        \"velocity\": [" << f.bh2.velocity.x << ", "
            << f.bh2.velocity.y << ", " << f.bh2.velocity.z << "]\n";
        out << "      },\n";

        out << "      \"orbital\": {\n";
        out << "        \"separation\": " << f.orbital.separation << ",\n";
        out << "        \"frequency\": " << f.orbital.orbital_frequency << ",\n";
        out << "        \"energy\": " << f.orbital.energy << "\n";
        out << "      },\n";

        out << "      \"gw\": {\n";
        out << "        \"h_plus\": " << f.gw.h_plus << ",\n";
        out << "        \"h_cross\": " << f.gw.h_cross << ",\n";
        out << "        \"amplitude\": " << f.gw.amplitude << ",\n";
        out << "        \"frequency\": " << f.gw.frequency << "\n";
        out << "      }\n";

        out << "    }" << (i + 1 < result.frames.size() ? "," : "") << "\n";
    }
    out << "  ]\n";

    out << "}\n";
    out.close();
    return true;
}

// ============================================================================
// Summary printer
// ============================================================================

void print_summary(const SimulationResult& result)
{
    printf("\n");
    printf("================================================================\n");
    printf("  BINARY BLACK HOLE MERGER SIMULATION - RESULTS\n");
    printf("================================================================\n\n");

    printf("Initial Conditions:\n");
    printf("  m1 = %.4f M, m2 = %.4f M (q = %.2f)\n",
           result.config.m1, result.config.m2,
           result.config.m1 / result.config.m2);
    printf("  chi1 = %.3f, chi2 = %.3f\n",
           result.config.chi1, result.config.chi2);
    printf("  Initial separation = %.2f M\n",
           result.config.initial_separation);
    printf("  Eccentricity = %.4f\n\n",
           result.config.eccentricity);

    double eta = result.config.m1 * result.config.m2 /
        std::pow(result.config.m1 + result.config.m2, 2.0);
    double chirp = (result.config.m1 + result.config.m2) * std::pow(eta, 0.6);
    printf("  Symmetric mass ratio eta = %.4f\n", eta);
    printf("  Chirp mass M_c = %.4f M\n\n", chirp);

    printf("Simulation Statistics:\n");
    printf("  Total frames recorded: %zu\n", result.frames.size());
    printf("  Inspiral frames: %d\n", result.num_inspiral_frames);
    printf("  Ringdown frames: %d\n", result.num_ringdown_frames);
    printf("  Total GW cycles: %.1f\n\n", result.total_gw_cycles);

    if (result.merger_occurred) {
        printf("Merger:\n");
        printf("  Merger time = %.2f M\n", result.merger_time);
        printf("  Energy radiated = %.4f M (%.2f%%)\n",
               result.total_energy_radiated,
               result.total_energy_radiated * 100.0);

        printf("\nRemnant Black Hole:\n");
        printf("  Mass = %.6f M\n", result.remnant.mass);
        printf("  Spin = %.6f\n", result.remnant.spin);
        printf("  Kick velocity = %.6f c (%.1f km/s)\n",
               result.remnant.kick_velocity,
               result.remnant.kick_velocity * 2.998e5);
        printf("  Position = (%.4f, %.4f, %.4f)\n",
               result.remnant.position.x,
               result.remnant.position.y,
               result.remnant.position.z);

        printf("\nQuasinormal Mode (l=2, m=2, n=0):\n");
        printf("  Frequency = %.6f / M_f\n", result.qnm.frequency);
        printf("  Damping time = %.4f M_f\n", result.qnm.damping_time);
        printf("  Amplitude = %.6e\n", result.qnm.amplitude);
    } else {
        printf("  No merger occurred within simulation time.\n");
    }

    printf("\n================================================================\n\n");
}

} // namespace bh
