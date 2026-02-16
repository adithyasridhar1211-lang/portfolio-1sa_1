/**
 * @file test_physics.cpp
 * @brief Unit tests for the binary black hole collision physics.
 *
 * Tests verify:
 *   1. Newtonian circular orbit stability (energy conservation)
 *   2. Orbital decay with 2.5PN radiation reaction
 *   3. GW frequency chirp behavior
 *   4. Merger detection
 *   5. Remnant properties (equal-mass non-spinning)
 *   6. QNM ringdown damping
 */

#include "bh_collision/physics.h"
#include "bh_collision/integrator.h"
#include "bh_collision/merger.h"
#include "bh_collision/simulation.h"

#include <cstdio>
#include <cmath>
#include <cassert>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("  TEST: %-50s ", name); \
    fflush(stdout);

#define PASS() \
    printf("[PASS]\n"); \
    tests_passed++;

#define FAIL(msg) \
    printf("[FAIL] %s\n", msg); \
    tests_failed++;

#define ASSERT_CLOSE(a, b, tol, msg) \
    if (std::abs((a) - (b)) > (tol)) { \
        printf("[FAIL] %s: expected %.8e, got %.8e (tol %.2e)\n", msg, (double)(b), (double)(a), (double)(tol)); \
        tests_failed++; \
        return; \
    }

#define ASSERT_TRUE(cond, msg) \
    if (!(cond)) { \
        FAIL(msg); \
        return; \
    }

// ============================================================================
// Test 1: Newtonian orbit stability
// ============================================================================
void test_newtonian_orbit() {
    TEST("Newtonian circular orbit energy conservation");

    // Set up a circular orbit with PN corrections disabled
    bh::BlackHole bh1, bh2;
    bh1.mass = 0.5; bh1.chi = 0.0;
    bh2.mass = 0.5; bh2.chi = 0.0;

    double r0 = 20.0;
    bh1.position = glm::dvec3(10.0, 0.0, 0.0);
    bh2.position = glm::dvec3(-10.0, 0.0, 0.0);

    double v_circ = std::sqrt(1.0 / r0);  // M=1
    bh1.velocity = glm::dvec3(0.0, 0.0, v_circ * 0.5);
    bh2.velocity = glm::dvec3(0.0, 0.0, -v_circ * 0.5);

    // Initial energy
    bh::OrbitalParams p0 = bh::compute_orbital_params(bh1, bh2);
    double E0 = p0.energy;

    // Integrate for one full orbit (Newtonian only, no PN)
    bh::BinaryState state;
    state.pos1 = bh1.position;
    state.vel1 = bh1.velocity;
    state.pos2 = bh2.position;
    state.vel2 = bh2.velocity;
    state.time = 0.0;

    auto deriv = [](const bh::BinaryState& s) -> bh::BinaryStateDerivative {
        glm::dvec3 r = s.pos1 - s.pos2;
        glm::dvec3 v = s.vel1 - s.vel2;
        bh::AccelerationResult acc = bh::compute_relative_acceleration(
            r, v, 0.5, 0.5, false, false, false  // Newtonian only
        );
        glm::dvec3 a_rel = acc.total();
        bh::BinaryStateDerivative d;
        d.dpos1 = s.vel1;
        d.dvel1 = 0.5 * a_rel;
        d.dpos2 = s.vel2;
        d.dvel2 = -0.5 * a_rel;
        return d;
    };

    double orbital_period = 2.0 * M_PI * std::sqrt(r0 * r0 * r0);
    double dt = 0.01;
    int steps = (int)(orbital_period / dt);

    for (int i = 0; i < steps; i++) {
        state = bh::rk4_step(state, dt, deriv);
    }

    // Check energy conservation
    bh1.position = state.pos1; bh1.velocity = state.vel1;
    bh2.position = state.pos2; bh2.velocity = state.vel2;
    bh::OrbitalParams pf = bh::compute_orbital_params(bh1, bh2);

    double dE = std::abs(pf.energy - E0) / std::abs(E0);
    ASSERT_TRUE(dE < 1e-6, "Energy conservation violated");
    PASS();
}

