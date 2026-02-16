import fs from 'fs'
import path from 'path'
import { fileURLToPath } from 'url'
import { projects } from '../src/data/projectData.js'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

const BASE_URL = 'https://adithyasridhar.com'
const OUT_DIR = path.resolve(__dirname, '../dist')

function generateSitemap() {
    console.log('Generating sitemap...')

    const staticPages = [
        { url: '/', changefreq: 'monthly', priority: 1.0 },
        { url: '/technical-process', changefreq: 'monthly', priority: 0.7 },
    ]

    const projectPages = projects.map((project) => ({
        url: `/project/${project.id}`,
        changefreq: 'monthly',
        priority: 0.8,
    }))

    const allPages = [...staticPages, ...projectPages]

    const sitemap = `<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
${allPages
            .map((page) => {
                return `  <url>
    <loc>${BASE_URL}${page.url}</loc>
    <changefreq>${page.changefreq}</changefreq>
    <priority>${page.priority}</priority>
  </url>`
            })
            .join('\n')}
</urlset>`

    if (!fs.existsSync(OUT_DIR)) {
        fs.mkdirSync(OUT_DIR, { recursive: true })
    }

    fs.writeFileSync(path.join(OUT_DIR, 'sitemap.xml'), sitemap)
    console.log(`Sitemap generated at ${path.join(OUT_DIR, 'sitemap.xml')}`)
}

function generateRobots() {
    console.log('Generating robots.txt...')

    const robots = `User-agent: *
Allow: /

Sitemap: ${BASE_URL}/sitemap.xml
`

    fs.writeFileSync(path.join(OUT_DIR, 'robots.txt'), robots)
    console.log(`Robots.txt generated at ${path.join(OUT_DIR, 'robots.txt')}`)
}

try {
    generateSitemap()
    generateRobots()
} catch (error) {
    console.error('Error generating SEO files:', error)
    process.exit(1)
}
