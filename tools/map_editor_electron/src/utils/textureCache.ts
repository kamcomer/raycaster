const imageCache = new Map<string, HTMLImageElement>()

export function getCachedTexture(dataUrl: string): HTMLImageElement {
  let img = imageCache.get(dataUrl)
  if (!img) {
    img = new Image()
    img.src = dataUrl
    imageCache.set(dataUrl, img)
  }
  return img
}

export function preloadTexture(dataUrl: string): Promise<HTMLImageElement> {
  const existing = imageCache.get(dataUrl)
  if (existing && existing.complete) return Promise.resolve(existing)

  return new Promise((resolve, reject) => {
    const img = new Image()
    img.onload = () => {
      imageCache.set(dataUrl, img)
      resolve(img)
    }
    img.onerror = reject
    img.src = dataUrl
  })
}