// ============================================================================
// Test 2: Orbital decay with radiation reaction
// ============================================================================
void test_orbital_decay() {
    TEST("2.5PN radiation reaction causes orbital decay");

    bh::BlackHole bh1, bh2;
    bh1.mass = 0.5; bh1.chi = 0.0;
    bh2.mass = 0.5; bh2.chi = 0.0;

    double r0 = 20.0;
    bh1.position = glm::dvec3(10.0, 0.0, 0.0);
    bh2.position = glm::dvec3(-10.0, 0.0, 0.0);

    double v_circ = std::sqrt(1.0 / r0);
    bh1.velocity = glm::dvec3(0.0, 0.0, v_circ * 0.5);
    bh2.velocity = glm::dvec3(0.0, 0.0, -v_circ * 0.5);

    double sep_initial = glm::length(bh1.position - bh2.position);

    // Integrate with full PN
    bh::BinaryState state;
    state.pos1 = bh1.position; state.vel1 = bh1.velocity;
    state.pos2 = bh2.position; state.vel2 = bh2.velocity;

    auto deriv = [](const bh::BinaryState& s) -> bh::BinaryStateDerivative {
        glm::dvec3 r = s.pos1 - s.pos2;
        glm::dvec3 v = s.vel1 - s.vel2;
        bh::AccelerationResult acc = bh::compute_relative_acceleration(
            r, v, 0.5, 0.5, true, true, true  // Full PN
        );
        glm::dvec3 a_rel = acc.total();
        bh::BinaryStateDerivative d;
        d.dpos1 = s.vel1;
        d.dvel1 = 0.5 * a_rel;
        d.dpos2 = s.vel2;
        d.dvel2 = -0.5 * a_rel;
        return d;
    };

    double dt = 0.05;
    for (int i = 0; i < 100000; i++) {
        state = bh::rk4_step(state, dt, deriv);
    }

    double sep_final = glm::length(state.pos1 - state.pos2);
    ASSERT_TRUE(sep_final < sep_initial, "Separation should decrease with radiation reaction");
    PASS();
}

// ============================================================================
// Test 3: GW frequency increases (chirp)
// ============================================================================
void test_gw_chirp() {
    TEST("GW frequency increases during inspiral (chirp)");

    // Use the full simulation engine which handles adaptive stepping
    bh::SimulationConfig config;
    config.binary.m1 = 0.5;
    config.binary.m2 = 0.5;
    config.binary.initial_separation = 15.0;
    config.binary.eccentricity = 0.0;
    config.record_interval = 100.0;  // Record every 100 M
    config.observer_distance = 1e6;
    config.observer_inclination = 0.0;

    bh::SimulationResult result = bh::run_simulation(config);

    // Need at least a few inspiral frames
    ASSERT_TRUE(result.num_inspiral_frames >= 3, "Need enough inspiral frames");

    // Compare orbital frequency at early vs late inspiral
    const auto& early = result.frames[1];  // skip frame 0 (might be initial)
    const auto& late = result.frames[result.num_inspiral_frames - 2]; // near merger

    ASSERT_TRUE(late.orbital.orbital_frequency > early.orbital.orbital_frequency,
                "Orbital frequency should increase as orbit shrinks");
    PASS();
}

// ============================================================================
// Test 4: Merger detection
// ============================================================================
void test_merger_detection() {
    TEST("Merger detected at close separation");

    bh::BlackHole bh1, bh2;
    bh1.mass = 0.5; bh2.mass = 0.5;
    bh1.chi = 0.0; bh2.chi = 0.0;

    // Far apart — no merger
    bh1.position = glm::dvec3(10.0, 0.0, 0.0);
    bh2.position = glm::dvec3(-10.0, 0.0, 0.0);
    bh1.velocity = bh2.velocity = glm::dvec3(0);
    ASSERT_TRUE(!bh::should_merge(bh1, bh2), "Should NOT merge at r=20");

    // Close together — merger
    bh1.position = glm::dvec3(1.5, 0.0, 0.0);
    bh2.position = glm::dvec3(-1.5, 0.0, 0.0);
    ASSERT_TRUE(bh::should_merge(bh1, bh2), "Should merge at r=3");
    PASS();
}

// ============================================================================
// Test 5: Remnant properties (equal-mass non-spinning)
// ============================================================================
void test_remnant_properties() {
    TEST("Equal-mass non-spinning remnant: ~3.5%% mass loss, spin ~0.69");

    double eta = 0.25;  // equal mass

    double mf = bh::final_mass_fraction(eta, 0.0, 0.0);
    double af = bh::final_spin(eta, 0.0, 0.0);

    // NR result: E_rad ≈ 3.5%, a_f ≈ 0.69
    ASSERT_CLOSE(mf, 0.965, 0.01,
        "Final mass fraction");  // 1 - 0.035 = 0.965

    ASSERT_CLOSE(af, 0.69, 0.05,
        "Final spin");

    PASS();
}

