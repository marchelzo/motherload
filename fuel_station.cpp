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
    if (Digger::money >= amount) {
        Digger::money -= amount;
        SDL::play_sound(fuel_sound_id);
        status_message = "Purchased: " + std::to_string(amount) + " liters";
    } else {
        status_message = "Insufficient funds";
    }
}


/*********************************/
/* Public Fuel Station Functions */
void FuelStation::load()
{
    fuel_station_id        = SDL::load_texture("./assets/fuel.png");
    fuel_interface_id      = SDL::load_texture("./assets/fuel_interface.png");
    fuel_status_message_id = SDL::texture_from_string(status_message, 255, 255, 0, 255);
    fuel_sound_id          = SDL::load_sound("./assets/fuel.wav");
}

void FuelStation::draw()
{
    /* return early if we are scrolled past the fuel station */
    if (World::scroll_y > 192) return;

    SDL::render_texture(fuel_station_id, 260, 66 - World::scroll_y);
}

bool FuelStation::in_use()
{
    return interface_open;
}

void FuelStation::update()
{
    /* if the fuel interface is open, we don't need to do anything here */
    if (interface_open) return;

    if ((int) Digger::x == 270 &&
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

    SDL::render_texture(fuel_status_message_id, 60, 60);
}

void FuelStation::click(int x, int y)
{
    /* the caller of this function has already checked
     * that the fuel interface is open
     */
    if (x > X_OFF + 30 && x < X_OFF + 160 && y > Y_OFF + 90 && y < Y_OFF + 220) 
        attempt_purchase(5);

    if ((x - (496 + X_OFF)) * (x - (496 + X_OFF)) +
        (y - (40  + Y_OFF)) * (y - (40  + Y_OFF)) <= 18 * 18) {
            Digger::enable();
            interface_open = false;
            /* move the digger slightly so that the fuel interface
             * doesn't immediately re-open
             */
             Digger::x += 1;
        }
}
