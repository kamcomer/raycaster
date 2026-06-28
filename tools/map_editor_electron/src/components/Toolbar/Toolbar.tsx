import { useState } from "react";
import { useMapStore } from "../../store/mapStore";
import { parseMap, serializeMap } from "../../map/parser";
import { EditorTool, EDITOR_TOOLS } from "../../types";
import { MAP_LAYERS } from "../../map/types";
import TextureToolBar from "./TextureToolBar";
import SpritToolBar from "./SpriteToolBar";

export default function Toolbar() {
  const map = useMapStore((s) => s.map);
  const filePath = useMapStore((s) => s.filePath);
  const loadMap = useMapStore((s) => s.loadMap);
  const newMap = useMapStore((s) => s.newMap);
  const setFilePath = useMapStore((s) => s.setFilePath);
  const activeLayer = useMapStore((s) => s.activeLayer);
  const setActiveLayer = useMapStore((s) => s.setActiveLayer);
  const setActiveTool = useMapStore((s) => s.setActiveTool);
  const activeTool = useMapStore((s) => s.activeTool);

  const showNewDialog = useMapStore((s) => s.showNewDialog);
  const setShowNewDialog = useMapStore((s) => s.setShowNewDialog);
  const [newWidth, setNewWidth] = useState(24);
  const [newHeight, setNewHeight] = useState(24);

  const api = window.api;

  const handleNew = () => {
    newMap(newWidth, newHeight);
    setShowNewDialog(false);
  };

  const handleOpen = async () => {
    const path = await api.openDialog();
    if (!path) return;
    const text = await api.readFile(path);
    const data = parseMap(text);
    loadMap(data);
    setFilePath(path);
    const textureEntries: { path: string; dataUrl: string }[] = [];
    for (const texPath of data.textures) {
      try {
        const absPath = await api.resolveFromRoot(path, texPath);
        const tex = await api.readTexture(absPath);
        textureEntries.push({ path: texPath, dataUrl: tex.data });
      } catch (err) {
        console.warn("could not load texture:", texPath, err);
      }
    }
    if (textureEntries.length > 0) {
      useMapStore.getState().populateTextures(textureEntries);
    }
    const spriteTypeEntries: {
      path: string;
      frameCount: number;
      frameDelay: number;
      dataUrl: string;
    }[] = [];
    for (const st of data.spriteTypes) {
      try {
        const absPath = await api.resolveFromRoot(path, st.path);
        const tex = await api.readTexture(absPath);
        spriteTypeEntries.push({
          path: st.path,
          frameCount: st.frameCount,
          frameDelay: st.frameDelay,
          dataUrl: tex.data,
        });
      } catch (err) {
        console.warn("could not load sprite type:", st.path, err);
      }
    }
    if (spriteTypeEntries.length > 0) {
      useMapStore.getState().populateSpriteTypes(spriteTypeEntries);
    }
  };

  const handleSave = async () => {
    const content = serializeMap(map);
    const path = filePath || (await api.saveDialog("map.txt"));
    if (!path) return;
    await api.writeFile(path, content);
    setFilePath(path);
  };

  const handleSaveAs = async () => {
    const content = serializeMap(map);
    const path = await api.saveDialog("map.txt");
    if (!path) return;
    await api.writeFile(path, content);
    setFilePath(path);
  };

  const handleExport = handleSaveAs;

  const [showResizeDialog, setShowResizeDialog] = useState(false);
  const [resizeWidth, setResizeWidth] = useState(map.width);
  const [resizeHeight, setResizeHeight] = useState(map.height);
  const resizeMap = useMapStore((s) => s.resizeMap);

  const handleResize = () => {
    resizeMap(resizeWidth, resizeHeight);
    setShowResizeDialog(false);
  };

  const menuButtons = {
    new: () => setShowNewDialog(true),
    open: handleOpen,
    save: handleSave,
    export: handleExport,
  };

  return (
    <>
      <div className="h-12 bg-surface-dark border-b border-muted flex items-center pl-[76px] pr-3 gap-2 drag-region shrink-0 justify-around">
        <div className="h-full">
          {Object.entries(menuButtons).map(([label, fn], index) => {
            const rounded =
              index === 0
                ? "rounded-l"
                : index === Object.keys(menuButtons).length - 1
                  ? "rounded-r"
                  : "";
            return (
              <button
                onClick={fn}
                className={`h-full no-drag px-3 py-1.5  hover:bg-muted-hover border border-muted ${rounded} text-sm`}
              >
                <span className="capitalize">{label}</span>
              </button>
            );
          })}
        </div>

        <div className="w-px h-6 bg-muted mx-2" />

        <select
          id="editorToolSelect"
          value={activeTool}
          onChange={(e) => setActiveTool(e.target.value as EditorTool)}
          className="no-drag"
        >
          {Object.values(EDITOR_TOOLS).map((item: string) => {
            return (
              <option key={item} value={item}>
                {item}
              </option>
            );
          })}
        </select>

        <div>
          {MAP_LAYERS.map((l, index) => {
            const rounded =
              index === 0
                ? "rounded-l"
                : index === MAP_LAYERS.length - 1
                  ? "rounded-r"
                  : "";
            return (
              <button
                key={l}
                onClick={() => setActiveLayer(l)}
                className={`no-drag px-3 py-1.5 text-sm ${
                  activeLayer === l
                    ? "bg-accent text-white"
                    : "bg-muted hover:bg-muted-hover"
                } ${rounded}`}
              >
                <span className="capitalize">{l}</span>
              </button>
            );
          })}
        </div>

        <div className="w-px h-6 bg-muted mx-2" />

        {activeTool === EDITOR_TOOLS.Texture && <TextureToolBar />}
        {activeTool === EDITOR_TOOLS.Sprite && <SpritToolBar />}

        {/* <div className="flex-1" /> */}

        <div className="h-8 flex rounded border border-muted items-center">
          <button
            onClick={() => {
              setResizeWidth(map.width);
              setResizeHeight(map.height);
              setShowResizeDialog(true);
            }}
            className="no-drag px-2 bg-muted hover:bg-muted-hover rounded text-xs"
          >
            Resize
          </button>
          <span className="no-drag px-2 text-xs text-gray-500">
            {map.width}x{map.height}
            {filePath ? ` · ${filePath.split("/").pop()}` : ""}
          </span>
        </div>
      </div>

      {showNewDialog && (
        <div className="fixed inset-0 bg-black/50 flex items-center justify-center z-50">
          <div className="bg-surface border border-muted rounded-lg p-6">
            <h2 className="text-lg mb-4">New Map</h2>
            <div className="flex gap-4 mb-4">
              <label>
                <span className="text-sm text-gray-400 mr-2">Width:</span>
                <input
                  type="number"
                  min={3}
                  max={256}
                  value={newWidth}
                  onChange={(e) => setNewWidth(Number(e.target.value))}
                  className="w-20 bg-surface-dark border border-muted rounded px-2 py-1 text-sm"
                />
              </label>
              <label>
                <span className="text-sm text-gray-400 mr-2">Height:</span>
                <input
                  type="number"
                  min={3}
                  max={256}
                  value={newHeight}
                  onChange={(e) => setNewHeight(Number(e.target.value))}
                  className="w-20 bg-surface-dark border border-muted rounded px-2 py-1 text-sm"
                />
              </label>
            </div>
            <div className="flex gap-2 justify-end">
              <button
                onClick={() => setShowNewDialog(false)}
                className="px-4 py-1.5 bg-muted hover:bg-muted-hover rounded text-sm"
              >
                Cancel
              </button>
              <button
                onClick={handleNew}
                className="px-4 py-1.5 bg-accent hover:bg-accent-hover rounded text-sm"
              >
                Create
              </button>
            </div>
          </div>
        </div>
      )}

      {showResizeDialog && (
        <div className="fixed inset-0 bg-black/50 flex items-center justify-center z-50">
          <div className="bg-surface border border-muted rounded-lg p-6">
            <h2 className="text-lg mb-4">Resize Map</h2>
            <p className="text-xs text-gray-400 mb-4">
              Existing content will be preserved. New cells start empty.
            </p>
            <div className="flex gap-4 mb-4">
              <label>
                <span className="text-sm text-gray-400 mr-2">Width:</span>
                <input
                  type="number"
                  min={3}
                  max={256}
                  value={resizeWidth}
                  onChange={(e) => setResizeWidth(Number(e.target.value))}
                  className="w-20 bg-surface-dark border border-muted rounded px-2 py-1 text-sm"
                />
              </label>
              <label>
                <span className="text-sm text-gray-400 mr-2">Height:</span>
                <input
                  type="number"
                  min={3}
                  max={256}
                  value={resizeHeight}
                  onChange={(e) => setResizeHeight(Number(e.target.value))}
                  className="w-20 bg-surface-dark border border-muted rounded px-2 py-1 text-sm"
                />
              </label>
            </div>
            <div className="flex gap-2 justify-end">
              <button
                onClick={() => setShowResizeDialog(false)}
                className="px-4 py-1.5 bg-muted hover:bg-muted-hover rounded text-sm"
              >
                Cancel
              </button>
              <button
                onClick={handleResize}
                className="px-4 py-1.5 bg-accent hover:bg-accent-hover rounded text-sm"
              >
                Apply
              </button>
            </div>
          </div>
        </div>
      )}
    </>
  );
}
