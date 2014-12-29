#include <memory>

#include "repair_shop.hpp"
#include "sdl_wrapper.hpp"
#include "block.hpp"
#include "world.hpp"
#include "mouse_interface.hpp"
#include "digger.hpp"

static constexpr int X_POS = 1000;
static constexpr int Y_POS = 0;

static bool interface_open;

static bool recently_opened;

static std::unique_ptr<MouseInterface> interface;
static std::string status_message {"Repair Shop"};


static size_t shop_id;      /* 192 x 192 */
static size_t status_message_id;

static void close_interface()
{
    recently_opened = true;
    Digger::enable();
    interface_open = false;
}

static void repair_hull()
{
    int repair_amount = std::min(Digger::money * 4, Digger::max_hull - Digger::hull);
    Digger::money -= repair_amount / 4;
    Digger::hull += repair_amount;
}

void RepairShop::load()
{
    shop_id           = SDL::load_texture("./assets/repair_shop.png");
    status_message_id = SDL::texture_from_string(status_message, 200, 200, 40, 255);


    /* reserve the blocks underneath of the repair shop */
    for (int i = 0; i < 4; ++i)
        World::blocks[(Y_POS + 192) / 64][X_POS / 64 + i].reserve();

    /* set up the repair shop interface */
    interface = std::make_unique<MouseInterface>(540, 400, 50, 40,
        SDL::load_texture("./assets/repair_shop_interface.png"),
        [&](){ status_message = "Repair Shop"; });
    size_t button_bg = SDL::load_texture("./assets/button_bg.png");

    /* add all of the necessary buttons to the interface */
    interface->add_button(128, 92, 94, 154, button_bg, SDL::load_texture("./assets/wrench.png"),
        [&]() { status_message = "Repair Hull"; },
        []() { repair_hull(); close_interface(); });
    interface->add_button(128, 92, 316, 154, button_bg, SDL::small_texture_from_string("Exit", 0, 0, 0, 255),
                          nullptr,
                          []() { close_interface(); });

}

void RepairShop::draw()
{
    int x = X_POS - World::scroll_x;
    int y = Y_POS  - World::scroll_y;

    /* don't draw the repair shop if it's not in range of the Digger */
    if (x < -192 || x > SDL::WINDOW_WIDTH || y < -192 || y > SDL::WINDOW_HEIGHT)
        return;

    SDL::render_texture(shop_id, x, y);
}

void RepairShop::update()
{
    if (Digger::x > X_POS - 60 && Digger::x < X_POS + 200 && Digger::y == 128) {
        if (!recently_opened) {
            interface_open = true;
            Digger::disable();
        }
    } else {
        recently_opened = false;
    }
}

void RepairShop::draw_interface()
{
    interface->update_hover();
    /* caller has ensured the interface is open */
    SDL::replace_texture(status_message_id, SDL::texture_from_string(status_message, 200, 200, 20, 255));
    interface->draw();
    SDL::render_texture(status_message_id, 100, 68);
}

bool RepairShop::in_use()
{
    return interface_open;
}

void RepairShop::click(int x, int y)
{
    interface->handle_click(x,y);
}
