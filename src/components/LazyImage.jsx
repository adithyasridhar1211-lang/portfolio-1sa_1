import { useState, useRef, useEffect } from 'react'

/**
 * LazyImage — loads images only when they enter the viewport.
 * Shows a subtle shimmer placeholder until the image is decoded.
 */
function LazyImage({ src, alt, className, style, onClick }) {
    const imgRef = useRef(null)
    const [isVisible, setIsVisible] = useState(false)
    const [isLoaded, setIsLoaded] = useState(false)

    useEffect(() => {
        const el = imgRef.current
        if (!el) return

        // Skip IntersectionObserver if not supported (SSR, old browsers)
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
            { rootMargin: '200px' } // start loading 200px before visible
        )

        observer.observe(el)
        return () => observer.disconnect()
    }, [])

    return (
        <div
            ref={imgRef}
            className={`lazy-image-wrapper ${isLoaded ? 'lazy-image--loaded' : ''}`}
            style={style}
            onClick={onClick}
        >
            {isVisible && (
                <img
                    src={src}
                    alt={alt || ''}
                    className={className}
                    decoding="async"
                    onLoad={() => setIsLoaded(true)}
                />
            )}
            {!isLoaded && <div className="lazy-image-placeholder" aria-hidden="true" />}
        </div>
    )
}

export default LazyImage
