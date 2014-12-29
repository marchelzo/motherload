#include "sdl_wrapper.hpp"
#include "digger.hpp"
#include "world.hpp"
#include "hud.hpp"
#include "fuel_station.hpp"
#include "mineral_processor.hpp"
#include "repair_shop.hpp"
#include "upgrade.hpp"

#include <ctime>
#include <SDL2/SDL.h>

/* after we die, we render 60 more frames before closing
 * to allow the explosion animation and sound to finish
 */
static int frames_after_death = 60;

int main(int argc, char *argv[])
{
    srand(time(NULL));

    SDL::init();

    /* load all of the sounds and textures required */
    World::load();
    Digger::load();
    HUD::load();
    FuelStation::load();
    MineralProcessor::load();
    RepairShop::load();
    Upgrade::load();

    SDL_Event e;
    bool quit{};
    while (!quit && frames_after_death) {

        /* update the mouse coordinates */
        SDL::update_mouse_coords();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            else if (e.type == SDL_KEYDOWN) {
                /* send key press to any buildings if necessary */
                if (FuelStation::in_use())
                    FuelStation::key_down(e.key.keysym.sym);
                else /* no interfaces are open, so the Digger handles the key */
                    Digger::handle_key_down(e.key.keysym.sym);
            }
            else if (e.type == SDL_KEYUP)
                Digger::handle_key_up(e.key.keysym.sym);
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (FuelStation::in_use()) FuelStation::click(SDL::mouse_x, SDL::mouse_y);
                else if (RepairShop::in_use()) RepairShop::click(SDL::mouse_x, SDL::mouse_y);
                else if (Upgrade::in_use()) Upgrade::click(SDL::mouse_x, SDL::mouse_y);
            }
                
        }

        /* clear the screen completely */
        SDL::render_clear();

        /* draw all of the blocks on the screen */
        World::draw();

        /* draw the various buildings */
        FuelStation::draw();
        MineralProcessor::draw();
        RepairShop::draw();
        Upgrade::draw();

        /* update the buildings (see if the Digger is trying to use them) */
        FuelStation::update();
        MineralProcessor::update();
        RepairShop::update();
        Upgrade::update();

        /* update the Digger's state, and then draw it */
        Digger::update();
        Digger::draw();

        /* draw the shop interfaces if they are open */
        if (FuelStation::in_use()) {
            FuelStation::draw_interface();
        } else if (RepairShop::in_use()) {
            RepairShop::draw_interface();
        } else if (Upgrade::in_use()) {
            Upgrade::draw_interface();
        }

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
