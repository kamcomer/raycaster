import { useMapStore } from "../../store/mapStore";

interface Binding {
  key: string;
  desc: string;
}

const sections: { heading: string; bindings: Binding[] }[] = [
  {
    heading: "File",
    bindings: [
      { key: "\u2318/Ctrl + N", desc: "New map" },
      { key: "\u2318/Ctrl + S", desc: "Save" },
      { key: "\u2318/Ctrl + O", desc: "Open" },
    ],
  },
  {
    heading: "Tools",
    bindings: [
      { key: "1 \u2013 9", desc: "Select texture" },
      { key: "0", desc: "Eraser" },
      { key: "P", desc: "Paint tool" },
      { key: "E", desc: "Eraser tool" },
      { key: "S", desc: "Sprite tool" },
    ],
  },
  {
    heading: "Layers",
    bindings: [
      { key: "W", desc: "Wall layer" },
      { key: "F", desc: "Floor layer" },
      { key: "C", desc: "Ceiling layer" },
    ],
  },
  {
    heading: "Sprite Editing",
    bindings: [
      { key: "Arrow keys", desc: "Move selected sprite" },
      { key: "Delete", desc: "Remove selected sprite" },
    ],
  },
  {
    heading: "Sprite Sheet",
    bindings: [
      { key: "Esc", desc: "Clear crop box" },
    ],
  },
  {
    heading: "General",
    bindings: [
      { key: "?", desc: "Toggle this modal" },
      { key: "Esc", desc: "Close" },
    ],
  },
];

export default function KeybindModal() {
  const show = useMapStore((s) => s.showKeybindModal);
  const setShow = useMapStore((s) => s.setShowKeybindModal);

  if (!show) return null;

  return (
    <div
      className="fixed inset-0 bg-black/50 flex items-center justify-center z-50"
      onClick={() => setShow(false)}
    >
      <div
        className="bg-surface border border-muted rounded-lg p-6 min-w-[320px] max-h-[80vh] overflow-y-auto"
        onClick={(e) => e.stopPropagation()}
      >
        <h2 className="text-lg mb-4">Keybindings</h2>
        {sections.map((section) => (
          <div key={section.heading} className="mb-3 last:mb-0">
            <h3 className="text-xs font-semibold text-gray-400 uppercase tracking-wider mb-1.5">
              {section.heading}
            </h3>
            <div className="space-y-1">
              {section.bindings.map((b) => (
                <div key={b.key} className="flex items-center gap-3 text-sm">
                  <kbd className="min-w-[120px] bg-muted border border-muted rounded px-2 py-0.5 font-mono text-xs text-gray-300 text-center">
                    {b.key}
                  </kbd>
                  <span className="text-gray-400">{b.desc}</span>
                </div>
              ))}
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
