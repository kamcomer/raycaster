import { useMapStore } from "../../store/mapStore";
import SpriteSheetCanvas from "./SpriteSheetCanvas";

export default function SpriteSheetTool() {
  const spriteSheets = useMapStore((s) => s.spriteSheets);
  const activeSheetIndex = useMapStore((s) => s.activeSheetIndex);

  const sheet =
    activeSheetIndex >= 0 && activeSheetIndex < spriteSheets.length
      ? spriteSheets[activeSheetIndex]
      : null;

  if (!sheet) {
    return (
      <div className="flex-1 overflow-hidden bg-surface-canvas flex items-center justify-center">
        <p className="text-gray-500 text-sm">
          Load a sprite sheet to begin
        </p>
      </div>
    );
  }

  return <SpriteSheetCanvas sheet={sheet} sheetIndex={activeSheetIndex} />;
}
