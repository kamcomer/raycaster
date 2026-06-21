#include "sprite.h"

void sort_sprites(int *s_sprite_order, double *s_sprit_dist,
                  int num_s_sprites) {
  for (int i = 0; i < num_s_sprites - 1; i++) {
    for (int j = 0; j < num_s_sprites - i - 1; j++) {
      // Compare distances of adjacent sprites
      if (s_sprit_dist[j] < s_sprit_dist[j + 1]) {
        // Swap distances
        float temp_dist = s_sprit_dist[j];
        s_sprit_dist[j] = s_sprit_dist[j + 1];
        s_sprit_dist[j + 1] = temp_dist;

        // Swap sprite orders to keep them aligned with distances
        int temp_order = s_sprite_order[j];
        s_sprite_order[j] = s_sprite_order[j + 1];
        s_sprite_order[j + 1] = temp_order;
      }
    }
  }
}
