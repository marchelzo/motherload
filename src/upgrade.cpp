#include <memory>

#include "upgrade.hpp"
#include "sdl_wrapper.hpp"
#include "block.hpp"
#include "world.hpp"
#include "mouse_interface.hpp"
#include "digger.hpp"

static bool interface_open;

static std::unique_ptr<MouseInterface> main;
static std::unique_ptr<MouseInterface> engines;
static std::unique_ptr<MouseInterface> hulls;
static std::unique_ptr<MouseInterface> fuel_tanks;
static std::unique_ptr<MouseInterface> cargo_bays;

static MouseInterface* current_interface = main.get();

static constexpr int X_POS = 1300;
static constexpr int Y_POS = 0;

static size_t shop_id;      /* 192 x 192 */
static size_t status_message_id;

static bool recently_opened;
static std::string status_message {"Upgrade Shop"};


/*** current upgrade costs ***/
 /* increase exponentially */

static int fuel_upgrade_cost = 100;

/*****************************/


/*** Static helper functions ***/
static void close_interface()
{
    Digger::enable();
    interface_open = false;
    current_interface = main.get();
}

static void upgrade_fuel_tank()
{
    if (Digger::money >= fuel_upgrade_cost) {
        Digger::money -= fuel_upgrade_cost;
        Digger::max_fuel += 4;
        fuel_upgrade_cost *= 2;
    }
}
/*******************************/

void Upgrade::load()
{
    shop_id = SDL::load_texture("./assets/upgrade.png");
    status_message_id = SDL::texture_from_string(status_message, 200, 200, 20, 255);

    /* reserve the blocks underneath of the upgrade shop */
    for (int i = 0; i < 3; ++i)
        World::blocks[(Y_POS + 192) / 64][X_POS / 64 + i].reserve();

    /* set up all of the interfaces */
    size_t button_bg        = SDL::load_texture("./assets/button_bg.png");
    size_t upgrade_arrow_id = SDL::load_texture("./assets/upgrade_arrow.png");
    size_t back_arrow_id    = SDL::load_texture("./assets/back_arrow.png");
    size_t interface_bg     = SDL::load_texture("./assets/repair_shop_interface.png");

    main = std::make_unique<MouseInterface>(540, 400, 50, 40, interface_bg, [&](){ status_message = "Upgrade Shop"; });
    main->add_button(128, 92, 94, 154, button_bg, SDL::small_texture_from_string("Fuel Tanks", 0, 0, 0, 255),
        nullptr,
        [&](){ current_interface = fuel_tanks.get(); });
    main->add_button(128, 92, 316, 154, button_bg, SDL::small_texture_from_string("Exit", 0, 0, 0, 255),
        nullptr,
        close_interface);


    fuel_tanks = std::make_unique<MouseInterface>(540, 400, 50, 40, interface_bg, [&](){ status_message = "Fuel Tanks"; });
    fuel_tanks->add_button(128, 92, 94, 90, button_bg, upgrade_arrow_id,
        [&](){ status_message = "Cost: " + std::to_string(fuel_upgrade_cost); },
        upgrade_fuel_tank);
    fuel_tanks->add_button(128, 92, 316, 90, button_bg, SDL::small_texture_from_string("Exit", 0, 0, 0, 255),
        nullptr,
        close_interface);
    fuel_tanks->add_button(128, 92, 206, 230, button_bg, back_arrow_id,
        [&](){ status_message = "Back"; },
        [&](){ current_interface = main.get(); });

}

void Upgrade::draw()
{
    int x = X_POS - World::scroll_x;
    int y = Y_POS  - World::scroll_y;

    /* don't draw the repair shop if it's not in range of the Digger */
    if (x < -192 || x > SDL::WINDOW_WIDTH || y < -192 || y > SDL::WINDOW_HEIGHT)
        return;

    SDL::render_texture(shop_id, x, y);
}

void Upgrade::update()
{
    if (Digger::x > X_POS - 60 && Digger::x < X_POS + 200 && Digger::y == 128) {
        if (!recently_opened) {
            recently_opened = true;
            interface_open = true;
            current_interface = main.get();
            Digger::disable();
        } else {
            current_interface->update_hover();
        }
    } else {
        recently_opened = false;
    }
}

bool Upgrade::in_use()
{
    return interface_open;
}

void Upgrade::draw_interface()
{
    current_interface->update_hover();
    SDL::replace_texture(status_message_id, SDL::texture_from_string(status_message, 200, 200, 20, 255));
    current_interface->draw();
    SDL::render_texture(status_message_id, 100, 68);
}

void Upgrade::click(int x, int y)
{
    current_interface->handle_click(x,y);
}
