import { useState, useEffect } from 'react'
import { useLocation } from 'react-router-dom'
import { motion, AnimatePresence } from 'framer-motion'
import './Navbar.css'

const navLinks = [
    { label: 'Home', href: '#hero' },
    { label: 'About', href: '#about' },
    { label: 'Skills', href: '#skills' },
    { label: 'Projects', href: '#projects' },
    { label: 'Contact', href: '#contact' },
]

function Navbar() {
    const [scrolled, setScrolled] = useState(false)
    const [menuOpen, setMenuOpen] = useState(false)
    const location = useLocation()

    useEffect(() => {
        const handleScroll = () => setScrolled(window.scrollY > 50)
        window.addEventListener('scroll', handleScroll)
        return () => window.removeEventListener('scroll', handleScroll)
    }, [])

    useEffect(() => {
        setMenuOpen(false)
    }, [location])

    const handleNavClick = (e, href) => {
        if (location.pathname !== '/') {
            return // Let the link navigate normally
        }
        e.preventDefault()
        const element = document.querySelector(href)
        if (element) {
            element.scrollIntoView({ behavior: 'smooth' })
            setMenuOpen(false)
        }
    }

    return (
        <motion.header
            className={`navbar ${scrolled ? 'navbar--scrolled' : ''}`}
            initial={{ y: -100 }}
            animate={{ y: 0 }}
            transition={{ duration: 0.6, ease: [0.16, 1, 0.3, 1] }}
        >
            <div className="navbar__inner container">
                <a href="/" className="navbar__logo">
                    <span className="navbar__logo-text">AS</span>
                    <span className="navbar__logo-dot" />
                </a>

                <nav className={`navbar__nav ${menuOpen ? 'navbar__nav--open' : ''}`}>
                    {navLinks.map((link, i) => (
                        <motion.a
                            key={link.label}
                            href={location.pathname === '/' ? link.href : `/${link.href}`}
                            className="navbar__link"
                            onClick={(e) => handleNavClick(e, link.href)}
                            initial={{ opacity: 0, y: -20 }}
                            animate={{ opacity: 1, y: 0 }}
                            transition={{ delay: 0.1 * i, duration: 0.4 }}
                        >
                            {link.label}
                        </motion.a>
                    ))}
                </nav>

                <button
                    className={`navbar__burger ${menuOpen ? 'navbar__burger--open' : ''}`}
                    onClick={() => setMenuOpen(!menuOpen)}
                    aria-label="Toggle menu"
                >
                    <span />
                    <span />
                    <span />
                </button>
            </div>
        </motion.header>
    )
}

export default Navbar
