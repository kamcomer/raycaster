#ifndef RENDERER_INT_H
#define RENDERER_INT_H

#include "internal/scene_int.h"
#include "raycaster/renderer.h"

typedef struct AeRenderBackend AeRenderBackend;
typedef struct AeRenderBackendVtbl AeRenderBackendVtbl;

typedef struct AeRenderTechnique AeRenderTechnique;
typedef struct AeRenderTechniqueVtbl AeRenderTechniqueVtbl;

struct AeRenderBackend {
  const AeRenderBackendVtbl *vtbl;
  void *impl;
};

struct AeRenderTechnique {
  const AeRenderTechniqueVtbl *vtbl;
  void *impl;
};

struct AeRenderer {
  AeRenderBackend backend;
  AeRenderTechnique technique;
};

struct AeRenderBackendVtbl {
  void (*render)(AeRenderBackend *backend);
  void (*destroy)(AeRenderBackend *backend);
};

struct AeRenderTechniqueVtbl {
  void (*process)(AeRenderTechnique *technique);
  void (*destroy)(AeRenderTechnique *technique);
};

void ae_renderer_backend_render(AeRenderBackend *backend);
void ae_renderer_technique_process(AeRenderTechnique *technique);
void ae_renderer_destroy(AeRenderer *renderer);

void ae_renderer_technique_raycaster(AeRaycasterScene *scene, AeRenderer *renderer);

#endif // RENDERER_INT_H
