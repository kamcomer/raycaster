#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>

typedef struct
{
  uint64_t last_time;
  uint64_t current_time;
  float delta_time;
  float accumulator;
  float fps;
  float frame_time;
  uint32_t frame_count;
  uint64_t fps_timer;
} GameTiming;

void timing_init(GameTiming *timing);
void timing_update(GameTiming *timing);
float timing_get_delta_time(GameTiming *timing);
void timing_cap_fps(int target_fps);

#endif
