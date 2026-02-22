#include "timing.h"
#include <SDL.h>
#include <stdio.h>

#define FIXED_DT 0.0166667f  // 60 Hz fixed timestep (1/60)

void timing_init(GameTiming *timing)
{
  timing->last_time = SDL_GetPerformanceCounter();
  timing->current_time = 0;
  timing->delta_time = 0.0f;
  timing->accumulator = 0.0f;
  timing->fps = 0.0f;
  timing->frame_time = 0.0f;
  timing->frame_count = 0;
  timing->fps_timer = 0;
}

void timing_update(GameTiming *timing)
{
  timing->current_time = SDL_GetPerformanceCounter();
  uint64_t perf_freq = SDL_GetPerformanceFrequency();

  timing->delta_time = (float)((timing->current_time - timing->last_time) / (double)perf_freq);
  timing->last_time = timing->current_time;

  timing->accumulator += timing->delta_time;

  timing->frame_count++;
  timing->fps_timer += timing->current_time;

  if (timing->fps_timer >= perf_freq)
  {
    timing->fps = (float)timing->frame_count / ((float)timing->fps_timer / (float)perf_freq);
    timing->frame_count = 0;
    timing->fps_timer = 0;
  }
}

float timing_get_delta_time(GameTiming *timing)
{
  return timing->delta_time;
}

void timing_cap_fps(int target_fps)
{
  if (target_fps <= 0)
  {
    return;
  }

  float target_frame_time = 1000.0f / (float)target_fps;
  uint32_t current_ticks = SDL_GetTicks();
  static uint32_t last_ticks = 0;

  if (last_ticks == 0)
  {
    last_ticks = current_ticks;
    return;
  }

  uint32_t elapsed = current_ticks - last_ticks;

  if (elapsed < (uint32_t)target_frame_time)
  {
    SDL_Delay((uint32_t)target_frame_time - elapsed);
  }

  last_ticks = SDL_GetTicks();
}
