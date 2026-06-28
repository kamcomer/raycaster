export interface SpriteTypeDef {
  path: string;
  frameCount: number;
  frameDelay: number;
}

export interface MapData {
  width: number;
  height: number;
  walls: number[][];
  floor: number[][];
  ceiling: number[][];
  textures: string[];
  spriteTypes: SpriteTypeDef[];
  sprites: Sprite[];
}

export interface Sprite {
  x: number;
  y: number;
  type: number;
}

export enum MapLayer {
  Walls = "walls",
  Floor = "floor",
  Ceiling = "ceiling",
}

export const MAP_LAYERS = [MapLayer.Walls, MapLayer.Floor, MapLayer.Ceiling];
