#include <string>

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
    bool set_texture(size_t);
    bool render_texture(size_t, int x, int y);
    bool render_texture(size_t, int x, int y, double angle);
    void render_texture(int x, int y);
}

