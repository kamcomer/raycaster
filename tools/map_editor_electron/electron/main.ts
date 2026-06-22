import { app, BrowserWindow, ipcMain, dialog } from 'electron'
import { readFileSync, writeFileSync, readdirSync, existsSync } from 'fs'
import { join, dirname, relative, resolve } from 'path'
import { fileURLToPath } from 'url'

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

let mainWindow: BrowserWindow | null = null

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1280,
    height: 800,
    minWidth: 900,
    minHeight: 600,
    titleBarStyle: 'hiddenInset',
    backgroundColor: '#1a1a2e',
    webPreferences: {
      preload: join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  })

  if (process.env.VITE_DEV_SERVER_URL) {
    mainWindow.loadURL(process.env.VITE_DEV_SERVER_URL)
  } else {
    mainWindow.loadFile(join(__dirname, '../dist/index.html'))
  }
}

app.whenReady().then(createWindow)

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
})

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) createWindow()
})

ipcMain.handle('dialog:open', async (_event, filters) => {
  const result = await dialog.showOpenDialog(mainWindow!, {
    properties: ['openFile'],
    filters: filters || [{ name: 'Map Files', extensions: ['txt'] }],
  })
  return result.canceled ? null : result.filePaths[0]
})

ipcMain.handle('dialog:save', async (_event, defaultName) => {
  const result = await dialog.showSaveDialog(mainWindow!, {
    defaultPath: defaultName || 'map.txt',
    filters: [{ name: 'Map Files', extensions: ['txt'] }],
  })
  return result.canceled ? null : result.filePath
})

ipcMain.handle('file:read', (_event, path: string) => {
  return readFileSync(path, 'utf-8')
})

ipcMain.handle('file:write', (_event, path: string, content: string) => {
  writeFileSync(path, content, 'utf-8')
})

ipcMain.handle('texture:read', (_event, path: string) => {
  const buffer = readFileSync(path)
  const ext = path.split('.').pop()?.toLowerCase() || 'png'
  const mime = ext === 'jpg' ? 'jpeg' : ext
  return {
    data: `data:image/${mime};base64,${buffer.toString('base64')}`,
    path,
    ext,
  }
})

ipcMain.handle('dialog:openTexture', async () => {
  const result = await dialog.showOpenDialog(mainWindow!, {
    properties: ['openFile'],
    filters: [{ name: 'Textures', extensions: ['png', 'jpg', 'jpeg'] }],
  })
  return result.canceled ? null : result.filePaths[0]
})

ipcMain.handle('dialog:selectDirectory', async () => {
  const result = await dialog.showOpenDialog(mainWindow!, {
    properties: ['openDirectory'],
  })
  return result.canceled ? null : result.filePaths[0]
})

function findProjectRoot(mapPath: string): string {
  let current = dirname(mapPath)
  while (true) {
    if (existsSync(join(current, 'assets', 'textures'))) {
      return current
    }
    const parent = dirname(current)
    if (parent === current) break
    current = parent
  }
  return dirname(mapPath)
}

ipcMain.handle('texture:relativeToRoot', (_event, mapPath: string, texPath: string) => {
  return relative(findProjectRoot(mapPath), texPath)
})

ipcMain.handle('texture:resolveFromRoot', (_event, mapPath: string, relPath: string) => {
  return resolve(findProjectRoot(mapPath), relPath)
})

ipcMain.handle('texture:listDirectory', (_event, dirPath: string) => {
  const files = readdirSync(dirPath)
  const imageExts = new Set(['.png', '.jpg', '.jpeg'])
  return files
    .filter((f) => {
      const ext = '.' + f.split('.').pop()?.toLowerCase()
      return imageExts.has(ext)
    })
    .map((f) => ({
      name: f,
      path: join(dirPath, f),
    }))
})
