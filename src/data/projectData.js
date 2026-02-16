// Project data for all 12 portfolio projects across 5 domains
// Asset paths are relative to the /portfolio directory in the public folder

const basePath = '/portfolio'

export const categories = [
    { id: 'all', label: 'All Projects' },
    { id: 'modelling', label: 'Modelling' },
    { id: 'texturing', label: 'Texturing' },
    { id: 'environments', label: 'Environment Design' },
    { id: 'coding', label: 'Coding' },
    { id: 'shading', label: 'Shading & Post Process' },
]

export const projects = [
    // ─── MODELLING ──────────────────────────────────────
    {
        id: 'guitar',
        title: 'Acoustic Guitar',
        category: 'modelling',
        description: 'A meticulously crafted acoustic guitar model featuring high-fidelity geometry, realistic string details, and accurate proportions. Built from reference to capture every nuance of the instrument\'s form and construction.',
        tools: ['Blender', '3D Modelling', 'UV Unwrapping'],
        thumbnail: `${basePath}/modelling - done/guitar/image render 1.png`,
        images: [
            `${basePath}/modelling - done/guitar/image render 1.png`,
            `${basePath}/modelling - done/guitar/image render 2.png`,
            `${basePath}/modelling - done/guitar/render 3.png`,
        ],
        videos: [`${basePath}/modelling - done/guitar/360 spin render0001-0250.mp4`],
        model3d: null,
        documentation: [
            {
                title: 'Reference & Blockout',
                content: 'The project began with collecting orthographic and photographic references of acoustic guitars to ensure dimensional accuracy. A rough blockout was created to establish the overall silhouette and proportions before committing to detailed geometry.'
            },
            {
                title: 'Body & Neck Modelling',
                content: 'The body uses subdivision surface modelling to achieve the smooth, organic curvature of the soundboard and back. The neck required careful edge flow management to maintain clean topology around the fretboard and headstock transition. Support loops were strategically placed to control curvature without adding unnecessary density.'
            },
            {
                title: 'String & Hardware Details',
                content: 'Individual strings were modelled as separate geometry with slight tension variation for realism. Tuning pegs, bridge pins, and the saddle were all modelled as distinct objects to allow independent material assignment and to capture the metallic details that sell the realism of the piece.'
            },
            {
                title: 'UV Unwrapping & Render Setup',
                content: 'UVs were unwrapped with minimal stretching, using seams placed along natural edges of the guitar body. The final render uses HDRI-based studio lighting with a three-point setup to highlight surface curvature and material transitions across the body.'
            },
        ],
    },
    {
        id: 'razer-kraken',
        title: 'Razer Kraken Headset',
        category: 'modelling',
        description: 'A precision-modelled recreation of the Razer Kraken gaming headset, capturing the product\'s sleek industrial design, padding details, and signature aesthetic. Rendered with studio lighting for a product-shot quality finish.',
        tools: ['Blender', 'Product Visualization', 'Studio Lighting'],
        thumbnail: `${basePath}/modelling - done/razer kraken headset/render 2.png`,
        images: [
            `${basePath}/modelling - done/razer kraken headset/render 2.png`,
        ],
        videos: [`${basePath}/modelling - done/razer kraken headset/Anim Render 10000-0720.mp4`],
        model3d: null,
        documentation: [
            {
                title: 'Product Reference Analysis',
                content: 'The Razer Kraken was chosen for its blend of organic curves and technical hard-surface details. Reference images were studied to understand the headband flex mechanism, ear cup rotation points, and the distinctive Razer branding elements that define the product\'s identity.'
            },
            {
                title: 'Hard-Surface Modelling Approach',
                content: 'The headband uses a combination of boolean operations and manual retopology to achieve crisp edges alongside smooth curvature. The ear cups required careful attention to the padding geometry — using displacement and subdivision to simulate the memory foam cushion compression pattern.'
            },
            {
                title: 'Material & Lighting',
                content: 'Materials differentiate between the matte plastic housing, leatherette padding, mesh fabric on the ear cups, and metallic adjustment sliders. Studio lighting was set up to mimic product photography — a key light to define form, a fill light to soften shadows, and a rim light to separate the silhouette from the background.'
            },
        ],
    },
    {
        id: 'lamborghini-model',
        title: 'Lamborghini Aventador LP 750-4 SV',
        category: 'modelling',
        description: 'A high-poly model of the Lamborghini Aventador supercar capturing the aggressive body lines, aerodynamic surfaces, and mechanical details. Rendered with automotive-grade lighting to showcase surface continuity.',
        tools: ['Blender', 'Automotive Modelling', 'HDRI Lighting'],
        thumbnail: `${basePath}/modelling - done/realistic lamborghini render/Lamborghini Aventador LP 750-4 SV image render 1.png`,
        images: [
            `${basePath}/modelling - done/realistic lamborghini render/Lamborghini Aventador LP 750-4 SV image render 1.png`,
            `${basePath}/modelling - done/realistic lamborghini render/Lamborghini Aventador LP 750-4 SV image render 2.png`,
        ],
        videos: [],
        model3d: null,
        documentation: [
            {
                title: 'Automotive Reference & Blueprint',
                content: 'Accurate automotive modelling demands precise blueprint alignment. Orthographic reference images were calibrated and placed on background planes in Blender to guide every panel line, vent, and aerodynamic surface. The LP 750-4 SV variant has distinctive aero elements that differentiate it from the standard Aventador.'
            },
            {
                title: 'Body Panel Workflow',
                content: 'Each body panel was modelled as a separate object to maintain clean edge flow and allow for accurate panel gaps. Subdivision surface modelling was used with carefully placed holding edges to achieve the sharp creases characteristic of Lamborghini\'s angular design language while maintaining smooth surface flow between creases.'
            },
            {
                title: 'Detailing & Render',
                content: 'Fine details include the aggressive front splitter, rear diffuser, quad exhaust tips, and the distinctive SV wing. Wheels were modelled with accurate spoke geometry and brake caliper details. The final render uses an HDRI environment to produce realistic reflections that demonstrate the surface continuity of the bodywork.'
            },
        ],
    },

    // ─── TEXTURING ──────────────────────────────────────
    {
        id: 'album-cover',
        title: 'Album Cover Art',
        category: 'texturing',
        description: 'A stylized 3D album cover composition utilizing procedural texturing techniques and creative material work. The piece blends artistic direction with technical texturing to produce a visually impactful result.',
        tools: ['Blender', 'Substance Painter', 'Procedural Texturing'],
        thumbnail: `${basePath}/texturing - done/album cover/album cover render 1.png`,
        images: [
            `${basePath}/texturing - done/album cover/album cover render 1.png`,
        ],
        videos: [],
        model3d: null,
        documentation: [
            {
                title: 'Concept & Art Direction',
                content: 'The album cover was conceived as an exercise in combining 3D geometry with bold, graphic texturing. The art direction prioritized strong visual impact at thumbnail scale — essential for album artwork that must read clearly on streaming platforms and social media.'
            },
            {
                title: 'Procedural Material Design',
                content: 'Textures were built almost entirely with procedural node networks rather than baked maps. This approach gives full resolution independence and allows for rapid iteration on color palettes and pattern variations without re-painting UV-mapped textures. Noise patterns, gradients, and wave textures were layered to create organic surface variation.'
            },
            {
                title: 'Composition & Final Render',
                content: 'The composition uses depth of field and strategic lighting to guide the viewer\'s eye toward the focal point. Color grading was applied in the compositor to unify the palette and add cinematic contrast, transforming the raw render into a finished piece of visual art.'
            },
        ],
    },
    {
        id: 'fountain',
        title: 'Ornamental Fountain',
        category: 'texturing',
        description: 'A detailed fountain model with hand-crafted textures showcasing weathering, moss growth, and water-stained stone materials. The texturing process emphasized realistic surface wear and environmental storytelling.',
        tools: ['Blender', 'Substance Painter', 'PBR Texturing'],
        thumbnail: `${basePath}/texturing - done/fountain/ffd717f1-0351-4cf6-adaa-4cb2f2ba055c.jpg`,
        images: [
            `${basePath}/texturing - done/fountain/16a6a2bb-d5c5-4800-b1db-4a310db8501f.jpg`,
            `${basePath}/texturing - done/fountain/b6a9c70c-bb56-4673-a68c-fa336381b10f.jpg`,
            `${basePath}/texturing - done/fountain/ffd717f1-0351-4cf6-adaa-4cb2f2ba055c.jpg`,
        ],
        videos: [],
        model3d: null,
        documentation: [
            {
                title: 'Surface Story & Weathering',
                content: 'Great texturing tells a story about an object\'s history. This fountain was textured to suggest years of outdoor exposure — water staining follows gravity paths, moss accumulates in crevices where moisture lingers, and surface erosion is greatest on exposed horizontal surfaces. Each of these details was painted with intentional placement, not random noise.'
            },
            {
                title: 'PBR Material Layering',
                content: 'The stone material uses a multi-layer PBR approach: a base stone layer provides the fundamental color and roughness, followed by edge wear that reveals lighter stone underneath, and finally organic layers of moss, dirt, and water staining. Each layer is masked using curvature, ambient occlusion, and hand-painted masks to control placement.'
            },
            {
                title: 'Texel Density & Detail',
                content: 'Consistent texel density was maintained across the model to ensure uniform detail resolution. UV islands were packed efficiently to maximize texture space utilization, and areas of highest visual interest (ornamental details, water contact surfaces) were given slightly higher texel density for added clarity.'
            },
        ],
    },
    {
        id: 'lamborghini-textured',
        title: 'Lamborghini — Textured',
        category: 'texturing',
        description: 'Building upon the base model, this project focused on creating photorealistic automotive materials — metallic paint with clear coat, carbon fiber composites, tinted glass, and rubber tire textures.',
        tools: ['Blender', 'Substance Painter', 'Automotive Materials'],
        thumbnail: `${basePath}/texturing - done/realistic lamborghini render/Lamborghini Aventador LP 750-4 SV image render 1.png`,
        images: [
            `${basePath}/texturing - done/realistic lamborghini render/Lamborghini Aventador LP 750-4 SV image render 1.png`,
            `${basePath}/texturing - done/realistic lamborghini render/Lamborghini Aventador LP 750-4 SV image render 2.png`,
        ],
        videos: [],
        model3d: null,
        documentation: [
            {
                title: 'Automotive Paint System',
                content: 'Car paint is one of the most complex real-world materials to replicate digitally. The base metallic layer uses a flake normal map that shifts with viewing angle, topped by a clear coat layer with its own independent roughness and reflection properties. This two-layer approach is critical for capturing the deep, wet look of modern automotive finishes.'
            },
            {
                title: 'Material Variation',
                content: 'Beyond the primary paint, the car features distinct material zones: carbon fiber weave on aero components with visible fiber direction and resin gloss, brushed aluminum on interior accents, rubber with micro-surface detail on tires, and tinted glass with accurate transparency and reflectance. Each material was calibrated to physically accurate values.'
            },
            {
                title: 'Environmental Reflections',
                content: 'Automotive rendering is heavily dependent on the environment reflected in the car\'s surfaces. The HDRI environment was chosen to provide both interesting reflections that reveal the curvature of the bodywork and realistic lighting conditions that test the paint material under varied illumination angles.'
            },
        ],
    },

    // ─── ENVIRONMENT DESIGN ─────────────────────────────
    {
        id: 'game-cinematic',
        title: 'Game Cinematic Environment',
        category: 'environments',
        description: 'A fully realized game-ready cinematic environment featuring custom character models, a vertex-shader-driven rain system with dynamic ripple effects, and atmospheric lighting to set a dramatic mood.',
        tools: ['Unreal Engine', 'Blender', 'Vertex Shaders', 'VFX'],
        thumbnail: `${basePath}/environment design - done/game cinematic/environment.jpg`,
        images: [
            `${basePath}/environment design - done/game cinematic/environment.jpg`,
            `${basePath}/environment design - done/game cinematic/characted model.jpg`,
            `${basePath}/environment design - done/game cinematic/custom rain system with a vertext shader for ripples.jpg`,
            `${basePath}/environment design - done/game cinematic/render 2.png`,
        ],
        videos: [],
        model3d: null,
        documentation: [
            {
                title: 'Environment Composition',
                content: 'The scene was designed around cinematic framing principles — leading lines guide the camera through the environment while strategic asset placement creates visual depth across foreground, midground, and background layers. The lighting was designed to evoke a specific mood and time of day, reinforcing the narrative tone of the cinematic.'
            },
            {
                title: 'Custom Rain System',
                content: 'The rain effect uses a vertex shader that displaces geometry along their normals to simulate water ripples on surfaces. Unlike particle-only rain systems, this approach creates interactive puddle ripples that respond to the rain\'s impact points. The ripple displacement is driven by a scrolling noise texture animated in the shader, providing organic variation without simulation overhead.'
            },
            {
                title: 'Character Integration',
                content: 'The custom character model was designed to fit the scene\'s art direction and scale. It serves as a focal point that grounds the environment in a narrative context — transforming abstract architecture into a lived-in scene with human presence and implied story.'
            },
            {
                title: 'Atmospheric Effects',
                content: 'Volumetric fog and lighting were used to add depth and atmosphere. The fog density is carefully tuned to create air perspective (distant objects appear hazier), which is essential for selling the scale of the environment. Emissive light sources contribute to the wet-surface reflections that bring the rainy scene to life.'
            },
        ],
    },
    {
        id: 'moai-environment',
        title: 'Moai Concept Art Environment',
        category: 'environments',
        description: 'A concept art-driven environment depicting ancient Moai statues in a lush, atmospheric setting. The scene combines sculpted terrain, volumetric lighting, and stylized foliage to evoke a sense of mystery and discovery.',
        tools: ['Blender', 'Concept Art', 'Environment Sculpting'],
        thumbnail: `${basePath}/environment design - done/moai conept art environment/render 5.png`,
        images: [
            `${basePath}/environment design - done/moai conept art environment/render 5.png`,
            `${basePath}/environment design - done/moai conept art environment/render 6.png`,
            `${basePath}/environment design - done/moai conept art environment/render 8.png`,
            `${basePath}/environment design - done/moai conept art environment/render 11.png`,
        ],
        videos: [],
        model3d: null,
        documentation: [
            {
                title: 'Concept & World-Building',
                content: 'This environment was conceived as concept art exploration — asking "what if these ancient statues existed in a lush, overgrown jungle?" The Moai statues were sculpted with intentional erosion and vegetation overgrowth to suggest centuries of abandonment, blending historical reference with fantasy world-building.'
            },
            {
                title: 'Terrain Sculpting',
                content: 'The terrain was sculpted using multi-resolution displacement to create natural landforms — rocky outcrops, uneven ground, and moss-covered surfaces. The sculpting process focused on creating interesting silhouettes at the scene boundary while maintaining a natural, organic feel to the ground plane that the statues rest upon.'
            },
            {
                title: 'Lighting & Mood',
                content: 'Volumetric lighting plays a central role, with god rays filtering through the canopy to create pools of light and shadow that draw the viewer\'s eye through the composition. The color temperature shifts from warm highlights to cool shadows, creating depth and atmosphere. Multiple renders explore different times of day and weather conditions.'
            },
        ],
    },
    {
        id: 'ruins',
        title: 'Ancient Ruins',
        category: 'environments',
        description: 'A detailed ruins environment featuring destruction simulation, custom-modelled flora, and hand-placed ground vegetation. The scene includes a physics-based destruction sequence to add dynamic visual interest.',
        tools: ['Blender', 'Physics Simulation', 'Foliage Systems'],
        thumbnail: `${basePath}/environment design - done/ruins/render 1.png`,
        images: [
            `${basePath}/environment design - done/ruins/render 1.png`,
            `${basePath}/environment design - done/ruins/environment.jpg`,
            `${basePath}/environment design - done/ruins/ground flora.jpg`,
            `${basePath}/environment design - done/ruins/tree model.jpg`,
        ],
        videos: [`${basePath}/environment design - done/ruins/destruction simulation.mp4`],
        model3d: null,
        documentation: [
            {
                title: 'Architectural Ruins',
                content: 'The ruins were modelled from intact architectural forms and then systematically broken down — walls were fractured using cell fracture tools, columns were partially collapsed, and rubble was scattered using physics simulations. This "build then destroy" approach produces more convincing ruins than modelling broken geometry directly.'
            },
            {
                title: 'Destruction Simulation',
                content: 'The destruction sequence uses rigid body physics simulation to animate the collapse of structural elements. Objects are pre-fractured into shards with realistic mass distribution, and constraints hold them together until triggered. The simulation captures natural-looking debris cascades, dust settling patterns, and structural chain reactions.'
            },
            {
                title: 'Flora & Vegetation',
                content: 'Custom tree and plant models were created using particle systems and manual placement. Ground flora uses a scatter system with density maps to concentrate vegetation in cracks, along walls, and in sheltered areas where plants would naturally take hold. Each species was modelled with botanical reference to ensure visual authenticity.'
            },
            {
                title: 'Environmental Storytelling',
                content: 'Every element in the scene contributes to a narrative of time passing — the degree of plant overgrowth, the weathering patterns on stone, and the placement of collapsed elements all suggest a specific history. This approach to environment design creates scenes that feel discovered rather than constructed.'
            },
        ],
    },

    // ─── CODING ─────────────────────────────────────────
    {
        id: 'black-hole-simulation',
        title: 'Black Hole Simulation & Ray Tracer',
        category: 'coding',
        description: 'A comprehensive C++ black hole simulation project featuring a physically-based ray tracer that simulates gravitational lensing and accretion disk dynamics, alongside an N-body collision simulation computing orbital decay and gravitational wave emission during binary black hole mergers.',
        tools: ['C++', 'CMake', 'Ray Tracing', 'Numerical Physics', 'Gravitational Waves'],
        thumbnail: null,
        images: [],
        videos: [
            `${basePath}/coding - done/version 1.2.mp4`,
            `${basePath}/coding - done/version 2.0.mp4`,
            `${basePath}/coding - done/final version.mp4`,
        ],
        model3d: null,
        github: null,
        documentation: [
            {
                title: 'Geodesic Ray Tracing — Photon Paths in Curved Spacetime',
                content: 'The ray tracer simulates light travelling through the curved spacetime of a Schwarzschild black hole. Instead of tracing straight lines, each photon\'s path is numerically integrated using the geodesic equation. The key insight is that in the Schwarzschild metric, the specific angular momentum h = r × v is conserved along each geodesic. The acceleration applied to each ray at every integration step is:',
                code: {
                    language: 'glsl',
                    label: 'blackhole_main.frag — Geodesic acceleration',
                    snippet: `vec3 accel(float h2, vec3 pos) {\n    float r2 = dot(pos, pos);\n    float r5 = pow(r2, 2.5);\n    vec3 acc = -1.5 * h2 * pos / r5;\n    return acc;\n}`
                }
            },
            {
                title: 'Ray Marching Loop — From Camera to Event Horizon',
                content: 'Each pixel fires a ray that is stepped forward 6,000 times. At each step, the gravitational acceleration bends the ray direction, the accretion disk is sampled volumetrically, and the ray is checked against the event horizon (r < 1 in Schwarzschild radii). Rays that fall below the event horizon return black — they\'ve been captured.',
                code: {
                    language: 'glsl',
                    label: 'blackhole_main.frag — Core ray-march loop',
                    snippet: `for (int i = 0; i < 6000; i++) {\n    if (gravatationalLensing > 0.5) {\n        vec3 acc = accel(h2, pos);  // Bend ray\n        dir += acc;\n    }\n    if (dot(pos, pos) < 1.0) {     // Event horizon\n        return color;\n    }\n    if (adiskEnabled > 0.5) {\n        adiskColor(pos, dir, color, alpha);\n    }\n    pos += dir;                     // Step forward\n}`
                }
            },
            {
                title: 'Accretion Disk — Volumetric Emission with Doppler Beaming',
                content: 'The accretion disk is rendered volumetrically using layered noise (FBM + domain warping) for realistic gas cloud structure. Light emission follows the Beer-Lambert law for absorption, and Doppler beaming brightens the approaching side of the disk. The disk is bounded by the Innermost Stable Circular Orbit (ISCO) at r = 3M (6GM/c² in SI units) — no stable orbits exist inside this radius.',
                code: {
                    language: 'glsl',
                    label: 'blackhole_main.frag — Doppler beaming & volumetric emission',
                    snippet: `// Doppler beaming — approaching side appears brighter\nvec3 orbitalDir = normalize(cross(vec3(0,1,0), normalize(pos)));\nfloat dopplerFactor = 1.0 + 0.5 * dot(normalize(dir), orbitalDir);\n\n// Beer-Lambert absorption law\nfloat opticalDensity = density * noise * 0.04;\nfloat transmittance = exp(-opticalDensity);\n\n// Emission weighted by current transparency\nvec3 emission = density * adiskLit * dustColor * noise * dopplerFactor;\ncolor += emission * alpha;\nalpha *= transmittance;  // Reduce transparency`
                }
            },
            {
                title: 'Post-Newtonian Orbital Dynamics — Equations of Motion',
                content: 'The binary black hole simulation uses post-Newtonian (PN) approximations to compute orbital dynamics. The acceleration is expanded in powers of v/c: Newtonian gravity at 0PN, relativistic corrections at 1PN and 2PN (conservative), and gravitational wave radiation reaction at 2.5PN (dissipative). The 2.5PN term is what causes the orbit to decay — it\'s the Burke-Thorne radiation reaction force.',
                code: {
                    language: 'cpp',
                    label: 'physics.cpp — Post-Newtonian acceleration (1PN term)',
                    snippet: `// Newtonian: a_N = -M/r² · n̂\nresult.a_newtonian = -M / r2 * n;\n\n// 1PN correction — O(v²/c²) from Blanchet (2014), Eq. (203)\ndouble n_coeff = -v2\n                + 2.0 * (2.0 + eta) * Mr\n                + 1.5 * eta * rdot * rdot;\ndouble v_coeff = 2.0 * (2.0 - eta) * rdot;\nresult.a_1pn = -Mr / r_mag * (n_coeff * n + v_coeff * v);`
                }
            },
            {
                title: '2.5PN Radiation Reaction — Gravitational Wave Energy Loss',
                content: 'The dissipative 2.5PN term implements the Burke-Thorne radiation reaction — the back-reaction on the orbit from emitting gravitational waves. This is what drives the inspiral: the binary loses orbital energy to gravitational radiation, causing the orbit to shrink. The energy loss rate follows the Peters formula: dE/dt = -(32/5) η² M⁵/r⁵.',
                code: {
                    language: 'cpp',
                    label: 'physics.cpp — 2.5PN radiation reaction (Burke-Thorne)',
                    snippet: `// 2.5PN dissipative term from Iyer & Will (1995)\ndouble prefactor = 8.0 / 5.0 * eta * M * Mr / r2;\n\ndouble n_coeff = rdot * (18.0*v2 + (2.0/3.0)*Mr - 25.0*rdot2);\ndouble v_coeff = -(6.0*v2 - 2.0*Mr - 15.0*rdot2);\n\nresult.a_25pn = prefactor * (n_coeff * n + v_coeff * v);\n\n// Peters formula: energy loss rate\n// dE/dt = -(32/5) · η² · M⁵ / r⁵\nreturn -(32.0/5.0) * eta*eta * pow(M,5.0) / pow(r,5.0);`
                }
            },
            {
                title: 'Gravitational Wave Strain — Quadrupole Formula',
                content: 'Gravitational wave strain is computed using the leading-order quadrupole approximation. The two polarizations h₊ and h× depend on the orbital frequency, reduced mass, observer distance and inclination angle. The GW frequency is twice the orbital frequency. This formula breaks down near merger, where numerical relativity fits take over.',
                code: {
                    language: 'cpp',
                    label: 'physics.cpp — Quadrupole gravitational wave strain',
                    snippet: `// h₊ = -(2μ/D) · (Mω)^(2/3) · (1+cos²ι)/2 · cos(2Φ)\n// h× = -(2μ/D) · (Mω)^(2/3) · cosι · sin(2Φ)\n\ndouble v_param = cbrt(M * omega);\ndouble prefactor = 2.0 * mu * v_param*v_param / D;\n\ngw.h_plus  = -prefactor * (1.0+cos2_iota)/2.0 * cos(2.0*Phi);\ngw.h_cross = -prefactor * cos_iota * sin(2.0*Phi);\ngw.frequency = omega / M_PI;  // f_GW = 2·f_orbital`
                }
            },
            {
                title: 'RK4 Integration with Adaptive Timestep',
                content: 'The orbital dynamics are integrated using a 4th-order Runge-Kutta (RK4) method with adaptive time stepping. As the orbit decays, the orbital period shrinks rapidly — the timestep must decrease proportionally to maintain accuracy. The adaptive step size is set as a fraction of the current orbital period, with configurable min/max bounds to prevent instability near merger.',
                code: {
                    language: 'cpp',
                    label: 'integrator.h — RK4 integrator interface',
                    snippet: `struct IntegratorConfig {\n    double dt_initial = 0.1;      // Initial time step\n    double dt_min     = 1e-6;     // Minimum (near merger)\n    double dt_max     = 1.0;      // Maximum\n    double safety_factor = 0.1;   // Fraction of orbital period\n    bool   adaptive   = true;\n};\n\n// Advance state by one RK4 step\nBinaryState rk4_step(\n    const BinaryState& state, double dt,\n    const DerivativeFunc& deriv);`
                }
            },
            {
                title: 'Merger & Remnant — Numerical Relativity Fitting Formulas',
                content: 'When the binary separation drops below a critical threshold (based on Schwarzschild radii), merger occurs. Post-merger remnant properties are computed using fits calibrated to numerical relativity simulations: final mass from Healy et al. (2014), final spin from Rezzolla et al. (2008), and quasinormal mode (QNM) frequencies from Berti, Cardoso & Starinets (2009). For equal-mass non-spinning black holes, about 3.5% of the total mass is radiated as gravitational waves.',
                code: {
                    language: 'cpp',
                    label: 'merger.cpp — Remnant mass & QNM ringdown',
                    snippet: `// Final mass — Healy et al. (2014) fitting formula\n// E_rad ≈ 3.5% for equal-mass non-spinning\ndouble E_rad = eta * (p0 + 4.0*eta*p0) * spin_corr;\nreturn 1.0 - E_rad;\n\n// QNM ringdown — Berti et al. (2009), l=2 m=2 n=0\n// ω = (f1 + f2·(1-aₑ)^f3) / Mf\ndouble omega_qnm = (1.5251 + (-1.1568)*pow(1-af, 0.1292)) / Mf;\n\n// Damped sinusoid: h(t) = A·exp(-t/τ)·cos(2πft + φ₀)\ndouble envelope = amplitude * exp(-t / damping_time);\ngw.h_plus = envelope * (1+cos²ι)/2 * cos(phase) / D;`
                }
            },
        ],
    },

    // ─── CUSTOM SHADING & POST PROCESS ──────────────────
    {
        id: 'line-shader',
        title: 'Stylized Line Shader',
        category: 'shading',
        description: 'A custom line-art shader that generates clean outlines and hatching patterns on 3D geometry. The shader dynamically adjusts line weight based on surface curvature and viewing angle for a hand-drawn aesthetic.',
        tools: ['Blender', 'Shader Nodes', 'GLSL', 'NPR Rendering'],
        thumbnail: `${basePath}/custom shading and post process - done/line shaer/shader setup.jpg`,
        images: [
            `${basePath}/custom shading and post process - done/line shaer/shader setup.jpg`,
        ],
        videos: [`${basePath}/custom shading and post process - done/line shaer/mushroom_turnable0000-0100.mp4`],
        model3d: null,
        documentation: [
            {
                title: 'NPR Rendering Philosophy',
                content: 'Non-photorealistic rendering (NPR) deliberately breaks from physical accuracy to achieve an artistic style. This shader aims to replicate the look of hand-drawn illustration — clean outlines defining form, hatching indicating shadow, and flat color fills. The challenge is making these stylistic choices work consistently across any 3D geometry without manual intervention.'
            },
            {
                title: 'Edge Detection via Shader Nodes',
                content: 'Outlines are generated by comparing surface normals with the viewing direction — edges where the surface turns sharply away from the camera are rendered as dark lines. The line weight varies based on the dot product between the surface normal and view vector, producing thicker lines on silhouettes and thinner lines on internal creases. This approach avoids the geometry duplication of inverted-hull methods.'
            },
            {
                title: 'Hatching & Tone',
                content: 'Shadow regions use a procedural hatching pattern generated from UV-space line textures. The hatching density increases in darker areas, mimicking the cross-hatching technique used in traditional illustration. A threshold ramp controls the transition between lit and shadowed regions, creating the characteristic sharp tonal breaks of cel-shaded artwork.'
            },
        ],
    },
    {
        id: 'toon-shader',
        title: 'Toon Post-Process Shader',
        category: 'shading',
        description: 'A real-time toon shading pipeline featuring cel-shading, edge detection, and color quantization post-processing effects. The shader creates a stylized cartoon look while preserving depth and form readability.',
        tools: ['Blender', 'Shader Nodes', 'Post Processing', 'NPR'],
        thumbnail: `${basePath}/custom shading and post process - done/toon shader/shader setup 1.jpg`,
        images: [
            `${basePath}/custom shading and post process - done/toon shader/shader setup 1.jpg`,
            `${basePath}/custom shading and post process - done/toon shader/shader setup part 2.jpg`,
        ],
        videos: [`${basePath}/custom shading and post process - done/toon shader/demo.mp4`],
        model3d: null,
        documentation: [
            {
                title: 'Toon Shading Approach',
                content: 'The toon shader uses a multi-pass approach: the first pass renders the scene normally, then a post-process pass applies the stylization. This separation means the toon effect can be applied to any scene without modifying individual object materials — it works as a screen-space filter that transforms photorealistic renders into cartoon-style output.'
            },
            {
                title: 'Color Quantization',
                content: 'Color quantization reduces the continuous color spectrum to discrete bands, creating the flat-color look of cartoon animation. The number of color bands is adjustable, allowing control over how "flat" or "smooth" the final look appears. The quantization operates in HSV space to preserve hue accuracy while simplifying value and saturation independently.'
            },
            {
                title: 'Edge Detection Pipeline',
                content: 'Outlines are detected using a Sobel filter applied to both the depth buffer and the normal buffer. Depth edges catch silhouettes and occlusion boundaries, while normal edges catch surface angle changes. Combining both sources produces comprehensive outlines that define every major form in the scene. Edge thickness and color are artist-controllable parameters.'
            },
            {
                title: 'Shader Node Setup',
                content: 'The included shader setup screenshots show the complete node graph. The network is organized into logical groups: lighting ramp, color quantization, edge detection, and compositing. This modular approach makes the shader easy to modify — artists can adjust individual parameters without understanding the entire network.'
            },
        ],
    },
]

