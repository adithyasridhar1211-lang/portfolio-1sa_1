import { useEffect, lazy, Suspense } from 'react'
import { BrowserRouter as Router, Routes, Route, useLocation } from 'react-router-dom'
import { AnimatePresence } from 'framer-motion'
import { Analytics } from '@vercel/analytics/react'
import Lenis from 'lenis'
import Navbar from './components/Navbar'
import Footer from './components/Footer'
import DotMatrix from './components/DotMatrix'

const Home = lazy(() => import('./pages/Home'))
const ProjectDetail = lazy(() => import('./pages/ProjectDetail'))
const TechnicalProcess = lazy(() => import('./pages/TechnicalProcess'))

function ScrollToTop() {
    const { pathname } = useLocation()

    useEffect(() => {
        // Disable native browser scroll restoration to prevent conflicts
        if ('scrollRestoration' in history) {
            history.scrollRestoration = 'manual'
        }

        window.scrollTo(0, 0)

        if (window.lenis) {
            window.lenis.scrollTo(0, { immediate: true })
        }
    }, [pathname])

    return null
}

function SmoothScroll() {
    useEffect(() => {
        const lenis = new Lenis({
            duration: 1.2,
            easing: (t) => Math.min(1, 1.001 - Math.pow(2, -10 * t)),
            smooth: true,
        })

        window.lenis = lenis

        function raf(time) {
            lenis.raf(time)
            requestAnimationFrame(raf)
        }

        requestAnimationFrame(raf)

        return () => {
            lenis.destroy()
            window.lenis = null
        }
    }, [])

    return null
}

function LoadingFallback() {
    return (
        <div style={{
            height: '100vh',
            width: '100%',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            color: 'var(--accent)',
            fontSize: '1.5rem',
            fontFamily: 'var(--font-heading)'
        }}>
            Loading...
        </div>
    )
}

function App() {
    return (
        <Router>
            <ScrollToTop />
            <SmoothScroll />
            <div className="app">
                <DotMatrix />
                <div className="noise-overlay" aria-hidden="true" />
                <Navbar />
                <AnimatePresence mode="wait">
                    <Suspense fallback={<LoadingFallback />}>
                        <Routes>
                            <Route path="/" element={<Home />} />
                            <Route path="/project/:id" element={<ProjectDetail />} />
                            <Route path="/technical-process" element={<TechnicalProcess />} />
                        </Routes>
                    </Suspense>
                </AnimatePresence>
                <Footer />
                <Analytics />
            </div>
        </Router>
    )
}

export default App
