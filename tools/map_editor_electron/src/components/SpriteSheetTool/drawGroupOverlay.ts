import type { DetectedTexture } from "../../types";

const GROUP_COLORS = [
  "rgba(0, 200, 255, 0.35)",
  "rgba(255, 200, 0, 0.35)",
  "rgba(0, 255, 100, 0.35)",
  "rgba(255, 80, 200, 0.35)",
  "rgba(180, 120, 255, 0.35)",
];

const GROUP_LABEL_COLORS = [
  "rgba(0, 200, 255, 1)",
  "rgba(255, 200, 0, 1)",
  "rgba(0, 220, 100, 1)",
  "rgba(255, 80, 200, 1)",
  "rgba(180, 120, 255, 1)",
];

const GROUP_BORDER_COLORS = [
  "rgba(0, 200, 255, 0.7)",
  "rgba(255, 200, 0, 0.7)",
  "rgba(0, 220, 100, 0.7)",
  "rgba(255, 80, 200, 0.7)",
  "rgba(180, 120, 255, 0.7)",
];

export function drawGroupOverlay(
  ctx: CanvasRenderingContext2D,
  scale: number,
  detectedTextures: DetectedTexture[],
  groups: { name: string; frames: number[] }[],
  activeGroupIndex: number,
): void {
  if (activeGroupIndex < 0 || activeGroupIndex >= groups.length) return;

  const group = groups[activeGroupIndex];
  const colorIndex = activeGroupIndex % GROUP_COLORS.length;
  const fillColor = GROUP_COLORS[colorIndex];
  const borderColor = GROUP_BORDER_COLORS[colorIndex];
  const labelColor = GROUP_LABEL_COLORS[colorIndex];

  const idToRect = new Map(detectedTextures.map((t) => [t.id, t]));

  for (const fi of group.frames) {
    const rect = idToRect.get(fi);
    if (!rect) continue;

    const sx = rect.x * scale;
    const sy = rect.y * scale;
    const sw = rect.w * scale;
    const sh = rect.h * scale;

    ctx.fillStyle = fillColor;
    ctx.fillRect(sx, sy, sw, sh);

    ctx.strokeStyle = borderColor;
    ctx.lineWidth = Math.max(1, 1 / scale);
    ctx.strokeRect(sx, sy, sw, sh);
  }

  if (group.frames.length > 0) {
    const firstRect = idToRect.get(group.frames[0]);
    if (firstRect) {
      const x = firstRect.x * scale;
      const y = firstRect.y * scale;
      const labelFontSize = Math.max(10, Math.round(12 / scale));
      ctx.font = `bold ${labelFontSize}px monospace`;

      ctx.fillStyle = "rgba(0, 0, 0, 0.6)";
      const textW = ctx.measureText(group.name).width;
      ctx.fillRect(x, y - labelFontSize - 4, textW + 6, labelFontSize + 4);

      ctx.fillStyle = labelColor;
      ctx.fillText(group.name, x + 3, y - 4);
    }
  }
}
