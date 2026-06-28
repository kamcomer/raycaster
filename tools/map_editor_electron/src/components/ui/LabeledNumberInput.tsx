interface Props {
  label: string;
  value: number;
  onChange: (value: number) => void;
  min?: number;
  max?: number;
  step?: number;
  size?: "md" | "sm";
}

export default function LabeledNumberInput({
  label,
  value,
  onChange,
  min,
  max,
  step,
  size = "md",
}: Props) {
  const inputClass =
    size === "sm"
      ? "w-16 bg-muted rounded px-1.5 py-0.5 text-xs [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none"
      : "w-20 bg-surface-dark border border-muted rounded px-2 py-1 text-sm";

  return (
    <label className="flex items-center gap-1.5">
      <span className="text-sm text-gray-400">{label}</span>
      <input
        type="number"
        min={min}
        max={max}
        step={step}
        value={value}
        onChange={(e) => onChange(Number(e.target.value))}
        className={inputClass}
      />
    </label>
  );
}
