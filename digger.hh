#pragma once

#include <SDL2/SDL.h>

namespace Digger {
    extern float x;
    extern float y;
    int bottom();
    int top();
    int left();
    int right();
    void load();
    void draw();
    extern void (*update)();
    void handle_key(SDL_Keycode);
}
