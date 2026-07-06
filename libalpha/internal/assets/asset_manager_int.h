#ifndef ASSET_MANAGER_INT_H
#define ASSET_MANAGER_INT_H

#include "internal/assets/texture_int.h"
#include "raycaster/types.h"

typedef struct AeAssetManager AeAssetManager;

struct AeAssetManager {
  AeTextureManager texture_manager;
};

AeResult ae_asset_manager_create(AeAssetManager **out);
void ae_asset_manager_destroy(AeAssetManager *asset_manager);

AeResult ae_asset_manager_init(AeAssetManager *out);
void ae_asset_manager_deinit(AeAssetManager *asset_manager);

const AeTexture *ae_asset_manager_get_texture(const AeAssetManager *asset_manager,
                                              AeTextureHandle handle);

#endif // ASSET_MANAGER_INT_H
