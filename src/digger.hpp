#include <SDL2/SDL.h>
#include <map>
#include "block.hpp"

#pragma once

namespace Digger {
    extern float x;
    extern float y;
    extern float vx;
    extern float vy;

    int bottom();
    int top();
    int left();
    int right();
    void load();
    void draw();
    void update();
    void handle_key_down(SDL_Keycode);
    void handle_key_up(SDL_Keycode);

    /* disable and re-enable Digger functionality               */
    /* this is used for when one of the shop interfaces is open */
    void enable();
    void disable();

    /* Game variables */
    extern const float MAX_HULL;
    extern const float MAX_FUEL;

    extern float hull;
    extern float fuel;
    extern int money;

    extern bool alive;
    
    extern int ores[static_cast<int>(Ore::NUM_ORE_TYPES)];
    //extern std::map<Item,int> items;

    void acquire(Ore);
    //void acquire(Item);
}
