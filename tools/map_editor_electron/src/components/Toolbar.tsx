import { useState } from 'react'
import { useMapStore } from '../store/mapStore'
import { parseMap, serializeMap } from '../map/parser'
import { preloadTexture } from '../utils/textureCache'

export default function Toolbar() {
  const map = useMapStore((s) => s.map)
  const filePath = useMapStore((s) => s.filePath)
  const loadMap = useMapStore((s) => s.loadMap)
  const newMap = useMapStore((s) => s.newMap)
  const setFilePath = useMapStore((s) => s.setFilePath)
  const activeLayer = useMapStore((s) => s.activeLayer)
  const setActiveLayer = useMapStore((s) => s.setActiveLayer)
  const activeTool = useMapStore((s) => s.activeTool)
  const setActiveTool = useMapStore((s) => s.setActiveTool)
  const selectedTexture = useMapStore((s) => s.selectedTexture)
  const setSelectedTexture = useMapStore((s) => s.setSelectedTexture)
  const textures = useMapStore((s) => s.textures)
  const spriteTypes = useMapStore((s) => s.spriteTypes)
  const selectedSpriteType = useMapStore((s) => s.selectedSpriteType)
  const setSelectedSpriteType = useMapStore((s) => s.setSelectedSpriteType)
  const importSpriteType = useMapStore((s) => s.importSpriteType)

  const showNewDialog = useMapStore((s) => s.showNewDialog)
  const setShowNewDialog = useMapStore((s) => s.setShowNewDialog)
  const [newWidth, setNewWidth] = useState(24)
  const [newHeight, setNewHeight] = useState(24)

  const api = window.api

  const handleNew = () => {
    newMap(newWidth, newHeight)
    setShowNewDialog(false)
  }

  const handleOpen = async () => {
    const path = await api.openDialog()
    if (!path) return
    const text = await api.readFile(path)
    const data = parseMap(text)
    loadMap(data)
    setFilePath(path)
    const textureEntries: { path: string; dataUrl: string }[] = []
    for (const texPath of data.textures) {
      try {
        const absPath = await api.resolveFromRoot(path, texPath)
        const tex = await api.readTexture(absPath)
        textureEntries.push({ path: texPath, dataUrl: tex.data })
      } catch (err) {
        console.warn('could not load texture:', texPath, err)
      }
    }
    if (textureEntries.length > 0) {
      useMapStore.getState().populateTextures(textureEntries)
    }
    const spriteTypeEntries: { path: string; frameCount: number; frameDelay: number; dataUrl: string }[] = []
    for (const st of data.spriteTypes) {
      try {
        const absPath = await api.resolveFromRoot(path, st.path)
        const tex = await api.readTexture(absPath)
        spriteTypeEntries.push({ path: st.path, frameCount: st.frameCount, frameDelay: st.frameDelay, dataUrl: tex.data })
      } catch (err) {
        console.warn('could not load sprite type:', st.path, err)
      }
    }
    if (spriteTypeEntries.length > 0) {
      useMapStore.getState().populateSpriteTypes(spriteTypeEntries)
    }
  }

  const handleSave = async () => {
    const content = serializeMap(map)
    const path = filePath || (await api.saveDialog('map.txt'))
    if (!path) return
    await api.writeFile(path, content)
    setFilePath(path)
  }

  const handleSaveAs = async () => {
    const content = serializeMap(map)
    const path = await api.saveDialog('map.txt')
    if (!path) return
    await api.writeFile(path, content)
    setFilePath(path)
  }

  const handleExport = handleSaveAs

  const [showResizeDialog, setShowResizeDialog] = useState(false)
  const [resizeWidth, setResizeWidth] = useState(map.width)
  const [resizeHeight, setResizeHeight] = useState(map.height)
  const resizeMap = useMapStore((s) => s.resizeMap)

  const handleResize = () => {
    resizeMap(resizeWidth, resizeHeight)
    setShowResizeDialog(false)
  }

  const importTexture = useMapStore((s) => s.importTexture)

  const resolveTexturePath = async (absolutePath: string) => {
    const mapFile = useMapStore.getState().filePath
    if (mapFile) {
      return await api.relativeToRoot(mapFile, absolutePath)
    }
    return absolutePath.split('/').pop() || absolutePath
  }

  const handleAddTexture = async () => {
    try {
      const absolutePath = await api.openTextureDialog()
      if (!absolutePath) return
      const tex = await api.readTexture(absolutePath)
      try {
        await preloadTexture(tex.data)
      } catch {
        console.warn('texture preload failed, using fallback:', absolutePath)
      }
      const relPath = await resolveTexturePath(tex.path)
      importTexture(relPath, tex.data)
    } catch (err) {
      console.error('failed to import texture:', err)
    }
  }

  const handleImportTextureDir = async () => {
    try {
      const dirPath = await api.selectDirectory()
      if (!dirPath) return
      const files = await api.listTexturesInDirectory(dirPath)
      if (files.length === 0) {
        console.warn('no image files found in:', dirPath)
        return
      }
      for (const file of files) {
        try {
          const tex = await api.readTexture(file.path)
          await preloadTexture(tex.data)
          const relPath = await resolveTexturePath(tex.path)
          importTexture(relPath, tex.data)
        } catch (err) {
          console.warn('skipping file:', file.name, err)
        }
      }
    } catch (err) {
      console.error('failed to import texture directory:', err)
    }
  }

  const handleAddSpriteType = async () => {
    try {
      const absolutePath = await api.openTextureDialog()
      if (!absolutePath) return
      const mapFile = useMapStore.getState().filePath
      let relPath = absolutePath.split('/').pop() || absolutePath
      if (mapFile) {
        relPath = await api.relativeToRoot(mapFile, absolutePath)
      }
      const tex = await api.readTexture(absolutePath)
      await preloadTexture(tex.data)
      importSpriteType(relPath, tex.data)
    } catch (err) {
      console.error('failed to import sprite type:', err)
    }
  }

  const handleAddSpriteTypeDir = async () => {
    try {
      const dirPath = await api.selectDirectory()
      if (!dirPath) return
      const files = await api.listTexturesInDirectory(dirPath)
      if (files.length === 0) {
        console.warn('no image files found in:', dirPath)
        return
      }
      const mapFile = useMapStore.getState().filePath
      for (const file of files) {
        try {
          const tex = await api.readTexture(file.path)
          await preloadTexture(tex.data)
          let relPath = file.path.split('/').pop() || file.path
          if (mapFile) relPath = await api.relativeToRoot(mapFile, file.path)
          importSpriteType(relPath, tex.data)
        } catch (err) {
          console.warn('skipping file:', file.name, err)
        }
      }
    } catch (err) {
      console.error('failed to import sprite type directory:', err)
    }
  }

  const layers: Array<typeof activeLayer> = ['walls', 'floor', 'ceiling']
  const layerLabel = { walls: 'Walls', floor: 'Floor', ceiling: 'Ceiling' }

  return (
    <>
      <div className="h-12 bg-[#16213e] border-b border-[#0f3460] flex items-center pl-[76px] pr-3 gap-2 drag-region shrink-0">
        <button
          onClick={() => setShowNewDialog(true)}
          className="no-drag px-3 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm"
        >
          New
        </button>
        <button
          onClick={handleOpen}
          data-action="open"
          className="no-drag px-3 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm"
        >
          Open
        </button>
        <button
          onClick={handleSave}
          data-action="save"
          className="no-drag px-3 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm"
        >
          Save
        </button>
        <button
          onClick={handleExport}
          className="no-drag px-3 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm"
        >
          Export As
        </button>

        <div className="w-px h-6 bg-[#0f3460] mx-2" />

        {layers.map((l) => (
          <button
            key={l}
            onClick={() => setActiveLayer(l)}
            className={`no-drag px-3 py-1.5 rounded text-sm ${
              activeLayer === l
                ? 'bg-[#e94560] text-white'
                : 'bg-[#0f3460] hover:bg-[#1a5276]'
            }`}
          >
            {layerLabel[l]}
          </button>
        ))}

        <div className="w-px h-6 bg-[#0f3460] mx-2" />

        <button
          onClick={() => setActiveTool('paint')}
          className={`no-drag px-3 py-1.5 rounded text-sm ${
            activeTool === 'paint'
              ? 'bg-[#e94560] text-white'
              : 'bg-[#0f3460] hover:bg-[#1a5276]'
          }`}
        >
          Paint
        </button>
        <button
          onClick={() => setActiveTool('erase')}
          className={`no-drag px-3 py-1.5 rounded text-sm ${
            activeTool === 'erase'
              ? 'bg-[#e94560] text-white'
              : 'bg-[#0f3460] hover:bg-[#1a5276]'
          }`}
        >
          Erase
        </button>
        <button
          onClick={() => setActiveTool('sprite')}
          className={`no-drag px-3 py-1.5 rounded text-sm ${
            activeTool === 'sprite'
              ? 'bg-[#e94560] text-white'
              : 'bg-[#0f3460] hover:bg-[#1a5276]'
          }`}
        >
          Sprite
        </button>

        <div className="flex-1" />

        <button
          onClick={() => {
            setResizeWidth(map.width)
            setResizeHeight(map.height)
            setShowResizeDialog(true)
          }}
          className="no-drag px-2 py-1 bg-[#0f3460] hover:bg-[#1a5276] rounded text-xs"
        >
          Resize
        </button>
        <span className="no-drag text-xs text-gray-500">
          {map.width}x{map.height}
          {filePath ? ` · ${filePath.split('/').pop()}` : ''}
        </span>

        {activeTool === 'sprite' ? (
          <>
            <button
              onClick={handleAddSpriteType}
              className="no-drag px-3 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm ml-2"
            >
              + Sprite
            </button>
            <button
              onClick={handleAddSpriteTypeDir}
              className="no-drag px-3 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm"
            >
              + Sprite Dir
            </button>
          </>
        ) : (
          <>
            <button
              onClick={handleAddTexture}
              className="no-drag px-3 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm ml-2"
            >
              + Texture
            </button>
            <button
              onClick={handleImportTextureDir}
              className="no-drag px-3 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm"
            >
              +Texture Dir
            </button>
          </>
        )}
      </div>

      {showNewDialog && (
        <div className="fixed inset-0 bg-black/50 flex items-center justify-center z-50">
          <div className="bg-[#1a1a2e] border border-[#0f3460] rounded-lg p-6">
            <h2 className="text-lg mb-4">New Map</h2>
            <div className="flex gap-4 mb-4">
              <label>
                <span className="text-sm text-gray-400 mr-2">Width:</span>
                <input
                  type="number"
                  min={3}
                  max={256}
                  value={newWidth}
                  onChange={(e) => setNewWidth(Number(e.target.value))}
                  className="w-20 bg-[#16213e] border border-[#0f3460] rounded px-2 py-1 text-sm"
                />
              </label>
              <label>
                <span className="text-sm text-gray-400 mr-2">Height:</span>
                <input
                  type="number"
                  min={3}
                  max={256}
                  value={newHeight}
                  onChange={(e) => setNewHeight(Number(e.target.value))}
                  className="w-20 bg-[#16213e] border border-[#0f3460] rounded px-2 py-1 text-sm"
                />
              </label>
            </div>
            <div className="flex gap-2 justify-end">
              <button
                onClick={() => setShowNewDialog(false)}
                className="px-4 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm"
              >
                Cancel
              </button>
              <button
                onClick={handleNew}
                className="px-4 py-1.5 bg-[#e94560] hover:bg-[#c73e54] rounded text-sm"
              >
                Create
              </button>
            </div>
          </div>
        </div>
      )}

      {showResizeDialog && (
        <div className="fixed inset-0 bg-black/50 flex items-center justify-center z-50">
          <div className="bg-[#1a1a2e] border border-[#0f3460] rounded-lg p-6">
            <h2 className="text-lg mb-4">Resize Map</h2>
            <p className="text-xs text-gray-400 mb-4">
              Existing content will be preserved. New cells start empty.
            </p>
            <div className="flex gap-4 mb-4">
              <label>
                <span className="text-sm text-gray-400 mr-2">Width:</span>
                <input
                  type="number"
                  min={3}
                  max={256}
                  value={resizeWidth}
                  onChange={(e) => setResizeWidth(Number(e.target.value))}
                  className="w-20 bg-[#16213e] border border-[#0f3460] rounded px-2 py-1 text-sm"
                />
              </label>
              <label>
                <span className="text-sm text-gray-400 mr-2">Height:</span>
                <input
                  type="number"
                  min={3}
                  max={256}
                  value={resizeHeight}
                  onChange={(e) => setResizeHeight(Number(e.target.value))}
                  className="w-20 bg-[#16213e] border border-[#0f3460] rounded px-2 py-1 text-sm"
                />
              </label>
            </div>
            <div className="flex gap-2 justify-end">
              <button
                onClick={() => setShowResizeDialog(false)}
                className="px-4 py-1.5 bg-[#0f3460] hover:bg-[#1a5276] rounded text-sm"
              >
                Cancel
              </button>
              <button
                onClick={handleResize}
                className="px-4 py-1.5 bg-[#e94560] hover:bg-[#c73e54] rounded text-sm"
              >
                Apply
              </button>
            </div>
          </div>
        </div>
      )}
    </>
  )
}
