import { useMapStore } from "../../store/mapStore";
import SpritePalette from "./SpritePalette";
import TexturePalette from "./TexturePalette";
import { EDITOR_TOOLS, EditorTool } from "../../types";

export default function AssetPalette() {
  const activeTool = useMapStore((s) => s.activeTool);

  return (
    <div className="w-44 bg-[#16213e] border-l border-[#0f3460] flex flex-col shrink-0">
      {activeTool === EDITOR_TOOLS.Sprite && <SpritePalette />}
      {activeTool === EDITOR_TOOLS.Texture && <TexturePalette />}
    </div>
  );
}
