#include "sdl_wrapper.hpp"
#include "digger.hpp"
#include "world.hpp"
#include "hud.hpp"
#include "fuel_station.hpp"

#include <ctime>
#include <SDL2/SDL.h>

/* after we die, we render 60 more frames before closing
 * to allow the explosion animation and sound to finish
 */
static int frames_after_death = 60;

static int mouse_x;
static int mouse_y;

int main(int argc, char *argv[])
{
    srand(time(NULL));

    SDL::init();

    /* load all of the sounds and textures required */
    World::load();
    Digger::load();
    HUD::load();
    FuelStation::load();

    size_t test_text_id = SDL::small_texture_from_string("Testing this: 234", 255, 0, 0, 255);

    SDL_Event e;
    bool quit{};
    while (!quit && frames_after_death) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_KEYDOWN)
                Digger::handle_key_down(e.key.keysym.sym);
            else if (e.type == SDL_KEYUP)
                Digger::handle_key_up(e.key.keysym.sym);
            else if (e.type == SDL_MOUSEBUTTONUP) {
                SDL_GetMouseState(&mouse_x, &mouse_y);
                if (FuelStation::in_use()) FuelStation::click(mouse_x, mouse_y);
            }
                
        }

        /* clear the screen completely */
        SDL::render_clear();

        /* draw all of the blocks on the screen */
        World::draw();

        /* draw the various buildings */
        FuelStation::draw();

        /* update the fuel station (see if the Digger is trying to use it) */
        FuelStation::update();

        /* update the Digger's state, and then draw it */
        Digger::update();
        Digger::draw();

        /* draw the shop interfaces if they are open */
        if (FuelStation::in_use())
            FuelStation::draw_interface();

        /* update the world state, and draw the HUD elements */
        World::update();
        HUD::draw();

        /* finally, we actually render to the screen */
        SDL::render_present();

        if (!Digger::alive)
            --frames_after_death;
    }

    /* free all textures and sounds and clean up the SDL state */
    SDL::quit();
}
