/**
 * @file integrator.cpp
 * @brief RK4 integrator implementation with adaptive time stepping.
 */

#include "bh_collision/integrator.h"
#include <cmath>
#include <algorithm>

namespace bh {

// Helper: apply a state derivative scaled by dt
BinaryState state_add(const BinaryState& s, const BinaryStateDerivative& d, double dt) {
    BinaryState result;
    result.pos1 = s.pos1 + d.dpos1 * dt;
    result.vel1 = s.vel1 + d.dvel1 * dt;
    result.pos2 = s.pos2 + d.dpos2 * dt;
    result.vel2 = s.vel2 + d.dvel2 * dt;
    result.time = s.time + dt;
    return result;
}

BinaryState rk4_step(
    const BinaryState& state,
    double dt,
    const DerivativeFunc& deriv)
{
    // Classic 4th-order Runge-Kutta
    BinaryStateDerivative k1 = deriv(state);
    BinaryStateDerivative k2 = deriv(state_add(state, k1, dt * 0.5));
    BinaryStateDerivative k3 = deriv(state_add(state, k2, dt * 0.5));
    BinaryStateDerivative k4 = deriv(state_add(state, k3, dt));

    BinaryState result;
    result.time = state.time + dt;

    // Weighted sum: y_{n+1} = y_n + (dt/6)(k1 + 2k2 + 2k3 + k4)
    result.pos1 = state.pos1 + (dt / 6.0) * (k1.dpos1 + 2.0 * k2.dpos1 + 2.0 * k3.dpos1 + k4.dpos1);
    result.vel1 = state.vel1 + (dt / 6.0) * (k1.dvel1 + 2.0 * k2.dvel1 + 2.0 * k3.dvel1 + k4.dvel1);
    result.pos2 = state.pos2 + (dt / 6.0) * (k1.dpos2 + 2.0 * k2.dpos2 + 2.0 * k3.dpos2 + k4.dpos2);
    result.vel2 = state.vel2 + (dt / 6.0) * (k1.dvel2 + 2.0 * k2.dvel2 + 2.0 * k3.dvel2 + k4.dvel2);

    return result;
}

double adaptive_timestep(
    const BinaryState& state,
    const IntegratorConfig& config,
    double total_mass)
{
    if (!config.adaptive) {
        return config.dt_initial;
    }

    // Compute separation
    glm::dvec3 r = state.pos1 - state.pos2;
    double separation = glm::length(r);

    if (separation < 1e-10) {
        return config.dt_min;
    }

    // Orbital period estimate: T = 2π / ω = 2π sqrt(r³/M)
    double orbital_period = 2.0 * M_PI * std::sqrt(
        separation * separation * separation / total_mass
    );

    // Time step = safety_factor * orbital_period
    // This ensures we take enough steps per orbit for accuracy
    double dt = config.safety_factor * orbital_period;

    // As BHs get very close, use even smaller steps
    // Scale additionally by (r / r_isco) when close
    double r_isco = 6.0 * total_mass;  // Schwarzschild ISCO for total mass
    if (separation < 2.0 * r_isco) {
        double scale = separation / (2.0 * r_isco);
        dt *= scale * scale;
    }

    // Clamp to allowed range
    dt = std::clamp(dt, config.dt_min, config.dt_max);

    return dt;
}

} // namespace bh
