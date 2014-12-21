#include "sdl_wrapper.hh"
#include "digger.hh"
#include "world.hh"

#include <ctime>
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    srand(time(NULL));
    SDL::init();
    World::load();
    Digger::load();

    SDL_Event e;
    bool quit{};
    while (!quit) {
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

        World::update();

        SDL::render_present();
    }

    SDL::quit();
}
