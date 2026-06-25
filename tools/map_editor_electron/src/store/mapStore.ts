import { create } from 'zustand'
import { MapData, Layer, Tool, Sprite } from '../map/types'
import { createEmptyMap } from '../map/parser'

interface TextureEntry {
  id: number
  path: string
  dataUrl: string
}

interface SpriteTypeEntry {
  path: string
  frameCount: number
  frameDelay: number
  dataUrl: string
}

interface MapStore {
  map: MapData
  selectedTexture: number
  selectedSpriteType: number
  activeLayer: Layer
  activeTool: Tool
  textures: TextureEntry[]
  spriteTypes: SpriteTypeEntry[]
  filePath: string | null
  selectedSpriteIndex: number

  setCell: (row: number, col: number, textureId: number) => void
  setSelectedTexture: (id: number) => void
  setSelectedSpriteType: (index: number) => void
  setActiveLayer: (layer: Layer) => void
  setActiveTool: (tool: Tool) => void
  addTexture: (path: string, dataUrl: string) => void
  importTexture: (path: string, dataUrl: string) => void
  removeTexture: (id: number) => void
  importSpriteType: (path: string, dataUrl: string, frameCount?: number, frameDelay?: number) => void
  selectSprite: (index: number) => void
  addSprite: (sprite: Sprite) => void
  removeSprite: (index: number) => void
  updateSprite: (index: number, partial: Partial<Sprite>) => void
  populateTextures: (entries: { path: string; dataUrl: string }[]) => void
  populateSpriteTypes: (entries: { path: string; frameCount: number; frameDelay: number; dataUrl: string }[]) => void
  loadMap: (data: MapData) => void
  newMap: (width: number, height: number) => void
  setFilePath: (path: string | null) => void
  showNewDialog: boolean
  setShowNewDialog: (show: boolean) => void
  resizeMap: (width: number, height: number) => void
}

function cloneGrid(grid: number[][], newWidth: number, newHeight: number) {
  return Array.from({ length: newHeight }, (_, r) =>
    Array.from({ length: newWidth }, (_, c) =>
      r < grid.length && c < grid[r].length ? grid[r][c] : 0
    )
  )
}

export const useMapStore = create<MapStore>()((set) => ({
  map: createEmptyMap(24, 24),
  selectedTexture: 1,
  selectedSpriteType: 0,
  activeLayer: 'walls',
  activeTool: 'paint',
  textures: [],
  spriteTypes: [],
  filePath: null,
  showNewDialog: false,
  selectedSpriteIndex: -1,

  setCell: (row, col, textureId) =>
    set((state) => {
      const { map } = state
      const grid = map[state.activeLayer]
      if (row < 0 || row >= map.height || col < 0 || col >= map.width) return state
      const copy = grid.map((r) => [...r])
      copy[row][col] = textureId
      return {
        map: {
          ...map,
          [state.activeLayer]: copy,
        },
      }
    }),

  setSelectedTexture: (id) => set({ selectedTexture: id }),

  setSelectedSpriteType: (index) => set({ selectedSpriteType: index }),

  setActiveLayer: (layer) => set({ activeLayer: layer }),

  setActiveTool: (tool) => set({ activeTool: tool }),

  addTexture: (path, dataUrl) =>
    set((state) => {
      const id = state.textures.length + 1
      return { textures: [...state.textures, { id, path, dataUrl }] }
    }),

  importTexture: (path, dataUrl) =>
    set((state) => {
      const id = state.textures.length + 1
      if (state.map.textures.includes(path)) return state
      return {
        textures: [...state.textures, { id, path, dataUrl }],
        map: {
          ...state.map,
          textures: [...state.map.textures, path],
        },
      }
    }),

  removeTexture: (id) =>
    set((state) => ({
      textures: state.textures.filter((t) => t.id !== id),
      map: {
        ...state.map,
        textures: state.map.textures.filter((_, i) => i + 1 !== id),
      },
    })),

  importSpriteType: (path, dataUrl, frameCount = 1, frameDelay = 0) =>
    set((state) => {
      if (state.spriteTypes.some((st) => st.path === path)) return state
      if (state.map.spriteTypes.some((st) => st.path === path)) return state
      return {
        spriteTypes: [...state.spriteTypes, { path, frameCount, frameDelay, dataUrl }],
        map: {
          ...state.map,
          spriteTypes: [...state.map.spriteTypes, { path, frameCount, frameDelay }],
        },
      }
    }),

  populateSpriteTypes: (entries) =>
    set({
      spriteTypes: entries.map((e) => ({
        path: e.path,
        frameCount: e.frameCount,
        frameDelay: e.frameDelay,
        dataUrl: e.dataUrl,
      })),
    }),

  selectSprite: (index) => set({ selectedSpriteIndex: index }),

  addSprite: (sprite) =>
    set((state) => ({
      map: { ...state.map, sprites: [...state.map.sprites, sprite] },
    })),

  removeSprite: (index) =>
    set((state) => ({
      map: {
        ...state.map,
        sprites: state.map.sprites.filter((_, i) => i !== index),
      },
      selectedSpriteIndex:
        state.selectedSpriteIndex === index
          ? -1
          : state.selectedSpriteIndex > index
            ? state.selectedSpriteIndex - 1
            : state.selectedSpriteIndex,
    })),

  updateSprite: (index, partial) =>
    set((state) => {
      const sprites = [...state.map.sprites]
      if (index >= 0 && index < sprites.length) {
        sprites[index] = { ...sprites[index], ...partial }
      }
      return { map: { ...state.map, sprites } }
    }),

  populateTextures: (entries) =>
    set((state) => ({
      textures: entries.map((e, i) => ({
        id: i + 1,
        path: e.path,
        dataUrl: e.dataUrl,
      })),
    })),

  loadMap: (data) =>
    set({
      map: data,
      textures: [],
      spriteTypes: [],
      filePath: null,
      selectedTexture: 1,
      selectedSpriteType: 0,
      selectedSpriteIndex: -1,
      activeLayer: 'walls',
      activeTool: 'paint',
    }),

  newMap: (width, height) =>
    set({
      map: createEmptyMap(width, height),
      textures: [],
      spriteTypes: [],
      filePath: null,
      selectedTexture: 1,
      selectedSpriteType: 0,
      selectedSpriteIndex: -1,
      activeLayer: 'walls',
      activeTool: 'paint',
    }),

  setFilePath: (path) => set({ filePath: path }),
  setShowNewDialog: (show) => set({ showNewDialog: show }),

  resizeMap: (width, height) =>
    set((state) => {
      const { map } = state
      return {
        map: {
          ...map,
          width,
          height,
          walls: cloneGrid(map.walls, width, height),
          floor: cloneGrid(map.floor, width, height),
          ceiling: cloneGrid(map.ceiling, width, height),
        },
      }
    }),
}))
