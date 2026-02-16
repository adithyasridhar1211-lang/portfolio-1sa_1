/**
 * @file physics.h
 * @brief Post-Newtonian equations of motion and gravitational wave computation
 *        for binary black hole systems.
 *
 * Implements:
 *   - Newtonian + 1PN + 2PN conservative dynamics
 *   - 2.5PN radiation reaction (dissipative)
 *   - Quadrupole gravitational wave strain
 *   - Energy/angular momentum loss rates
 *
 * References:
 *   - Blanchet, "Gravitational Radiation from Post-Newtonian Sources",
 *     Living Rev. Relativity 17 (2014) 2
 *   - Peters & Mathews, Phys. Rev. 131 (1963) 435
 *   - Maggiore, "Gravitational Waves" (Oxford, 2007)
 */

#ifndef BH_COLLISION_PHYSICS_H
#define BH_COLLISION_PHYSICS_H

#include "black_hole.h"
#include <glm/glm.hpp>

namespace bh {

/// Result of computing acceleration on a body
struct AccelerationResult {
    glm::dvec3 a_newtonian;
    glm::dvec3 a_1pn;
    glm::dvec3 a_2pn;
    glm::dvec3 a_25pn;

    glm::dvec3 total() const {
        return a_newtonian + a_1pn + a_2pn + a_25pn;
    }
};

/// Gravitational wave strain at a given instant
struct GWStrain {
    double h_plus;      // Plus polarization
    double h_cross;     // Cross polarization
    double amplitude;   // sqrt(h+^2 + hx^2)
    double frequency;   // Instantaneous GW frequency (twice orbital)
};

/// Orbital parameters derived from the binary state
struct OrbitalParams {
    double separation;          // |r1 - r2|
    double orbital_frequency;   // ω
    double orbital_phase;       // Φ
    double radial_velocity;     // dr/dt
    double velocity_param;      // v/c = (Mω)^(1/3)
    double reduced_mass;        // μ = m1*m2/(m1+m2)
    double total_mass;          // M = m1 + m2
    double symmetric_mass_ratio;// η = μ/M = m1*m2/M^2
    double chirp_mass;          // M_c = M * η^(3/5)
    double energy;              // Binding energy
    double angular_momentum;    // Orbital angular momentum magnitude
};

/// Compute the PN acceleration of body 1 due to body 2
AccelerationResult compute_acceleration(
    const BlackHole& bh1, const BlackHole& bh2,
    bool enable_1pn = true,
    bool enable_2pn = true,
    bool enable_25pn = true
);

/// Compute the relative acceleration for the binary system
/// Returns acceleration of the relative coordinate r = r1 - r2
AccelerationResult compute_relative_acceleration(
    const glm::dvec3& rel_pos,
    const glm::dvec3& rel_vel,
    double m1, double m2,
    bool enable_1pn = true,
    bool enable_2pn = true,
    bool enable_25pn = true
);

/// Compute orbital parameters from the binary state
OrbitalParams compute_orbital_params(
    const BlackHole& bh1, const BlackHole& bh2
);

/// Compute gravitational wave strain (quadrupole approximation)
GWStrain compute_gw_strain(
    const BlackHole& bh1, const BlackHole& bh2,
    double observer_distance,
    double observer_inclination
);

/// Energy loss rate due to gravitational radiation (Peters formula)
/// dE/dt = -(32/5) * η² * M⁵ / r⁵ (leading order)
double energy_loss_rate(double eta, double total_mass, double separation);

/// Angular momentum loss rate
/// dL/dt = -(32/5) * η² * M^(9/2) / r^(7/2) (leading order)
double angular_momentum_loss_rate(double eta, double total_mass, double separation);

/// Compute the orbital frequency for a circular orbit at given separation
/// ω = sqrt(M/r³) (Kepler's law in geometrized units)
double kepler_frequency(double total_mass, double separation);

/// Compute time to merger from given separation (leading-order Peters formula)
/// T = (5/256) * r⁴ / (η * M³)
double time_to_merger_estimate(double eta, double total_mass, double separation);

} // namespace bh

#endif // BH_COLLISION_PHYSICS_H
