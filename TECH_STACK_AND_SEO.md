# Tech Stack & SEO Techniques

> Documentation for **Adithya Sridhar's Portfolio** — a single-page application showcasing 3D art, texturing, environment design, shading, and coding projects.

---

## Table of Contents

1. [Technology Stack](#technology-stack)
2. [SEO Techniques](#seo-techniques)

---

## Technology Stack

### Build Tooling & Bundler

| Tool | Version | Role |
|------|---------|------|
| **Vite** | 7.3.1 | Lightning-fast dev server and production bundler powered by native ES modules and Rollup under the hood. Handles HMR (Hot Module Replacement), asset hashing, and tree-shaking. |
| **@vitejs/plugin-react** | 5.1.4 | Official Vite plugin that enables React Fast Refresh during development and optimizes JSX transforms for production builds. |

**Custom Vite configuration highlights** (`vite.config.js`):

- **Asset inclusion** — `.glb`, `.gltf`, `.mp4`, `.webm`, `.mov`, and `.avi` files are treated as importable assets, enabling direct reference to 3D models and video files in source code.
- **Manual chunk splitting** — Vendor libraries are split into two chunks (`vendor-react` for React/Router/DOM, `vendor-anim` for Framer Motion/GSAP), improving caching and reducing initial load times.
- **Chunk size warning limit** — Raised to 1500 KB to accommodate media-heavy bundles without false build warnings.

---

### Frontend Framework & UI

| Library | Version | Role |
|---------|---------|------|
| **React** | 19.2.4 | Core UI library. The app uses functional components, hooks (`useState`, `useEffect`, `useRef`), `lazy()` + `Suspense` for code-splitting, and `React.StrictMode` for development safeguards. |
| **React DOM** | 19.2.4 | DOM renderer. Uses the `createRoot` API (React 18+ concurrent mode). |
| **React Router DOM** | 7.13.0 | Client-side routing with `BrowserRouter`, `Routes`, and `Route`. Three routes are defined: `/` (Home), `/project/:id` (Project Detail), and `/technical-process` (Technical Process). |

---

### Animation & Motion

| Library | Version | Role |
|---------|---------|------|
| **Framer Motion** | 12.34.0 | Declarative animations via `motion` components, `AnimatePresence` for exit animations, `useInView` for scroll-triggered reveals, `useScroll` / `useTransform` for parallax effects, and `layout` animations for the project grid filter. |
| **GSAP** (GreenSock) | 3.14.2 | Imperative timeline animations used primarily in the Hero section for staggered text reveals. The `ScrollTrigger` plugin is registered for scroll-linked animations. GSAP contexts are properly cleaned up on unmount. |
| **Lenis** | 1.3.17 | Smooth-scroll library providing buttery inertial scrolling with a custom easing function. Integrated globally via `window.lenis` so the `ScrollToTop` component can programmatically reset scroll position on route changes. |

---

### SEO & Head Management

| Library | Version | Role |
|---------|---------|------|
| **react-helmet-async** | 2.0.5 | Manages the `<head>` of each page at the component level using the `<Helmet>` component. Wrapped in `<HelmetProvider>` at the application root (`main.jsx`). Enables per-route title tags, meta descriptions, canonical URLs, Open Graph tags, Twitter Cards, and JSON-LD structured data. |

---

### Deployment & Hosting

| Tool | Role |
|------|------|
| **Vercel** | Production hosting platform. Configured via `vercel.json` with a universal rewrite rule (`"source": "/(.*)"` → `"/index.html"`) to support client-side routing—all paths resolve to `index.html` so React Router handles navigation. |

---

### Custom Scripts

| Script | Role |
|--------|------|
| **`scripts/generate-sitemap.js`** | A Node.js build-time script that auto-generates `sitemap.xml` and `robots.txt` into the `dist/` output directory. It imports the project data array to dynamically create URL entries for every project page. Runs automatically after `vite build` via the npm `build` script. |

---

### Custom Components

| Component | Role |
|-----------|------|
| **`LazyImage`** | Intersection Observer-based lazy loader for images. Renders a shimmer placeholder until the image enters the viewport (with a 200 px pre-load margin), then decodes it asynchronously (`decoding="async"`). |
| **`LazyVideo`** | Intersection Observer-based lazy loader for videos. Defers `<video>` element mounting until the wrapper scrolls into view (100 px margin). Detects file format and renders a download link for unsupported formats (e.g. `.avi`). |
| **`DotMatrix`** | Decorative canvas-based dot-grid background for visual texture. |
| **`Navbar`** / **`Footer`** | Standard navigation and footer components. |

---

### Typography

- **Inter** (weights 300–700) — Used for body text and UI elements.
- **Outfit** (weights 300–900) — Used for headings and display text.

Both loaded from **Google Fonts** via `<link>` tags with `preconnect` hints to `fonts.googleapis.com` and `fonts.gstatic.com` for optimized delivery.

---

## SEO Techniques

Below is a detailed explanation of every SEO technique implemented across the project, organized by category.

---

### 1. Title Tags (Per-Page)

**Where:** `<Helmet>` in `Home.jsx`, `ProjectDetail.jsx`, `TechnicalProcess.jsx`

Each route sets a unique, descriptive `<title>` tag that includes the page's primary keyword and the site-wide brand name:

- **Home** → `Adithya Sridhar | 3D Artist & Developer`
- **Project Detail** → `{project.title} | Adithya Sridhar`
- **Technical Process** → `In-Depth Technical 3D Art Pipeline & Workflow Documentation | Adithya Sridhar | PES University`

The base `index.html` also sets a fallback title (`Adithya Sridhar | Technical 3D Artist — Modelling, Texturing, Environment Design | Bangalore`) that is rendered by crawlers if JavaScript hasn't executed yet.

**SEO benefit:** Descriptive titles with target keywords directly influence click-through rates from search engine results pages (SERPs).

---

### 2. Meta Description Tags

**Where:** `index.html` (global), `<Helmet>` in `Home.jsx`, `ProjectDetail.jsx`, `TechnicalProcess.jsx`

Every page has a unique `<meta name="description">` summarizing the page content in 150–160 characters with relevant keywords:

- **Global fallback** — Mentions "Technical 3D Artist", "PES University", "Bangalore", "3D modelling", "texturing", "custom shaders".
- **Home** — Describes the portfolio's scope.
- **Project Detail** — Uses the project's `brief` or `description` field dynamically.
- **Technical Process** — Covers "advanced shader programming", "optimization techniques", "professional workflows".

**SEO benefit:** Meta descriptions appear as the snippet text under the page title in search results, directly influencing click-through rates.

---

### 3. Meta Keywords Tag

**Where:** `index.html`

```html
<meta name="keywords" content="Technical 3D Artist, PES University, Adithya, Adithya Sridhar, 3D Modelling, Texturing, Environment Design, Custom Shaders, Bangalore, Game Developer, Render Pipeline" />
```

**SEO benefit:** While largely deprecated by Google, some secondary search engines still index keywords meta tags, and it costs nothing to include.

---

### 4. Meta Author & Robots Tags

**Where:** `index.html`

```html
<meta name="author" content="Adithya Sridhar" />
<meta name="robots" content="index, follow" />
```

- **Author** — Confirms content authorship for search engines.
- **Robots** — Explicitly instructs crawlers to index the page and follow all links. This is the default behavior but declaring it removes ambiguity.

---

### 5. Canonical URLs

**Where:** `<Helmet>` in `Home.jsx`, `ProjectDetail.jsx`

```html
<link rel="canonical" href="https://adithyasridhar.com/" />
<link rel="canonical" href="https://adithyasridhar.com/project/{id}" />
```

**SEO benefit:** Canonical tags tell search engines which URL is the "master" version of a page, preventing duplicate content penalties when the same page is accessible via multiple URLs (e.g. with/without trailing slash, query parameters, etc.).

---

### 6. Open Graph (OG) Tags

**Where:** `index.html` (global), `<Helmet>` in `Home.jsx`, `ProjectDetail.jsx`

Full Open Graph metadata is set per page:

| OG Property | Description |
|-------------|-------------|
| `og:title` | Page title optimized for social sharing |
| `og:description` | Page summary for social cards |
| `og:type` | `website` for the home page, `article` for project pages |
| `og:url` | Canonical URL of the page |
| `og:site_name` | `Adithya Sridhar Portfolio` |
| `og:image` | Thumbnail image for the social card (project thumbnail for detail pages) |
| `og:locale` | `en_IN` (English, India) |

**SEO benefit:** Open Graph tags control how the page appears when shared on Facebook, LinkedIn, Discord, Slack, and other platforms that render link previews. Rich previews substantially increase engagement and click-through.

---

### 7. Twitter Card Tags

**Where:** `<Helmet>` in `Home.jsx`, `ProjectDetail.jsx`

```html
<meta name="twitter:card" content="summary_large_image" />
<meta name="twitter:title" content="..." />
<meta name="twitter:description" content="..." />
<meta name="twitter:image" content="..." />
```

**SEO benefit:** Enables large image cards when links are shared on Twitter/X, maximizing visual impact in the feed.

---

### 8. JSON-LD Structured Data (Schema.org)

**Where:** `index.html` (global `Person` schema), `ProjectDetail.jsx` (per-project `CreativeWork` schema)

#### Person Schema (Global)

```json
{
  "@context": "https://schema.org",
  "@type": "Person",
  "name": "Adithya Sridhar",
  "jobTitle": "Technical 3D Artist",
  "alumniOf": { "@type": "CollegeOrUniversity", "name": "PES University" },
  "address": { "addressLocality": "Bangalore", "addressCountry": "India" },
  "sameAs": ["LinkedIn", "GitHub", "ArtStation"],
  "knowsAbout": ["3D Modelling", "Texturing", "Environment Design", ...]
}
```

#### CreativeWork Schema (Per Project)

```json
{
  "@context": "https://schema.org",
  "@type": "CreativeWork",
  "name": "Project Title",
  "description": "...",
  "image": "...",
  "url": "...",
  "author": { "@type": "Person", "name": "Adithya Sridhar" },
  "genre": "Modelling | Texturing | ...",
  "keywords": "Tool1, Tool2, ..."
}
```

**SEO benefit:** JSON-LD structured data allows search engines to understand the content semantically. It powers Knowledge Graph panels, rich snippets, and enhanced search results. The `Person` schema can trigger a personal knowledge panel; `CreativeWork` helps individual projects appear in relevant searches with rich metadata.

---

### 9. Automated Sitemap Generation

**Where:** `scripts/generate-sitemap.js` (runs during `npm run build`)

A build-time Node.js script generates `sitemap.xml` containing:

- **Static pages** — `/` (priority 1.0), `/technical-process` (priority 0.7)
- **Dynamic project pages** — All projects from `projectData.js` (priority 0.8)

Each entry includes `<loc>`, `<changefreq>` (monthly), and `<priority>`.

**SEO benefit:** Sitemaps are a direct communication channel with search engines, ensuring all pages are discovered and indexed, including dynamically generated project pages that might not be linked from the main navigation.

---

### 10. Robots.txt Generation

**Where:** `scripts/generate-sitemap.js`

The same build script generates a `robots.txt` file:

```
User-agent: *
Allow: /

Sitemap: https://adithyasridhar.dev/sitemap.xml
```

**SEO benefit:** `robots.txt` instructs crawlers on what to index and provides the sitemap location, ensuring efficient crawl budget usage.

---

### 11. Semantic HTML Structure

**Where:** Throughout all page components

The app uses semantic HTML5 elements:

- `<main>` — Primary content wrapper on every page
- `<section>` — Distinct content sections (Hero, About, Skills, Projects, Contact)
- `<h1>` — Single per page (Hero title on Home, project title on Detail, article title on Technical Process)
- `<h2>` / `<h3>` — Proper heading hierarchy within sections
- `<nav>` — Navigation component
- `<footer>` — Footer component
- `aria-hidden="true"` — Used on decorative elements (noise overlay, lazy-load placeholders, dot matrix) so screen readers skip them

**SEO benefit:** Semantic HTML helps search engines understand the page structure, content hierarchy, and relative importance of different sections, directly improving content indexation quality.

---

### 12. Image Alt Text

**Where:** `Home.jsx` (Hero background, project cards), `ProjectDetail.jsx` (gallery images)

- Hero images: `alt="Hero Background"`
- Project card images: `alt={project.title}`
- Gallery images: `alt="{project.title} - render {i + 1}"`

**SEO benefit:** Descriptive alt text makes images indexable by Google Image Search and is a core web accessibility requirement.

---

### 13. Keyword-Rich Content Page

**Where:** `TechnicalProcess.jsx`

A dedicated long-form content page (~5,000+ words) targeting key search terms: **"Adithya Sridhar"**, **"Technical 3D Artist"**, **"PES University"**, **"PESU"**, and **"Bangalore"**. The keywords are systematically injected via a reusable `keywords` object and wrapped in `<strong>` tags for emphasis.

The page covers 9 topic sections with keyword-rich headings:

1. Introduction: Adithya Sridhar and the Technical Art Landscape in Bangalore
2. Advanced 3D Modeling Paradigms and Topology Optimization
3. The Art of PBR Texturing and Material Definition
4. Shader Development and Graphics Programming at PESU
5. Real-Time Rendering Engines: Unity and Unreal Engine
6. Web-Based 3D: React Three Fiber and WebGL
7. The PES University Influence on Adithya Sridhar's Career
8. Environment Design and Lighting Composition
9. Conclusion: The Future of Adithya Sridhar in Bangalore

**SEO benefit:** Long-form, keyword-dense content signals topical authority to search engines. The use of `<strong>` tags provides light semantic emphasis. Multiple heading levels with keywords improve the page's ranking for long-tail searches.

---

### 14. Performance Optimizations (Core Web Vitals)

Performance is a direct ranking factor. The project implements several techniques:

| Technique | Implementation | SEO Impact |
|-----------|---------------|------------|
| **Code splitting** | `React.lazy()` + `Suspense` for all page-level components (`Home`, `ProjectDetail`, `TechnicalProcess`) | Reduces initial bundle size, improving First Contentful Paint (FCP) and Largest Contentful Paint (LCP). |
| **Lazy-loaded images** | `LazyImage` component with `IntersectionObserver` and `decoding="async"` | Defers offscreen image loading, speeds up initial page load. |
| **Lazy-loaded videos** | `LazyVideo` component — `<video>` elements are not even mounted until in viewport | Prevents large video files from blocking page load. |
| **Font preconnect** | `<link rel="preconnect">` for Google Fonts domains | Eliminates DNS/TLS handshake delay for font resources. |
| **Vendor chunk splitting** | Manual chunks in Vite config (`vendor-react`, `vendor-anim`) | Enables long-term browser caching of stable vendor code. |
| **Asset hashing** | Vite's default content-hash filenames in production | Enables aggressive cache-control headers without stale content risk. |

**SEO benefit:** Google uses Core Web Vitals (LCP, FID/INP, CLS) as ranking signals. Faster pages rank higher and provide better user experience, reducing bounce rates.

---

### 15. SPA Routing with History API Fallback

**Where:** `vercel.json`, `App.jsx`

The Vercel rewrite rule sends all requests to `index.html`, allowing React Router to handle client-side navigation. The `ScrollToTop` component ensures consistent scroll position on navigation, and `history.scrollRestoration = 'manual'` prevents browser interference.

**SEO benefit:** Clean URLs (e.g. `/project/guitar` instead of `/#/project/guitar`) are more crawlable and user-friendly. Combined with `react-helmet-async`, each route presents unique meta tags to crawlers that execute JavaScript (Googlebot does).

---

### 16. External Link Security

**Where:** `Home.jsx` (Contact section), `ProjectDetail.jsx` (GitHub links)

All external links use:

```html
<a href="..." target="_blank" rel="noopener noreferrer">
```

**SEO benefit:** `rel="noopener noreferrer"` prevents the linked page from accessing the `window.opener` object (security) and avoids passing referrer information. While not a direct ranking signal, secure link handling contributes to overall site trust.

---

### Summary

The portfolio implements a multi-layered SEO strategy covering **on-page optimization** (titles, descriptions, headings, keywords, alt text), **technical SEO** (sitemap, robots.txt, canonical URLs, structured data, performance), and **social SEO** (Open Graph, Twitter Cards). These techniques work together to maximize discoverability across search engines and social platforms.
