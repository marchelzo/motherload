#include "mineral_processor.hpp"
#include "world.hpp"
#include "block.hpp"
#include "digger.hpp"
#include "sdl_wrapper.hpp"

static constexpr int X_POS = 740;
static constexpr int Y_POS = 64;

static size_t mineral_processor_id;

void MineralProcessor::load()
{
    mineral_processor_id = SDL::load_texture("./assets/mineral_processor.png");

    /* reserve the blocks beneath the mineral processor */
    for (int i = 0; i < 3; ++i)
        World::blocks[(Y_POS + 128) / 64][X_POS / 64 + i].reserve();
}

void MineralProcessor::draw()
{
    int x = X_POS - World::scroll_x;
    int y = Y_POS  - World::scroll_y;

    /* don't draw the mineral processor if it's not in range of the Digger */
    if (x < -128 || x > SDL::WINDOW_WIDTH || y < -128 || y > SDL::WINDOW_HEIGHT)
        return;

    SDL::render_texture(mineral_processor_id, x, y);
}

void MineralProcessor::update() {}
