import { MapData, Sprite } from './types'
import { DEFAULT_WIDTH, DEFAULT_HEIGHT } from './constants'

function makeEmptyGrid(rows: number, cols: number): number[][] {
  return Array.from({ length: rows }, () => new Array(cols).fill(0))
}

function makeDefaultMap(): MapData {
  return {
    width: DEFAULT_WIDTH,
    height: DEFAULT_HEIGHT,
    walls: makeEmptyGrid(DEFAULT_HEIGHT, DEFAULT_WIDTH),
    floor: makeEmptyGrid(DEFAULT_HEIGHT, DEFAULT_WIDTH),
    ceiling: makeEmptyGrid(DEFAULT_HEIGHT, DEFAULT_WIDTH),
    textures: [],
    spriteTypes: [],
    sprites: [],
  }
}

function trimLine(line: string): string {
  return line.replace(/#.*$/, '').trim()
}

function parseGridRow(line: string, width: number): number[] | null {
  const trimmed = trimLine(line)
  if (!trimmed) return null
  const parts = trimmed.split(/\s+/).map(Number)
  if (parts.length !== width || parts.some(isNaN)) return null
  return parts
}

type ParseState =
  | 'none'
  | 'map'
  | 'ceil'
  | 'floor'
  | 'textures'
  | 'sprite_types'
  | 'sprites'

export function parseMap(text: string): MapData {
  const map = makeDefaultMap()
  let state: ParseState = 'none'
  let rowIndex = 0
  let dimsRead = false

  const lines = text.split('\n')

  for (const raw of lines) {
    const trimmed = raw.trim()

    if (!trimmed || trimmed.startsWith('#')) continue

    if (trimmed === '[MAP]') {
      state = 'map'
      rowIndex = 0
      dimsRead = false
      continue
    }
    if (trimmed === '[CEIL]') {
      state = 'ceil'
      rowIndex = 0
      continue
    }
    if (trimmed === '[FLOOR]') {
      state = 'floor'
      rowIndex = 0
      continue
    }
    if (trimmed === '[TEXTURES]') {
      state = 'textures'
      continue
    }
    if (trimmed === '[SPRITE_TYPES]') {
      state = 'sprite_types'
      continue
    }
    if (trimmed === '[SPRITES]') {
      state = 'sprites'
      continue
    }

    switch (state) {
      case 'map': {
        if (!dimsRead) {
          const parts = trimmed.split(/\s+/).map(Number)
          if (parts.length >= 2 && !isNaN(parts[0]) && !isNaN(parts[1])) {
            map.width = parts[0]
            map.height = parts[1]
            map.walls = makeEmptyGrid(map.height, map.width)
            map.floor = makeEmptyGrid(map.height, map.width)
            map.ceiling = makeEmptyGrid(map.height, map.width)
            dimsRead = true
            rowIndex = 0
          }
          break
        }
        const row = parseGridRow(raw, map.width)
        if (row && rowIndex < map.height) {
          map.walls[rowIndex] = row
          rowIndex++
        }
        break
      }
      case 'ceil': {
        const row = parseGridRow(raw, map.width)
        if (row && rowIndex < map.height) {
          map.ceiling[rowIndex] = row
          rowIndex++
        }
        break
      }
      case 'floor': {
        const row = parseGridRow(raw, map.width)
        if (row && rowIndex < map.height) {
          map.floor[rowIndex] = row
          rowIndex++
        }
        break
      }
      case 'textures': {
        if (map.textures.length < 11) {
          const colon = trimmed.indexOf(': ')
          const path = colon > 0 ? trimmed.slice(colon + 2) : trimmed
          map.textures.push(path)
        }
        break
      }
      case 'sprite_types': {
        const colon = trimmed.indexOf(': ')
        if (colon > 0) {
          const key = trimmed.slice(0, colon)
          const path = trimmed.slice(colon + 2)
          if (key && path) {
            map.spriteTypes.push({ key, path })
          }
        }
        break
      }
      case 'sprites': {
        const parts = trimmed.split(/\s+/)
        if (parts.length >= 3) {
          const x = parseFloat(parts[0])
          const y = parseFloat(parts[1])
          if (!isNaN(x) && !isNaN(y)) {
            map.sprites.push({ x, y, type: parts[2] })
          }
        }
        break
      }
    }
  }

  return map
}

export function serializeMap(data: MapData): string {
  const lines: string[] = []

  lines.push('[MAP]')
  lines.push(`${data.width} ${data.height}`)
  for (let r = 0; r < data.height; r++) {
    lines.push(data.walls[r].join(' '))
  }

  if (data.ceiling.some((row) => row.some((v) => v !== 0))) {
    lines.push('')
    lines.push('[CEIL]')
    for (let r = 0; r < data.height; r++) {
      lines.push(data.ceiling[r].join(' '))
    }
  }

  if (data.floor.some((row) => row.some((v) => v !== 0))) {
    lines.push('')
    lines.push('[FLOOR]')
    for (let r = 0; r < data.height; r++) {
      lines.push(data.floor[r].join(' '))
    }
  }

  if (data.textures.length > 0) {
    lines.push('')
    lines.push('[TEXTURES]')
    data.textures.forEach((tex, i) => {
      lines.push(`${i + 1}: ${tex}`)
    })
  }

  if (data.spriteTypes.length > 0) {
    lines.push('')
    lines.push('[SPRITE_TYPES]')
    for (const st of data.spriteTypes) {
      lines.push(`${st.key}: ${st.path}`)
    }
  }

  if (data.sprites.length > 0) {
    lines.push('')
    lines.push('[SPRITES]')
    for (const sprite of data.sprites) {
      lines.push(`${sprite.x} ${sprite.y} ${sprite.type}`)
    }
  }

  return lines.join('\n')
}

export function createEmptyMap(width: number, height: number): MapData {
  return {
    width,
    height,
    walls: makeEmptyGrid(height, width),
    floor: makeEmptyGrid(height, width),
    ceiling: makeEmptyGrid(height, width),
    textures: [],
    spriteTypes: [],
    sprites: [],
  }
}
