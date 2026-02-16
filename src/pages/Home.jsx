import { useState, useEffect, useRef } from 'react'
import { Link } from 'react-router-dom'
import { Helmet } from 'react-helmet-async'
import { motion, useInView, AnimatePresence, useScroll, useTransform } from 'framer-motion'
import { gsap } from 'gsap'
import { ScrollTrigger } from 'gsap/ScrollTrigger'
import { projects, categories, skills, personalInfo } from '../data/projectData'
import LazyImage from '../components/LazyImage'
import './Home.css'

gsap.registerPlugin(ScrollTrigger)

/* ─── Animation Variants ──────────────────────────── */
const fadeUp = {
    hidden: { opacity: 0, y: 40 },
    visible: (i = 0) => ({
        opacity: 1,
        y: 0,
        transition: { duration: 0.6, delay: i * 0.1, ease: [0.16, 1, 0.3, 1] },
    }),
}

const stagger = {
    visible: { transition: { staggerChildren: 0.08 } },
}

/* ─── Section Wrapper with InView ─────────────────── */
function AnimatedSection({ children, className, id }) {
    const ref = useRef(null)
    const isInView = useInView(ref, { once: true, margin: '-100px' })

    return (
        <motion.section
            ref={ref}
            id={id}
            className={`section ${className || ''}`}
            initial="hidden"
            animate={isInView ? 'visible' : 'hidden'}
            variants={stagger}
        >
            {children}
        </motion.section>
    )
}



/* ─── HERO BACKGROUND ──────────────────────────────── */
const heroImages = [
    '/portfolio/hero-section/render%206.jpg',
    '/portfolio/hero-section/v6%20render%202.jpg',
    '/portfolio/hero-section/v6%20render%204.jpg',
]

function HeroBackground() {
    const [index, setIndex] = useState(0)
    const { scrollY } = useScroll()
    const y = useTransform(scrollY, [0, 1000], [0, 400]) // Parallax effect: moves slower than scroll

    useEffect(() => {
        const timer = setInterval(() => {
            setIndex((prev) => (prev + 1) % heroImages.length)
        }, 6000)
        return () => clearInterval(timer)
    }, [])

    return (
        <div className="hero__background">
            <AnimatePresence mode="wait">
                <motion.img
                    key={index}
                    src={heroImages[index]}
                    alt="Hero Background"
                    className="hero__bg-image"
                    style={{ y }}
                    initial={{ opacity: 0, scale: 1.1 }}
                    animate={{ opacity: 1, scale: 1 }}
                    exit={{ opacity: 0 }}
                    transition={{ duration: 1.5, ease: 'easeOut' }}
                />
            </AnimatePresence>
            <div className="hero__overlay" />
        </div>
    )
}

/* ─── HERO SECTION ────────────────────────────────── */
function Hero() {
    const heroRef = useRef(null)

    useEffect(() => {
        const ctx = gsap.context(() => {
            gsap.fromTo('.hero__title-line',
                { y: 80, opacity: 0 },
                { y: 0, opacity: 1, duration: 1, stagger: 0.15, ease: 'power3.out', delay: 0.5 }
            )
            gsap.fromTo('.hero__subtitle',
                { y: 30, opacity: 0 },
                { y: 0, opacity: 1, duration: 0.8, delay: 1.1, ease: 'power3.out' }
            )
            gsap.fromTo('.hero__cta',
                { y: 30, opacity: 0 },
                { y: 0, opacity: 1, duration: 0.8, delay: 1.3, ease: 'power3.out' }
            )
            gsap.fromTo('.hero__scroll-indicator',
                { opacity: 0 },
                { opacity: 1, delay: 1.8, duration: 0.6 }
            )
        }, heroRef)

        return () => ctx.revert()
    }, [])

    return (
        <section id="hero" className="hero" ref={heroRef}>
            <HeroBackground />

            <div className="hero__content container">
                <div className="hero__text">
                    <p className="hero__badge">
                        <span className="hero__badge-dot" />
                        Available for opportunities
                    </p>
                    <h1 className="hero__title">
                        <span className="hero__title-line">Hi, I'm <span className="gradient-text">{personalInfo.firstName}</span></span>
                        <span className="hero__title-line">{personalInfo.role}</span>
                    </h1>
                    <p className="hero__subtitle">
                        I craft immersive 3D worlds, photorealistic renders, and custom technical solutions —
                        from modelling and texturing to shader programming and physics simulations.
                    </p>
                    <div className="hero__cta">
                        <a href="#projects" className="btn btn-primary">View My Work</a>
                        <a href="#contact" className="btn btn-outline">Get in Touch</a>
                    </div>
                </div>
            </div>

            <div className="hero__scroll-indicator">
                <div className="hero__scroll-line" />
                <span>Scroll</span>
            </div>
        </section>
    )
}

