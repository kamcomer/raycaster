import { useCallback } from "react";
import { useMapStore } from "../store/mapStore";
import { preloadTexture } from "../utils/textureCache";

const api = window.api;

async function resolveRelativePath(absolutePath: string): Promise<string> {
  const mapFile = useMapStore.getState().filePath;
  if (mapFile) {
    return await api.relativeToRoot(mapFile, absolutePath);
  }
  return absolutePath.split("/").pop() || absolutePath;
}

export function useImportAsset(
  importFn: (path: string, dataUrl: string) => void,
  label?: string,
) {
  const handleAdd = useCallback(async () => {
    try {
      const absolutePath = await api.openTextureDialog();
      if (!absolutePath) return;
      const tex = await api.readTexture(absolutePath);
      try {
        await preloadTexture(tex.data);
      } catch {
        console.warn("texture preload failed, using fallback:", absolutePath);
      }
      const relPath = await resolveRelativePath(tex.path);
      importFn(relPath, tex.data);
    } catch (err) {
      console.error(`failed to import ${label ?? "asset"}:`, err);
    }
  }, [importFn, label]);

  const handleImportDir = useCallback(async () => {
    try {
      const dirPath = await api.selectDirectory();
      if (!dirPath) return;
      const files = await api.listTexturesInDirectory(dirPath);
      if (files.length === 0) {
        console.warn("no image files found in:", dirPath);
        return;
      }
      for (const file of files) {
        try {
          const tex = await api.readTexture(file.path);
          await preloadTexture(tex.data);
          const relPath = await resolveRelativePath(tex.path);
          importFn(relPath, tex.data);
        } catch (err) {
          console.warn("skipping file:", file.name, err);
        }
      }
    } catch (err) {
      console.error(`failed to import ${label ?? "asset"} directory:`, err);
    }
  }, [importFn, label]);

  return { handleAdd, handleImportDir };
}
