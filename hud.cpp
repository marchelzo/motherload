#include <SDL2/SDL.h>

#include "hud.hh"
#include "sdl_wrapper.hh"
#include "digger.hh"

static size_t border_id;
static size_t hull_text_id;
static size_t fuel_text_id;

static SDL_Rect hull_rect = { 60, 40, 24, 82 };
static SDL_Rect fuel_rect = { 110, 40, 24, 82 };



/************************/
/* public HUD functions */
void HUD::load()
{
    border_id = SDL::load_texture("./assets/meter_border.png");
    hull_text_id = SDL::small_texture_from_string("Hull", 60, 180, 60, 255);
    fuel_text_id = SDL::small_texture_from_string("Fuel", 60, 180, 60, 255);
}

void HUD::draw()
{
    /* update the hull and fuel meters */
    hull_rect.h = (int) ceil(82.0 * Digger::hull / Digger::MAX_HULL);
    hull_rect.y = 40 + (82 - hull_rect.h);
    fuel_rect.h = (int) ceil(82.0 * Digger::fuel / Digger::MAX_FUEL);
    fuel_rect.y = 40 + (82 - fuel_rect.h);

    SDL::render_texture(border_id, 57, 37);
    SDL::render_texture(border_id, 107, 37);
    SDL::render_texture(hull_text_id, 54, 5);
    SDL::render_texture(fuel_text_id, 104, 5);

    SDL::render_rect(&hull_rect, 200, 20, 20, 140);
    SDL::render_rect(&fuel_rect, 20, 20, 200, 140);
}
