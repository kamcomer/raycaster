#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

static int create_config()
{
    FILE *file = fopen(CONFIG_PATH, "w+");
    if (!file)
    {
        fprintf(stderr, "Could not create new window_config file: %s\n", CONFIG_PATH);
        return -1;
    }
    fprintf(file, "win_title=%s\n", DEFAULT_WIN_TITLE);
    fprintf(file, "win_width=%d\n", DEFAULT_WIN_WIDTH);
    fprintf(file, "win_height=%d\n", DEFAULT_WIN_HEIGHT);
    fprintf(file, "win_max_fps=%d\n", DEFAULT_WIN_MAX_FPS);
    fprintf(file, "win_show_fps=%d\n", true);

    fclose(file);
    return 0;
}

static FILE *open_config()
{
    FILE *file = fopen(CONFIG_PATH, "r+");
    if (!file)
    {
        fprintf(stderr, "Could not open window_config file: %s\n", CONFIG_PATH);
        return NULL;
    }
    return file;
}

static Config *read_config(FILE *file)
{
    if (!file)
        return NULL;

    Config *config = malloc(sizeof(Config));
    if (!config)
        return NULL;
    config->window_config = malloc(sizeof(WindowConfig));
    if (!config->window_config)
    {
        free(config);
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (key && value)
        {
            if (strcmp(key, "win_title") == 0)
                config->window_config->title = strdup(value);
            else if (strcmp(key, "win_width") == 0)
                config->window_config->width = atoi(value);
            else if (strcmp(key, "win_height") == 0)
                config->window_config->height = atoi(value);
            else if (strcmp(key, "win_max_fps") == 0)
                config->window_config->max_fps = atoi(value);
            else if (strcmp(key, "win_show_fps") == 0)
                config->window_config->show_fps = atoi(value);
        }
    }
    return config;
}

Config *init_config()
{
    FILE *file = open_config();
    if (!file)
    {
        if (create_config() != 0)
            return NULL;
        file = open_config();
        if (!file)
            return NULL;
    }

    Config *config = read_config(file);
    fclose(file);
    return config;
}

void print_config()
{
    FILE *file = open_config();
    if (!file)
        return;
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        printf("%s", line);
    }
    fclose(file);
}

void free_config(Config *config)
{
    if (!config)
        return;
    if (config->window_config)
    {
        free(config->window_config->title);
        free(config->window_config);
    }
    free(config);
}
