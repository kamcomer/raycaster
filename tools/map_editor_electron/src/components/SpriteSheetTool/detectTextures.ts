import type { DetectedTexture } from "../../types";

interface Rgb { r: number; g: number; b: number }

function parseHex(hex: string): Rgb {
  return {
    r: parseInt(hex.slice(1, 3), 16),
    g: parseInt(hex.slice(3, 5), 16),
    b: parseInt(hex.slice(5, 7), 16),
  };
}

export function detectTextures(
  imageData: ImageData,
  bgHex: string,
  ignoreColors: string[] = [],
  scanBounds?: { x: number; y: number; w: number; h: number },
): DetectedTexture[] {
  const { data, width, height } = imageData;
  const visited = new Uint8Array(width * height);
  const textures: DetectedTexture[] = [];
  let nextId = 0;

  const transparentColors = [parseHex(bgHex), ...ignoreColors.map(parseHex)];

  const minX = scanBounds?.x ?? 0;
  const minY = scanBounds?.y ?? 0;
  const maxX = scanBounds ? scanBounds.x + scanBounds.w : width;
  const maxY = scanBounds ? scanBounds.y + scanBounds.h : height;

  function isTransparent(x: number, y: number): boolean {
    const i = (y * width + x) * 4;
    const pr = data[i], pg = data[i + 1], pb = data[i + 2];
    for (const c of transparentColors) {
      if (pr === c.r && pg === c.g && pb === c.b) return true;
    }
    return false;
  }

  function floodFill(
    startX: number,
    startY: number,
  ): { minX: number; minY: number; maxX: number; maxY: number } {
    let fx = startX;
    let fy = startY;
    let fx2 = startX;
    let fy2 = startY;
    const stack: [number, number][] = [[startX, startY]];
    visited[startY * width + startX] = 1;

    while (stack.length > 0) {
      const [x, y] = stack.pop()!;
      if (x < fx) fx = x;
      if (y < fy) fy = y;
      if (x > fx2) fx2 = x;
      if (y > fy2) fy2 = y;

      if (x > minX && !visited[y * width + (x - 1)] && !isTransparent(x - 1, y)) {
        visited[y * width + (x - 1)] = 1;
        stack.push([x - 1, y]);
      }
      if (x < maxX - 1 && !visited[y * width + (x + 1)] && !isTransparent(x + 1, y)) {
        visited[y * width + (x + 1)] = 1;
        stack.push([x + 1, y]);
      }
      if (y > minY && !visited[(y - 1) * width + x] && !isTransparent(x, y - 1)) {
        visited[(y - 1) * width + x] = 1;
        stack.push([x, y - 1]);
      }
      if (y < maxY - 1 && !visited[(y + 1) * width + x] && !isTransparent(x, y + 1)) {
        visited[(y + 1) * width + x] = 1;
        stack.push([x, y + 1]);
      }
    }

    return { minX: fx, minY: fy, maxX: fx2, maxY: fy2 };
  }

  for (let y = minY; y < maxY; y++) {
    for (let x = minX; x < maxX; x++) {
      if (!visited[y * width + x] && !isTransparent(x, y)) {
        const { minX: rminX, minY: rminY, maxX: rmaxX, maxY: rmaxY } = floodFill(x, y);
        textures.push({
          id: nextId++,
          x: rminX,
          y: rminY,
          w: rmaxX - rminX + 1,
          h: rmaxY - rminY + 1,
        });
      }
    }
  }

  return textures;
}
