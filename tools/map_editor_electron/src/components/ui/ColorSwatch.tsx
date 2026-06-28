interface Props {
  color: string | null;
  label: string;
  onClear?: () => void;
}

export default function ColorSwatch({ color, label, onClear }: Props) {
  return (
    <div className="flex items-center gap-2 text-xs">
      <span className="text-gray-400">{label}:</span>
      {color ? (
        <>
          <span
            className="inline-block w-4 h-4 rounded border border-muted shrink-0"
            style={{ backgroundColor: color }}
          />
          <span className="text-gray-500 font-mono">{color}</span>
          {onClear && (
            <button
              onClick={onClear}
              className="text-gray-600 hover:text-gray-400 ml-auto"
            >
              ✕
            </button>
          )}
        </>
      ) : (
        <span className="text-gray-600 italic">none</span>
      )}
    </div>
  );
}
