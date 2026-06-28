import { detectTextures } from "./detectTextures";

self.onmessage = (e: MessageEvent<{
  buffer: ArrayBuffer;
  width: number;
  height: number;
  bgHex: string;
  ignoreColors: string[];
  scanBounds?: { x: number; y: number; w: number; h: number };
}>) => {
  const { buffer, width, height, bgHex, ignoreColors, scanBounds } = e.data;
  const clamped = new Uint8ClampedArray(buffer);
  const imageData = new ImageData(clamped, width, height);
  const result = detectTextures(imageData, bgHex, ignoreColors, scanBounds);
  self.postMessage(result);
};
