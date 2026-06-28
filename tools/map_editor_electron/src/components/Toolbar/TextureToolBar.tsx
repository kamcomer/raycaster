import { useState } from "react";
import { useMapStore } from "../../store/mapStore";
import ButtonGroup from "../ui/ButtonGroup";

type PaintTool = "paint" | "erase";

export default function TextureToolBar() {
  const selectedTexture = useMapStore((s) => s.selectedTexture);
  const setSelectedTexture = useMapStore((s) => s.setSelectedTexture);

  const [paintTool, setPaintTool] = useState<PaintTool>(
    selectedTexture === 0 ? "erase" : "paint",
  );

  const handleSelectPaintTool = (tool: PaintTool) => {
    setPaintTool(tool);
    setSelectedTexture(tool === "erase" ? 0 : 1);
  };

  return (
    <ButtonGroup
      items={["paint", "erase"]}
      selected={paintTool}
      onSelect={(v) => handleSelectPaintTool(v as PaintTool)}
      variant="rounded"
    />
  );
}
