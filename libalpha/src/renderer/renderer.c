#include "internal/i_alpha.h"
#include "internal/platform/sdl.h"
#include <stdlib.h>

//------- Backend Vtbl Callbacks -------
void a_renderer_backend_destroy(ARendererBackend *backend)
{
  if (!backend) {
    return;
  }

  if (backend->vtbl) {
    backend->vtbl->destroy(backend);
  }

  free(backend);
}

void a_renderer_backend_deinit(ARendererBackend *backend)
{
  if (!backend) {
    return;
  }
  backend->vtbl->deinit(backend);
}

AResult a_renderer_backend_init(ARendererConfig config, ARendererBackend *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = (ARendererBackend){0};

  switch (config.backend) {
  case A_RENDERER_BACKEND_SDL: {
    AResult res = a_sdl_renderer_backend_init(config, out);
    if (res != A_RES_OK) {
      *out = (ARendererBackend){0};
      return res;
    }
    break;
  }
  }

  return A_RES_OK;
}

//------- Technique Vtbl Callbacks -------
void a_renderer_technique_destroy(ARendererTechnique *technique)
{
  if (!technique) {
    return;
  }

  if (technique->vtbl) {
    technique->vtbl->destroy(technique);
  }
  free(technique);
}

void a_renderer_technique_deinit(ARendererTechnique *technique)
{
  if (!technique) {
    return;
  }
  technique->vtbl->deinit(technique);
}

AResult a_renderer_technique_init(const ARendererBackend *backend, ARendererConfig config,
                                  ARendererTechnique *out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = (ARendererTechnique){0};

  switch (config.technique) {
  case A_RENDERER_TECHNIQUE_RAYCASTER: {
    AResult res = a_renderer_technique_raycaster_init(
        (ARendererTechniqueRaycasterData){NULL, a_renderer_backend_window_dimensions(backend),
                                          a_renderer_backend_framebuffer(backend),
                                          a_renderer_backend_zbuffer(backend)},
        out);
    if (res != A_RES_OK) {
      *out = (ARendererTechnique){0};
      return res;
    }

    break;
  }
  }

  return A_RES_OK;
}

// ------- Renderer Functions -------
AResult a_renderer_create(ARendererConfig config, ARenderer **out)
{
  if (!out) {
    return A_RES_INVLD_ARG;
  }

  *out = NULL;

  ARenderer *renderer = malloc(sizeof(ARenderer));
  if (!renderer) {
    return A_RES_ALLOC_ERR;
  }

  AResult res = a_renderer_backend_init(config, &renderer->backend);
  if (res != A_RES_OK) {
    free(renderer);
    return res;
  }

  res = a_renderer_technique_init(&renderer->backend, config, &renderer->technique);
  if (res != A_RES_OK) {
    free(renderer);
    return res;
  }

  *out = renderer;
  return A_RES_OK;
}

void a_renderer_destroy(ARenderer *renderer)
{
  a_renderer_backend_deinit(&renderer->backend);
  a_renderer_technique_deinit(&renderer->technique);
}

void a_renderer_backend_render(const ARendererBackend *backend) { backend->vtbl->render(backend); }

uint32 *a_renderer_backend_framebuffer(const ARendererBackend *backend)
{
  return backend->vtbl->get_framebuffer(backend);
}

double *a_renderer_backend_zbuffer(const ARendererBackend *backend)
{
  return backend->vtbl->get_zbuffer(backend);
}

ADimensions *a_renderer_backend_window_dimensions(const ARendererBackend *backend)
{
  return backend->vtbl->get_window_diminsions(backend);
}

void a_renderer_technique_process(ARendererTechnique *technique)
{
  technique->vtbl->process(technique);
}

void a_renderer_technique_set_scene(ARendererTechnique *technique, AScene *scene)
{
  technique->vtbl->set_scene(technique, scene);
}

AScene *a_renderer_technique_get_scene(ARendererTechnique *technique)
{
  return technique->vtbl->get_scene(technique);
}
