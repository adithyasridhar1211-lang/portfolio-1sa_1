import { useState } from 'react'
import { useParams, Link } from 'react-router-dom'
import { Helmet } from 'react-helmet-async'
import { motion, AnimatePresence } from 'framer-motion'
import { projects } from '../data/projectData'

import LazyImage from '../components/LazyImage'
import LazyVideo from '../components/LazyVideo'
import './ProjectDetail.css'

const categoryLabels = {
    modelling: 'Modelling',
    texturing: 'Texturing',
    environments: 'Environment Design',
    coding: 'Coding',
    shading: 'Shading & Post Process',
}

function ProjectDetail() {
    const { id } = useParams()
    const project = projects.find((p) => p.id === id)
    const [lightboxImage, setLightboxImage] = useState(null)

    if (!project) {
        return (
            <main className="project-detail">
                <div className="container" style={{ paddingTop: '150px', textAlign: 'center' }}>
                    <h1>Project not found</h1>
                    <Link to="/" className="btn btn-outline" style={{ marginTop: '2rem' }}>
                        ← Back to Home
                    </Link>
                </div>
            </main>
        )
    }

    return (
        <motion.main
            className="project-detail"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            transition={{ duration: 0.4 }}
        >
            <Helmet>
                <title>{project.title} | Adithya Sridhar</title>
                <meta name="description" content={project.brief || project.description} />
                <link rel="canonical" href={`https://adithyasridhar.com/project/${project.id}`} />

                {/* Open Graph */}
                <meta property="og:title" content={`${project.title} | Adithya Sridhar`} />
                <meta property="og:description" content={project.brief || project.description} />
                <meta property="og:url" content={`https://adithyasridhar.com/project/${project.id}`} />
                <meta property="og:site_name" content="Adithya Sridhar Portfolio" />
                <meta property="og:type" content="article" />
                {project.thumbnail && <meta property="og:image" content={`https://adithyasridhar.com${project.thumbnail}`} />}

                {/* Twitter */}
                <meta name="twitter:card" content="summary_large_image" />
                <meta name="twitter:title" content={project.title} />
                <meta name="twitter:description" content={project.brief || project.description} />
                {project.thumbnail && <meta name="twitter:image" content={`https://adithyasridhar.com${project.thumbnail}`} />}

                {/* Structured Data */}
                <script type="application/ld+json">
                    {JSON.stringify({
                        "@context": "https://schema.org",
                        "@type": "CreativeWork",
                        "name": project.title,
                        "description": project.brief || project.description,
                        "image": project.thumbnail ? `https://adithyasridhar.com${project.thumbnail}` : undefined,
                        "url": `https://adithyasridhar.com/project/${project.id}`,
                        "author": {
                            "@type": "Person",
                            "name": "Adithya Sridhar"
                        },
                        "genre": categoryLabels[project.category],
                        "keywords": project.tools.join(", ")
                    })}
                </script>
            </Helmet>
            <div className="container">
                {/* Header */}
                <div className="project-detail__header">
                    <Link to="/#projects" className="project-detail__back">
                        ← Back to Projects
                    </Link>
                    <motion.div
                        initial={{ opacity: 0, y: 30 }}
                        animate={{ opacity: 1, y: 0 }}
                        transition={{ duration: 0.6, delay: 0.1 }}
                    >
                        <span className="project-detail__category">
                            {categoryLabels[project.category]}
                        </span>
                        <h1 className="project-detail__title">{project.title}</h1>
                        <p className="project-detail__description">{project.description}</p>
                        <div className="project-detail__tools">
                            {project.tools.map((tool) => (
                                <span key={tool} className="project-detail__tool">{tool}</span>
                            ))}
                        </div>
                    </motion.div>
                </div>



                {/* Videos */}
                {project.videos && project.videos.length > 0 && (
                    <motion.div
                        className="project-detail__section"
                        initial={{ opacity: 0, y: 30 }}
                        animate={{ opacity: 1, y: 0 }}
                        transition={{ duration: 0.6, delay: 0.3 }}
                    >
                        <h2 className="project-detail__section-title">
                            {project.category === 'texturing' ? 'Animated Textures' :
                                project.category === 'shading' ? 'Shader Demo' : 'Video'}
                        </h2>
                        <div className="project-detail__videos">
                            {project.videos.map((video, i) => (
                                <div key={i} className="project-detail__video-wrapper">
                                    <LazyVideo
                                        src={video}
                                        className="project-detail__video"
                                    />
                                </div>
                            ))}
                        </div>
                    </motion.div>
                )}

                {/* Image Gallery */}
                {project.images && project.images.length > 0 && (
                    <motion.div
                        className="project-detail__section"
                        initial={{ opacity: 0, y: 30 }}
                        animate={{ opacity: 1, y: 0 }}
                        transition={{ duration: 0.6, delay: 0.4 }}
                    >
                        <h2 className="project-detail__section-title">Gallery</h2>
                        <div className="project-detail__gallery">
                            {project.images.map((img, i) => (
                                <motion.div
                                    key={i}
                                    className="project-detail__gallery-item"
                                    onClick={() => setLightboxImage(img)}
                                    whileHover={{ scale: 1.02 }}
                                    transition={{ duration: 0.2 }}
                                >
                                    <LazyImage
                                        src={img}
                                        alt={`${project.title} - render ${i + 1}`}
                                    />
                                </motion.div>
                            ))}
                        </div>
                    </motion.div>
                )}

                {/* Documentation */}
                {project.documentation && project.documentation.length > 0 && (
                    <motion.div
                        className="project-detail__section"
                        initial={{ opacity: 0, y: 30 }}
                        animate={{ opacity: 1, y: 0 }}
                        transition={{ duration: 0.6, delay: 0.45 }}
                    >
                        <h2 className="project-detail__section-title">Project Documentation</h2>
                        <div className="project-detail__docs">
                            {project.documentation.map((doc, i) => (
                                <motion.div
                                    key={i}
                                    className="project-detail__doc-block"
                                    initial={{ opacity: 0, y: 20 }}
                                    animate={{ opacity: 1, y: 0 }}
                                    transition={{ duration: 0.5, delay: 0.5 + i * 0.08 }}
                                >
                                    <div className="project-detail__doc-marker">
                                        <span className="project-detail__doc-number">{String(i + 1).padStart(2, '0')}</span>
                                    </div>
                                    <div className="project-detail__doc-content">
                                        <h3 className="project-detail__doc-title">{doc.title}</h3>
                                        <p className="project-detail__doc-text">{doc.content}</p>
                                        {doc.code && (
                                            <div className="project-detail__code-block">
                                                <div className="project-detail__code-header">
                                                    <span className="project-detail__code-lang">{doc.code.language}</span>
                                                    <span className="project-detail__code-label">{doc.code.label}</span>
                                                </div>
                                                <pre className="project-detail__code-pre"><code>{doc.code.snippet}</code></pre>
                                            </div>
                                        )}
                                    </div>
                                </motion.div>
                            ))}
                        </div>
                    </motion.div>
                )}
                {project.github && (
                    <motion.div
                        className="project-detail__section"
                        initial={{ opacity: 0, y: 30 }}
                        animate={{ opacity: 1, y: 0 }}
                        transition={{ duration: 0.6, delay: 0.5 }}
                    >
                        <a
                            href={project.github}
                            target="_blank"
                            rel="noopener noreferrer"
                            className="btn btn-primary"
                        >
                            View on GitHub →
                        </a>
                    </motion.div>
                )}

                {/* Next / Prev Navigation */}
                <motion.div
                    className="project-detail__nav"
                    initial={{ opacity: 0 }}
                    animate={{ opacity: 1 }}
                    transition={{ delay: 0.6 }}
                >
                    {(() => {
                        const currentIndex = projects.findIndex((p) => p.id === id)
                        const prev = currentIndex > 0 ? projects[currentIndex - 1] : null
                        const next = currentIndex < projects.length - 1 ? projects[currentIndex + 1] : null
                        return (
                            <>
                                {prev ? (
                                    <Link to={`/project/${prev.id}`} className="project-detail__nav-link">
                                        <span className="project-detail__nav-label">← Previous</span>
                                        <span className="project-detail__nav-title">{prev.title}</span>
                                    </Link>
                                ) : <div />}
                                {next ? (
                                    <Link to={`/project/${next.id}`} className="project-detail__nav-link project-detail__nav-link--next">
                                        <span className="project-detail__nav-label">Next →</span>
                                        <span className="project-detail__nav-title">{next.title}</span>
                                    </Link>
                                ) : <div />}
                            </>
                        )
                    })()}
                </motion.div>
            </div>

            {/* Lightbox */}
            <AnimatePresence>
                {lightboxImage && (
                    <motion.div
                        className="lightbox"
                        initial={{ opacity: 0 }}
                        animate={{ opacity: 1 }}
                        exit={{ opacity: 0 }}
                        onClick={() => setLightboxImage(null)}
                    >
                        <button className="lightbox__close" onClick={() => setLightboxImage(null)}>✕</button>
                        <motion.img
                            src={lightboxImage}
                            alt="Full size"
                            className="lightbox__image"
                            initial={{ scale: 0.9, opacity: 0 }}
                            animate={{ scale: 1, opacity: 1 }}
                            exit={{ scale: 0.9, opacity: 0 }}
                            transition={{ duration: 0.3 }}
                        />
                    </motion.div>
                )}
            </AnimatePresence>
        </motion.main>
    )
}

export default ProjectDetail
