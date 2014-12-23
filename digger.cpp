#include <cmath>

#include "digger.hh"
#include "sdl_wrapper.hh"
#include "world.hh"
#include "block.hh"

#define UP_PRESSED (key_states[SDL_SCANCODE_UP])
#define DOWN_PRESSED (key_states[SDL_SCANCODE_DOWN])
#define RIGHT_PRESSED (key_states[SDL_SCANCODE_RIGHT])
#define LEFT_PRESSED (key_states[SDL_SCANCODE_LEFT])


/************************/
/* game variables       */
const float Digger::MAX_HULL = 100.0;
const float Digger::MAX_FUEL = 10.0;

float Digger::fuel  = 10.0;
float Digger::hull  = 20.0;
int   Digger::money = 20;
bool  Digger::alive = true;
/************************/


/********************************/
/* digger acceleration (static) */
static float ax;
static float ay;
/********************************/


/********************************/
/* Digger position and velocity */
float Digger::vx;
float Digger::vy;
float Digger::x;
float Digger::y;
/********************************/


static constexpr float MAX_SPEED = 5.0;
static constexpr float ACCELERATION = 0.3;
static constexpr float ACCELERATION_DUE_TO_GRAVITY = -0.3;

/*********************************************/
/* Declarations for various update functions */
static void default_update();
static void drilling_left_update();
static void drilling_down_update();
static void drilling_right_update();
static void explode_update();
static void (*current_update)() = default_update; /* the current state (gets called on every frame) */
/*********************************************/

static size_t texture_id;
static size_t propeller_ids[3];
static size_t drill_ids[2];
static size_t explosion_ids[10];
static size_t current_prop_id;
static size_t current_drill_id;
static size_t current_explosion_id;
static int drill_x_off;
static int drill_y_off;
static size_t drill_sound_id;
static size_t propeller_sound_id;
static size_t explosion_sound_id;
static bool show_propeller;
static bool show_drill;
static size_t frames_since_last_rotate;
static double drill_angle;
static bool propeller_sound_playing;
static bool exploding;
static bool enabled = true;


/* static helper functions */
static void game_over()
{
    current_update = explode_update;
    exploding = true;
    SDL::play_sound(explosion_sound_id);
}

static void normalize_velocity()
{
    if (std::abs(Digger::vx) > MAX_SPEED)
        Digger::vx = copysignf(MAX_SPEED, Digger::vx);

    if (std::abs(Digger::vy) > MAX_SPEED)
        Digger::vy = copysignf(MAX_SPEED, Digger::vy);
}

static void rotate_drill_and_propeller()
{
    /* Rotate the propeller and drill */
    if (++frames_since_last_rotate % 3 == 0) {
        current_prop_id = (current_prop_id + 1) % 3;
        current_drill_id = (current_drill_id + 1) % 2;
    }
    frames_since_last_rotate %= 3;
}

static void clip()
{
    bool should_clip{};
    size_t bottom_pos = Digger::bottom() / 64;
    size_t top_pos    = Digger::top()    / 64;
    size_t left_pos   = Digger::left()   / 64;
    size_t right_pos  = Digger::right()  / 64;

    /* LEFT */
    should_clip |= !World::blocks[top_pos][left_pos].drilled() && (bottom_pos*64 - Digger::top() > 12);
    should_clip |= !World::blocks[bottom_pos][left_pos].drilled() && (Digger::bottom() - bottom_pos*64 > 12);
    if (should_clip) {
        Digger::x = 64 * right_pos;
        Digger::vx = 0;
    }

    /* RIGHT */
    should_clip = false;
    should_clip |= !World::blocks[top_pos][right_pos].drilled() && (bottom_pos*64 - Digger::top() > 8);
    should_clip |= !World::blocks[bottom_pos][right_pos].drilled() && (Digger::bottom() - bottom_pos*64 > 12);
    if (should_clip) {
        Digger::x = 64 * left_pos;
        Digger::vx = 0;
    }

    /* BOTTOM */
    should_clip = false;
    should_clip |= !World::blocks[bottom_pos][left_pos].drilled() && (right_pos*64 - Digger::left() > 12);
    should_clip |= !World::blocks[bottom_pos][right_pos].drilled() && (Digger::right() - right_pos*64 > 12);
    if (should_clip) {
        Digger::y = 64 * top_pos;
        Digger::vy = 0;
        Digger::vx *= 0.9; if (std::abs(Digger::vx) < 0.1) Digger::vx = 0;
    }

    /* TOP */
    should_clip = false;
    should_clip |= !World::blocks[top_pos][left_pos].drilled() && (right_pos*64 - Digger::left() > 12);
    should_clip |= !World::blocks[top_pos][right_pos].drilled() && (Digger::right() - right_pos*64 > 12);
    if (should_clip) {
        Digger::y = 64 * bottom_pos;
        Digger::vy = 0;
    }

    /* keep the digger inside the map */
    if (Digger::x < 0)
        Digger::x = 0;
    else if (Digger::x + 64 > SDL::WINDOW_WIDTH)
        Digger::x = SDL::WINDOW_WIDTH - 64;
    if (Digger::y < 0) {
        Digger::y = 0;
        Digger::vy = 0;
    }

}