// ============================================================================
// Test 6: QNM ringdown damping
// ============================================================================
void test_ringdown_damping() {
    TEST("Ringdown amplitude decays exponentially");

    bh::QNMParams qnm = bh::compute_qnm_222(0.965, 0.69, 1.0);

    bh::GWStrain gw0 = bh::ringdown_strain(qnm, 0.0, 1e6, 0.0);
    bh::GWStrain gw1 = bh::ringdown_strain(qnm, qnm.damping_time, 1e6, 0.0);
    bh::GWStrain gw2 = bh::ringdown_strain(qnm, 3.0 * qnm.damping_time, 1e6, 0.0);

    ASSERT_TRUE(gw0.amplitude > 0, "Initial amplitude should be positive");
    ASSERT_TRUE(gw1.amplitude < gw0.amplitude,
                "Amplitude should decrease after one damping time");
    ASSERT_TRUE(gw2.amplitude < gw1.amplitude,
                "Amplitude should continue decreasing");

    // After one damping time, amplitude should be ~1/e of initial
    double ratio = gw1.amplitude / gw0.amplitude;
    ASSERT_CLOSE(ratio, 1.0 / M_E, 0.1, "Damping ratio at t=tau");

    PASS();
}

// ============================================================================
// Test 7: Kepler frequency
// ============================================================================
void test_kepler_frequency() {
    TEST("Kepler frequency ω = sqrt(M/r³)");

    double M = 1.0;
    double r = 10.0;
    double omega = bh::kepler_frequency(M, r);
    double expected = std::sqrt(M / (r * r * r));

    ASSERT_CLOSE(omega, expected, 1e-12, "Kepler frequency");
    PASS();
}

// ============================================================================
// Test 8: Energy loss rate sign
// ============================================================================
void test_energy_loss_sign() {
    TEST("Energy loss rate is negative (energy radiated away)");

    double eta = 0.25;
    double M = 1.0;
    double r = 10.0;

    double dEdt = bh::energy_loss_rate(eta, M, r);
    ASSERT_TRUE(dEdt < 0, "dE/dt should be negative");
    PASS();
}

// ============================================================================
// Test 9: Time to merger estimate
// ============================================================================
void test_merger_time_estimate() {
    TEST("Peters merger time estimate is positive and finite");

    double eta = 0.25;
    double M = 1.0;
    double r = 20.0;

    double t = bh::time_to_merger_estimate(eta, M, r);
    ASSERT_TRUE(t > 0, "Merger time should be positive");
    ASSERT_TRUE(std::isfinite(t), "Merger time should be finite");

    // For r=20, eta=0.25, M=1: T = (5/256) * 20^4 / (0.25 * 1) = 12500
    double expected = (5.0 / 256.0) * std::pow(20.0, 4.0) / (0.25);
    ASSERT_CLOSE(t, expected, 1.0, "Peters estimate value");
    PASS();
}

// ============================================================================
// Test 10: Gravitational recoil
// ============================================================================
void test_recoil_kick() {
    TEST("Equal-mass non-spinning recoil is near zero");

    double kick = bh::recoil_kick(0.25, 0.0, 0.0);  // equal mass, no spin
    ASSERT_TRUE(kick < 0.001, "Equal-mass kick should be ~0");

    // Unequal mass should have non-zero kick
    double kick_unequal = bh::recoil_kick(0.2222, 0.0, 0.0);  // q=4:1 approx
    // (this isn't a strict test, just checking it's non-zero and reasonable)
    ASSERT_TRUE(kick_unequal >= 0, "Kick should be non-negative");
    PASS();
}

// ============================================================================
// Main
// ============================================================================

int main() {
    printf("\n================================================================\n");
    printf("  BLACK HOLE COLLISION PHYSICS - UNIT TESTS\n");
    printf("================================================================\n\n");

    test_newtonian_orbit();
    test_orbital_decay();
    test_gw_chirp();
    test_merger_detection();
    test_remnant_properties();
    test_ringdown_damping();
    test_kepler_frequency();
    test_energy_loss_sign();
    test_merger_time_estimate();
    test_recoil_kick();

    printf("\n================================================================\n");
    printf("  Results: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("================================================================\n\n");

    return tests_failed > 0 ? 1 : 0;
}
