#include "digger.hh"
#include "sdl_wrapper.hh"
#include "world.hh"
#include "block.hh"

static constexpr int MAP_HEIGHT = 500;
static constexpr int MAP_WIDTH = 10;

int World::scroll_x;
int World::scroll_y;

static size_t dirt_texture_id;
std::vector<std::vector<Block>> World::blocks{MAP_HEIGHT};

void World::load()
{
    Block::load();
    dirt_texture_id = SDL::load_texture("./assets/dirt.png");
    for (int y = 0; y < MAP_HEIGHT; ++y)
        for (int x = 0; x < MAP_WIDTH; ++x)
            blocks[y].emplace_back(y >= 3 && ((double) rand() / (double) RAND_MAX) > 0.05);
}

void World::draw()
{
    int offset = scroll_y % 64;
    for (int x = 0; x < SDL::WINDOW_WIDTH / 64; ++x) {
        for (int y = scroll_y; y < SDL::WINDOW_HEIGHT + scroll_y + 64; y += 64) {
            if (!blocks[y / 64][x].drilled()) {
                SDL::render_texture(dirt_texture_id, 64 * x, y - scroll_y - offset);
                if (blocks[y / 64][x].has_ore()) {
                    SDL::render_texture(blocks[y / 64][x].texture(), 64 * x, y - scroll_y - offset);
                }
            }
        }
    }
}

void World::update()
{
    World::scroll_y = std::max(0, Digger::top() - 128);
}
