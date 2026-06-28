export enum MouseButton {
  Left,
  Middle,
  Right,
}

export const EDITOR_TOOLS = {
  Texture: "texture",
  Sprite: "sprite",
  SpriteSheet: "spritesheet",
} as const;

export type EditorTool = (typeof EDITOR_TOOLS)[keyof typeof EDITOR_TOOLS];

export type SheetPickMode = "box" | "select" | "bg" | "ignore" | "alpha";

export interface DetectedTexture {
  id: number;
  x: number;
  y: number;
  w: number;
  h: number;
}

export interface SpriteSheetGroup {
  name: string;
  frames: number[];
}

export interface SpriteSheetEntry {
  path: string;
  dataUrl: string;
  detectedTextures: DetectedTexture[];
  bgColor: string | null;
  alphaColor: string | null;
  showAlphaMask: boolean;
  groups: SpriteSheetGroup[];
  cropBox: { x: number; y: number; w: number; h: number } | null;
  edgePadding: number;
  ignoreColors: string[];
}
