#include <string>
#include <SDL2/SDL.h>

#pragma once

/* public functions */
namespace SDL {
    extern const int WINDOW_WIDTH;
    extern const int WINDOW_HEIGHT;
    extern const int FPS;
    bool init();
    void quit();
    void render_clear();
    void render_present();
    size_t load_texture(const std::string&);
    size_t texture_from_string(const std::string&, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    size_t small_texture_from_string(const std::string&, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    size_t load_sound(const std::string&);
    bool set_texture(size_t);
    bool render_texture(size_t, int x, int y);
    bool render_texture(size_t, int x, int y, double angle);
    void replace_texture(size_t, size_t);
    void render_texture(int x, int y);
    void render_rect(const SDL_Rect *rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void play_sound(size_t id);
    void play_sound_loop(size_t id);
    void stop_loop(size_t id);
    void stop_all_sounds();
}

