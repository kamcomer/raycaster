import { useState } from "react";
import { useMapStore } from "../../store/mapStore";
import { EditorTool, EDITOR_TOOLS } from "../../types";
import { MAP_LAYERS, MapLayer } from "../../map/types";
import ButtonGroup from "../ui/ButtonGroup";
import Dialog, { DialogActions } from "../ui/Dialog";
import LabeledNumberInput from "../ui/LabeledNumberInput";
import TextureToolBar from "./TextureToolBar";
import SpriteSheetToolbar from "../SpriteSheetTool/SpriteSheetToolbar";
import { openMap, saveMap, saveMapAs } from "../../utils/fileActions";

export default function Toolbar() {
  const map = useMapStore((s) => s.map);
  const filePath = useMapStore((s) => s.filePath);
  const newMap = useMapStore((s) => s.newMap);
  const setShowNewDialog = useMapStore((s) => s.setShowNewDialog);
  const activeLayer = useMapStore((s) => s.activeLayer);
  const setActiveLayer = useMapStore((s) => s.setActiveLayer);
  const setActiveTool = useMapStore((s) => s.setActiveTool);
  const activeTool = useMapStore((s) => s.activeTool);

  const showNewDialog = useMapStore((s) => s.showNewDialog);
  const [newWidth, setNewWidth] = useState(24);
  const [newHeight, setNewHeight] = useState(24);

  const handleNew = () => {
    newMap(newWidth, newHeight);
    setShowNewDialog(false);
  };

  const handleExport = saveMapAs;

  const [showResizeDialog, setShowResizeDialog] = useState(false);
  const [resizeWidth, setResizeWidth] = useState(map.width);
  const [resizeHeight, setResizeHeight] = useState(map.height);
  const resizeMap = useMapStore((s) => s.resizeMap);

  const handleResize = () => {
    resizeMap(resizeWidth, resizeHeight);
    setShowResizeDialog(false);
  };

  const setShowKeybindModal = useMapStore((s) => s.setShowKeybindModal);

  const menuActions: Record<string, () => void> = {
    new: () => setShowNewDialog(true),
    open: openMap,
    save: saveMap,
    export: handleExport,
  };

  return (
    <>
      <div className="h-12 bg-surface-dark border-b border-muted flex items-center pl-[76px] pr-3 gap-3 drag-region shrink-0">
        <div className="h-full">
          <ButtonGroup
            items={["new", "open", "save", "export"]}
            onSelect={(v) => menuActions[v]()}
            variant="split"
            fill
          />
        </div>

        <div className="w-px h-6 bg-muted" />

        <div className="flex items-center gap-2">
          <span className="text-xs text-gray-400 uppercase tracking-wider font-semibold">
            Tool:
          </span>
          <div className="relative">
            <select
              id="editorToolSelect"
              value={activeTool}
              onChange={(e) => setActiveTool(e.target.value as EditorTool)}
              className="no-drag bg-muted text-sm text-white border border-muted rounded px-3 py-1.5 pr-7 appearance-none cursor-pointer"
            >
              {Object.values(EDITOR_TOOLS).map((item) => (
                <option key={item} value={item}>
                  {item}
                </option>
              ))}
            </select>
            <div className="pointer-events-none absolute inset-y-0 right-0 flex items-center pr-2">
              <i className="bi bi-chevron-down text-xs text-gray-400" />
            </div>
          </div>
        </div>

        {activeTool === EDITOR_TOOLS.Texture && (
          <>
            <TextureToolBar />
            <ButtonGroup
              items={MAP_LAYERS}
              selected={activeLayer}
              onSelect={(v) => setActiveLayer(v as MapLayer)}
              variant="split"
            />
          </>
        )}

        {activeTool === EDITOR_TOOLS.SpriteSheet && <SpriteSheetToolbar />}

        <div className="flex-1" />

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
          <button
            onClick={() => setShowKeybindModal(true)}
            className="no-drag ml-1 w-6 h-6 flex items-center justify-center bg-muted hover:bg-muted-hover rounded text-xs text-gray-400"
            title="Keybindings (?)"
          >
            ?
          </button>
        </div>
      </div>

      <Dialog open={showNewDialog} onClose={() => setShowNewDialog(false)} title="New Map">
        <div className="flex gap-4 mb-4">
          <LabeledNumberInput label="Width:" value={newWidth} onChange={setNewWidth} min={3} max={256} />
          <LabeledNumberInput label="Height:" value={newHeight} onChange={setNewHeight} min={3} max={256} />
        </div>
        <DialogActions
          onCancel={() => setShowNewDialog(false)}
          confirmLabel="Create"
          onConfirm={handleNew}
        />
      </Dialog>

      <Dialog open={showResizeDialog} onClose={() => setShowResizeDialog(false)} title="Resize Map" description="Existing content will be preserved. New cells start empty.">
        <div className="flex gap-4 mb-4">
          <LabeledNumberInput label="Width:" value={resizeWidth} onChange={setResizeWidth} min={3} max={256} />
          <LabeledNumberInput label="Height:" value={resizeHeight} onChange={setResizeHeight} min={3} max={256} />
        </div>
        <DialogActions
          onCancel={() => setShowResizeDialog(false)}
          confirmLabel="Apply"
          onConfirm={handleResize}
        />
      </Dialog>
    </>
  );
}
