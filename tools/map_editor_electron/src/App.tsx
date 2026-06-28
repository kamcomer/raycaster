import { useEffect } from "react";
import Toolbar from "./components/Toolbar/Toolbar";
import GridEditor from "./components/GridEditor";
import AssetPalette from "./components/AssetPalette/AssetPalette";
import SpritePanel from "./components/SpritePanel";
import KeybindModal from "./components/ui/KeybindModal";
import { useMapStore } from "./store/mapStore";
import { EDITOR_TOOLS } from "./types";
import { MapLayer } from "./map/types";
import { openMap, saveMap } from "./utils/fileActions";

export default function App() {
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      const store = useMapStore.getState();
      const meta = e.ctrlKey || e.metaKey;

      if (meta && e.key === "n") {
        e.preventDefault();
        store.setShowNewDialog(true);
        return;
      }
      if (meta && e.key === "s") {
        e.preventDefault();
        saveMap();
        return;
      }
      if (meta && e.key === "o") {
        e.preventDefault();
        openMap();
        return;
      }

      if (!meta && !e.altKey && !e.shiftKey) {
        if (e.key === "?" || e.key === "/") {
          store.setShowKeybindModal(!store.showKeybindModal);
          return;
        }
        if (e.key === "Escape") {
          store.setShowKeybindModal(false);
          return;
        }
        if (e.key >= "1" && e.key <= "9") {
          store.setSelectedTexture(Number(e.key));
          return;
        }
        if (e.key === "0") {
          store.setSelectedTexture(0);
          return;
        }
        if (e.key === "p" || e.key === "P") {
          store.setActiveTool(EDITOR_TOOLS.Texture);
          return;
        }
        if (e.key === "e" || e.key === "E") {
          store.setActiveTool(EDITOR_TOOLS.Texture);
          store.setSelectedTexture(0);
          return;
        }
        if (e.key === "s" || e.key === "S") {
          store.setActiveTool(EDITOR_TOOLS.Sprite);
          return;
        }
        if (e.key === "w" || e.key === "W") {
          store.setActiveLayer(MapLayer.Walls);
          return;
        }
        if (e.key === "f" || e.key === "F") {
          store.setActiveLayer(MapLayer.Floor);
          return;
        }
        if (e.key === "c" || e.key === "C") {
          store.setActiveLayer(MapLayer.Ceiling);
          return;
        }

        const si = store.selectedSpriteIndex;
        if (si >= 0 && si < store.map.sprites.length) {
          const sprite = store.map.sprites[si];
          if (e.key === "ArrowUp") {
            e.preventDefault();
            store.updateSprite(si, { y: Math.max(0, sprite.y - 0.1) });
            return;
          }
          if (e.key === "ArrowDown") {
            e.preventDefault();
            store.updateSprite(si, {
              y: Math.min(store.map.height, sprite.y + 0.1),
            });
            return;
          }
          if (e.key === "ArrowLeft") {
            e.preventDefault();
            store.updateSprite(si, { x: Math.max(0, sprite.x - 0.1) });
            return;
          }
          if (e.key === "ArrowRight") {
            e.preventDefault();
            store.updateSprite(si, {
              x: Math.min(store.map.width, sprite.x + 0.1),
            });
            return;
          }
          if (e.key === "Delete" || e.key === "Backspace") {
            store.removeSprite(si);
            return;
          }
        }
      }
    };

    window.addEventListener("keydown", handleKeyDown);
    return () => window.removeEventListener("keydown", handleKeyDown);
  }, []);

  return (
    <>
      <Toolbar />
      <div className="flex flex-1 overflow-hidden">
        <GridEditor />
        <AssetPalette />
      </div>
      <KeybindModal />
    </>
  );
}
