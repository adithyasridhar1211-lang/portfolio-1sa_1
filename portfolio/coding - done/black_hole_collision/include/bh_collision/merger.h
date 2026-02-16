/**
 * @file merger.h
 * @brief Merger detection, remnant properties, and ringdown modeling.
 *
 * Uses fitting formulas from numerical relativity:
 *   - Final mass: Healy et al. (2014), arXiv:1406.7295
 *   - Final spin: Rezzolla et al. (2008), arXiv:0708.3999
 *   - QNM frequencies: Berti, Cardoso & Starinets (2009), arXiv:0905.2975
 */

#ifndef BH_COLLISION_MERGER_H
#define BH_COLLISION_MERGER_H

#include "black_hole.h"
#include "physics.h"
#include <glm/glm.hpp>

namespace bh {

/// Properties of the final merged remnant black hole
struct RemnantProperties {
    double mass;            // Final mass (< m1 + m2 due to GW emission)
    double spin;            // Final dimensionless spin
    glm::dvec3 position;   // Position at merger
    glm::dvec3 velocity;   // Velocity (including recoil kick)
    double kick_velocity;   // Magnitude of gravitational recoil (v/c)
    double energy_radiated; // Fraction of total mass radiated as GWs
};

/// Quasinormal mode parameters for ringdown
struct QNMParams {
    double frequency;       // Oscillation frequency (in units of 1/M_f)
    double damping_time;    // e-folding damping time (in units of M_f)
    double amplitude;       // Initial amplitude
    double phase;           // Initial phase
};

/// Check if merger condition is met
/// Merger when separation <= critical_separation or velocity parameter > 0.4
bool should_merge(
    const BlackHole& bh1, const BlackHole& bh2,
    double critical_factor = 0.5  // merger at r < 0.5 * (r_s1 + r_s2)/2 (deep overlap)
);

/// Compute the final remnant properties after merger (NR fits)
RemnantProperties compute_remnant(
    const BlackHole& bh1, const BlackHole& bh2
);

/// Compute final mass fraction for non-spinning equal-mass merger
/// M_f/M = 1 - E_rad, where E_rad ≈ 0.0547η² + ... (NR fits)
double final_mass_fraction(double eta, double chi1 = 0.0, double chi2 = 0.0);

/// Compute final spin using Rezzolla et al. fitting formula
double final_spin(double eta, double chi1 = 0.0, double chi2 = 0.0);

/// Compute QNM frequency for the l=2, m=2 fundamental mode
/// Uses Berti et al. fitting formulas
QNMParams compute_qnm_222(double remnant_mass, double remnant_spin,
                           double merger_amplitude);

/// Compute ringdown waveform at time t after merger
/// Returns h+ and h× strain
GWStrain ringdown_strain(const QNMParams& qnm, double t_after_merger,
                         double observer_distance, double observer_inclination);

/// Estimate gravitational recoil kick velocity (v/c)
/// Gonzalez et al. (2007) and Lousto & Zlochower (2008) fits
double recoil_kick(double eta, double chi1 = 0.0, double chi2 = 0.0);

} // namespace bh

#endif // BH_COLLISION_MERGER_H
