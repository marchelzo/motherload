#include <cstdio>
#include <string>
#include <vector>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "sdl_wrapper.hpp"

/* Global SDL State */
static SDL_Window *window;
static SDL_Renderer *renderer;
static std::vector<SDL_Texture*> textures;
static std::vector<Mix_Chunk*> sounds;
static std::map<size_t, int> channels;
static std::vector<SDL_Rect> dimensions;
static size_t current_texture;

/* TTF Font pointers for big and small versions of Mesmerize */
static TTF_Font *big_font;
static TTF_Font *small_font;

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
    } else SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize: %s\n", IMG_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer could not initialize: %s\n", Mix_GetError());
        return false;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "SDL_ttf could not initialize: %s\n", TTF_GetError());
        return false;
    } else {
        big_font = TTF_OpenFont("./assets/mesmerize.ttf", 28);
        small_font = TTF_OpenFont("./assets/mesmerize.ttf", 18);
    }

    return true;
}

void SDL::quit()
{
    for (auto t : textures)
        SDL_DestroyTexture(t);
    for (auto s : sounds)
        Mix_FreeChunk(s);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
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

size_t SDL::load_sound(const std::string& path)
{
    sounds.push_back(Mix_LoadWAV(path.c_str()));
    int channel = channels.size();
    channels[sounds.size() - 1] = channel;
    return sounds.size() - 1;
}

void SDL::play_sound(size_t id)
{
    Mix_PlayChannel(channels[id], sounds[id], 0);
}

void SDL::play_sound_loop(size_t id)
{
    channels[id] = id;
    Mix_PlayChannel(channels[id], sounds[id], -1);
}

void SDL::stop_loop(size_t id)
{
    Mix_HaltChannel(channels[id]);
}

size_t SDL::texture_from_string(const std::string& s, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_Color color = { r, g, b, a};
    SDL_Surface *text_surface = TTF_RenderText_Solid(big_font, s.c_str(), color);
    /* TODO: check text_surface == NULL ... maybe? */
    textures.push_back(SDL_CreateTextureFromSurface(renderer, text_surface));
    dimensions.push_back(create_rect(text_surface->w, text_surface->h));
    SDL_FreeSurface(text_surface);
    return textures.size() - 1;
}

size_t SDL::small_texture_from_string(const std::string& s, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_Color color = { r, g, b, a };
    SDL_Surface *text_surface = TTF_RenderText_Solid(small_font, s.c_str(), color);
    /* TODO: check text_surface == NULL ... maybe? */
    textures.push_back(SDL_CreateTextureFromSurface(renderer, text_surface));
    dimensions.push_back(create_rect(text_surface->w, text_surface->h));
    SDL_FreeSurface(text_surface);
    return textures.size() - 1;
}

void SDL::replace_texture(size_t old_id, size_t new_id)
{
    /* free the old texture */
    SDL_DestroyTexture(textures[old_id]);

    /* replace the old texture with the new one and then remove the old copy of
     * the new texture pointer from the textures vector
     */
    textures[old_id] = textures[new_id];
    textures.erase(textures.begin() + new_id);

    /* do the same thing for the dimensions vector */
    dimensions[old_id] = dimensions[new_id];
    dimensions.erase(dimensions.begin() + new_id);

}

void SDL::render_rect(const SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

void SDL::stop_all_sounds()
{
    Mix_HaltChannel(-1);
}
