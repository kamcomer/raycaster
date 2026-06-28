import { useMapStore } from "../../store/mapStore";
import { TEXTURE_COLORS } from "../../map/constants";
import { preloadTexture } from "../../utils/textureCache";

export default function TexturePalette() {
  const textures = useMapStore((s) => s.textures);
  const selectedTexture = useMapStore((s) => s.selectedTexture);
  const setSelectedTexture = useMapStore((s) => s.setSelectedTexture);
  const emptySlotCount = Math.max(1, 11 - textures.length);
  const importTexture = useMapStore((s) => s.importTexture);

  const api = window.api;

  const resolveTexturePath = async (absolutePath: string) => {
    const mapFile = useMapStore.getState().filePath;
    if (mapFile) {
      return await api.relativeToRoot(mapFile, absolutePath);
    }
    return absolutePath.split("/").pop() || absolutePath;
  };

  const handleAddTexture = async () => {
    try {
      const absolutePath = await api.openTextureDialog();
      if (!absolutePath) return;
      const tex = await api.readTexture(absolutePath);
      try {
        await preloadTexture(tex.data);
      } catch {
        console.warn("texture preload failed, using fallback:", absolutePath);
      }
      const relPath = await resolveTexturePath(tex.path);
      importTexture(relPath, tex.data);
    } catch (err) {
      console.error("failed to import texture:", err);
    }
  };

  const handleImportTextureDir = async () => {
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
          const relPath = await resolveTexturePath(tex.path);
          importTexture(relPath, tex.data);
        } catch (err) {
          console.warn("skipping file:", file.name, err);
        }
      }
    } catch (err) {
      console.error("failed to import texture directory:", err);
    }
  };

  return (
    <>
      <div className="px-3 py-2 text-xs font-semibold text-gray-400 uppercase tracking-wider border-b border-muted flex items-center justify-between">
        Textures
        <div className="flex flex-row">
          <button
            onClick={handleAddTexture}
            className="no-drag px-3 py-1.5 hover:bg-muted-hover border border-muted rounded-l text-sm ml-2"
          >
            +
          </button>

          <button
            onClick={handleImportTextureDir}
            className="no-drag px-3 py-1.5 hover:bg-muted-hover border border-muted rounded-r text-sm"
          >
            <i className="bi bi-folder" />
          </button>
        </div>
      </div>
      <div className="flex-1 overflow-y-auto scrollbar-thin p-2">
        <div className="grid grid-cols-2 gap-1.5">
          {textures.map((tex) => {
            const isSelected = tex.id === selectedTexture;
            return (
              <button
                key={tex.id}
                onClick={() => setSelectedTexture(tex.id)}
                className={`relative aspect-square rounded overflow-hidden border-2 transition-colors ${
                  isSelected
                    ? "border-accent"
                    : "border-transparent hover:border-gray-600"
                }`}
                title={tex.path.split("/").pop()}
              >
                <img
                  src={tex.dataUrl}
                  alt={tex.path}
                  className="w-full h-full object-cover"
                  onError={(e) => {
                    const target = e.currentTarget;
                    target.style.display = "none";
                    const parent = target.parentElement;
                    if (parent) {
                      const idx = tex.id - 1;
                      parent.style.backgroundColor =
                        TEXTURE_COLORS[idx % TEXTURE_COLORS.length];
                    }
                  }}
                />
                <span className="absolute bottom-0.5 right-1 text-[10px] bg-black/60 px-1 rounded">
                  {tex.id}
                </span>
              </button>
            );
          })}
          {Array.from({ length: emptySlotCount }).map((_, i) => {
            const slotId = textures.length + i + 1;
            const isSelected = slotId === selectedTexture;
            return (
              <button
                key={`empty-${slotId}`}
                onClick={() => setSelectedTexture(slotId)}
                className={`aspect-square rounded border-2 flex items-center justify-center text-xs text-gray-600 transition-colors ${
                  isSelected
                    ? "border-accent bg-surface"
                    : "border-dashed border-gray-700 hover:border-gray-500"
                }`}
              >
                {slotId}
              </button>
            );
          })}
          <button
            onClick={() => setSelectedTexture(0)}
            className={`aspect-square rounded border-2 flex items-center justify-center text-xs transition-colors ${
              selectedTexture === 0
                ? "border-accent bg-[#3a1a1a] text-accent"
                : "border-dashed border-gray-700 hover:border-red-700 text-red-800"
            }`}
            title="Eraser"
          >
            ✕
          </button>
        </div>
      </div>
    </>
  );
}
