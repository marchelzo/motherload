#include <vector>
#include <functional>

#pragma once

class MouseInterface {

struct Button {
    size_t bg;
    size_t fg;
    int w, h;
    int x, y;
    std::function<void()> on_click;
    std::function<void()> on_hover;
    Button(int, int, int, int,
           size_t,
           size_t,
           std::function<void()>,
           std::function<void()>);
    bool inside(int x, int y);
};

int x, y;
int w, h;

size_t bg;

std::vector<Button> buttons;

std::function<void()> default_hover_action;

public:
    MouseInterface( int w
                  , int h
                  , int x
                  , int y
                  , size_t bg
                  , std::function<void()>
                  );
    void draw();
    void handle_click(int x, int y);
    void update_hover();
    void add_button(int, int, int, int,
           size_t,
           size_t,
           std::function<void()>,
           std::function<void()>);
};
