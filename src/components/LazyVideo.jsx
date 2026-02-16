import { useState, useRef, useEffect } from 'react'

/**
 * LazyVideo — mounts the <video> element only when it scrolls into view.
 * Uses preload="none" so metadata isn't fetched until visible, then
 * switches to preload="metadata" once in view.
 */
function LazyVideo({ src, className, type = 'video/mp4', poster }) {
    const wrapperRef = useRef(null)
    const [isVisible, setIsVisible] = useState(false)

    useEffect(() => {
        const el = wrapperRef.current
        if (!el) return

        if (!('IntersectionObserver' in window)) {
            setIsVisible(true)
            return
        }

        const observer = new IntersectionObserver(
            ([entry]) => {
                if (entry.isIntersecting) {
                    setIsVisible(true)
                    observer.disconnect()
                }
            },
            { rootMargin: '100px' }
        )

        observer.observe(el)
        return () => observer.disconnect()
    }, [])

    // Detect if the source file is a web-playable format
    const ext = src?.split('.').pop()?.toLowerCase()
    const isPlayable = ['mp4', 'webm', 'ogg'].includes(ext)
    const mimeType = ext === 'webm' ? 'video/webm' : ext === 'ogg' ? 'video/ogg' : type

    if (!isPlayable) {
        return (
            <div ref={wrapperRef} className={`lazy-video-wrapper ${className || ''}`}>
                <div className="lazy-video-unsupported">
                    <span className="lazy-video-unsupported__icon">🎬</span>
                    <p>Video format <strong>.{ext}</strong> is not supported in browsers.</p>
                    <a href={src} download className="btn btn-outline btn-sm">
                        Download Video
                    </a>
                </div>
            </div>
        )
    }

    return (
        <div ref={wrapperRef} className={`lazy-video-wrapper ${className || ''}`}>
            {isVisible ? (
                <video
                    controls
                    loop
                    muted
                    playsInline
                    preload="metadata"
                    poster={poster}
                    className="lazy-video"
                >
                    <source src={src} type={mimeType} />
                    Your browser does not support the video tag.
                </video>
            ) : (
                <div className="lazy-video-placeholder" aria-hidden="true">
                    <div className="lazy-video-placeholder__icon">▶</div>
                </div>
            )}
        </div>
    )
}

export default LazyVideo
