import { useEffect } from 'react'
import Toolbar from './components/Toolbar'
import GridEditor from './components/GridEditor'
import TexturePalette from './components/TexturePalette'
import SpritePanel from './components/SpritePanel'
import { useMapStore } from './store/mapStore'

export default function App() {
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      const store = useMapStore.getState()
      const meta = e.ctrlKey || e.metaKey

      if (meta && e.key === 'n') {
        e.preventDefault()
        store.setShowNewDialog(true)
        return
      }
      if (meta && e.key === 's') {
        e.preventDefault()
        const btn = document.querySelector('[data-action="save"]') as HTMLButtonElement
        btn?.click()
        return
      }
      if (meta && e.key === 'o') {
        e.preventDefault()
        const btn = document.querySelector('[data-action="open"]') as HTMLButtonElement
        btn?.click()
        return
      }

      if (!meta && !e.altKey && !e.shiftKey) {
        if (e.key >= '1' && e.key <= '9') {
          store.setSelectedTexture(Number(e.key))
          return
        }
        if (e.key === '0') {
          store.setActiveTool('erase')
          return
        }
        if (e.key === 'p' || e.key === 'P') {
          store.setActiveTool('paint')
          return
        }
        if (e.key === 'e' || e.key === 'E') {
          store.setActiveTool('erase')
          return
        }
        if (e.key === 's' || e.key === 'S') {
          store.setActiveTool('sprite')
          return
        }
        if (e.key === 'w' || e.key === 'W') {
          store.setActiveLayer('walls')
          return
        }
        if (e.key === 'f' || e.key === 'F') {
          store.setActiveLayer('floor')
          return
        }
        if (e.key === 'c' || e.key === 'C') {
          store.setActiveLayer('ceiling')
          return
        }

        const si = store.selectedSpriteIndex
        if (si >= 0 && si < store.map.sprites.length) {
          const sprite = store.map.sprites[si]
          if (e.key === 'ArrowUp') {
            e.preventDefault()
            store.updateSprite(si, { y: Math.max(0, sprite.y - 0.1) })
            return
          }
          if (e.key === 'ArrowDown') {
            e.preventDefault()
            store.updateSprite(si, { y: Math.min(store.map.height, sprite.y + 0.1) })
            return
          }
          if (e.key === 'ArrowLeft') {
            e.preventDefault()
            store.updateSprite(si, { x: Math.max(0, sprite.x - 0.1) })
            return
          }
          if (e.key === 'ArrowRight') {
            e.preventDefault()
            store.updateSprite(si, { x: Math.min(store.map.width, sprite.x + 0.1) })
            return
          }
          if (e.key === 'Delete' || e.key === 'Backspace') {
            store.removeSprite(si)
            return
          }
        }
      }
    }

    window.addEventListener('keydown', handleKeyDown)
    return () => window.removeEventListener('keydown', handleKeyDown)
  }, [])

  return (
    <>
      <Toolbar />
      <div className="flex flex-1 overflow-hidden">
        <GridEditor />
        <TexturePalette />
      </div>
      <SpritePanel />
    </>
  )
}
