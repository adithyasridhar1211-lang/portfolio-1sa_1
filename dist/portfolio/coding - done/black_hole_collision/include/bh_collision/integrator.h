/**
 * @file integrator.h
 * @brief RK4 numerical integrator with adaptive time stepping for
 *        binary black hole orbital dynamics.
 */

#ifndef BH_COLLISION_INTEGRATOR_H
#define BH_COLLISION_INTEGRATOR_H

#include "black_hole.h"
#include <glm/glm.hpp>
#include <functional>
#include <vector>

namespace bh {

/// Full state vector for a binary system: positions and velocities
struct BinaryState {
    glm::dvec3 pos1, vel1;
    glm::dvec3 pos2, vel2;
    double time = 0.0;
};

/// Derivative of the state vector (velocities and accelerations)
struct BinaryStateDerivative {
    glm::dvec3 dpos1, dvel1;
    glm::dvec3 dpos2, dvel2;
};

/// Configuration for the integrator
struct IntegratorConfig {
    double dt_initial = 0.1;        // Initial time step
    double dt_min = 1e-6;           // Minimum allowed time step
    double dt_max = 1.0;            // Maximum allowed time step
    double safety_factor = 0.1;     // Fraction of orbital period for time step
    bool adaptive = true;           // Enable adaptive time stepping
};

/// Type alias for the derivative function
using DerivativeFunc = std::function<BinaryStateDerivative(const BinaryState&)>;

/// Advance the binary state by one RK4 step
/// Returns the new state after time step dt
BinaryState rk4_step(
    const BinaryState& state,
    double dt,
    const DerivativeFunc& deriv
);

/// Compute an adaptive time step based on current orbital parameters
double adaptive_timestep(
    const BinaryState& state,
    const IntegratorConfig& config,
    double total_mass
);

/// Helper: add a scalar multiple of derivative to a state
BinaryState state_add(const BinaryState& s, const BinaryStateDerivative& d, double dt);

} // namespace bh

#endif // BH_COLLISION_INTEGRATOR_H
