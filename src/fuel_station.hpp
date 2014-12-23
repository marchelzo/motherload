#pragma once

#include <SDL2/SDL.h>

namespace FuelStation {
    void load();
    void draw();
    void draw_interface();
    bool in_use();
    void click(int x, int y);
    void key_down(SDL_Keycode k);
    void update();
}
