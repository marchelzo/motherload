#include "sdl_wrapper.hh"
#include "digger.hh"
#include "world.hh"
#include "hud.hh"

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

    World::load();
    Digger::load();
    HUD::load();

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
        }

        SDL::render_clear();

        World::draw();

        Digger::update();
        Digger::draw();

        if (!Digger::alive)
            --frames_after_death;

        World::update();

        HUD::draw();

        SDL::render_present();
    }

    SDL::quit();
}
