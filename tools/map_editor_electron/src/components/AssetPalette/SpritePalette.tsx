import { useMapStore } from "../../store/mapStore";
import { preloadTexture } from "../../utils/textureCache";

export default function SpritePalette() {
  const spriteTypes = useMapStore((s) => s.spriteTypes);
  const selectedSpriteType = useMapStore((s) => s.selectedSpriteType);
  const setSelectedSpriteType = useMapStore((s) => s.setSelectedSpriteType);
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

  const handleImportSpriteTypeDir = async () => {
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
      <div className="px-3 py-2 text-xs font-semibold text-gray-400 uppercase tracking-wider border-b border-muted flex items-center justify-between">
        Sprites
        <div className="flex flex-row">
          <button
            onClick={handleAddSpriteType}
            className="no-drag px-3 py-1.5 hover:bg-muted-hover border border-muted rounded-l text-sm ml-2"
          >
            +
          </button>

          <button
            onClick={handleImportSpriteTypeDir}
            className="no-drag px-3 py-1.5 hover:bg-muted-hover border border-muted rounded-r text-sm"
          >
            <i className="bi bi-folder" />
          </button>
        </div>
      </div>
      <div className="flex-1 overflow-y-auto scrollbar-thin p-2">
        {spriteTypes.length === 0 ? (
          <div className="text-xs text-gray-600 text-center mt-8">
            No sprite types loaded
          </div>
        ) : (
          <div className="grid grid-cols-2 gap-1.5">
            {spriteTypes.map((st, i) => (
              <button
                key={i}
                onClick={() => setSelectedSpriteType(i)}
                className={`relative aspect-square rounded overflow-hidden border-2 transition-colors ${
                  selectedSpriteType === i
                    ? "border-accent"
                    : "border-transparent hover:border-gray-600"
                }`}
                title={st.path.split("/").pop() || ""}
              >
                <img
                  src={st.dataUrl}
                  alt={st.path}
                  className="w-full h-full object-cover"
                />
                {st.frameCount > 1 && (
                  <span className="absolute top-0.5 right-0.5 text-[9px] bg-black/70 text-yellow-400 px-1 rounded leading-tight">
                    {st.frameCount}f
                  </span>
                )}
                <span className="absolute bottom-0.5 left-1 right-1 text-[10px] bg-black/60 px-1 rounded text-center truncate">
                  {i}
                </span>
              </button>
            ))}
          </div>
        )}
      </div>
    </>
  );
}
