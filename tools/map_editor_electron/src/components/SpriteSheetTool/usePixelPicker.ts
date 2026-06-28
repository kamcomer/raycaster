import { useCallback, useRef } from "react";

export function usePixelPicker(canvasRef: React.RefObject<HTMLCanvasElement | null>) {
  const processedRef = useRef<HTMLCanvasElement | null>(null);

  const setProcessedSource = useCallback((canvas: HTMLCanvasElement | null) => {
    processedRef.current = canvas;
  }, []);

  const pickColor = useCallback(
    (clientX: number, clientY: number): string | null => {
      const canvas = processedRef.current || canvasRef.current;
      if (!canvas) return null;
      const rect = canvas.getBoundingClientRect();
      const x = Math.floor(clientX - rect.left);
      const y = Math.floor(clientY - rect.top);
      if (x < 0 || x >= canvas.width || y < 0 || y >= canvas.height) return null;
      const ctx = canvas.getContext("2d");
      if (!ctx) return null;
      const pixel = ctx.getImageData(x, y, 1, 1).data;
      const hex = `#${[pixel[0], pixel[1], pixel[2]]
        .map((c) => c.toString(16).padStart(2, "0"))
        .join("")}`;
      return hex;
    },
    [canvasRef],
  );

  return { pickColor, setProcessedSource };
}
