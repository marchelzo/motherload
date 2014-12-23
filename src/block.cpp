#include <string>

#include "block.hh"
#include "sdl_wrapper.hh"

static size_t ore_texture_ids[(int) Ore::NUM_ORE_TYPES];

static const std::string ORE_TEXTURE_PATHS[] = {
    "./assets/copper.png",
    "./assets/tin.png",
    "./assets/iron.png",
    "./assets/silver.png",
    "./assets/gold.png",
    "./assets/ruby.png",
    "./assets/diamond.png",
    "./assets/rock.png"
};

void Block::load()
{
    for (int i = 0; i < (int) Ore::NUM_ORE_TYPES; ++i) {
        ore_texture_ids[i] = SDL::load_texture(ORE_TEXTURE_PATHS[i]);
    }
}

Block::Block(bool not_empty): _drilled{!not_empty}
{
    ore = not_empty ? (Ore) (rand() % (int) Ore::NUM_ORE_TYPES) : Ore::NONE;
    if ((float) rand() / RAND_MAX > 0.08) ore = Ore::NONE;
    _drillable = ore != Ore::ROCK;
}

size_t Block::texture()
{
    return ore_texture_ids[(int) ore];
}

bool Block::drillable()
{
    return _drillable;
}

bool Block::drilled()
{
    return _drilled;
}

void Block::drill()
{
    _drilled = true;
}

bool Block::has_ore()
{
    return ore != Ore::NONE;
}

void Block::reserve()
{
    ore = Ore::NONE;
    _drilled = false;
    _drillable = false;
}
