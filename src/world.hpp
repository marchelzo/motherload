#include <vector>
#include "block.hpp"

#pragma once

namespace World {
    extern const int MAP_HEIGHT;
    extern const int MAP_WIDTH;
    extern int scroll_x;
    extern int scroll_y;
    extern std::vector<std::vector<Block>> blocks;

    void load();
    void draw();
    void update();
}
