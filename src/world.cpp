#include "digger.hpp"
#include "sdl_wrapper.hpp"
#include "world.hpp"
#include "block.hpp"

const int World::MAP_HEIGHT = 500;
const int World::MAP_WIDTH  = 30;

static const int MAX_SCROLL_X = World::MAP_WIDTH * 64 - SDL::WINDOW_WIDTH;
static const int MAX_SCROLL_Y = World::MAP_HEIGHT * 64;

int World::scroll_x;
int World::scroll_y;

static size_t dirt_texture_id;
std::vector<std::vector<Block>> World::blocks{World::MAP_HEIGHT};

void World::load()
{
    Block::load();
    dirt_texture_id = SDL::load_texture("./assets/dirt.png");
    for (int y = 0; y < World::MAP_HEIGHT; ++y)
        for (int x = 0; x < World::MAP_WIDTH; ++x)
            blocks[y].emplace_back(y >= 3 && ((double) rand() / (double) RAND_MAX) > 0.05);
}

void World::draw()
{
    int offset_y = scroll_y % 64;
    int offset_x = scroll_x % 64;
    for (int x = scroll_x; x < SDL::WINDOW_WIDTH + scroll_x + 64; x += 64) {
        for (int y = scroll_y; y < SDL::WINDOW_HEIGHT + scroll_y + 64; y += 64) {
            Block& block = blocks[std::min(y / 64, MAP_HEIGHT - 1)][std::min(x / 64, MAP_WIDTH - 1)];
            if (!block.drilled()) {
                SDL::render_texture(dirt_texture_id, x - scroll_x - offset_x, y - scroll_y - offset_y);
                if (block.has_ore()) {
                    SDL::render_texture(block.texture(), x - scroll_x - offset_x, y - scroll_y - offset_y);
                }
            }
        }
    }
}

void World::update()
{
    World::scroll_y = std::max(0, Digger::top() - 128);
    World::scroll_x = std::max(0, Digger::left() - SDL::WINDOW_WIDTH / 2);
    if (World::scroll_x > MAX_SCROLL_X)
        World::scroll_x = MAX_SCROLL_X;
}
