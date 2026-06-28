import { useCallback } from "react";
import { useMapStore } from "../../store/mapStore";

const api = window.api;

export default function SpriteSheetToolbar() {
  const spriteSheets = useMapStore((s) => s.spriteSheets);
  const activeSheetIndex = useMapStore((s) => s.activeSheetIndex);

  const sheet =
    activeSheetIndex >= 0 && activeSheetIndex < spriteSheets.length
      ? spriteSheets[activeSheetIndex]
      : null;

  const handleExport = useCallback(async () => {
    if (!sheet) return;
    const name = sheet.path.split("/").pop()?.replace(/\.[^.]+$/, "") || "sheet";
    const json = JSON.stringify(
      {
        path: sheet.path,
        bgColor: sheet.bgColor,
        alphaColor: sheet.alphaColor,
        textures: sheet.detectedTextures,
        groups: sheet.groups,
      },
      null,
      2,
    );
    const path = await api.saveDialog(`${name}.sheet.json`);
    if (!path) return;
    await api.writeFile(path, json);
  }, [sheet]);

  if (!sheet || sheet.detectedTextures.length === 0) {
    return null;
  }

  return (
    <button
      onClick={handleExport}
      className="no-drag px-3 py-1.5 bg-accent hover:bg-accent-hover rounded text-sm text-white"
    >
      Export
    </button>
  );
}