/* ─── ABOUT SECTION ───────────────────────────────── */
function About() {
    return (
        <AnimatedSection id="about" className="about">
            <div className="container">
                <motion.div variants={fadeUp} className="section-label">About Me</motion.div>
                <motion.h2 variants={fadeUp} custom={1} className="section-title">
                    Bridging Art & Engineering
                </motion.h2>
                <div className="about__grid">
                    <motion.div variants={fadeUp} custom={2} className="about__bio">
                        <p>{personalInfo.bio}</p>
                    </motion.div>
                    <motion.div variants={fadeUp} custom={3} className="about__stats">
                        <div className="about__stat">
                            <span className="about__stat-number gradient-text">13+</span>
                            <span className="about__stat-label">Projects</span>
                        </div>
                        <div className="about__stat">
                            <span className="about__stat-number gradient-text">5</span>
                            <span className="about__stat-label">Domains</span>
                        </div>
                        <div className="about__stat">
                            <span className="about__stat-number gradient-text">10+</span>
                            <span className="about__stat-label">Tools Mastered</span>
                        </div>
                    </motion.div>
                </div>
            </div>
        </AnimatedSection>
    )
}

/* ─── SKILLS SECTION ──────────────────────────────── */
function Skills() {
    return (
        <AnimatedSection id="skills" className="skills-section">
            <div className="container">
                <motion.div variants={fadeUp} className="section-label">Expertise</motion.div>
                <motion.h2 variants={fadeUp} custom={1} className="section-title">
                    Skills & Specializations
                </motion.h2>
                <div className="skills__grid">
                    {skills.map((skill, i) => (
                        <motion.div
                            key={skill.title}
                            variants={fadeUp}
                            custom={i + 2}
                            className="skill-card glass-card"
                        >
                            <span className="skill-card__icon">{skill.icon}</span>
                            <h3 className="skill-card__title">{skill.title}</h3>
                            <p className="skill-card__desc">{skill.description}</p>
                            <div className="skill-card__tools">
                                {skill.tools.map((tool) => (
                                    <span key={tool} className="skill-card__tool">{tool}</span>
                                ))}
                            </div>
                        </motion.div>
                    ))}
                </div>
            </div>
        </AnimatedSection>
    )
}

/* ─── PROJECTS SECTION ────────────────────────────── */
function Projects() {
    const [activeCategory, setActiveCategory] = useState('all')

    const filtered = activeCategory === 'all'
        ? projects
        : projects.filter((p) => p.category === activeCategory)

    return (
        <AnimatedSection id="projects" className="projects-section">
            <div className="container">
                <motion.div variants={fadeUp} className="section-label">Portfolio</motion.div>
                <motion.h2 variants={fadeUp} custom={1} className="section-title">
                    Featured Projects
                </motion.h2>
                <motion.p variants={fadeUp} custom={2} className="section-subtitle">
                    A selection of work spanning modelling, texturing, environments, coding, and shader development.
                </motion.p>

                {/* Category Filters */}
                <motion.div variants={fadeUp} custom={3} className="projects__filters">
                    {categories.map((cat) => (
                        <button
                            key={cat.id}
                            className={`projects__filter ${activeCategory === cat.id ? 'projects__filter--active' : ''}`}
                            onClick={() => setActiveCategory(cat.id)}
                        >
                            {cat.label}
                        </button>
                    ))}
                </motion.div>

                {/* Project Grid */}
                <motion.div layout className="projects__grid">
                    {filtered.map((project, i) => (
                        <ProjectCard key={project.id} project={project} index={i} />
                    ))}
                </motion.div>
            </div>
        </AnimatedSection>
    )
}

