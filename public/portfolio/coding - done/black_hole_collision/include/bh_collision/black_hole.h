/**
 * @file black_hole.h
 * @brief Black hole state representation for collision simulation.
 *
 * Uses geometrized units (G = c = 1). Mass is in units of total system mass M,
 * distances in M, time in M.
 */

#ifndef BH_COLLISION_BLACK_HOLE_H
#define BH_COLLISION_BLACK_HOLE_H

#include <glm/glm.hpp>
#include <cmath>
#include <string>

namespace bh {

struct BlackHole {
    double mass;            // Mass in geometrized units
    double chi;             // Dimensionless spin parameter [0, 1)
    glm::dvec3 position;   // Position in geometrized units
    glm::dvec3 velocity;   // Velocity (v/c, dimensionless)
    glm::dvec3 spin_axis;  // Unit vector along spin axis

    /// Schwarzschild radius r_s = 2m
    double schwarzschild_radius() const {
        return 2.0 * mass;
    }

    /// Innermost stable circular orbit radius
    /// For Schwarzschild (chi=0): r_ISCO = 6m
    /// For Kerr: depends on spin, prograde orbit
    double isco_radius() const {
        if (chi < 1e-10) return 6.0 * mass;
        // Bardeen, Press, Teukolsky (1972) formula
        double a = chi;
        double Z1 = 1.0 + std::cbrt(1.0 - a * a) *
                     (std::cbrt(1.0 + a) + std::cbrt(1.0 - a));
        double Z2 = std::sqrt(3.0 * a * a + Z1 * Z1);
        // Prograde orbit
        return mass * (3.0 + Z2 - std::sqrt((3.0 - Z1) * (3.0 + Z1 + 2.0 * Z2)));
    }

    /// Gravitational radius r_g = m (in geometrized units)
    double gravitational_radius() const {
        return mass;
    }
};

/// Initial conditions for a binary system
struct BinaryConfig {
    double m1 = 0.5;               // Mass of BH 1 (fraction of total mass)
    double m2 = 0.5;               // Mass of BH 2
    double chi1 = 0.0;             // Spin of BH 1
    double chi2 = 0.0;             // Spin of BH 2
    glm::dvec3 spin_axis1 = {0, 1, 0};
    glm::dvec3 spin_axis2 = {0, 1, 0};
    double initial_separation = 20.0;  // Initial separation in units of M
    double eccentricity = 0.0;         // Orbital eccentricity [0, 1)
    double inclination = 0.0;          // Observer inclination angle (radians)
    double distance = 1e6;             // Distance to observer in units of M

    std::string to_string() const;
};

/// Conversion factors from geometrized units to SI
struct UnitConversion {
    double total_mass_kg;      // Total system mass in kg
    double length_m;           // M in meters = G*M_total/c^2
    double time_s;             // M in seconds = G*M_total/c^3

    static UnitConversion from_solar_masses(double solar_masses) {
        UnitConversion u;
        constexpr double M_sun = 1.989e30;    // kg
        constexpr double G = 6.674e-11;       // m^3 kg^-1 s^-2
        constexpr double c = 2.998e8;         // m/s
        u.total_mass_kg = solar_masses * M_sun;
        u.length_m = G * u.total_mass_kg / (c * c);
        u.time_s = G * u.total_mass_kg / (c * c * c);
        return u;
    }
};

} // namespace bh

#endif // BH_COLLISION_BLACK_HOLE_H
