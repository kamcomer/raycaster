import { useEffect, useRef, useState, useCallback } from "react";
import { useMapStore } from "../../store/mapStore";
import { usePixelPicker } from "./usePixelPicker";
import { applyAlphaMask } from "./applyAlphaMask";
import { drawGroupOverlay } from "./drawGroupOverlay";
import type { SpriteSheetEntry } from "../../types";

interface Props {
  sheet: SpriteSheetEntry;
  sheetIndex: number;
}

const TEXTURE_OUTLINE = "rgba(255, 255, 255, 0.25)";
const TEXTURE_FILL = "rgba(255, 255, 255, 0.04)";

export default function SpriteSheetCanvas({ sheet, sheetIndex }: Props) {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const containerRef = useRef<HTMLDivElement>(null);
  const imageRef = useRef<HTMLImageElement | null>(null);
  const alphaCanvasRef = useRef<HTMLCanvasElement | null>(null);
  const [loaded, setLoaded] = useState(false);
  const [zoom, setZoom] = useState(1);
  const [pan, setPan] = useState({ x: 0, y: 0 });
  const panStartRef = useRef<{ startX: number; startY: number; panX: number; panY: number } | null>(null);
  const dragStartRef = useRef<{ x: number; y: number } | null>(null);
  const dragCurrentRef = useRef<{ x: number; y: number } | null>(null);
  const dragBoxRef = useRef<{ x: number; y: number; w: number; h: number } | null>(null);
  const drawFnRef = useRef<() => void>(() => {});

  const updateSpriteSheet = useMapStore((s) => s.updateSpriteSheet);
  const activeGroupIndex = useMapStore((s) => s.activeGroupIndex);
  const toggleFrameInGroup = useMapStore((s) => s.toggleFrameInGroup);
  const sheetPickMode = useMapStore((s) => s.sheetPickMode);

  const { pickColor, setProcessedSource } = usePixelPicker(canvasRef);

  const loadAlphaMasked = useCallback(
    (img: HTMLImageElement) => {
      if (!sheet.alphaColor) {
        alphaCanvasRef.current = null;
        setProcessedSource(null);
        return;
      }
      const offscreen = document.createElement("canvas");
      offscreen.width = img.naturalWidth;
      offscreen.height = img.naturalHeight;
      const octx = offscreen.getContext("2d");
      if (!octx) return;
      octx.drawImage(img, 0, 0);
      const imageData = octx.getImageData(0, 0, offscreen.width, offscreen.height);
      applyAlphaMask(imageData, sheet.alphaColor);
      octx.putImageData(imageData, 0, 0);
      alphaCanvasRef.current = offscreen;
      setProcessedSource(offscreen);
    },
    [sheet.alphaColor, setProcessedSource],
  );

  const loadAlphaMaskVisualization = useCallback(
    (img: HTMLImageElement) => {
      if (!sheet.alphaColor) return null;
      const offscreen = document.createElement("canvas");
      offscreen.width = img.naturalWidth;
      offscreen.height = img.naturalHeight;
      const octx = offscreen.getContext("2d");
      if (!octx) return null;
      octx.drawImage(img, 0, 0);
      const imageData = octx.getImageData(0, 0, offscreen.width, offscreen.height);
      const { data } = imageData;
      const r = parseInt(sheet.alphaColor.slice(1, 3), 16);
      const g = parseInt(sheet.alphaColor.slice(3, 5), 16);
      const b = parseInt(sheet.alphaColor.slice(5, 7), 16);
      for (let i = 0; i < data.length; i += 4) {
        const isMasked = data[i] === r && data[i + 1] === g && data[i + 2] === b;
        data[i] = isMasked ? 0 : 255;
        data[i + 1] = isMasked ? 0 : 255;
        data[i + 2] = isMasked ? 0 : 255;
        data[i + 3] = 255;
      }
      octx.putImageData(imageData, 0, 0);
      return offscreen;
    },
    [sheet.alphaColor],
  );

  useEffect(() => {
    imageRef.current = null;
    setLoaded(false);
    alphaCanvasRef.current = null;
    setProcessedSource(null);
    setZoom(1);
    setPan({ x: 0, y: 0 });
    const img = new Image();
    img.onload = () => {
      imageRef.current = img;
      setLoaded(true);
    };
    img.src = sheet.dataUrl;
  }, [sheet.dataUrl, setProcessedSource]);

  useEffect(() => {
    const img = imageRef.current;
    if (!img || !loaded) return;
    loadAlphaMasked(img);
  }, [sheet.alphaColor, loadAlphaMasked, loaded]);

  useEffect(() => {
    const canvas = canvasRef.current;
    const container = containerRef.current;
    if (!canvas || !container) return;

    const img = imageRef.current;
    if (!img) return;

    const draw = () => {
      const cw = container.clientWidth;
      const ch = container.clientHeight;
      canvas.width = cw;
      canvas.height = ch;

      const baseScale = Math.min(cw / img.naturalWidth, ch / img.naturalHeight);
      const effectiveScale = baseScale * zoom;
      const offsetX = (cw - img.naturalWidth * effectiveScale) / 2 + pan.x;
      const offsetY = (ch - img.naturalHeight * effectiveScale) / 2 + pan.y;

      const ctx = canvas.getContext("2d");
      if (!ctx) return;

      ctx.clearRect(0, 0, cw, ch);

      if (sheet.bgColor) {
        ctx.fillStyle = sheet.bgColor;
        ctx.fillRect(0, 0, cw, ch);
      }

      ctx.save();
      ctx.translate(offsetX, offsetY);
      ctx.scale(effectiveScale, effectiveScale);

      if (sheet.showAlphaMask && sheet.alphaColor) {
        const mask = loadAlphaMaskVisualization(img);
        if (mask) {
          ctx.drawImage(mask, 0, 0);
        } else {
          ctx.drawImage(img, 0, 0);
        }
      } else {
        const source = alphaCanvasRef.current || img;
        ctx.drawImage(source, 0, 0);
      }

      for (const t of sheet.detectedTextures) {
        ctx.fillStyle = TEXTURE_FILL;
        ctx.fillRect(t.x, t.y, t.w, t.h);
        ctx.strokeStyle = TEXTURE_OUTLINE;
        ctx.lineWidth = Math.max(1, 1 / effectiveScale);
        ctx.strokeRect(t.x, t.y, t.w, t.h);
      }

      if (activeGroupIndex >= 0) {
        drawGroupOverlay(
          ctx,
          1,
          sheet.detectedTextures,
          sheet.groups,
          activeGroupIndex,
        );
      }

      if (sheet.cropBox) {
        const { x, y, w, h } = sheet.cropBox;
        ctx.strokeStyle = "rgba(0, 255, 255, 0.8)";
        ctx.lineWidth = Math.max(1, 2 / effectiveScale);
        ctx.setLineDash([6 / effectiveScale, 4 / effectiveScale]);
        ctx.strokeRect(x, y, w, h);
        ctx.setLineDash([]);
      }

      const db = dragBoxRef.current;
      if (db) {
        ctx.strokeStyle = "rgba(0, 255, 255, 0.5)";
        ctx.lineWidth = Math.max(1, 2 / effectiveScale);
        ctx.setLineDash([4 / effectiveScale, 4 / effectiveScale]);
        ctx.strokeRect(db.x, db.y, db.w, db.h);
        ctx.setLineDash([]);
      }

      ctx.restore();
    };

    drawFnRef.current = draw;

    const ro = new ResizeObserver(draw);
    ro.observe(container);
    draw();

    return () => ro.disconnect();
  }, [sheet, loadAlphaMaskVisualization, loaded, zoom, pan, activeGroupIndex]);

  const screenToImage = useCallback(
    (clientX: number, clientY: number): { x: number; y: number } | null => {
      const container = containerRef.current;
      const img = imageRef.current;
      if (!container || !img) return null;
      const rect = container.getBoundingClientRect();
      const baseScale = Math.min(
        container.clientWidth / img.naturalWidth,
        container.clientHeight / img.naturalHeight,
      );
      const scale = baseScale * zoom;
      const offsetX = (container.clientWidth - img.naturalWidth * scale) / 2 + pan.x;
      const offsetY = (container.clientHeight - img.naturalHeight * scale) / 2 + pan.y;
      const px = (clientX - rect.left - offsetX) / scale;
      const py = (clientY - rect.top - offsetY) / scale;
      if (px < 0 || py < 0 || px >= img.naturalWidth || py >= img.naturalHeight) return null;
      return { x: px, y: py };
    },
    [zoom, pan],
  );

  const pixelToTexture = useCallback(
    (clientX: number, clientY: number): number | null => {
      const pt = screenToImage(clientX, clientY);
      if (!pt) return null;
      for (const t of sheet.detectedTextures) {
        if (pt.x >= t.x && pt.x < t.x + t.w && pt.y >= t.y && pt.y < t.y + t.h) {
          return t.id;
        }
      }
      return null;
    },
    [screenToImage, sheet.detectedTextures],
  );

  const handleMouseDown = useCallback(
    (e: React.MouseEvent<HTMLCanvasElement>) => {
      if (e.button === 1 || e.ctrlKey) {
        e.preventDefault();
        panStartRef.current = { startX: e.clientX, startY: e.clientY, panX: pan.x, panY: pan.y };
        return;
      }
      if (sheetPickMode === "box") {
        const pt = screenToImage(e.clientX, e.clientY);
        if (pt) {
          dragStartRef.current = pt;
          dragCurrentRef.current = pt;
        }
        return;
      }
      if (sheetPickMode === "bg") {
        const color = pickColor(e.clientX, e.clientY);
        if (color) {
          updateSpriteSheet(sheetIndex, { bgColor: color });
        }
        return;
      }
      if (sheetPickMode === "ignore") {
        const color = pickColor(e.clientX, e.clientY);
        if (color && !sheet.ignoreColors.includes(color) && color !== sheet.bgColor) {
          updateSpriteSheet(sheetIndex, { ignoreColors: [...sheet.ignoreColors, color] });
        }
        return;
      }
      if (sheetPickMode === "alpha") {
        const color = pickColor(e.clientX, e.clientY);
        if (color) {
          updateSpriteSheet(sheetIndex, { alphaColor: color });
        }
        return;
      }
      if (sheetPickMode === "select" && activeGroupIndex >= 0) {
        const id = pixelToTexture(e.clientX, e.clientY);
        if (id !== null) {
          toggleFrameInGroup(activeGroupIndex, id);
        }
      }
    },
    [sheetPickMode, pickColor, updateSpriteSheet, sheetIndex, pan, pixelToTexture, activeGroupIndex, toggleFrameInGroup, screenToImage],
  );

  const handleMouseMove = useCallback(
    (e: React.MouseEvent<HTMLCanvasElement>) => {
      const ps = panStartRef.current;
      if (ps) {
        setPan({ x: ps.panX + e.clientX - ps.startX, y: ps.panY + e.clientY - ps.startY });
        return;
      }
      if (dragStartRef.current) {
        const pt = screenToImage(e.clientX, e.clientY);
        if (pt) {
          dragCurrentRef.current = pt;
          const sx = dragStartRef.current.x;
          const sy = dragStartRef.current.y;
          const ex = pt.x;
          const ey = pt.y;
          dragBoxRef.current = {
            x: Math.min(sx, ex),
            y: Math.min(sy, ey),
            w: Math.abs(ex - sx),
            h: Math.abs(ey - sy),
          };
          drawFnRef.current();
        }
      }
    },
    [screenToImage],
  );

  const handleMouseUp = useCallback(
    () => {
      if (dragStartRef.current) {
        const db = dragBoxRef.current;
        if (db && (db.w < 3 || db.h < 3)) {
          updateSpriteSheet(sheetIndex, { cropBox: null });
        } else if (db) {
          updateSpriteSheet(sheetIndex, { cropBox: db });
        }
        dragStartRef.current = null;
        dragCurrentRef.current = null;
        dragBoxRef.current = null;
        return;
      }
      panStartRef.current = null;
    },
    [updateSpriteSheet, sheetIndex],
  );

  const handleKeyDown = useCallback(
    (e: React.KeyboardEvent) => {
      if (e.key === "Escape" && sheet.cropBox) {
        updateSpriteSheet(sheetIndex, { cropBox: null });
      }
    },
    [sheet.cropBox, updateSpriteSheet, sheetIndex],
  );

  return (
    <div
      ref={containerRef}
      className="flex-1 overflow-hidden bg-surface-canvas"
      tabIndex={0}
      onKeyDown={handleKeyDown}
    >
      <canvas
        ref={canvasRef}
        className="w-full h-full"
        onMouseDown={handleMouseDown}
        onMouseMove={handleMouseMove}
        onMouseUp={handleMouseUp}
        onWheel={useCallback((e: React.WheelEvent<HTMLCanvasElement>) => {
          e.preventDefault();
          const container = containerRef.current;
          const img = imageRef.current;
          if (!container || !img) return;
          const rect = container.getBoundingClientRect();
          const baseScale = Math.min(
            container.clientWidth / img.naturalWidth,
            container.clientHeight / img.naturalHeight,
          );
          const oldScale = baseScale * zoom;
          const factor = e.deltaY < 0 ? 1.1 : 1 / 1.1;
          const newZoom = Math.min(10, Math.max(0.1, zoom * factor));
          const newScale = baseScale * newZoom;
          const cx = e.clientX - rect.left;
          const cy = e.clientY - rect.top;
          const cw = container.clientWidth;
          const ch = container.clientHeight;
          const oldOffsetX = (cw - img.naturalWidth * oldScale) / 2 + pan.x;
          const oldOffsetY = (ch - img.naturalHeight * oldScale) / 2 + pan.y;
          const imgX = (cx - oldOffsetX) / oldScale;
          const imgY = (cy - oldOffsetY) / oldScale;
          const newOffsetX = (cw - img.naturalWidth * newScale) / 2;
          const newOffsetY = (ch - img.naturalHeight * newScale) / 2;
          setZoom(newZoom);
          setPan({ x: cx - newOffsetX - imgX * newScale, y: cy - newOffsetY - imgY * newScale });
        }, [zoom, pan])}
        onDoubleClick={useCallback(() => {
          setZoom(1);
          setPan({ x: 0, y: 0 });
        }, [])}
      />
    </div>
  );
}
