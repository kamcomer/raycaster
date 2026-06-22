interface ElectronAPI {
  openDialog(
    filters?: { name: string; extensions: string[] }[]
  ): Promise<string | null>
  saveDialog(defaultName?: string): Promise<string | null>
  readFile(path: string): Promise<string>
  writeFile(path: string, content: string): Promise<void>
  readTexture(
    path: string
  ): Promise<{ data: string; path: string; ext: string }>
  openTextureDialog(): Promise<string | null>
  selectDirectory(): Promise<string | null>
  listTexturesInDirectory(
    dirPath: string
  ): Promise<{ name: string; path: string }[]>
  relativeToRoot(mapPath: string, texPath: string): Promise<string>
  resolveFromRoot(mapPath: string, relPath: string): Promise<string>
}

interface Window {
  api: ElectronAPI
}
