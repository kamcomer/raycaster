import { parseMap, serializeMap } from "../map/parser";
import { useMapStore } from "../store/mapStore";

const api = window.api;

export async function openMap() {
  const path = await api.openDialog();
  if (!path) return;

  const text = await api.readFile(path);
  const data = parseMap(text);

  const store = useMapStore.getState();
  store.loadMap(data);
  store.setFilePath(path);

  const textureEntries: { path: string; dataUrl: string }[] = [];
  for (const texPath of data.textures) {
    try {
      const absPath = await api.resolveFromRoot(path, texPath);
      const tex = await api.readTexture(absPath);
      textureEntries.push({ path: texPath, dataUrl: tex.data });
    } catch (err) {
      console.warn("could not load texture:", texPath, err);
    }
  }
  if (textureEntries.length > 0) {
    useMapStore.getState().populateTextures(textureEntries);
  }

  const spriteTypeEntries: {
    path: string;
    frameCount: number;
    frameDelay: number;
    dataUrl: string;
  }[] = [];
  for (const st of data.spriteTypes) {
    try {
      const absPath = await api.resolveFromRoot(path, st.path);
      const tex = await api.readTexture(absPath);
      spriteTypeEntries.push({
        path: st.path,
        frameCount: st.frameCount,
        frameDelay: st.frameDelay,
        dataUrl: tex.data,
      });
    } catch (err) {
      console.warn("could not load sprite type:", st.path, err);
    }
  }
  if (spriteTypeEntries.length > 0) {
    useMapStore.getState().populateSpriteTypes(spriteTypeEntries);
  }
}

export async function saveMap() {
  const store = useMapStore.getState();
  const content = serializeMap(store.map);
  const path = store.filePath || (await api.saveDialog("map.txt"));
  if (!path) return;
  await api.writeFile(path, content);
  store.setFilePath(path);
}

export async function saveMapAs() {
  const store = useMapStore.getState();
  const content = serializeMap(store.map);
  const path = await api.saveDialog("map.txt");
  if (!path) return;
  await api.writeFile(path, content);
  store.setFilePath(path);
}
