#include "fuel_station.hh"
#include "sdl_wrapper.hh"
#include "digger.hh"
#include "world.hh"

static bool interface_open;             /* is the fuel interface open? */

static const int X_OFF = SDL::WINDOW_WIDTH  / 2 - 540 / 2;
static const int Y_OFF = SDL::WINDOW_HEIGHT / 2 - 400 / 2;

/* texture ids for the textures required by the fuel station */
static size_t fuel_station_id;          /* 128 x 128 */
static size_t fuel_interface_id;        /* 540 x 400 */
static size_t fuel_status_message_id;

/* sound if for the gas nozzle sound effect */
static size_t fuel_sound_id;

/* status message displayed in the fuel interface */
static std::string status_message {"Fuel Shop"};


/****************************************/
/* Static Helper Fuel Station Functions */
static void attempt_purchase(int amount)
{
    if (amount) {
        if (Digger::money >= amount) {
        int spent = std::min(amount, 10 - (int)Digger::fuel);
            Digger::money -= spent;
            status_message = "Purchased: " + std::to_string(spent) + " liters";
            Digger::fuel = (float) ((int)Digger::fuel + spent);
            SDL::play_sound(fuel_sound_id);
        } else {
            status_message = "Insufficient funds";
        }
    } else { /* 0 was passed, so we attempt to fill the tank */
        int spent = std::min(Digger::money, 10 - (int)Digger::fuel);
        Digger::money -= spent;
        Digger::fuel = (float) ((int)Digger::fuel + spent);
        status_message = "Purchased: " + std::to_string(spent) + " liters";
        SDL::play_sound(fuel_sound_id);
    }
}
/****************************************/


/*********************************/
/* Public Fuel Station Functions */
void FuelStation::load()
{
    fuel_station_id        = SDL::load_texture("./assets/fuel.png");
    fuel_interface_id      = SDL::load_texture("./assets/fuel_interface.png");
    fuel_status_message_id = SDL::texture_from_string(status_message, 255, 255, 0, 255);
    fuel_sound_id          = SDL::load_sound("./assets/fuel.wav");

    /* ensure that the blocks beneath the fuel station are
     * there, and are undrillable
     */
    World::blocks[3][4].reserve();
    World::blocks[3][5].reserve();
}

void FuelStation::draw()
{
    /* return early if we are scrolled past the fuel station */
    if (World::scroll_y > 192) return;
    if (World::scroll_x > 300) return;

    SDL::render_texture(fuel_station_id, 260 - World::scroll_x, 64 - World::scroll_y);
}

bool FuelStation::in_use()
{
    return interface_open;
}

void FuelStation::update()
{
    /* if the fuel interface is open, we don't need to do anything here */
    if (interface_open) return;

    if ((int) Digger::x > 260 &&
        (int) Digger::x < 280 &&
        (int) Digger::y == 128   ) {

        interface_open = true;
        Digger::disable();
    }
}

void FuelStation::draw_interface()
{
    /* update the status message */
    SDL::replace_texture(fuel_status_message_id,
                         SDL::texture_from_string(status_message, 255, 255, 0, 255));

    SDL::render_texture(fuel_interface_id,
                        (SDL::WINDOW_WIDTH / 2)  - (540 / 2),
                        (SDL::WINDOW_HEIGHT / 2) - (400 / 2));

    SDL::render_texture(fuel_status_message_id, 80, 60);
}

void FuelStation::click(int x, int y)
{
    /* the caller of this function has already checked
     * that the fuel interface is open
     */

    if (x > X_OFF + 30 && x < X_OFF + 160 && y > Y_OFF + 90 && y < Y_OFF + 220) 
        attempt_purchase(5);
    else if (x > X_OFF + 202 && x < X_OFF + 332 && y > Y_OFF + 90 && y < Y_OFF + 220) 
        attempt_purchase(10);
    else if (x > X_OFF + 374 && x < X_OFF + 504 && y > Y_OFF + 90 && y < Y_OFF + 220) 
        attempt_purchase(25);
    else if (x > X_OFF + 116 && x < X_OFF + 246 && y > Y_OFF + 236 && y < Y_OFF + 366) 
        attempt_purchase(50);
    else if (x > X_OFF + 296 && x < X_OFF + 426 && y > Y_OFF + 236 && y < Y_OFF + 366) 
        attempt_purchase(0); /* 0 means fill the tank */
    else if ((x - (496 + X_OFF)) * (x - (496 + X_OFF)) +
        (y - (40  + Y_OFF)) * (y - (40  + Y_OFF)) <= 18 * 18) {
            status_message = "Fuel Shop";
            Digger::enable();
            interface_open = false;
            /* move the digger slightly so that the fuel interface
             * doesn't immediately re-open
             */
            if (Digger::vx > 0)
                Digger::x = 282;
            else
                Digger::x = 258;
        }
}
/*********************************/
