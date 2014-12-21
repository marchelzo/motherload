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
            Digger::handle_key(e.key.keysym.sym);
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
