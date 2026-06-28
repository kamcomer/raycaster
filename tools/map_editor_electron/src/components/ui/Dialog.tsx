import type { ReactNode } from "react";

interface DialogProps {
  open: boolean;
  onClose: () => void;
  title: string;
  description?: string;
  children: ReactNode;
}

export default function Dialog({
  open,
  onClose,
  title,
  description,
  children,
}: DialogProps) {
  if (!open) return null;

  return (
    <div className="fixed inset-0 bg-black/50 flex items-center justify-center z-50" onClick={onClose}>
      <div className="bg-surface border border-muted rounded-lg p-6" onClick={(e) => e.stopPropagation()}>
        <h2 className="text-lg mb-4">{title}</h2>
        {description && (
          <p className="text-xs text-gray-400 mb-4">{description}</p>
        )}
        {children}
      </div>
    </div>
  );
}

interface DialogActionsProps {
  cancelLabel?: string;
  onCancel: () => void;
  confirmLabel: string;
  onConfirm: () => void;
}

export function DialogActions({
  cancelLabel = "Cancel",
  onCancel,
  confirmLabel,
  onConfirm,
}: DialogActionsProps) {
  return (
    <div className="flex gap-2 justify-end">
      <button
        onClick={onCancel}
        className="px-4 py-1.5 bg-muted hover:bg-muted-hover rounded text-sm"
      >
        {cancelLabel}
      </button>
      <button
        onClick={onConfirm}
        className="px-4 py-1.5 bg-accent hover:bg-accent-hover rounded text-sm"
      >
        {confirmLabel}
      </button>
    </div>
  );
}
