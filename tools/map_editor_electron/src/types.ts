export enum MouseButton {
  Left,
  Middle,
  Right,
}

export const EDITOR_TOOLS = {
  Texture: "texture",
  Sprite: "sprite",
} as const;

export type EditorTool = (typeof EDITOR_TOOLS)[keyof typeof EDITOR_TOOLS];
