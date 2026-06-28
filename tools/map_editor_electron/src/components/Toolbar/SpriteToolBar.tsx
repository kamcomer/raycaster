import { useMapStore } from "../../store/mapStore";
import { preloadTexture } from "../../utils/textureCache";

export default function SpritToolBar() {
  const importSpriteType = useMapStore((s) => s.importSpriteType);

  const api = window.api;

  const handleAddSpriteType = async () => {
    try {
      const absolutePath = await api.openTextureDialog();
      if (!absolutePath) return;
      const mapFile = useMapStore.getState().filePath;
      let relPath = absolutePath.split("/").pop() || absolutePath;
      if (mapFile) {
        relPath = await api.relativeToRoot(mapFile, absolutePath);
      }
      const tex = await api.readTexture(absolutePath);
      await preloadTexture(tex.data);
      importSpriteType(relPath, tex.data);
    } catch (err) {
      console.error("failed to import sprite type:", err);
    }
  };

  const handleAddSpriteTypeDir = async () => {
    try {
      const dirPath = await api.selectDirectory();
      if (!dirPath) return;
      const files = await api.listTexturesInDirectory(dirPath);
      if (files.length === 0) {
        console.warn("no image files found in:", dirPath);
        return;
      }
      const mapFile = useMapStore.getState().filePath;
      for (const file of files) {
        try {
          const tex = await api.readTexture(file.path);
          await preloadTexture(tex.data);
          let relPath = file.path.split("/").pop() || file.path;
          if (mapFile) relPath = await api.relativeToRoot(mapFile, file.path);
          importSpriteType(relPath, tex.data);
        } catch (err) {
          console.warn("skipping file:", file.name, err);
        }
      }
    } catch (err) {
      console.error("failed to import sprite type directory:", err);
    }
  };

  return (
    <>
      <button
        onClick={handleAddSpriteType}
        className="no-drag px-3 py-1.5 bg-muted hover:bg-muted-hover rounded text-sm ml-2"
      >
        + Sprite
      </button>
      <button
        onClick={handleAddSpriteTypeDir}
        className="no-drag px-3 py-1.5 bg-muted hover:bg-muted-hover rounded text-sm"
      >
        + Sprite Dir
      </button>
    </>
  );
}
