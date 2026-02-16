/**
 * @file main.cpp
 * @brief CLI driver for the binary black hole collision simulation.
 *
 * Runs a complete inspiral → merger → ringdown simulation and outputs
 * results to stdout and a JSON file.
 *
 * Usage:
 *   bh_collision [options]
 *
 * Options:
 *   --m1 <mass>           Mass of BH1 (fraction of M, default 0.5)
 *   --m2 <mass>           Mass of BH2 (fraction of M, default 0.5)
 *   --chi1 <spin>         Spin of BH1 [0,1) (default 0.0)
 *   --chi2 <spin>         Spin of BH2 [0,1) (default 0.0)
 *   --sep <separation>    Initial separation in M (default 20.0)
 *   --ecc <eccentricity>  Orbital eccentricity (default 0.0)
 *   --output <file>       Output JSON file (default output/simulation_data.json)
 *   --no-1pn              Disable 1PN corrections
 *   --no-2pn              Disable 2PN corrections
 *   --no-25pn             Disable 2.5PN radiation reaction
 *   --solar-mass <M_sun>  Total mass in solar masses (for SI conversion info)
 *   --help                Show this help
 */

#include "bh_collision/simulation.h"
#include "bh_collision/integration_api.h"
#include "bh_collision/black_hole.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include <filesystem>

void print_help() {
    printf(
        "Binary Black Hole Collision Simulator\n"
        "=====================================\n\n"
        "Computes the inspiral, merger, and ringdown of a binary black hole\n"
        "system using post-Newtonian equations of motion (up to 2.5PN order).\n\n"
        "Usage: bh_collision [options]\n\n"
        "Options:\n"
        "  --m1 <mass>           Mass of BH1 (fraction of M, default 0.5)\n"
        "  --m2 <mass>           Mass of BH2 (fraction of M, default 0.5)\n"
        "  --chi1 <spin>         Spin of BH1 [0,1) (default 0.0)\n"
        "  --chi2 <spin>         Spin of BH2 [0,1) (default 0.0)\n"
        "  --sep <distance>      Initial separation in M (default 20.0)\n"
        "  --ecc <eccentricity>  Orbital eccentricity (default 0.0)\n"
        "  --output <file>       Output JSON file (default output/simulation_data.json)\n"
        "  --no-1pn              Disable 1PN corrections\n"
        "  --no-2pn              Disable 2PN corrections\n"
        "  --no-25pn             Disable 2.5PN radiation reaction\n"
        "  --solar-mass <M>      Total mass in solar masses (for SI info)\n"
        "  --record-interval <t> Time between recorded frames (default 1.0 M)\n"
        "  --help                Show this help\n\n"
        "Units:\n"
        "  All internal quantities use geometrized units (G = c = 1).\n"
        "  Mass is in units of total system mass M.\n"
        "  Distances in M, time in M.\n\n"
        "Example:\n"
        "  bh_collision --m1 0.6 --m2 0.4 --sep 25 --chi1 0.3\n\n"
    );
}

