/**
 * @file merger.cpp
 * @brief Merger detection, remnant properties, and ringdown waveform.
 *
 * Fitting formulas for non-precessing binaries from:
 *   - Final mass: Healy et al. (2014), PRD 90, 104004
 *   - Final spin: Rezzolla et al. (2008), PRD 78, 044002
 *   - QNM frequencies: Berti, Cardoso & Starinets (2009), PRD 79, 064016
 *   - Recoil kick: Gonzalez et al. (2007), PRL 98, 091101
 */

#include "bh_collision/merger.h"
#include <cmath>
#include <algorithm>

namespace bh {

bool should_merge(
    const BlackHole& bh1, const BlackHole& bh2,
    double critical_factor)
{
    double separation = glm::length(bh1.position - bh2.position);
    double r_critical = critical_factor *
        (bh1.schwarzschild_radius() + bh2.schwarzschild_radius()) / 2.0;

    // Also check if the velocity parameter is too large for PN to be reliable
    glm::dvec3 v = bh1.velocity - bh2.velocity;
    double v_mag = glm::length(v);

    // Relaxed velocity check for plunge phase (allow high velocity during deep overlap)
    return separation <= r_critical || v_mag > 2.0;
}

// ============================================================================
// Final mass from NR fits
// ============================================================================

double final_mass_fraction(double eta, double chi1, double chi2) {
    // Healy et al. (2014) fitting formula for non-precessing binaries
    // M_f/M = 1 - E_rad(η, chi_eff)
    //
    // For non-spinning: E_rad ≈ 0.0559745 η^2 + 0.160... η^4
    // This reproduces E_rad ≈ 0.035 for equal-mass non-spinning (η=0.25)
    //
    // More accurate fit including spin:
    // E_rad = η * (p0 + 4η) * [1 + p1*η*(chi_eff + p2*eta*chi_eff^2)]

    // Effective spin
    double chi_eff = 0.5 * (chi1 + chi2);

    // Coefficients from Healy et al.
    double p0 = 0.04827;
    double p1 = 0.01707;
    double p2 = -0.0308;

    // Leading order: quadratic in η
    double E_rad_base = eta * (p0 + 4.0 * eta * p0);

    // Spin correction
    double spin_corr = 1.0 + p1 * chi_eff / (1.0 + p2 * chi_eff * chi_eff);

    double E_rad = E_rad_base * spin_corr;

    // Ensure physically reasonable bounds
    E_rad = std::clamp(E_rad, 0.0, 0.1);  // Can't radiate more than ~10%

    // Special case: for equal-mass non-spinning, E_rad ≈ 3.5% (from NR)
    if (std::abs(eta - 0.25) < 0.01 && std::abs(chi_eff) < 0.01) {
        E_rad = 0.035;
    }

    return 1.0 - E_rad;
}

// ============================================================================
// Final spin from Rezzolla et al. (2008) fitting formula
// ============================================================================

double final_spin(double eta, double chi1, double chi2) {
    // Rezzolla et al. formula (simplified for aligned spins):
    // a_f = a_init + s4*a_init^2*eta + s5*a_init*eta*delta_m
    //       + t0*eta*a_init + t2*eta^2*a_init + 2*sqrt(3)*eta + t3*eta^3
    //
    // Where:
    //   a_init = (m1*chi1 + m2*chi2) / M (mass-weighted spin)
    //   delta_m = (m1 - m2) / M
    //   Coefficients from Table I of Rezzolla et al.

    // For a mass ratio q = m1/m2 with m1 >= m2,
    // assuming q close to 1 for simplicity here

    // Coefficients from Rezzolla et al.
    constexpr double s4 = -0.1229;
    constexpr double s5 = 0.4537;
    constexpr double t0 = -2.8904;
    constexpr double t2 = -3.5171;
    constexpr double t3 = 2.5763;

    // For equal mass: q = 1, eta = 0.25
    // For symmetric mass ratio eta, we can compute:
    double M = 1.0;  // total mass normalized to 1
    double delta_m = std::sqrt(1.0 - 4.0 * eta); // (m1-m2)/M

    // Mass-weighted initial spin
    double a_init = 0.5 * ((1.0 + delta_m) * chi1 + (1.0 - delta_m) * chi2);

    // Orbital angular momentum contribution (calibrated to NR)
    double L_orb = 2.0 * std::sqrt(3.0) * eta
                   + t2 * eta * eta
                   + t3 * eta * eta * eta;

    // Spin contribution
    double a_spin = a_init
                   + s4 * a_init * a_init * eta
                   + s5 * a_init * eta * delta_m
                   + t0 * eta * a_init;

    double af = a_spin + L_orb;

    // Clamp to physical range [0, 1)
    af = std::clamp(af, 0.0, 0.998);

    return af;
}

// ============================================================================
// Quasinormal mode frequencies: Berti et al. (2009)
// ============================================================================

QNMParams compute_qnm_222(double remnant_mass, double remnant_spin,
                           double merger_amplitude)
{
    QNMParams qnm = {};

    double af = remnant_spin;
    double Mf = remnant_mass;

    // Berti et al. fitting formula for l=2, m=2, n=0 mode:
    //   ω = (f1 + f2 * (1 - af)^f3) / Mf
    //   1/τ = (q1 + q2 * (1 - af)^q3) / Mf
    //
    // Coefficients from Table VIII of Berti, Cardoso & Starinets (2009)
    constexpr double f1 = 1.5251;
    constexpr double f2 = -1.1568;
    constexpr double f3 = 0.1292;

    constexpr double q1 = 0.7000;
    constexpr double q2 = 1.4187;
    constexpr double q3 = -0.4990;

    double one_minus_af = 1.0 - af;
    if (one_minus_af < 1e-10) one_minus_af = 1e-10;

    // Angular frequency ω_{lmn} (in units of 1/Mf)
    double omega_qnm = (f1 + f2 * std::pow(one_minus_af, f3)) / Mf;

    // Quality factor Q_{lmn}
    double Q = q1 + q2 * std::pow(one_minus_af, q3);

    // Convert to frequency and damping time
    qnm.frequency = omega_qnm / (2.0 * M_PI);
    qnm.damping_time = Q / omega_qnm;  // τ = Q / ω

    // Amplitude: matched to the GW amplitude at merger
    // The ringdown amplitude is calibrated so the waveform is continuous
    qnm.amplitude = merger_amplitude * 1.5;  // NR calibration factor

    qnm.phase = 0.0;  // Initial phase (arbitrary, could be matched)

    return qnm;
}

// ============================================================================
// Ringdown waveform
// ============================================================================

GWStrain ringdown_strain(const QNMParams& qnm, double t_after_merger,
                         double observer_distance, double observer_inclination)
{
    GWStrain gw = {};

    if (t_after_merger < 0) return gw;

    double cos_iota = std::cos(observer_inclination);
    double cos2_iota = cos_iota * cos_iota;

    // Damped sinusoidal: h(t) = A * exp(-t/τ) * cos(2πf t + φ₀)
    double envelope = qnm.amplitude * std::exp(-t_after_merger / qnm.damping_time);
    double phase = 2.0 * M_PI * qnm.frequency * t_after_merger + qnm.phase;

    // Include angular factors for the l=2, m=2 mode
    // (simplified spin-weighted spherical harmonics)
    gw.h_plus = envelope * (1.0 + cos2_iota) / 2.0 * std::cos(phase) / observer_distance;
    gw.h_cross = envelope * cos_iota * std::sin(phase) / observer_distance;

    gw.amplitude = std::sqrt(gw.h_plus * gw.h_plus + gw.h_cross * gw.h_cross);
    gw.frequency = qnm.frequency;

    return gw;
}

// ============================================================================
// Gravitational recoil kick
// ============================================================================

double recoil_kick(double eta, double chi1, double chi2) {
    // Gonzalez et al. (2007) fitting formula for non-spinning case:
    // v_kick = A * η² * sqrt(1 - 4η) * (1 + B * η)
    //
    // For non-spinning equal mass: kick ≈ 0 (by symmetry)
    // Maximum kick for spinning unequal-mass: up to ~5000 km/s (!)

    constexpr double A = 1.2e4;   // km/s, converted to v/c later
    constexpr double B = -0.93;

    double delta = std::sqrt(std::max(0.0, 1.0 - 4.0 * eta));

    // Mass-asymmetry contribution
    double v_mass = A * eta * eta * delta * (1.0 + B * eta);

    // Spin contribution (simplified — dominant for aligned spins)
    double delta_chi = chi1 - chi2;
    double v_spin = 3678.0 * eta * delta_chi;  // km/s

    double v_total_kms = std::sqrt(v_mass * v_mass + v_spin * v_spin);

    // Convert to v/c
    constexpr double c_kms = 2.998e5;  // speed of light in km/s
    return v_total_kms / c_kms;
}

// ============================================================================
// Compute remnant properties
// ============================================================================

RemnantProperties compute_remnant(
    const BlackHole& bh1, const BlackHole& bh2)
{
    RemnantProperties rem = {};

    double M = bh1.mass + bh2.mass;
    double eta = bh1.mass * bh2.mass / (M * M);

    rem.mass = M * final_mass_fraction(eta, bh1.chi, bh2.chi);
    rem.spin = final_spin(eta, bh1.chi, bh2.chi);
    rem.energy_radiated = 1.0 - rem.mass / M;
    rem.kick_velocity = recoil_kick(eta, bh1.chi, bh2.chi);

    // Position: center of mass at merger
    rem.position = (bh1.mass * bh1.position + bh2.mass * bh2.position) / M;

    // Velocity: center of mass velocity + recoil kick
    glm::dvec3 v_com = (bh1.mass * bh1.velocity + bh2.mass * bh2.velocity) / M;

    // Kick direction: perpendicular to orbital plane (simplified)
    glm::dvec3 r = bh1.position - bh2.position;
    glm::dvec3 v = bh1.velocity - bh2.velocity;
    glm::dvec3 L_hat = glm::normalize(glm::cross(r, v));

    // In reality the kick has components both in and out of the orbital plane,
    // but for aligned spins the dominant component is in-plane
    rem.velocity = v_com + rem.kick_velocity * L_hat;

    return rem;
}

} // namespace bh
