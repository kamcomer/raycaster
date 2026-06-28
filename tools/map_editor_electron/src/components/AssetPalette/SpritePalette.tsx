import { useMapStore } from "../../store/mapStore";
import PanelHeader, { ImportButtonPair } from "../ui/PanelHeader";
import { useImportAsset } from "../../hooks/useImportAsset";

export default function SpritePalette() {
  const spriteTypes = useMapStore((s) => s.spriteTypes);
  const selectedSpriteType = useMapStore((s) => s.selectedSpriteType);
  const setSelectedSpriteType = useMapStore((s) => s.setSelectedSpriteType);
  const importSpriteType = useMapStore((s) => s.importSpriteType);

  const { handleAdd: handleAddSpriteType, handleImportDir: handleImportSpriteTypeDir } =
    useImportAsset(importSpriteType, "sprite type");

  return (
    <>
      <PanelHeader
        title="Sprites"
        actions={<ImportButtonPair onAdd={handleAddSpriteType} onImportDir={handleImportSpriteTypeDir} />}
      />
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
