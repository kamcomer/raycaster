import { useMapStore } from '../store/mapStore'
import { getCachedTexture } from '../utils/textureCache'

export default function SpritePanel() {
  const sprites = useMapStore((s) => s.map.sprites)
  const removeSprite = useMapStore((s) => s.removeSprite)
  const updateSprite = useMapStore((s) => s.updateSprite)
  const selectSprite = useMapStore((s) => s.selectSprite)
  const selectedSpriteIndex = useMapStore((s) => s.selectedSpriteIndex)
  const spriteTypes = useMapStore((s) => s.spriteTypes)

  if (sprites.length === 0) return null

  const selected = selectedSpriteIndex >= 0 && selectedSpriteIndex < sprites.length
    ? sprites[selectedSpriteIndex]
    : null

  return (
    <div className="h-32 bg-[#16213e] border-t border-[#0f3460] shrink-0 flex flex-col">
      <div className="px-3 py-1.5 text-xs font-semibold text-gray-400 uppercase tracking-wider">
        Sprites ({sprites.length})
      </div>
      <div className="flex-1 overflow-y-auto scrollbar-thin px-3 pb-2">
        <div className="flex flex-wrap gap-2">
          {sprites.map((sprite, i) => {
            const st = spriteTypes[sprite.type]
            const isSelected = i === selectedSpriteIndex
            return (
              <div
                key={i}
                onClick={() => selectSprite(isSelected ? -1 : i)}
                className={
                  `flex items-center gap-2 rounded px-2 py-1 text-xs cursor-pointer ` +
                  (isSelected
                    ? 'bg-[#1a5276] ring-2 ring-cyan-400'
                    : 'bg-[#0f3460] hover:bg-[#1a5276]')
                }
              >
                {st && (
                  <img
                    src={st.dataUrl}
                    className="w-4 h-4 rounded object-cover"
                    alt={`sprite ${sprite.type}`}
                  />
                )}
                <span>
                  ({sprite.x.toFixed(1)}, {sprite.y.toFixed(1)}) [{sprite.type}]
                </span>
                <button
                  onClick={(e) => {
                    e.stopPropagation()
                    removeSprite(i)
                  }}
                  className="text-red-400 hover:text-red-300 ml-1"
                >
                  ✕
                </button>
              </div>
            )
          })}
        </div>
      </div>
      {selected && (
        <div className="px-3 py-2 border-t border-[#0f3460] flex items-center gap-4 text-xs">
          <label className="flex items-center gap-1.5">
            <span className="text-gray-400">X</span>
            <input
              type="number"
              step="0.1"
              value={selected.x}
              onChange={(e) => updateSprite(selectedSpriteIndex, { x: parseFloat(e.target.value) || 0 })}
              className="w-16 bg-[#0f3460] rounded px-1.5 py-0.5 text-white text-xs [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none"
            />
          </label>
          <label className="flex items-center gap-1.5">
            <span className="text-gray-400">Y</span>
            <input
              type="number"
              step="0.1"
              value={selected.y}
              onChange={(e) => updateSprite(selectedSpriteIndex, { y: parseFloat(e.target.value) || 0 })}
              className="w-16 bg-[#0f3460] rounded px-1.5 py-0.5 text-white text-xs [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none"
            />
          </label>
          <span className="text-gray-500 ml-auto">arrow keys to nudge</span>
        </div>
      )}
    </div>
  )
}
