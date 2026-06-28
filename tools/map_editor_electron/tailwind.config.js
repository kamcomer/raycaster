/** @type {import('tailwindcss').Config} */
export default {
  content: ['./index.html', './src/**/*.{js,ts,jsx,tsx}'],
  theme: {
    extend: {
      colors: {
        surface: {
          DEFAULT: 'var(--color-surface)',
          dark: 'var(--color-surface-dark)',
          canvas: 'var(--color-surface-canvas)',
        },
        muted: {
          DEFAULT: 'var(--color-muted)',
          hover: 'var(--color-muted-hover)',
        },
        accent: {
          DEFAULT: 'var(--color-accent)',
          hover: 'var(--color-accent-hover)',
        },
        primary: 'var(--color-primary)',
      },
    },
  },
  plugins: [],
}
