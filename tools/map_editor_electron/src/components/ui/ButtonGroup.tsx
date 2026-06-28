type ButtonItem = string | { label: string; value?: string };

type ButtonGroupProps = {
  items: ButtonItem[];
  selected?: string;
  onSelect: (value: string) => void;
  variant?: "split" | "rounded";
  fill?: boolean;
  size?: "sm" | "xs";
  className?: string;
};

function normalize(item: ButtonItem): { label: string; value: string } {
  if (typeof item === "string") return { label: item, value: item };
  return { label: item.label, value: item.value ?? item.label };
}

export default function ButtonGroup({
  items,
  selected,
  onSelect,
  variant = "split",
  fill = false,
  size,
  className = "",
}: ButtonGroupProps) {
  const isSelectable = selected !== undefined;
  const sizeClass = size === "xs" ? "px-2 py-0.5 text-xs" : "px-3 py-1.5 text-sm";

  return (
    <div className={`inline-flex ${fill ? "h-full" : ""} ${className}`}>
      {items.map((raw, i) => {
        const item = normalize(raw);
        const isFirst = i === 0;
        const isLast = i === items.length - 1;
        const isActive = item.value === selected;

        const roundClass =
          variant === "split"
            ? `${isFirst ? "rounded-l" : ""} ${isLast ? "rounded-r" : ""}`
            : "rounded";

        const stateClass = isSelectable
          ? isActive
            ? "bg-accent text-white"
            : "bg-muted hover:bg-muted-hover"
          : "bg-muted hover:bg-muted-hover";

        return (
          <button
            key={item.value}
            onClick={() => onSelect(item.value)}
            className={`no-drag border border-muted ${stateClass} ${roundClass} ${sizeClass} ${fill ? "h-full" : ""}`}
          >
            <span className="capitalize">{item.label}</span>
          </button>
        );
      })}
    </div>
  );
}
