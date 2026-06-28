import { useState } from "react";
import { useMapStore } from "../../store/mapStore";
import { preloadTexture } from "../../utils/textureCache";

type PaintTool = "paint" | "erase";

export default function TextureToolBar() {
  const importTexture = useMapStore((s) => s.importTexture);
  const selectedTexture = useMapStore((s) => s.selectedTexture);
  const setSelectedTexture = useMapStore((s) => s.setSelectedTexture);

  const [paintTool, setPaintTool] = useState<PaintTool>(
    selectedTexture === 0 ? "erase" : "paint",
  );

  const handleSelectPaintTool = (tool: PaintTool) => {
    setPaintTool(tool);
    setSelectedTexture(tool === "erase" ? 0 : 1);
  };

  return (
    <div>
      <button
        onClick={() => handleSelectPaintTool("paint")}
        className={`no-drag px-3 py-1.5 rounded text-sm ${
          paintTool === "paint"
            ? "bg-[#e94560] text-white"
            : "bg-[#0f3460] hover:bg-[#1a5276]"
        }`}
      >
        Paint
      </button>
      <button
        onClick={() => handleSelectPaintTool("erase")}
        className={`no-drag px-3 py-1.5 rounded text-sm ${
          paintTool === "erase"
            ? "bg-[#e94560] text-white"
            : "bg-[#0f3460] hover:bg-[#1a5276]"
        }`}
      >
        Erase
      </button>
    </div>
  );
}
