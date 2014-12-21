#include <cstdio>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#include "sdl_wrapper.hh"

/* Global SDL State */
static SDL_Window *window;
static SDL_Renderer *renderer;
static std::vector<SDL_Texture*> textures;
static std::vector<SDL_Rect> dimensions;
static size_t current_texture;

static constexpr char WINDOW_TITLE[] = "Motherload";
const int SDL::WINDOW_HEIGHT = 480;
const int SDL::WINDOW_WIDTH  = 640;
const int SDL::FPS           = 60;

/* static helper functions */
static SDL_Rect create_rect(int w, int h)
{
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = w;
    r.h = h;
    return r;
}

bool SDL::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

void SDL::quit()
{
    for (auto t : textures)
        SDL_DestroyTexture(t);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void SDL::render_clear()
{
    SDL_RenderClear(renderer);
}

void SDL::render_present()
{
    SDL_RenderPresent(renderer);
}

size_t SDL::load_texture(const std::string& path)
{
    SDL_Texture *new_texture = nullptr;

    SDL_Surface *loaded_surface = IMG_Load(path.c_str());
    if (!loaded_surface) {
        fprintf(stderr, "Unable to load image %s:\n%s\n", path.c_str(), IMG_GetError());
    } else {
        dimensions.push_back(create_rect(loaded_surface->w, loaded_surface->h));
        SDL_SetColorKey(loaded_surface, SDL_TRUE, SDL_MapRGB(loaded_surface->format, 0xFF, 0, 0xFF));
        new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
        if (!new_texture) {
            fprintf(stderr, "Unable to create texture from %s:\n%s\n", path.c_str(), SDL_GetError());
        }
        SDL_FreeSurface(loaded_surface);
    }
    textures.push_back(new_texture);
    return textures.size() - 1;
}

bool SDL::set_texture(size_t id)
{
    if (textures.size() > id) {
        current_texture = id;
        return true;
    } else {
        fprintf(stderr, "Error setting texture ID %zu. No such texture has been loaded\n", id);
        return false;
    }

}

bool SDL::render_texture(size_t id, int x, int y)
{
    size_t old_id = current_texture;
    if (set_texture(id)) {
        render_texture(x,y);
        current_texture = old_id;
        return true;
    } else return false;
}

bool SDL::render_texture(size_t id, int x, int y, double angle)
{
    size_t old_id = current_texture;
    if (set_texture(id)) {
        SDL_Rect r = dimensions[current_texture];
        r.x = x;
        r.y = y;
        SDL_RenderCopyEx(renderer, textures[current_texture], &dimensions[current_texture], &r,
                         angle, NULL, SDL_FLIP_NONE);
        current_texture = old_id;
        return true;
    } else return false;
}

void SDL::render_texture(int x, int y)
{
    SDL_Rect r = dimensions[current_texture];
    r.x = x;
    r.y = y;
    SDL_RenderCopy(renderer, textures[current_texture], &dimensions[current_texture], &r);
}
