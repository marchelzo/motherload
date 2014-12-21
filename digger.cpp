#include <cmath>

#include "digger.hh"
#include "sdl_wrapper.hh"
#include "world.hh"
#include "block.hh"

#define UP_PRESSED (key_states[SDL_SCANCODE_UP])
#define DOWN_PRESSED (key_states[SDL_SCANCODE_DOWN])
#define RIGHT_PRESSED (key_states[SDL_SCANCODE_RIGHT])
#define LEFT_PRESSED (key_states[SDL_SCANCODE_LEFT])

static float ax;
static float ay;
static float vx;
static float vy;

static constexpr float MAX_SPEED = 3.0;
static constexpr float ACCELERATION = 0.3;

static size_t texture_id;
static size_t propeller_ids[3];
static size_t drill_ids[2];
static size_t current_prop_id;
static size_t current_drill_id;
static int drill_x_off;
static int drill_y_off;
static size_t drill_sound_id;
static size_t propeller_sound_id;
static bool show_propeller;
static bool show_drill;
static size_t frames_since_last_rotate;
static double drill_angle;
static bool propeller_sound_playing;

float Digger::x;
float Digger::y;

void (*Digger::update)();


/* static helper functions */
static void normalize_velocity()
{
    if (std::abs(vx) > MAX_SPEED)
        vx = copysignf(MAX_SPEED, vx);

    if (std::abs(vy) > MAX_SPEED)
        vy = copysignf(MAX_SPEED, vy);
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
        vx = 0;
    }

    /* RIGHT */
    should_clip = false;
    should_clip |= !World::blocks[top_pos][right_pos].drilled() && (bottom_pos*64 - Digger::top() > 8);
    should_clip |= !World::blocks[bottom_pos][right_pos].drilled() && (Digger::bottom() - bottom_pos*64 > 12);
    if (should_clip) {
        Digger::x = 64 * left_pos;
        vx = 0;
    }

    /* BOTTOM */
    should_clip = false;
    should_clip |= !World::blocks[bottom_pos][left_pos].drilled() && (right_pos*64 - Digger::left() > 12);
    should_clip |= !World::blocks[bottom_pos][right_pos].drilled() && (Digger::right() - right_pos*64 > 12);
    if (should_clip) {
        Digger::y = 64 * top_pos;
        vy = 0;
        vx *= 0.9; if (std::abs(vx) < 0.1) vx = 0;
    }

    /* TOP */
    should_clip = false;
    should_clip |= !World::blocks[top_pos][left_pos].drilled() && (right_pos*64 - Digger::left() > 12);
    should_clip |= !World::blocks[top_pos][right_pos].drilled() && (Digger::right() - right_pos*64 > 12);
    if (should_clip) {
        Digger::y = 64 * bottom_pos;
        vy = 0;
    }
}

static void default_update()
{
    rotate_drill_and_propeller();

    const Uint8 *key_states = SDL_GetKeyboardState(NULL);
    if (key_states[SDL_SCANCODE_UP]) {
        ay = ACCELERATION;
        show_propeller = true;
    } else if (key_states[SDL_SCANCODE_DOWN]) {
        ay = -ACCELERATION;
        show_propeller = false;
    } else {
        ay = -ACCELERATION;
        show_propeller = false;
    }

    if (key_states[SDL_SCANCODE_RIGHT]) {
        ax = ACCELERATION;
    } else if (key_states[SDL_SCANCODE_LEFT]) {
        ax = -ACCELERATION;
    } else ax = 0.0;

    vx += ax;
    vy += ay;

    normalize_velocity();

    Digger::x += vx;
    Digger::y -= vy;

    clip();
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
        Digger::update = default_update;
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
        Digger::update = default_update;
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
        Digger::update = default_update;
        show_drill = false;
    }
    
}

static void drill_down_prepare()
{
    SDL::play_sound(drill_sound_id);
    Digger::update = drilling_down_update;
    show_drill = true;
    drill_angle = 0.0;
    drill_x_off = 0;
    drill_y_off = 0;
    vx = 0;
}

static void drill_right_prepare()
{
    SDL::play_sound(drill_sound_id);
    Digger::update = drilling_right_update;
    show_drill = true;
    drill_angle = 270.0;
    drill_x_off = 60;
    drill_y_off = -60;
}

static void drill_left_prepare()
{
    SDL::play_sound(drill_sound_id);
    Digger::update = drilling_left_update;
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
    
    /* Load sound effects for drill and propeller */
    drill_sound_id = SDL::load_sound("./assets/drill.wav");
    propeller_sound_id = SDL::load_sound("./assets/disco.wav");

    /* set the initial update function to default_update */
    Digger::update = default_update;
}

void Digger::draw()
{
    SDL::render_texture(texture_id, (int) Digger::x, (int) Digger::y - World::scroll_y);

    if (show_propeller)
        SDL::render_texture(propeller_ids[current_prop_id], (int) Digger::x,
                            (int) Digger::y - 54 - World::scroll_y);
    else if (show_drill)
        SDL::render_texture(drill_ids[current_drill_id], (int) Digger::x + drill_x_off,
                            (int) Digger::y + 64 + drill_y_off - World::scroll_y, drill_angle);
}

void Digger::handle_key_down(SDL_Keycode k)
{
    if (show_drill) return;

    const Uint8 *key_states = SDL_GetKeyboardState(NULL);
    switch (k) {
    case SDLK_DOWN:
        if (!(World::blocks[Digger::bottom() / 64][(Digger::left() + 32) / 64].drilled()) &&
             !LEFT_PRESSED && !RIGHT_PRESSED && std::abs(vx) < 0.3)
             drill_down_prepare();
        break;
    case SDLK_RIGHT:
        if (!(World::blocks[(Digger::top() + 32) / 64][Digger::right() / 64].drilled()) &&
             !UP_PRESSED && !DOWN_PRESSED && vy == 0)
             drill_right_prepare();
        break;
    case SDLK_LEFT:
        if (!(World::blocks[(Digger::top() + 32) / 64][(Digger::left() - 1) / 64].drilled()) &&
             !UP_PRESSED && !DOWN_PRESSED && vy == 0)
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
    }
}

int Digger::bottom()
{
    return (int) Digger::y + 64;
}

int Digger::top()
{
    return (int) Digger::y;
}

int Digger::left()
{
    return (int) Digger::x;
}

int Digger::right()
{
    return (int) Digger::x + 64;
}
