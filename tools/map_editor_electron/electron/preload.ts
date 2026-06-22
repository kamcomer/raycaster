import { contextBridge, ipcRenderer } from 'electron'

contextBridge.exposeInMainWorld('api', {
  openDialog: (filters?: { name: string; extensions: string[] }[]) =>
    ipcRenderer.invoke('dialog:open', filters),
  saveDialog: (defaultName?: string) =>
    ipcRenderer.invoke('dialog:save', defaultName),
  readFile: (path: string) => ipcRenderer.invoke('file:read', path),
  writeFile: (path: string, content: string) =>
    ipcRenderer.invoke('file:write', path, content),
  readTexture: (
    path: string
  ): Promise<{ data: string; path: string; ext: string }> =>
    ipcRenderer.invoke('texture:read', path),
  openTextureDialog: () => ipcRenderer.invoke('dialog:openTexture'),
  selectDirectory: (): Promise<string | null> =>
    ipcRenderer.invoke('dialog:selectDirectory'),
  listTexturesInDirectory: (
    dirPath: string
  ): Promise<{ name: string; path: string }[]> =>
    ipcRenderer.invoke('texture:listDirectory', dirPath),
  relativeToRoot: (
    mapPath: string,
    texPath: string
  ): Promise<string> => ipcRenderer.invoke('texture:relativeToRoot', mapPath, texPath),
  resolveFromRoot: (
    mapPath: string,
    relPath: string
  ): Promise<string> => ipcRenderer.invoke('texture:resolveFromRoot', mapPath, relPath),
})
