import { Link } from 'react-router-dom'
import { personalInfo } from '../data/projectData'
import './Footer.css'

function Footer() {
    return (
        <footer className="footer">
            <div className="footer__inner container">
                <div className="footer__top">
                    <div className="footer__brand">
                        <span className="footer__logo gradient-text">{personalInfo.firstName}.</span>
                        <p className="footer__tagline">{personalInfo.role}</p>
                    </div>
                    <div className="footer__links">
                        <a href={personalInfo.linkedin} target="_blank" rel="noopener noreferrer" className="footer__link">LinkedIn</a>
                        <a href={personalInfo.github} target="_blank" rel="noopener noreferrer" className="footer__link">GitHub</a>
                        <a href={`mailto:${personalInfo.email}`} className="footer__link">Email</a>
                        <Link to="/technical-process" className="footer__link">Process</Link>
                    </div>
                </div>
                <div className="footer__bottom">
                    <p>&copy; {new Date().getFullYear()} {personalInfo.name}. All rights reserved.</p>
                    <p className="footer__seo-text">
                        Technical 3D Artist | {personalInfo.university} | {personalInfo.location}
                    </p>
                </div>
            </div>
        </footer>
    )
}

export default Footer
