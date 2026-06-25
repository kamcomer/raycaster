import { useRef, useEffect, useState, useCallback } from 'react'
import { useMapStore } from '../store/mapStore'
import { TEXTURE_COLORS } from '../map/constants'
import { getCachedTexture } from '../utils/textureCache'

function drawSpriteFallback(ctx: CanvasRenderingContext2D, sx: number, sy: number, size: number) {
  ctx.fillStyle = '#ffdd00'
  ctx.beginPath()
  ctx.arc(sx, sy, Math.max(3, size / 5), 0, Math.PI * 2)
  ctx.fill()
  ctx.strokeStyle = '#000'
  ctx.lineWidth = 1
  ctx.stroke()
}

export default function GridEditor() {
  const canvasRef = useRef<HTMLCanvasElement>(null)
  const containerRef = useRef<HTMLDivElement>(null)
  const [isPainting, setIsPainting] = useState(false)
  const [hoverPos, setHoverPos] = useState<{ r: number; c: number } | null>(null)
  const [cellSize, setCellSize] = useState(24)
  const isDraggingSprite = useRef(false)
  const dragSpriteIndex = useRef(-1)

  const map = useMapStore((s) => s.map)
  const activeLayer = useMapStore((s) => s.activeLayer)
  const selectedTexture = useMapStore((s) => s.selectedTexture)
  const selectedSpriteType = useMapStore((s) => s.selectedSpriteType)
  const activeTool = useMapStore((s) => s.activeTool)
  const setCell = useMapStore((s) => s.setCell)
  const addSprite = useMapStore((s) => s.addSprite)
  const selectSprite = useMapStore((s) => s.selectSprite)
  const updateSprite = useMapStore((s) => s.updateSprite)
  const selectedSpriteIndex = useMapStore((s) => s.selectedSpriteIndex)
  const textures = useMapStore((s) => s.textures)
  const spriteTypes = useMapStore((s) => s.spriteTypes)

  const recalcCellSize = useCallback(() => {
    if (!containerRef.current) return
    const { width, height } = containerRef.current.getBoundingClientRect()
    const cw = Math.floor(width / map.width)
    const ch = Math.floor(height / map.height)
    setCellSize(Math.max(8, Math.min(cw, ch)))
  }, [map.width, map.height])

  useEffect(() => {
    recalcCellSize()
    const ro = new ResizeObserver(recalcCellSize)
    if (containerRef.current) ro.observe(containerRef.current)
    return () => ro.disconnect()
  }, [recalcCellSize])

  const getGridPos = useCallback(
    (clientX: number, clientY: number) => {
      const canvas = canvasRef.current
      if (!canvas) return null
      const rect = canvas.getBoundingClientRect()
      const col = Math.floor((clientX - rect.left) / cellSize)
      const row = Math.floor((clientY - rect.top) / cellSize)
      if (row < 0 || row >= map.height || col < 0 || col >= map.width)
        return null
      return { r: row, c: col }
    },
    [cellSize, map.height, map.width]
  )

  const paintCell = useCallback(
    (r: number, c: number) => {
      if (r < 0 || r >= map.height || c < 0 || c >= map.width) return
      if (activeTool === 'sprite') {
        if (selectedSpriteType >= 0 && selectedSpriteType < spriteTypes.length)
          addSprite({ x: c + 0.5, y: r + 0.5, type: selectedSpriteType })
        return
      }
      const texId = activeTool === 'erase' ? 0 : selectedTexture
      setCell(r, c, texId)
    },
    [activeTool, selectedTexture, selectedSpriteType, spriteTypes, setCell, addSprite, map.height, map.width]
  )

  const hitTestSprite = useCallback(
    (clientX: number, clientY: number) => {
      const canvas = canvasRef.current
      if (!canvas) return -1
      const rect = canvas.getBoundingClientRect()
      const px = clientX - rect.left
      const py = clientY - rect.top
      const threshold = cellSize * 0.4
      for (let i = map.sprites.length - 1; i >= 0; i--) {
        const s = map.sprites[i]
        const sx = s.x * cellSize
        const sy = s.y * cellSize
        const dx = px - sx
        const dy = py - sy
        if (dx * dx + dy * dy < threshold * threshold) return i
      }
      return -1
    },
    [cellSize, map.sprites]
  )

  const handlePointerDown = useCallback(
    (e: React.PointerEvent) => {
      const hit = hitTestSprite(e.clientX, e.clientY)
      if (hit >= 0) {
        selectSprite(hit)
        isDraggingSprite.current = true
        dragSpriteIndex.current = hit
        return
      }
      selectSprite(-1)
      const pos = getGridPos(e.clientX, e.clientY)
      if (!pos) return
      setIsPainting(true)
      paintCell(pos.r, pos.c)
    },
    [hitTestSprite, selectSprite, getGridPos, paintCell]
  )

  const handlePointerMove = useCallback(
    (e: React.PointerEvent) => {
      if (isDraggingSprite.current) {
        const canvas = canvasRef.current
        if (!canvas) return
        const rect = canvas.getBoundingClientRect()
        const gx = (e.clientX - rect.left) / cellSize
        const gy = (e.clientY - rect.top) / cellSize
        updateSprite(dragSpriteIndex.current, {
          x: Math.max(0, Math.min(gx, map.width)),
          y: Math.max(0, Math.min(gy, map.height)),
        })
        return
      }
      const pos = getGridPos(e.clientX, e.clientY)
      setHoverPos(pos)
      if (isPainting && pos) paintCell(pos.r, pos.c)
    },
    [getGridPos, isPainting, paintCell, cellSize, updateSprite, map.width, map.height]
  )

  const handlePointerUp = useCallback(() => {
    isDraggingSprite.current = false
    dragSpriteIndex.current = -1
    setIsPainting(false)
  }, [])

  useEffect(() => {
    const canvas = canvasRef.current
    if (!canvas) return
    const ctx = canvas.getContext('2d')!
    const w = map.width * cellSize
    const h = map.height * cellSize

    canvas.width = w
    canvas.height = h

    ctx.fillStyle = '#1a1a1a'
    ctx.fillRect(0, 0, w, h)

    const grid = map[activeLayer]

    for (let r = 0; r < map.height; r++) {
      for (let c = 0; c < map.width; c++) {
        const texId = grid[r][c]
        const x = c * cellSize
        const y = r * cellSize

        if (texId > 0) {
          const tex = textures[texId - 1]
          if (tex) {
            try {
              const img = getCachedTexture(tex.dataUrl)
              if (img.complete) {
                ctx.drawImage(img, x, y, cellSize, cellSize)
              } else {
                ctx.fillStyle = TEXTURE_COLORS[(texId - 1) % TEXTURE_COLORS.length]
                ctx.fillRect(x, y, cellSize, cellSize)
              }
            } catch {
              ctx.fillStyle = TEXTURE_COLORS[(texId - 1) % TEXTURE_COLORS.length]
              ctx.fillRect(x, y, cellSize, cellSize)
            }
          } else {
            ctx.fillStyle = TEXTURE_COLORS[(texId - 1) % TEXTURE_COLORS.length]
            ctx.fillRect(x, y, cellSize, cellSize)
          }
        } else {
          ctx.fillStyle = '#2a2a2a'
          ctx.fillRect(x, y, cellSize, cellSize)
        }
      }
    }

    ctx.strokeStyle = '#333'
    ctx.lineWidth = 1
    for (let r = 0; r <= map.height; r++) {
      ctx.beginPath()
      ctx.moveTo(0, r * cellSize)
      ctx.lineTo(w, r * cellSize)
      ctx.stroke()
    }
    for (let c = 0; c <= map.width; c++) {
      ctx.beginPath()
      ctx.moveTo(c * cellSize, 0)
      ctx.lineTo(c * cellSize, h)
      ctx.stroke()
    }

    if (activeLayer === 'walls') {
      for (const sprite of map.sprites) {
        const sx = sprite.x * cellSize
        const sy = sprite.y * cellSize
        const st = spriteTypes[sprite.type]
        if (st) {
          try {
            const img = getCachedTexture(st.dataUrl)
            if (img.complete) {
              ctx.drawImage(img, sx - cellSize / 2, sy - cellSize / 2, cellSize, cellSize)
            } else {
              throw new Error('not loaded')
            }
          } catch {
            drawSpriteFallback(ctx, sx, sy, cellSize)
          }
        } else {
          drawSpriteFallback(ctx, sx, sy, cellSize)
        }
      }

      const si = selectedSpriteIndex
      if (si >= 0 && si < map.sprites.length) {
        const s = map.sprites[si]
        const sx = s.x * cellSize
        const sy = s.y * cellSize
        ctx.strokeStyle = '#00ffff'
        ctx.lineWidth = 2
        ctx.strokeRect(sx - cellSize / 2 - 1, sy - cellSize / 2 - 1, cellSize + 2, cellSize + 2)
      }
    }

    if (hoverPos) {
      ctx.fillStyle = 'rgba(255,255,255,0.12)'
      ctx.fillRect(hoverPos.c * cellSize, hoverPos.r * cellSize, cellSize, cellSize)
    }
  }, [map, activeLayer, cellSize, hoverPos, textures, selectedSpriteIndex, spriteTypes])

  return (
    <div
      ref={containerRef}
      className="flex-1 overflow-hidden flex items-center justify-center bg-[#1a1a1a]"
    >
      <canvas
        ref={canvasRef}
        onPointerDown={handlePointerDown}
        onPointerMove={handlePointerMove}
        onPointerUp={handlePointerUp}
        onPointerLeave={() => {
          isDraggingSprite.current = false
          dragSpriteIndex.current = -1
          setIsPainting(false)
          setHoverPos(null)
        }}
        style={{ touchAction: 'none' }}
        className="cursor-crosshair"
      />
    </div>
  )
}
