#include <vector>
#include <string>
#include <functional>

#include "mouse_interface.hpp"
#include "sdl_wrapper.hpp"

MouseInterface::Button::Button(int w, int h, int x, int y,
                               size_t bg,
                               size_t fg,
                               std::function<void()> on_hover,
                               std::function<void()> on_click):
    w{w},
    h{h},
    x{x},
    y{y},
    fg{fg},
    bg{bg},
    on_hover{on_hover},
    on_click{on_click}
{ }

bool MouseInterface::Button::inside(int x, int y)
{
    return x >= this->x
        && x <= this->x + this->w
        && y >= this->y
        && y <= this->y + this->h;
}

MouseInterface::MouseInterface( int w
                  , int h
                  , int x
                  , int y
                  , size_t bg
                  , std::function<void()> default_hover_action
                  ):
    w{w},
    h{h},
    x{x},
    y{y},
    bg{bg},
    default_hover_action{default_hover_action},
    buttons{}
{ }

void MouseInterface::draw()
{
    SDL::render_texture(bg, x, y);
    
    for (auto& b : buttons) {
        SDL::render_texture(b.bg, b.x + x, b.y + y);
        SDL::render_texture(b.fg,
            b.x + x + (b.w - SDL::texture_width(b.fg)) / 2,
            b.y + y + (b.h - SDL::texture_height(b.fg)) / 2);
    }
}

void MouseInterface::handle_click(int xc, int yc)
{
    int x = xc - this->x;
    int y = yc - this->y;

    for (auto& b : buttons) {
        if (b.inside(x,y)) {
            if (b.on_click) b.on_click();
        }
    }
}

void MouseInterface::update_hover()
{
    int x = SDL::mouse_x - this->x;
    int y = SDL::mouse_y - this->y;

    for (auto& b : buttons) {
        if (b.inside(x,y)) {
            if (b.on_hover) {
                b.on_hover();
                return;
            }
        }
    }

    default_hover_action();
}

void MouseInterface::add_button(int w, int h, int x, int y,
           size_t bg,
           size_t fg,
           std::function<void()> on_hover,
           std::function<void()> on_click)
{
    buttons.emplace_back(w, h, x, y, bg, fg, on_hover, on_click);
}