int main(int argc, char** argv) {
    bh::SimulationConfig config;
    std::string output_file = "output/simulation_data.json";
    double solar_masses = 60.0;  // default: 60 solar mass system (like GW150914)

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
        else if (strcmp(argv[i], "--m1") == 0 && i + 1 < argc) {
            config.binary.m1 = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--m2") == 0 && i + 1 < argc) {
            config.binary.m2 = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--chi1") == 0 && i + 1 < argc) {
            config.binary.chi1 = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--chi2") == 0 && i + 1 < argc) {
            config.binary.chi2 = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--sep") == 0 && i + 1 < argc) {
            config.binary.initial_separation = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--ecc") == 0 && i + 1 < argc) {
            config.binary.eccentricity = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        }
        else if (strcmp(argv[i], "--no-1pn") == 0) {
            config.enable_1pn = false;
        }
        else if (strcmp(argv[i], "--no-2pn") == 0) {
            config.enable_2pn = false;
        }
        else if (strcmp(argv[i], "--no-25pn") == 0) {
            config.enable_25pn = false;
        }
        else if (strcmp(argv[i], "--solar-mass") == 0 && i + 1 < argc) {
            solar_masses = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--record-interval") == 0 && i + 1 < argc) {
            config.record_interval = atof(argv[++i]);
        }
        else {
            printf("Unknown option: %s\n", argv[i]);
            print_help();
            return 1;
        }
    }

    // Normalize masses so m1 + m2 = 1
    double M_total = config.binary.m1 + config.binary.m2;
    config.binary.m1 /= M_total;
    config.binary.m2 /= M_total;

    // MAXIMUM Fidelity Settings (100x more detailed)
    config.integrator.safety_factor = 0.000001; // Extremely conservative steps
    config.integrator.dt_min = 1e-10;           // Sub-nanosecond resolution
    config.integrator.dt_max = 0.1;

    config.binary.distance = 1e6;
    config.binary.inclination = 0.0;

    // Set observer parameters
    config.observer_distance = config.binary.distance;
    config.observer_inclination = config.binary.inclination;

    // Print header
    printf("\n");
    printf("================================================================\n");
    printf("  BINARY BLACK HOLE COLLISION SIMULATOR\n");
    printf("  Post-Newtonian order: ");
    if (config.enable_25pn) printf("2.5PN");
    else if (config.enable_2pn) printf("2PN");
    else if (config.enable_1pn) printf("1PN");
    else printf("Newtonian");
    printf("\n");
    printf("================================================================\n\n");

    printf("%s\n", config.binary.to_string().c_str());

    // Estimated merger time
    double eta = config.binary.m1 * config.binary.m2;
    double t_est = bh::time_to_merger_estimate(eta, 1.0, config.binary.initial_separation);
    printf("  Estimated merger time: %.0f M\n", t_est);

    // SI conversion info
    bh::UnitConversion units = bh::UnitConversion::from_solar_masses(solar_masses);
    printf("\n  SI Conversion (%.1f solar masses):\n", solar_masses);
    printf("    1 M = %.4e meters\n", units.length_m);
    printf("    1 M = %.4e seconds\n", units.time_s);
    printf("    Estimated merger time: %.4f seconds\n\n", t_est * units.time_s);

    // Progress callback
    config.progress_callback = [](double time, double frac, const char* phase) {
        printf("\r  [%s] t = %.1f M (%.1f%%)", phase, time, frac * 100.0);
        fflush(stdout);
    };

    // Run simulation
    printf("  Running simulation...\n");
    bh::SimulationResult result = bh::run_simulation(config);
    printf("\r  Simulation complete!                              \n");

    // Print results
    bh::print_summary(result);

    // Export to JSON
    // Create output directory if needed
    std::filesystem::path outpath(output_file);
    if (outpath.has_parent_path()) {
        std::filesystem::create_directories(outpath.parent_path());
    }

    if (bh::export_to_json(result, output_file)) {
        printf("  Data exported to: %s\n", output_file.c_str());
        printf("  Total frames: %zu\n", result.frames.size());
    } else {
        printf("  ERROR: Failed to export to %s\n", output_file.c_str());
    }

    // Build render timeline (demonstrates integration API)
    bh::CollisionTimeline timeline = bh::CollisionTimeline::build(result);
    printf("  Render timeline: %.1f M duration, %zu frames\n",
           timeline.total_duration, timeline.frames.size());

    if (timeline.merger_frame_index >= 0) {
        printf("  Merger at frame %d\n", timeline.merger_frame_index);

        // Demo: interpolate at merger time
        bh::CollisionRenderData rd = timeline.interpolate(timeline.merger_time);
        printf("  At merger: %d BH(s), GW amplitude = %.6e\n",
               rd.num_black_holes, rd.gw_amplitude);
    }

    printf("\n");
    return 0;
}
