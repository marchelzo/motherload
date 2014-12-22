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
    void update();
    void handle_key_down(SDL_Keycode);
    void handle_key_up(SDL_Keycode);

    /* Game variables */
    extern const float MAX_HULL;
    extern const float MAX_FUEL;

    extern float hull;
    extern float fuel;
    extern int money;

    extern bool alive;
}