/*============================================================================*/
/***************** Digger update functions ************************************/
static void default_update()
{
    rotate_drill_and_propeller();

    const Uint8 *key_states = SDL_GetKeyboardState(NULL);
    if (key_states[SDL_SCANCODE_UP]) {
        ay = ACCELERATION;
        show_propeller = true;
    } else {
        ay = ACCELERATION_DUE_TO_GRAVITY;
        show_propeller = false;
    }

    if (key_states[SDL_SCANCODE_RIGHT]) {
        ax = ACCELERATION;
    } else if (key_states[SDL_SCANCODE_LEFT]) {
        ax = -ACCELERATION;
    } else ax = 0.0;

    Digger::vx += ax;
    Digger::vy += ay;

    normalize_velocity();

    Digger::x += Digger::vx;
    Digger::y -= Digger::vy;

    clip();

    /* if engine is going, remove some fuel */
    if (ay > 0 || ax) Digger::fuel -= 0.01;
}

static void drilling_right_update()
{
    static int distance_left = 64;

    Digger::x += 1;
    --distance_left;

    rotate_drill_and_propeller();

    if (distance_left == 0) {
        distance_left = 64;
        World::blocks[(Digger::top() + 32) / 64][Digger::right() / 64 - 1].drill();
        current_update = default_update;
        show_drill = false;
    }
}

static void drilling_left_update()
{
    static int distance_left = 64;

    Digger::x -= 1;
    --distance_left;

    rotate_drill_and_propeller();

    if (distance_left == 0) {
        distance_left = 64;
        World::blocks[(Digger::top() + 32) / 64][Digger::left() / 64].drill();
        current_update = default_update;
        show_drill = false;
    }
    
}

static void drilling_down_update()
{
    static int distance_left = 64;

    Digger::y += 1;
    --distance_left;

    rotate_drill_and_propeller();

    if (distance_left == 0) {
        distance_left = 64;
        World::blocks[(Digger::top() + 33) / 64][(Digger::left() + 32) / 64].drill();
        current_update = default_update;
        show_drill = false;
    }
    
}

static void explode_update()
{
    static int frames;
    if (++frames % 3 == 0 && frames) ++current_explosion_id;

    /* check to see if we are done the explosion animation */
    if (current_explosion_id == 10) {
        Digger::alive = false;
    }
    
}

static void idle_update() {}
/*============================================================================*/
/***************** End update functions ***************************************/

static void drill_down_prepare()
{
    Digger::fuel -= 0.6;
    SDL::play_sound(drill_sound_id);
    current_update = drilling_down_update;
    show_drill = true;
    drill_angle = 0.0;
    drill_x_off = 0;
    drill_y_off = 0;
    Digger::vx = 0;
}

static void drill_right_prepare()
{
    Digger::fuel -= 0.6;
    SDL::play_sound(drill_sound_id);
    current_update = drilling_right_update;
    show_drill = true;
    drill_angle = 270.0;
    drill_x_off = 60;
    drill_y_off = -60;
}

static void drill_left_prepare()
{
    Digger::fuel -= 0.6;
    SDL::play_sound(drill_sound_id);
    current_update = drilling_left_update;
    show_drill = true;
    drill_angle = 90.0;
    drill_x_off = -60;
    drill_y_off = -60;
}


