#include "map.h"
#include "config.h"
#include <stdio.h>

// Function to print the map values to the console
void print_map(Map map)
{
  printf("Walls:\n");
  for (int i = 0; i < map.height; i++)
  {
    for (int j = 0; j < map.width; j++)
    {
      printf("%d ", map.walls[i][j]); // Print each value in the row
    }
    printf("\n"); // Newline after each row
  }

  printf("Ceil:\n");
  for (int i = 0; i < map.height; i++)
  {
    for (int j = 0; j < map.width; j++)
    {
      printf("%d ", map.ceil[i][j]); // Print each value in the row
    }
    printf("\n"); // Newline after each row
  }

  printf("Floor:\n");
  for (int i = 0; i < map.height; i++)
  {
    for (int j = 0; j < map.width; j++)
    {
      printf("%d ", map.floor[i][j]); // Print each value in the row
    }
    printf("\n"); // Newline after each row
  }
}

Map load_map(char *filename)
{
  Map map;
  map.unit_size = DEFAULT_MAP_UNIT_SIZE;
  load_map_grid_from_file(filename, &map);
  return map;
}

void load_map_grid_from_file(const char *filename, Map *map)
{
  // Open the file
  FILE *file = fopen(filename, "r");
  if (!file)
  {
    fprintf(stderr, "Failed to open map file: %s\n", filename);
    exit(1);
  }

  // Read the width and height of the map
  fscanf(file, "%d %d", &map->width, &map->height);

  // Allocate memory for the map
  uint8_t **walls = malloc(map->height * sizeof(uint8_t *));
  uint8_t **ceil = malloc(map->height * sizeof(uint8_t *));
  uint8_t **floor = malloc(map->height * sizeof(uint8_t *));
  if (!walls || !ceil || !floor)
  {
    fprintf(stderr, "Failed to allocate memory for map\n");
    if (walls) free(walls);
    if (ceil) free(ceil);
    if (floor) free(floor);
    fclose(file);
    return;
  }

  for (int i = 0; i < map->height; i++)
  {
    walls[i] = malloc(map->width * sizeof(uint8_t));
    if (!walls[i])
    {
      fprintf(stderr, "Failed to allocate memory for walls row\n");
      fclose(file);
      return;
    }

    // Read each value into the map
    for (int j = 0; j < map->width; j++)
    {
      if (fscanf(file, "%hhu", &walls[i][j]) != 1)
      {
        fprintf(stderr, "Failed to read value for walls[%d][%d]\n", i, j);
        fclose(file);
        return;
      }
    }
  }

  for (int i = 0; i < map->height; i++)
  {
    ceil[i] = malloc(map->width * sizeof(uint8_t));
    if (!ceil[i])
    {
      fprintf(stderr, "Failed to allocate memory for ceil row\n");
      fclose(file);
      return;
    }

    // Read each value into the map
    for (int j = 0; j < map->width; j++)
    {
      if (fscanf(file, "%hhu", &ceil[i][j]) != 1)
      {
        fprintf(stderr, "Failed to read value for ceil[%d][%d]\n", i, j);
        fclose(file);
        return;
      }
    }
  }

  for (int i = 0; i < map->height; i++)
  {
    floor[i] = malloc(map->width * sizeof(uint8_t));
    if (!floor[i])
    {
      fprintf(stderr, "Failed to allocate memory for floor row\n");
      fclose(file);
      return;
    }

    // Read each value into the map
    for (int j = 0; j < map->width; j++)
    {
      if (fscanf(file, "%hhu", &floor[i][j]) != 1)
      {
        fprintf(stderr, "Failed to read value for floor[%d][%d]\n", i, j);
        fclose(file);
        return;
      }
    }
  }
  map->walls = walls;
  map->ceil = ceil;
  map->floor = floor;
  fclose(file);
}

// Function to free the dynamically allocated map memory
void free_map(Map map)
{
  for (int i = 0; i < map.height; i++)
  {
    free(map.walls[i]);
    free(map.ceil[i]);
    free(map.floor[i]);
  }
  free(map.walls);
  free(map.ceil);
  free(map.floor);
}