function ProjectCard({ project, index }) {
    const categoryLabels = {
        modelling: 'Modelling',
        texturing: 'Texturing',
        environments: 'Environment Design',
        coding: 'Coding',
        shading: 'Shading',
    }

    const thumbnailSrc = project.thumbnail || (project.images && project.images[0]) || null

    return (
        <motion.div
            layout
            initial={{ opacity: 0, y: 30 }}
            animate={{ opacity: 1, y: 0 }}
            exit={{ opacity: 0, y: 30 }}
            transition={{ duration: 0.4, delay: index * 0.05 }}
        >
            <Link to={`/project/${project.id}`} className="project-card glass-card">
                <div className="project-card__image-wrapper">
                    {thumbnailSrc ? (
                        <LazyImage
                            src={thumbnailSrc}
                            alt={project.title}
                            className="project-card__image"
                        />
                    ) : (
                        <div className="project-card__placeholder">
                            <span>🎬</span>
                        </div>
                    )}
                    <div className="project-card__overlay">
                        <span className="project-card__view">View Project →</span>
                    </div>
                </div>
                <div className="project-card__info">
                    <span className="project-card__category">{categoryLabels[project.category]}</span>
                    <h3 className="project-card__title">{project.title}</h3>
                    <div className="project-card__tools">
                        {project.tools.slice(0, 3).map((tool) => (
                            <span key={tool} className="project-card__tool">{tool}</span>
                        ))}
                    </div>
                </div>
            </Link>
        </motion.div>
    )
}

/* ─── CONTACT SECTION ─────────────────────────────── */
function Contact() {
    return (
        <AnimatedSection id="contact" className="contact-section">
            <div className="container">
                <div className="contact__inner">
                    <motion.div variants={fadeUp} className="section-label">Get in Touch</motion.div>
                    <motion.h2 variants={fadeUp} custom={1} className="section-title">
                        Let's Work Together
                    </motion.h2>
                    <motion.p variants={fadeUp} custom={2} className="section-subtitle">
                        I'm always open to discussing new projects, creative ideas, or opportunities
                        to be part of something great. Whether it's game development, VFX, or technical art —
                        let's connect.
                    </motion.p>
                    <motion.div variants={fadeUp} custom={3} className="contact__buttons">
                        <a href={`mailto:${personalInfo.email}`} className="btn btn-primary">
                            Send an Email
                        </a>
                        <a href={personalInfo.linkedin} target="_blank" rel="noopener noreferrer" className="btn btn-outline">
                            Connect on LinkedIn
                        </a>
                    </motion.div>
                    <motion.div variants={fadeUp} custom={4} className="contact__info">
                        <div className="contact__info-item">
                            <span className="contact__info-label">Location</span>
                            <span className="contact__info-value">{personalInfo.location}</span>
                        </div>
                        <div className="contact__info-item">
                            <span className="contact__info-label">University</span>
                            <span className="contact__info-value">{personalInfo.university}</span>
                        </div>
                    </motion.div>
                </div>
            </div>
        </AnimatedSection>
    )
}

/* ─── HOME PAGE ───────────────────────────────────── */
function Home() {
    return (
        <motion.main
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            transition={{ duration: 0.4 }}
        >
            <Helmet>
                <title>Adithya Sridhar | 3D Artist & Developer</title>
                <meta
                    name="description"
                    content="Portfolio of Adithya Sridhar, a Technical 3D Artist and Developer specializing in 3D modeling, texturing, and interactive web experiences."
                />
                <link rel="canonical" href="https://adithyasridhar.com/" />

                {/* Open Graph */}
                <meta property="og:title" content="Adithya Sridhar | 3D Artist & Developer" />
                <meta property="og:description" content="Portfolio of Adithya Sridhar — Technical 3D Artist specializing in 3D modelling, texturing, custom shaders, and coding for games." />
                <meta property="og:url" content="https://adithyasridhar.com/" />
                <meta property="og:site_name" content="Adithya Sridhar Portfolio" />
                <meta property="og:image" content="https://adithyasridhar.com/og-image.jpg" /> {/* Placeholder */}
                <meta property="og:type" content="website" />

                {/* Twitter */}
                <meta name="twitter:card" content="summary_large_image" />
                <meta name="twitter:title" content="Adithya Sridhar | 3D Artist & Developer" />
                <meta name="twitter:description" content="Technical 3D Artist specializing in 3D modelling, texturing, custom shaders, and coding." />
                <meta name="twitter:image" content="https://adithyasridhar.com/og-image.jpg" />
            </Helmet>
            <Hero />
            <About />
            <Skills />
            <Projects />
            <Contact />
        </motion.main>
    )
}

export default Home