export const skills = [
    {
        title: '3D Modelling',
        icon: '🎨',
        description: 'High-poly & game-ready modelling, hard-surface & organic sculpting',
        tools: ['Blender', 'ZBrush'],
    },
    {
        title: 'Texturing',
        icon: '🖌️',
        description: 'PBR texturing, procedural materials, hand-painted techniques',
        tools: ['Substance Painter', 'Blender Shader Nodes'],
    },
    {
        title: 'Environment Design',
        icon: '🌍',
        description: 'Game-ready environments, concept art scenes, foliage & terrain',
        tools: ['Unreal Engine', 'Blender', 'World Machine'],
    },
    {
        title: 'Custom Shading',
        icon: '✨',
        description: 'NPR shaders, post-process effects, real-time shader programming',
        tools: ['GLSL', 'Shader Nodes', 'Unreal Materials'],
    },
    {
        title: 'Coding & Simulation',
        icon: '💻',
        description: 'Physics simulations, ray tracing, render pipeline development',
        tools: ['C++', 'Python', 'CMake'],
    },
]

export const personalInfo = {
    name: 'Adithya Sridhar',
    firstName: 'Adithya',
    role: 'Technical 3D Artist',
    university: 'PES University',
    location: 'Bangalore, India',
    bio: 'I\'m a Technical 3D Artist who bridges the gap between art and engineering. With deep expertise in modelling, texturing, environment design, and shader programming, I craft immersive 3D experiences that push the boundaries of real-time graphics. Currently studying at PES University, I combine my passion for visual storytelling with a strong programming foundation to build everything from photorealistic renders to custom physics simulations.',
    linkedin: '#',
    github: '#',
    email: 'contact@adithyasridhar.com',
}
