import { useMapStore } from '../store/mapStore'
import { TEXTURE_COLORS } from '../map/constants'
import { getCachedTexture } from '../utils/textureCache'

export default function TexturePalette() {
  const textures = useMapStore((s) => s.textures)
  const selectedTexture = useMapStore((s) => s.selectedTexture)
  const setSelectedTexture = useMapStore((s) => s.setSelectedTexture)
  const spriteTypes = useMapStore((s) => s.spriteTypes)
  const selectedSpriteType = useMapStore((s) => s.selectedSpriteType)
  const setSelectedSpriteType = useMapStore((s) => s.setSelectedSpriteType)
  const activeTool = useMapStore((s) => s.activeTool)

  if (activeTool === 'sprite') {
    return (
      <div className="w-44 bg-[#16213e] border-l border-[#0f3460] flex flex-col shrink-0">
        <div className="px-3 py-2 text-xs font-semibold text-gray-400 uppercase tracking-wider border-b border-[#0f3460]">
          Sprite Types
        </div>
        <div className="flex-1 overflow-y-auto scrollbar-thin p-2">
          {spriteTypes.length === 0 ? (
            <div className="text-xs text-gray-600 text-center mt-8">
              No sprite types loaded
            </div>
          ) : (
            <div className="grid grid-cols-2 gap-1.5">
              {spriteTypes.map((st, i) => (
                <button
                  key={i}
                  onClick={() => setSelectedSpriteType(i)}
                  className={`relative aspect-square rounded overflow-hidden border-2 transition-colors ${
                    selectedSpriteType === i
                      ? 'border-[#e94560]'
                      : 'border-transparent hover:border-gray-600'
                  }`}
                  title={st.path.split('/').pop() || ''}
                >
                  <img
                    src={st.dataUrl}
                    alt={st.path}
                    className="w-full h-full object-cover"
                  />
                  {st.frameCount > 1 && (
                    <span className="absolute top-0.5 right-0.5 text-[9px] bg-black/70 text-yellow-400 px-1 rounded leading-tight">
                      {st.frameCount}f
                    </span>
                  )}
                  <span className="absolute bottom-0.5 left-1 right-1 text-[10px] bg-black/60 px-1 rounded text-center truncate">
                    {i}
                  </span>
                </button>
              ))}
            </div>
          )}
        </div>
      </div>
    )
  }

  const emptySlotCount = Math.max(1, 11 - textures.length)

  return (
    <div className="w-44 bg-[#16213e] border-l border-[#0f3460] flex flex-col shrink-0">
      <div className="px-3 py-2 text-xs font-semibold text-gray-400 uppercase tracking-wider border-b border-[#0f3460]">
        Textures
      </div>
      <div className="flex-1 overflow-y-auto scrollbar-thin p-2">
        <div className="grid grid-cols-2 gap-1.5">
          {textures.map((tex) => {
            const isSelected = tex.id === selectedTexture
            return (
              <button
                key={tex.id}
                onClick={() => setSelectedTexture(tex.id)}
                className={`relative aspect-square rounded overflow-hidden border-2 transition-colors ${
                  isSelected
                    ? 'border-[#e94560]'
                    : 'border-transparent hover:border-gray-600'
                }`}
                title={tex.path.split('/').pop()}
              >
                <img
                  src={tex.dataUrl}
                  alt={tex.path}
                  className="w-full h-full object-cover"
                  onError={(e) => {
                    const target = e.currentTarget
                    target.style.display = 'none'
                    const parent = target.parentElement
                    if (parent) {
                      const idx = tex.id - 1
                      parent.style.backgroundColor =
                        TEXTURE_COLORS[idx % TEXTURE_COLORS.length]
                    }
                  }}
                />
                <span className="absolute bottom-0.5 right-1 text-[10px] bg-black/60 px-1 rounded">
                  {tex.id}
                </span>
              </button>
            )
          })}
          {Array.from({ length: emptySlotCount }).map((_, i) => {
            const slotId = textures.length + i + 1
            const isSelected = slotId === selectedTexture
            return (
              <button
                key={`empty-${slotId}`}
                onClick={() => setSelectedTexture(slotId)}
                className={`aspect-square rounded border-2 flex items-center justify-center text-xs text-gray-600 transition-colors ${
                  isSelected
                    ? 'border-[#e94560] bg-[#1a1a2e]'
                    : 'border-dashed border-gray-700 hover:border-gray-500'
                }`}
              >
                {slotId}
              </button>
            )
          })}
          <button
            onClick={() => setSelectedTexture(0)}
            className={`aspect-square rounded border-2 flex items-center justify-center text-xs transition-colors ${
              selectedTexture === 0
                ? 'border-[#e94560] bg-[#3a1a1a] text-[#e94560]'
                : 'border-dashed border-gray-700 hover:border-red-700 text-red-800'
            }`}
            title="Eraser"
          >
            ✕
          </button>
        </div>
      </div>
    </div>
  )
}
