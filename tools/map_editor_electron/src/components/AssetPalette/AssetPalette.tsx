import { useState, useCallback } from "react";
import { useMapStore } from "../../store/mapStore";
import SpritePalette from "./SpritePalette";
import TexturePalette from "./TexturePalette";
import SpriteSheetSidebar from "../SpriteSheetTool/SpriteSheetSidebar";
import { EDITOR_TOOLS } from "../../types";

export default function AssetPalette() {
  const activeTool = useMapStore((s) => s.activeTool);
  const [sidebarWidth, setSidebarWidth] = useState(176);

  const handleResizeStart = useCallback(
    (e: React.MouseEvent) => {
      e.preventDefault();
      const startX = e.clientX;
      const startW = sidebarWidth;

      const onMove = (ev: MouseEvent) => {
        const newW = startW + (startX - ev.clientX);
        setSidebarWidth(Math.max(160, Math.min(500, newW)));
      };

      const onUp = () => {
        document.removeEventListener("mousemove", onMove);
        document.removeEventListener("mouseup", onUp);
        document.body.style.cursor = "";
        document.body.style.userSelect = "";
      };

      document.addEventListener("mousemove", onMove);
      document.addEventListener("mouseup", onUp);
      document.body.style.cursor = "col-resize";
      document.body.style.userSelect = "none";
    },
    [sidebarWidth],
  );

  return (
    <div
      className="bg-surface-dark border-l border-muted flex flex-col shrink-0 relative"
      style={{ width: sidebarWidth }}
    >
      <div
        className="absolute left-0 top-0 bottom-0 w-[5px] -ml-[2px] cursor-col-resize z-10 hover:bg-accent/30 active:bg-accent/50"
        onMouseDown={handleResizeStart}
      />
      {activeTool === EDITOR_TOOLS.Sprite && <SpritePalette />}
      {activeTool === EDITOR_TOOLS.Texture && <TexturePalette />}
      {activeTool === EDITOR_TOOLS.SpriteSheet && <SpriteSheetSidebar />}
    </div>
  );
}
