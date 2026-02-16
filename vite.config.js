import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

export default defineConfig({
  plugins: [react()],
  server: {
    port: 3000,
    open: true,
  },
  assetsInclude: ['**/*.glb', '**/*.gltf', '**/*.mp4', '**/*.webm', '**/*.mov', '**/*.avi'],
  build: {
    chunkSizeWarningLimit: 1500,
    rollupOptions: {
      output: {
        manualChunks: {
          'vendor-react': ['react', 'react-dom', 'react-router-dom'],
          'vendor-anim': ['framer-motion', 'gsap'],
        },
      },
    },
  },
})
