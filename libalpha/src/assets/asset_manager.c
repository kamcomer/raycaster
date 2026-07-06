#include "internal/i_alpha.h"

AResult a_asset_manager_create(AAssetManager **out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  AAssetManager *asset_manager = malloc(sizeof(AAssetManager));
  if (!asset_manager) {
    return A_RES_ALLOC_ERR;
  }

  AResult res = a_asset_manager_init(asset_manager);
  if (res != A_RES_OK) {
    free(asset_manager);
    return A_RES_ALLOC_ERR;
  }

  *out = asset_manager;

  return A_RES_OK;
}

AResult a_asset_manager_init(AAssetManager *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = (AAssetManager){0};

  AResult res = a_texture_manager_init(100, &out->texture_manager);
  if (res != A_RES_OK) {
    *out = (AAssetManager){0};
    return res;
  }
  return A_RES_OK;
}

void a_asset_manager_destroy(AAssetManager *asset_manager)
{
  if (!asset_manager) {
    return;
  }
  a_asset_manager_deinit(asset_manager);
  free(asset_manager);
}
void a_asset_manager_deinit(AAssetManager *asset_manager)
{
  if (!asset_manager) {
    return;
  }

  a_texture_manager_deinit(&asset_manager->texture_manager);

  *asset_manager = (AAssetManager){0};
}
