import { useEffect, useRef } from 'react'

function DotMatrix() {
    const canvasRef = useRef(null)
    const mouse = useRef({ x: -1000, y: -1000 })
    const animRef = useRef(null)

    useEffect(() => {
        const canvas = canvasRef.current
        if (!canvas) return
        // desynchronized hint reduces input-to-display latency
        // willReadFrequently hint optimizations for software rendering if needed
        const ctx = canvas.getContext('2d', { alpha: true, desynchronized: true })

        // --- Config ---
        const isMobile = window.innerWidth < 768
        const SPACING = isMobile ? 48 : 32
        const DOT_RADIUS = isMobile ? 1.0 : 1.2
        const GLOW_RADIUS = 4
        const INFLUENCE = isMobile ? 200 : 280
        const INFLUENCE_SQ = INFLUENCE * INFLUENCE
        const LINE_DISTANCE = (isMobile ? 48 : 56) * 1.5
        const LINE_DISTANCE_SQ = LINE_DISTANCE * LINE_DISTANCE
        const BASE_ALPHA = 0.15
        const GLOW_ALPHA = 0.7
        const LINE_ALPHA = 0.15
        const COLOR = [124, 58, 237]
        const LERP_RATE = 8 // units per second (frame-rate independent)

        let dotRadii = null   // Float32Arrays for perf
        let dotAlphas = null
        let dotsX = null
        let dotsY = null
        let dotCount = 0
        let W, H
        let lastTime = 0

        function buildGrid() {
            W = canvas.width = window.innerWidth
            H = canvas.height = window.innerHeight

            // Re-check mobile on resize
            const isMob = window.innerWidth < 768
            const spacing = isMob ? 48 : 32

            const cols = Math.ceil(W / spacing) + 1
            const rows = Math.ceil(H / spacing) + 1
            dotCount = cols * rows

            dotsX = new Float32Array(dotCount)
            dotsY = new Float32Array(dotCount)
            dotRadii = new Float32Array(dotCount)
            dotAlphas = new Float32Array(dotCount)

            let idx = 0
            for (let r = 0; r < rows; r++) {
                for (let c = 0; c < cols; c++) {
                    dotsX[idx] = c * spacing
                    dotsY[idx] = r * spacing
                    dotRadii[idx] = DOT_RADIUS
                    dotAlphas[idx] = BASE_ALPHA
                    idx++
                }
            }
        }

        buildGrid()
        window.addEventListener('resize', buildGrid)

        function handleMouseMove(e) {
            mouse.current.x = e.clientX
            mouse.current.y = e.clientY
        }

        function handleMouseLeave() {
            mouse.current.x = -1000
            mouse.current.y = -1000
        }

        window.addEventListener('mousemove', handleMouseMove, { passive: true })
        document.addEventListener('mouseleave', handleMouseLeave)

        function draw(timestamp) {
            // Delta-time for frame-rate independent animation
            const dt = lastTime ? Math.min((timestamp - lastTime) / 1000, 0.05) : 0.016
            lastTime = timestamp
            const lerpFactor = 1 - Math.exp(-LERP_RATE * dt)

            ctx.clearRect(0, 0, W, H)

            const mx = mouse.current.x
            const my = mouse.current.y

            // Collect active dots for line drawing
            const activeX = []
            const activeY = []
            const activeStrength = []

            for (let i = 0; i < dotCount; i++) {
                const x = dotsX[i]
                const y = dotsY[i]
                const dx = x - mx
                const dy = y - my
                const distSq = dx * dx + dy * dy

                let targetRadius = DOT_RADIUS
                let targetAlpha = BASE_ALPHA

                if (distSq < INFLUENCE_SQ) {
                    const dist = Math.sqrt(distSq)
                    const t = 1 - dist / INFLUENCE
                    const ease = t * t * (3 - 2 * t)
                    targetRadius = DOT_RADIUS + (GLOW_RADIUS - DOT_RADIUS) * ease
                    targetAlpha = BASE_ALPHA + (GLOW_ALPHA - BASE_ALPHA) * ease
                }

                // Frame-rate independent lerp
                const curR = dotRadii[i] + (targetRadius - dotRadii[i]) * lerpFactor
                const curA = dotAlphas[i] + (targetAlpha - dotAlphas[i]) * lerpFactor
                dotRadii[i] = curR
                dotAlphas[i] = curA

                ctx.beginPath()
                ctx.arc(x, y, curR, 0, Math.PI * 2)
                ctx.fillStyle = `rgba(${COLOR[0]},${COLOR[1]},${COLOR[2]},${curA})`
                ctx.fill()

                if (curA > BASE_ALPHA + 0.03) {
                    activeX.push(x)
                    activeY.push(y)
                    activeStrength.push(curA)
                }
            }

            // Connection lines between nearby active dots
            const aLen = activeX.length
            ctx.lineWidth = 1
            for (let i = 0; i < aLen; i++) {
                for (let j = i + 1; j < aLen; j++) {
                    const dx = activeX[i] - activeX[j]
                    const dy = activeY[i] - activeY[j]
                    const distSq = dx * dx + dy * dy

                    if (distSq < LINE_DISTANCE_SQ) {
                        const t = 1 - Math.sqrt(distSq) / LINE_DISTANCE
                        const minStr = Math.min(activeStrength[i], activeStrength[j]) / GLOW_ALPHA
                        ctx.beginPath()
                        ctx.moveTo(activeX[i], activeY[i])
                        ctx.lineTo(activeX[j], activeY[j])
                        ctx.strokeStyle = `rgba(${COLOR[0]},${COLOR[1]},${COLOR[2]},${LINE_ALPHA * t * minStr})`
                        ctx.stroke()
                    }
                }
            }

            animRef.current = requestAnimationFrame(draw)
        }

        animRef.current = requestAnimationFrame(draw)

        return () => {
            cancelAnimationFrame(animRef.current)
            window.removeEventListener('resize', buildGrid)
            window.removeEventListener('mousemove', handleMouseMove)
            document.removeEventListener('mouseleave', handleMouseLeave)
        }
    }, [])

    return (
        <canvas
            ref={canvasRef}
            style={{
                position: 'fixed',
                top: 0,
                left: 0,
                width: '100%',
                height: '100%',
                pointerEvents: 'none',
                zIndex: 0,
            }}
            aria-hidden="true"
        />
    )
}

export default DotMatrix