/***************************/
/* Digger public functions */
void Digger::load()
{
    texture_id       = SDL::load_texture("./assets/digger.png");
    propeller_ids[0] = SDL::load_texture("./assets/prop1.png");
    propeller_ids[1] = SDL::load_texture("./assets/prop2.png");
    propeller_ids[2] = SDL::load_texture("./assets/prop3.png");
    drill_ids[0]     = SDL::load_texture("./assets/drill1.png");
    drill_ids[1]     = SDL::load_texture("./assets/drill2.png");
    explosion_ids[0] = SDL::load_texture("./assets/explosion1.png");
    explosion_ids[1] = SDL::load_texture("./assets/explosion2.png");
    explosion_ids[2] = SDL::load_texture("./assets/explosion3.png");
    explosion_ids[3] = SDL::load_texture("./assets/explosion4.png");
    explosion_ids[4] = SDL::load_texture("./assets/explosion5.png");
    explosion_ids[5] = SDL::load_texture("./assets/explosion6.png");
    explosion_ids[6] = SDL::load_texture("./assets/explosion7.png");
    explosion_ids[7] = SDL::load_texture("./assets/explosion8.png");
    explosion_ids[8] = SDL::load_texture("./assets/explosion9.png");
    explosion_ids[9] = SDL::load_texture("./assets/explosion10.png");

    
    /* Load sound effects for drill, propeller, and explosion */
    drill_sound_id = SDL::load_sound("./assets/drill.wav");
    propeller_sound_id = SDL::load_sound("./assets/disco.wav");
    explosion_sound_id = SDL::load_sound("./assets/explosion.wav");
}

void Digger::draw()
{
    Digger::x += 7;

    /* don't draw anything if the game is over */
    if (!Digger::alive) return;

    SDL::render_texture(texture_id, (int) Digger::x, (int) Digger::y - World::scroll_y);

    if (show_propeller)
        SDL::render_texture(propeller_ids[current_prop_id], (int) Digger::x,
                            (int) Digger::y - 54 - World::scroll_y);
    else if (show_drill)
        SDL::render_texture(drill_ids[current_drill_id], (int) Digger::x + drill_x_off,
                            (int) Digger::y + 64 + drill_y_off - World::scroll_y, drill_angle);
    if (exploding)
        SDL::render_texture(explosion_ids[current_explosion_id], (int) Digger::x - 64, (int) Digger::y - World::scroll_y - 64);

    Digger::x -= 7;
}

void Digger::handle_key_down(SDL_Keycode k)
{
    if (show_drill) return;

    const Uint8 *key_states = SDL_GetKeyboardState(NULL);
    switch (k) {
    case SDLK_DOWN:
        if (!(World::blocks[Digger::bottom() / 64][(Digger::left() + 32) / 64].drilled()) &&
             (World::blocks[Digger::bottom() / 64][(Digger::left() + 32) / 64].drillable()) &&
             !LEFT_PRESSED && !RIGHT_PRESSED && std::abs(Digger::vx) < 0.3)
             drill_down_prepare();
        break;
    case SDLK_RIGHT:
        if (!(World::blocks[(Digger::top() + 32) / 64][Digger::right() / 64].drilled()) &&
             (World::blocks[(Digger::top() + 32) / 64][Digger::right() / 64].drillable()) &&
             !UP_PRESSED && !DOWN_PRESSED && Digger::vy == 0)
             drill_right_prepare();
        break;
    case SDLK_LEFT:
        if (!(World::blocks[(Digger::top() + 32) / 64][(Digger::left() - 1) / 64].drilled()) &&
             (World::blocks[(Digger::top() + 32) / 64][(Digger::left() - 1) / 64].drillable()) &&
             !UP_PRESSED && !DOWN_PRESSED && Digger::vy == 0)
            drill_left_prepare();
        break;
    case SDLK_UP:
        if (!propeller_sound_playing) {
            SDL::play_sound_loop(propeller_sound_id);
            propeller_sound_playing = true;
        }
        break;
    }
}

void Digger::handle_key_up(SDL_Keycode k)
{
    if (k == SDLK_UP) {
        SDL::stop_loop(propeller_sound_id);
        propeller_sound_playing = false;
    } else if (k == SDLK_e) {
        game_over();
    } else if (k == SDLK_j) {
        Digger::money -= 2;
    } else if (k == SDLK_k) {
        Digger::money += 2;
    }
        
}

/* Helper functions for getting the position of various edges of the Digger's model */
int Digger::bottom() { return (int) Digger::y + 64; }
int Digger::top() { return (int) Digger::y; }
int Digger::left() { return (int) Digger::x; }
int Digger::right() { return (int) Digger::x + 64; }

void Digger::update()
{
    /* call the current update function (state machine) */
    current_update();
    
    /******************/
    /* game logic     */

    if (enabled) Digger::fuel -= 0.0007;
    if (Digger::fuel < 0 && !exploding)
        game_over();
    /******************/
}

/* These functions are called by buildings (repair shop, fuel station, etc.)
 * to disable the Digger while their interfaces are in use, and then
 * re-enable it when the interface is closed
 */
void Digger::enable()
{
    enabled = true;
    current_update = default_update;
}

void Digger::disable()
{
    enabled = false;
    current_update = idle_update;
}
