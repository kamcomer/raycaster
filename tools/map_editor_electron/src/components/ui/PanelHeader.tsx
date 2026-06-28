import type { ReactNode } from "react";

interface PanelHeaderProps {
  title: string;
  actions?: ReactNode;
  border?: boolean;
  compact?: boolean;
}

export default function PanelHeader({
  title,
  actions,
  border = true,
  compact = false,
}: PanelHeaderProps) {
  return (
    <div
      className={`px-3 text-xs font-semibold text-gray-400 uppercase tracking-wider flex items-center justify-between shrink-0 ${
        border ? "border-b border-muted" : ""
      } ${compact ? "py-1.5" : "py-2"}`}
    >
      {title}
      {actions && <div className="flex flex-row">{actions}</div>}
    </div>
  );
}

interface ImportButtonPairProps {
  onAdd: () => void;
  onImportDir: () => void;
}

export function ImportButtonPair({ onAdd, onImportDir }: ImportButtonPairProps) {
  return (
    <>
      <button
        onClick={onAdd}
        className="no-drag px-3 py-1.5 hover:bg-muted-hover border border-muted rounded-l text-sm ml-2"
      >
        +
      </button>
      <button
        onClick={onImportDir}
        className="no-drag px-3 py-1.5 hover:bg-muted-hover border border-muted rounded-r text-sm"
      >
        <i className="bi bi-folder" />
      </button>
    </>
  );
}
