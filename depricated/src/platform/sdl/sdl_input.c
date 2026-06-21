#include "input.h"
#include <SDL3/SDL.h>

uint8_t* get_keyboard_state()
{
    return (uint8_t*)SDL_GetKeyboardState(NULL);
}