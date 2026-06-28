export function applyAlphaMask(
  imageData: ImageData,
  hexColor: string,
): ImageData {
  const { data } = imageData;
  const r = parseInt(hexColor.slice(1, 3), 16);
  const g = parseInt(hexColor.slice(3, 5), 16);
  const b = parseInt(hexColor.slice(5, 7), 16);
  for (let i = 0; i < data.length; i += 4) {
    if (data[i] === r && data[i + 1] === g && data[i + 2] === b) {
      data[i + 3] = 0;
    }
  }
  return imageData;
}
