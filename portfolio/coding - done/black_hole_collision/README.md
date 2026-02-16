# Binary Black Hole Collision Simulator

A physically accurate simulation of binary black hole mergers using post-Newtonian equations of motion (up to 2.5PN order), with gravitational wave computation, merger physics from numerical relativity fits, and quasinormal mode ringdown modeling.

## Physics

### Inspiral Phase
- **Post-Newtonian dynamics**: Equations of motion include Newtonian gravity plus 1PN, 2PN (conservative), and 2.5PN (radiation reaction) corrections from [Blanchet, Living Rev. Relativity 17 (2014) 2](https://doi.org/10.12942/lrr-2014-2)
- **Gravitational waves**: Quadrupole-formula strain (h+, h×) with proper angular dependence
- **Energy loss**: Peters formula for orbital energy and angular momentum radiated
- **Integration**: 4th-order Runge-Kutta with adaptive time stepping

### Merger Phase
- **Remnant mass**: Fits from Healy et al. (2014), calibrated to NR simulations
- **Remnant spin**: Rezzolla et al. (2008) fitting formula
- **Gravitational recoil**: Gonzalez et al. (2007) kick velocity fits

### Ringdown Phase
- **QNM frequencies**: Berti, Cardoso & Starinets (2009), l=2 m=2 n=0 mode
- **Damped sinusoidal waveform** with exponential decay

## Build

### Prerequisites
- CMake 3.16+
- C++17 compiler (MSVC, GCC, or Clang)
- Internet connection (for GLM fetch)

### Build Commands
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Run
```bash
# Default equal-mass merger
./build/bin/Release/bh_collision.exe

# Custom parameters
./build/bin/Release/bh_collision.exe --m1 0.6 --m2 0.4 --sep 25 --chi1 0.3

# Run tests
./build/bin/Release/bh_collision_tests.exe
```

## Output

The simulation exports a JSON file (`output/simulation_data.json`) containing:
- Full trajectory data for both black holes
- Gravitational wave strain (h+, h×) at each timestep
- Orbital parameters (separation, frequency, energy)
- Remnant properties (mass, spin, kick velocity)
- QNM ringdown waveform

## Integration with Renderer

This project is designed for integration with the [black_hole_v2.2.0](../black_hole_v2.2.0) visual renderer. The `integration_api.h` header provides:

- `BHRenderState`: GPU-friendly struct mapping to shader uniforms (position, mass, Schwarzschild radius, spin)
- `CollisionTimeline`: Frame-interpolated playback timeline
- `CollisionRenderData`: Per-frame data with GW strain for visual distortion effects

## Units

All computations use geometrized units (G = c = 1):
- Mass: fractions of total system mass M
- Distance: units of M (= GM/c²)
- Time: units of M (= GM/c³)

The CLI reports SI conversion factors for a given total mass in solar masses.

## References

1. Blanchet, L. "Gravitational Radiation from Post-Newtonian Sources", Living Rev. Relativity 17 (2014) 2
2. Peters, P. C. & Mathews, J. "Gravitational Radiation from Point Masses in a Keplerian Orbit", Phys. Rev. 131 (1963) 435
3. Healy, J. et al. "Remnant mass, spin, and recoil from binary black hole merger", PRD 90 (2014) 104004
4. Rezzolla, L. et al. "Final spin from the coalescence of two black holes", PRD 78 (2008) 044002
5. Berti, E., Cardoso, V. & Starinets, A. O. "Quasinormal modes of black holes and black branes", PRD 79 (2009) 064016
6. Gonzalez, J. A. et al. "Maximum kick from nonspinning black-hole binary inspiral", PRL 98 (2007) 091101
