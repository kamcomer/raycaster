import { useState, useCallback, useRef, useEffect } from "react";
import { useMapStore } from "../../store/mapStore";
import PanelHeader from "../ui/PanelHeader";
import ButtonGroup from "../ui/ButtonGroup";
import ColorSwatch from "../ui/ColorSwatch";
import LabeledNumberInput from "../ui/LabeledNumberInput";
import type { SpriteSheetEntry, SheetPickMode } from "../../types";

const api = window.api;

async function loadSheet(
  addSpriteSheet: (path: string, dataUrl: string) => void,
) {
  const absolutePath = await api.openTextureDialog();
  if (!absolutePath) return;
  const tex = await api.readTexture(absolutePath);
  addSpriteSheet(tex.path, tex.data);
}

async function exportSheet(sheet: SpriteSheetEntry) {
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
}

export default function SpriteSheetSidebar() {
  const spriteSheets = useMapStore((s) => s.spriteSheets);
  const activeSheetIndex = useMapStore((s) => s.activeSheetIndex);
  const setActiveSheetIndex = useMapStore((s) => s.setActiveSheetIndex);
  const addSpriteSheet = useMapStore((s) => s.addSpriteSheet);
  const updateSpriteSheet = useMapStore((s) => s.updateSpriteSheet);
  const addSheetGroup = useMapStore((s) => s.addSheetGroup);
  const removeSheetGroup = useMapStore((s) => s.removeSheetGroup);
  const renameSheetGroup = useMapStore((s) => s.renameSheetGroup);
  const setActiveGroupIndex = useMapStore((s) => s.setActiveGroupIndex);
  const activeGroupIndex = useMapStore((s) => s.activeGroupIndex);

  const sheetPickMode = useMapStore((s) => s.sheetPickMode);
  const setSheetPickMode = useMapStore((s) => s.setSheetPickMode);

  const workerRef = useRef<Worker | null>(null);
  const [editingGroup, setEditingGroup] = useState<number | null>(null);
  const [editName, setEditName] = useState("");
  const [detecting, setDetecting] = useState(false);

  useEffect(() => {
    return () => {
      workerRef.current?.terminate();
    };
  }, []);

  const sheet =
    activeSheetIndex >= 0 && activeSheetIndex < spriteSheets.length
      ? spriteSheets[activeSheetIndex]
      : null;

  const handleLoad = useCallback(() => loadSheet(addSpriteSheet), [addSpriteSheet]);

  const handleExport = useCallback(() => {
    if (sheet) exportSheet(sheet);
  }, [sheet]);

  const handleDetect = useCallback(async () => {
    if (!sheet || !sheet.bgColor) return;
    setDetecting(true);
    const img = await new Promise<HTMLImageElement>((resolve, reject) => {
      const i = new Image();
      i.onload = () => resolve(i);
      i.onerror = reject;
      i.src = sheet.dataUrl;
    });
    const offscreen = document.createElement("canvas");
    offscreen.width = img.naturalWidth;
    offscreen.height = img.naturalHeight;
    const octx = offscreen.getContext("2d");
    if (!octx) { setDetecting(false); return; }
    octx.drawImage(img, 0, 0);
    const imageData = octx.getImageData(0, 0, offscreen.width, offscreen.height);
    const buffer = imageData.data.buffer;
    const pad = sheet.edgePadding;
    const bounds = sheet.cropBox ?? { x: 0, y: 0, w: img.naturalWidth, h: img.naturalHeight };
    const sx = bounds.x + pad;
    const sy = bounds.y + pad;
    const sw = bounds.w - pad * 2;
    const sh = bounds.h - pad * 2;
    const scanBounds = sw > 0 && sh > 0 ? { x: sx, y: sy, w: sw, h: sh } : undefined;

    const worker = new Worker(
      new URL("./detectWorker.ts", import.meta.url),
      { type: "module" },
    );
    workerRef.current = worker;
    worker.postMessage(
      {
        buffer,
        width: imageData.width,
        height: imageData.height,
        bgHex: sheet.bgColor,
        ignoreColors: sheet.ignoreColors,
        scanBounds,
      },
      [buffer],
    );
    worker.onmessage = (ev: MessageEvent) => {
      updateSpriteSheet(activeSheetIndex, { detectedTextures: ev.data });
      setDetecting(false);
      worker.terminate();
      workerRef.current = null;
    };
    worker.onerror = () => {
      setDetecting(false);
      worker.terminate();
      workerRef.current = null;
    };
  }, [sheet, activeSheetIndex, updateSpriteSheet]);

  const handleStartRename = (i: number) => {
    setEditingGroup(i);
    setEditName(sheet?.groups[i]?.name ?? "");
  };

  const handleFinishRename = (i: number) => {
    if (editName.trim()) {
      renameSheetGroup(i, editName.trim());
    }
    setEditingGroup(null);
  };

  return (
    <div className="flex-1 flex flex-col overflow-hidden">
      <PanelHeader
        title="Sprite Sheets"
        actions={
          <button
            onClick={handleLoad}
            className="no-drag px-3 py-1.5 hover:bg-muted-hover border border-muted rounded text-sm ml-2"
          >
            +
          </button>
        }
      />

      <div className="flex-1 overflow-y-auto scrollbar-thin p-2 space-y-1">
        {spriteSheets.map((s, i) => (
          <button
            key={i}
            onClick={() => setActiveSheetIndex(i)}
            className={`w-full text-left text-xs px-2 py-1 rounded truncate ${
              i === activeSheetIndex
                ? "bg-accent text-white"
                : "text-gray-400 hover:bg-muted-hover"
            }`}
          >
            {s.path.split("/").pop()}
          </button>
        ))}
      </div>

      {sheet && (
        <div className="border-t border-muted p-2 space-y-3 overflow-y-auto scrollbar-thin">
          <div>
            <div className="text-xs font-semibold text-gray-400 uppercase tracking-wider mb-1">
              Colors
            </div>
            <div className="space-y-1">
              <ColorSwatch
                label="BG"
                color={sheet.bgColor}
                onClear={() =>
                  updateSpriteSheet(activeSheetIndex, { bgColor: null })
                }
              />
              <ColorSwatch
                label="Alpha"
                color={sheet.alphaColor}
                onClear={() =>
                  updateSpriteSheet(activeSheetIndex, { alphaColor: null })
                }
              />
            </div>
          </div>

          <div>
            <div className="text-xs font-semibold text-gray-400 uppercase tracking-wider mb-1">
              Detection
            </div>
            <button
              onClick={handleDetect}
              disabled={!sheet.bgColor || detecting}
              className="w-full no-drag px-3 py-1.5 bg-muted hover:bg-muted-hover disabled:opacity-40 rounded text-sm"
            >
              {detecting
                ? "Detecting..."
                : `Detect Textures${sheet.bgColor ? "" : " (set BG color first)"}`}
            </button>
            {sheet.detectedTextures.length > 0 && (
              <p className="text-xs text-gray-500 mt-1">
                {sheet.detectedTextures.length} textures found
              </p>
            )}
          </div>

          {sheet.cropBox && (
            <p className="text-xs text-gray-500">
              Crop: {Math.round(sheet.cropBox.x)},{Math.round(sheet.cropBox.y)} &rarr;{" "}
              {Math.round(sheet.cropBox.x + sheet.cropBox.w)},{Math.round(sheet.cropBox.y + sheet.cropBox.h)} &mdash;{" "}
              {Math.round(sheet.cropBox.w)}&times;{Math.round(sheet.cropBox.h)}
              <button
                onClick={() => updateSpriteSheet(activeSheetIndex, { cropBox: null })}
                className="ml-2 text-gray-600 hover:text-red-400"
              >
                ✕
              </button>
            </p>
          )}

          <LabeledNumberInput
            label="Pad"
            value={sheet.edgePadding}
            onChange={(v) => updateSpriteSheet(activeSheetIndex, { edgePadding: v })}
            min={0}
            max={200}
            step={1}
            size="sm"
          />

          <div>
            <div className="text-xs font-semibold text-gray-400 uppercase tracking-wider mb-1">
              Pick mode
            </div>
            <ButtonGroup
              items={["box", "select", "bg", "ignore", "alpha"] as SheetPickMode[]}
              selected={sheetPickMode}
              onSelect={(v) => setSheetPickMode(v as SheetPickMode)}
              variant="rounded"
              size="xs"
            />
          </div>

          <div className="space-y-1">
            <label className="flex items-center gap-2 text-xs text-gray-400 cursor-pointer">
              <input
                type="checkbox"
                checked={sheet.showAlphaMask}
                onChange={(e) =>
                  updateSpriteSheet(activeSheetIndex, {
                    showAlphaMask: e.target.checked,
                  })
                }
                className="accent-accent"
              />
              Show alpha mask
            </label>
          </div>

          <div>
            <div className="text-xs font-semibold text-gray-400 uppercase tracking-wider mb-1">
              Ignored Colors
            </div>
            <div className="flex flex-wrap gap-1">
              {sheet.ignoreColors.length === 0 && (
                <span className="text-xs text-gray-600">None</span>
              )}
              {sheet.ignoreColors.map((c, i) => (
                <div
                  key={i}
                  className="flex items-center gap-1 px-1.5 py-0.5 rounded text-xs border border-muted"
                >
                  <span
                    className="inline-block w-3 h-3 rounded"
                    style={{ backgroundColor: c }}
                  />
                  <span className="text-gray-400">{c}</span>
                  <button
                    onClick={() => {
                      const next = sheet.ignoreColors.filter((_, j) => j !== i);
                      updateSpriteSheet(activeSheetIndex, { ignoreColors: next });
                    }}
                    className="text-gray-600 hover:text-red-400"
                  >
                    ✕
                  </button>
                </div>
              ))}
            </div>
          </div>

          <div>
            <div className="text-xs font-semibold text-gray-400 uppercase tracking-wider mb-1 flex items-center justify-between">
              Groups
              <button
                onClick={() => addSheetGroup("New Group")}
                className="text-accent hover:text-accent-hover text-xs"
              >
                + New
              </button>
            </div>
            <div className="space-y-0.5">
              {sheet.groups.map((g, i) => (
                <div
                  key={i}
                  className={`flex items-center gap-1 px-1.5 py-0.5 rounded cursor-pointer text-xs ${
                    i === activeGroupIndex
                      ? "bg-accent/20 text-white"
                      : "hover:bg-muted-hover text-gray-400"
                  }`}
                  onClick={() => setActiveGroupIndex(i)}
                >
                  {editingGroup === i ? (
                    <input
                      autoFocus
                      value={editName}
                      onChange={(e) => setEditName(e.target.value)}
                      onBlur={() => handleFinishRename(i)}
                      onKeyDown={(e) => {
                        if (e.key === "Enter") handleFinishRename(i);
                        if (e.key === "Escape") setEditingGroup(null);
                      }}
                      className="flex-1 bg-surface-dark border border-muted rounded px-1 py-0.5 text-xs text-white"
                      onClick={(e) => e.stopPropagation()}
                    />
                  ) : (
                    <span
                      className="flex-1 truncate"
                      onDoubleClick={() => handleStartRename(i)}
                    >
                      {g.name}
                    </span>
                  )}
                  <span className="text-gray-600 shrink-0">
                    {g.frames.length}
                  </span>
                  <button
                    onClick={(e) => {
                      e.stopPropagation();
                      removeSheetGroup(i);
                    }}
                    className="text-gray-600 hover:text-red-400 shrink-0"
                  >
                    ✕
                  </button>
                </div>
              ))}
            </div>
          </div>

          <button
            onClick={handleExport}
            className="w-full no-drag px-3 py-1.5 bg-accent hover:bg-accent-hover rounded text-sm text-white"
          >
            Export Sheet
          </button>
        </div>
      )}
    </div>
  );
}
