#include <vector>
#include "block.hh"

#pragma once

namespace World {
    extern int scroll_x;
    extern int scroll_y;
    extern std::vector<std::vector<Block>> blocks;

    void load();
    void draw();
    void update();
}
