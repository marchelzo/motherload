#include <SDL2/SDL.h>

#include "hud.hh"
#include "sdl_wrapper.hh"
#include "digger.hh"

static size_t border_id;
static size_t hull_text_id;
static size_t fuel_text_id;
static size_t money_text_id;

/* constant versions of status bars */
static constexpr SDL_Rect HULL_RECT = { 50, 5, 82, 24 };
static constexpr SDL_Rect FUEL_RECT = { 196, 5, 82, 24 };

/* mutable versions that change according to hull / fuel status */
static SDL_Rect hull_rect = HULL_RECT;
static SDL_Rect fuel_rect = FUEL_RECT;



/************************/
/* public HUD functions */
void HUD::load()
{
    border_id = SDL::load_texture("./assets/meter_border.png");
    hull_text_id  = SDL::small_texture_from_string("Hull:", 60, 180, 60, 255);
    fuel_text_id  = SDL::small_texture_from_string("Fuel:", 60, 180, 60, 255);
    money_text_id = SDL::small_texture_from_string("Funds: $20", 60, 180, 60, 255);
}

void HUD::draw()
{
    
    /* update the money text */
    SDL::replace_texture(money_text_id,
                         SDL::small_texture_from_string("Funds: $" + std::to_string(Digger::money),
                                                        60, 180, 60, 255));

    /* update the hull and fuel meters */
    hull_rect.w = (int) ceil(82.0 * Digger::hull / Digger::MAX_HULL);
    fuel_rect.w = (int) ceil(82.0 * Digger::fuel / Digger::MAX_FUEL);

    SDL::render_texture(border_id, 47, 2);
    SDL::render_texture(border_id, 193, 2);
    SDL::render_texture(hull_text_id, 5, 5);
    SDL::render_texture(fuel_text_id, 148, 5);
    SDL::render_texture(money_text_id, 380, 5);

    /* draw the transparent background rects */
    SDL::render_rect(&HULL_RECT, 200, 20, 20, 100);
    SDL::render_rect(&FUEL_RECT, 20, 20, 200, 100);

    /* draw the opaque part indicating remaining amount */
    SDL::render_rect(&hull_rect, 200, 20, 20, 255);
    SDL::render_rect(&fuel_rect, 20, 20, 200, 255);
}
