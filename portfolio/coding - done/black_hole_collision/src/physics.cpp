/**
 * @file physics.cpp
 * @brief Implementation of post-Newtonian equations of motion and
 *        gravitational wave computation.
 *
 * The PN acceleration follows the ADM-TT gauge formulation from
 * Blanchet, Living Rev. Relativity 17 (2014) 2.
 *
 * In geometrized units (G = c = 1):
 *   - Newtonian: a_N = -M * n / r^2
 *   - 1PN:       O(v^2) corrections
 *   - 2PN:       O(v^4) corrections
 *   - 2.5PN:     O(v^5) radiation reaction (dissipative)
 *
 * All formulas are written for the relative coordinate r = x1 - x2.
 */

#include "bh_collision/physics.h"
#include <cmath>
#include <algorithm>

namespace bh {

// ============================================================================
// Post-Newtonian acceleration in the center-of-mass frame
// ============================================================================

AccelerationResult compute_relative_acceleration(
    const glm::dvec3& r,       // relative position r = x1 - x2
    const glm::dvec3& v,       // relative velocity v = v1 - v2
    double m1, double m2,
    bool enable_1pn,
    bool enable_2pn,
    bool enable_25pn)
{
    AccelerationResult result = {};

    double M = m1 + m2;
    double eta = m1 * m2 / (M * M);  // symmetric mass ratio
    double r2 = glm::dot(r, r);
    double r_mag = std::sqrt(r2);

    if (r_mag < 1e-10) {
        return result;  // Avoid singularity
    }

    double r3 = r2 * r_mag;
    glm::dvec3 n = r / r_mag;        // Unit vector r1->r2 direction

    double v2 = glm::dot(v, v);      // |v|^2
    double rdot = glm::dot(n, v);    // radial velocity (dr/dt)

    // ========================================================================
    // Newtonian acceleration
    // a_N = -M/r^2 * n
    // ========================================================================
    result.a_newtonian = -M / r2 * n;

    // ========================================================================
    // 1PN correction (first post-Newtonian order)
    // From Blanchet (2014), Eq. (203) / Maggiore (2007) Eq. (5.47)
    //
    // a_1PN = -M/r^2 * { n * [ -v^2 + 2(2+η)M/r + (3/2)η*rdot^2 ]
    //                   + v * [ 2(2-η)*rdot ] }
    // ========================================================================
    if (enable_1pn) {
        double Mr = M / r_mag;

        double n_coeff = -v2
                        + 2.0 * (2.0 + eta) * Mr
                        + 1.5 * eta * rdot * rdot;

        double v_coeff = 2.0 * (2.0 - eta) * rdot;

        result.a_1pn = -Mr / r_mag * (n_coeff * n + v_coeff * v);
    }

    // ========================================================================
    // 2PN correction (second post-Newtonian order)
    // From Blanchet (2014), Eq. (203)
    //
    // Includes terms proportional to v^4, M^2/r^2, cross terms.
    // This is the complete 2PN expression for non-spinning bodies.
    // ========================================================================
    if (enable_2pn) {
        double Mr = M / r_mag;
        double Mr2 = Mr * Mr;
        double rdot2 = rdot * rdot;
        double v4 = v2 * v2;

        // Coefficient of n (radial part)
        double n_coeff =
            -2.0 * (2.0 + 25.0 * eta + 2.0 * eta * eta) * Mr2
            + 1.5 * eta * (3.0 - 4.0 * eta) * v4
            + 0.5 * eta * (13.0 - 4.0 * eta) * Mr * v2
            - (2.0 + 15.0 * eta - 2.0 * eta * eta) * Mr * rdot2
            - 1.875 * eta * (1.0 - 3.0 * eta) * rdot2 * rdot2
            + 1.5 * eta * (3.0 - 4.0 * eta) * v2 * rdot2;

        // Coefficient of v (tangential part)
        double v_coeff =
            -0.5 * eta * (15.0 + 4.0 * eta) * v2 * rdot
            + (4.0 + 41.0 * eta / 4.0 + eta * eta) * Mr * rdot
            + 1.5 * eta * (3.0 + 2.0 * eta) * rdot * rdot2;

        result.a_2pn = -Mr / r_mag * (n_coeff * n + v_coeff * v);
    }

    // ========================================================================
    // 2.5PN radiation reaction (leading-order dissipation)
    // From Blanchet (2014), Eq. (204) / Iyer & Will (1995)
    //
    // This is the Burke-Thorne radiation reaction:
    // a_2.5PN = (8/5) * η * M^2/r^3 * { rdot * n * [18v^2 + (2/3)M/r - 25 rdot^2]
    //                                   - v * [6v^2 - 2M/r - 15 rdot^2] }
    // ========================================================================
    if (enable_25pn) {
        double Mr = M / r_mag;
        double Mr3 = M * M / (r2 * r_mag);
        double rdot2 = rdot * rdot;

        double prefactor = 8.0 / 5.0 * eta * M * Mr / r2;

        double n_coeff = rdot * (18.0 * v2 + (2.0 / 3.0) * Mr - 25.0 * rdot2);
        double v_coeff = -(6.0 * v2 - 2.0 * Mr - 15.0 * rdot2);

        result.a_25pn = prefactor * (n_coeff * n + v_coeff * v);
    }

    return result;
}

AccelerationResult compute_acceleration(
    const BlackHole& bh1, const BlackHole& bh2,
    bool enable_1pn, bool enable_2pn, bool enable_25pn)
{
    glm::dvec3 r = bh1.position - bh2.position;
    glm::dvec3 v = bh1.velocity - bh2.velocity;

    AccelerationResult rel_acc = compute_relative_acceleration(
        r, v, bh1.mass, bh2.mass,
        enable_1pn, enable_2pn, enable_25pn
    );

    // Convert relative acceleration to acceleration of body 1
    // In the two-body problem: a1 = (m2/M) * a_rel, a2 = -(m1/M) * a_rel
    double M = bh1.mass + bh2.mass;
    double factor = bh2.mass / M;

    AccelerationResult result;
    result.a_newtonian = factor * rel_acc.a_newtonian;
    result.a_1pn = factor * rel_acc.a_1pn;
    result.a_2pn = factor * rel_acc.a_2pn;
    result.a_25pn = factor * rel_acc.a_25pn;

    return result;
}

// ============================================================================
// Orbital parameters
// ============================================================================

OrbitalParams compute_orbital_params(
    const BlackHole& bh1, const BlackHole& bh2)
{
    OrbitalParams p = {};

    p.total_mass = bh1.mass + bh2.mass;
    p.reduced_mass = bh1.mass * bh2.mass / p.total_mass;
    p.symmetric_mass_ratio = p.reduced_mass / p.total_mass;
    p.chirp_mass = p.total_mass * std::pow(p.symmetric_mass_ratio, 0.6);

    glm::dvec3 r = bh1.position - bh2.position;
    glm::dvec3 v = bh1.velocity - bh2.velocity;

    p.separation = glm::length(r);
    if (p.separation < 1e-10) return p;

    glm::dvec3 n = r / p.separation;
    p.radial_velocity = glm::dot(v, n);

    // Angular momentum L = μ * r × v
    glm::dvec3 L = p.reduced_mass * glm::cross(r, v);
    p.angular_momentum = glm::length(L);

    // Orbital frequency from angular momentum: ω = L / (μ r²)
    p.orbital_frequency = p.angular_momentum / (p.reduced_mass * p.separation * p.separation);

    // Velocity parameter: v = (M ω)^(1/3)
    if (p.orbital_frequency > 0) {
        p.velocity_param = std::cbrt(p.total_mass * p.orbital_frequency);
    }

    // Orbital phase from position (in the orbital plane)
    p.orbital_phase = std::atan2(r.z, r.x);

    // Newtonian binding energy: E = -μM/(2r) + μv²/2
    double v2 = glm::dot(v, v);
    p.energy = 0.5 * p.reduced_mass * v2 - p.reduced_mass * p.total_mass / p.separation;

    return p;
}

// ============================================================================
// Gravitational wave strain (quadrupole formula)
// ============================================================================

GWStrain compute_gw_strain(
    const BlackHole& bh1, const BlackHole& bh2,
    double observer_distance,
    double observer_inclination)
{
    GWStrain gw = {};

    OrbitalParams p = compute_orbital_params(bh1, bh2);
    if (p.separation < 1e-10 || observer_distance < 1e-10) return gw;

    double mu = p.reduced_mass;
    double M = p.total_mass;
    double r = p.separation;
    double omega = p.orbital_frequency;
    double Phi = p.orbital_phase;
    double D = observer_distance;
    double iota = observer_inclination;

    // Quadrupole formula for circular-ish orbits:
    // h+ = -(2μ/D) * (M ω)^(2/3) * (1 + cos²ι)/2 * cos(2Φ)
    // h× = -(2μ/D) * (M ω)^(2/3) * cos(ι) * sin(2Φ)
    //
    // But (M ω)^(2/3) = (M/r) for Keplerian circular orbit,
    // so we use the more general velocity-based form:
    double v_param = std::cbrt(M * omega);
    double v2 = v_param * v_param;

    double prefactor = 2.0 * mu * v2 / D;

    double cos_iota = std::cos(iota);
    double cos2_iota = cos_iota * cos_iota;

    gw.h_plus = -prefactor * (1.0 + cos2_iota) / 2.0 * std::cos(2.0 * Phi);
    gw.h_cross = -prefactor * cos_iota * std::sin(2.0 * Phi);

    gw.amplitude = std::sqrt(gw.h_plus * gw.h_plus + gw.h_cross * gw.h_cross);
    gw.frequency = omega / M_PI;  // GW frequency = 2 * orbital frequency / (2π) = ω/π

    return gw;
}

// ============================================================================
// Energy and angular momentum loss rates
// ============================================================================

double energy_loss_rate(double eta, double total_mass, double separation) {
    if (separation < 1e-10) return 0.0;
    // Peters formula: dE/dt = -(32/5) * η² * M⁵ / r⁵
    double M = total_mass;
    double r5 = std::pow(separation, 5.0);
    return -(32.0 / 5.0) * eta * eta * std::pow(M, 5.0) / r5;
}

double angular_momentum_loss_rate(double eta, double total_mass, double separation) {
    if (separation < 1e-10) return 0.0;
    // dL/dt = -(32/5) * η² * M^(9/2) / r^(7/2)
    double M = total_mass;
    return -(32.0 / 5.0) * eta * eta *
           std::pow(M, 4.5) / std::pow(separation, 3.5);
}

double kepler_frequency(double total_mass, double separation) {
    if (separation < 1e-10) return 0.0;
    return std::sqrt(total_mass / (separation * separation * separation));
}

double time_to_merger_estimate(double eta, double total_mass, double separation) {
    // Leading-order Peters estimate:
    // T = (5/256) * r^4 / (η * M^3)
    return (5.0 / 256.0) * std::pow(separation, 4.0) /
           (eta * std::pow(total_mass, 3.0));
}

// BinaryConfig::to_string implementation
std::string BinaryConfig::to_string() const {
    char buf[512];
    std::snprintf(buf, sizeof(buf),
        "Binary Config:\n"
        "  m1 = %.4f, m2 = %.4f (q = %.2f)\n"
        "  chi1 = %.3f, chi2 = %.3f\n"
        "  separation = %.2f M\n"
        "  eccentricity = %.4f\n"
        "  inclination = %.4f rad\n"
        "  distance = %.2e M\n",
        m1, m2, m1 / m2,
        chi1, chi2,
        initial_separation,
        eccentricity,
        inclination,
        distance);
    return std::string(buf);
}

} // namespace bh
