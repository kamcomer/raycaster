export interface SpriteTypeDef {
  key: string
  path: string
}

export interface MapData {
  width: number
  height: number
  walls: number[][]
  floor: number[][]
  ceiling: number[][]
  textures: string[]
  spriteTypes: SpriteTypeDef[]
  sprites: Sprite[]
}

export interface Sprite {
  x: number
  y: number
  type: string
}

export type Layer = 'walls' | 'floor' | 'ceiling'

export type Tool = 'paint' | 'erase' | 'sprite'
