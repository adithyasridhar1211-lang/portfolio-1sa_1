# Deployment Guide: Adithya Sridhar Portfolio

This guide outlines the steps to deploy the portfolio website to **Vercel**. The project is built with **Vite + React** and includes specific configurations for SEO and routing.

## Prerequisites

1.  **Node.js** installed locally (to run builds if needed).
2.  A **GitHub** account.
3.  A **Vercel** account (connected to your GitHub).

## 1. Prepare Local Project

Ensure your project is clean and all changes are committed.

1.  Open your terminal in the project folder: `c:\Users\adith\OneDrive\Documents\a_g\projects\portfolio 1sa_1`
2.  Initialize Git (if not already done):
    ```bash
    git init
    git add .
    git commit -m "Ready for deployment"
    ```

## 2. Push to GitHub

1.  Go to [GitHub.com](https://github.com) and create a **New Repository**.
    *   Name it `portfolio-v1` (or similar).
    *   Make it **Public** (recommended for free Vercel tier) or Private.
    *   **Do not** initialize with README/gitignore (you already have them).
2.  Follow the instructions to push your local code:
    ```bash
    git remote add origin https://github.com/YOUR_USERNAME/portfolio-v1.git
    git branch -M main
    git push -u origin main
    ```

## 3. Deploy to Vercel

1.  Go to your [Vercel Dashboard](https://vercel.com/dashboard).
2.  Click **"Add New..."** -> **"Project"**.
3.  Select the `portfolio-v1` repository you just created.
4.  **Configure Project:**
    *   **Framework Preset:** `Vite` (Vercel should detect this automatically).
    *   **Root Directory:** `./` (default).
    *   **Build Command:** `npm run build`
        *   *Note:* This command runs `vite build && node scripts/generate-sitemap.js`, which automatically generates your `sitemap.xml` and `robots.txt`.
    *   **Output Directory:** `dist` (default).
    *   **Environment Variables:** None needed for this static site.
5.  Click **"Deploy"**.

## 4. Post-Deployment Verification

Once the deployment is complete, Vercel will give you a live URL (e.g., `https://portfolio-v1.vercel.app`).

**Check the following:**

1.  **Homepage:** Loads correctly with all animations.
2.  **Routing:** Click on a project, then **refresh the page**. It should **not** 404.
    *   *Why?* The `vercel.json` file in your project handles this rewrite rule.
3.  **SEO Page:** Go to `/technical-process` (or click "Process" in the footer).
    *   Verify the content loads.
4.  **Sitemap:** Go to `/sitemap.xml`.
    *   Verify it lists all your projects and the technical process page.

## Troubleshooting

*   **404 on Refresh:** Ensure `vercel.json` exists in the root with the `rewrites` configuration.
*   **Missing Assets:** If images/videos are missing, check that they are in the `public/portfolio` folder and referenced correctly in `projectData.js` (paths should start with `/portfolio`).
