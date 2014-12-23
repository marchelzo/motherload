#include <cstdlib>

#pragma once

enum class Ore {
    COPPER,
    TIN,
    IRON,
    SILVER,
    GOLD,
    RUBY,
    DIAMOND,
    ROCK,
    NUM_ORE_TYPES,
    NONE
};

class Block {
    Ore ore;
    bool _drilled;
    bool _drillable;
public:
    Block(bool);
    size_t texture();
    bool drillable();
    bool drilled();
    bool has_ore();
    void drill();
    void reserve();

    static void load();
};
