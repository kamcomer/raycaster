import { useMapStore } from "../../store/mapStore";
import { TEXTURE_COLORS } from "../../map/constants";
import PanelHeader, { ImportButtonPair } from "../ui/PanelHeader";
import { useImportAsset } from "../../hooks/useImportAsset";

export default function TexturePalette() {
  const textures = useMapStore((s) => s.textures);
  const selectedTexture = useMapStore((s) => s.selectedTexture);
  const setSelectedTexture = useMapStore((s) => s.setSelectedTexture);
  const emptySlotCount = Math.max(1, 11 - textures.length);
  const importTexture = useMapStore((s) => s.importTexture);

  const { handleAdd: handleAddTexture, handleImportDir: handleImportTextureDir } =
    useImportAsset(importTexture, "texture");

  return (
    <>
      <PanelHeader
        title="Textures"
        actions={<ImportButtonPair onAdd={handleAddTexture} onImportDir={handleImportTextureDir} />}
      />
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
