/**
 * @file simulation.h
 * @brief Simulation manager: orchestrates inspiral → merger → ringdown phases,
 *        records state history, and exports data.
 */

#ifndef BH_COLLISION_SIMULATION_H
#define BH_COLLISION_SIMULATION_H

#include "black_hole.h"
#include "physics.h"
#include "merger.h"
#include "integrator.h"
#include <vector>
#include <string>
#include <functional>

namespace bh {

/// A single snapshot of the simulation state
struct SimulationFrame {
    double time;
    BlackHole bh1, bh2;
    OrbitalParams orbital;
    GWStrain gw;
    int phase;  // 0=inspiral, 1=merger, 2=ringdown, 3=post-ringdown
};

/// Complete result of a simulation run
struct SimulationResult {
    std::vector<SimulationFrame> frames;
    BinaryConfig config;
    RemnantProperties remnant;
    QNMParams qnm;
    double merger_time;
    double total_gw_cycles;
    double total_energy_radiated;
    bool merger_occurred;
    int num_inspiral_frames;
    int num_ringdown_frames;
};

/// Progress callback: called periodically with (current_time, fraction_complete, phase_name)
using ProgressCallback = std::function<void(double, double, const char*)>;

/// Simulation configuration
struct SimulationConfig {
    BinaryConfig binary;
    IntegratorConfig integrator;

    double max_time = 1e6;          // Maximum simulation time in M
    double record_interval = 10.0;  // Time between recorded frames in M
    double ringdown_duration = 100.0; // How long to simulate ringdown in M_f
    int ringdown_samples = 500;     // Number of ringdown waveform samples

    double observer_distance = 1e6;   // Observer distance in M
    double observer_inclination = 0.0; // Observer inclination in radians

    bool enable_1pn = true;
    bool enable_2pn = true;
    bool enable_25pn = true;     // Must be true for realistic inspiral

    ProgressCallback progress_callback = nullptr;
};

/// Run a complete binary black hole merger simulation
SimulationResult run_simulation(const SimulationConfig& config);

/// Export simulation results to JSON file
bool export_to_json(const SimulationResult& result, const std::string& filename);

/// Print a summary of the simulation results to stdout
void print_summary(const SimulationResult& result);

} // namespace bh

#endif // BH_COLLISION_SIMULATION_H
