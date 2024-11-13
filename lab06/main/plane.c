#include "plane.h";

#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#define PLANE_HEIGHT 2

float plane_curr_x, plane_curr_y;
coord_t plane_fire_x;



bool plane_explode_me;

bool plane_missile_fired;

missile_t *missile;

enum {
    INIT_ST,
    MOVING_ST,
    EXPLODE_ST,
    FIRE_ST,
    IDLE_ST,
} plane_curr_state;

//Draw plane at current location
void draw_plane(void);

/******************** Plane Init Function ********************/

// Initialize the plane state machine. Pass a pointer to the missile
// that will be (re)launched by the plane. It will only have one missile.
void plane_init(missile_t *plane_missile) {
    plane_curr_state = INIT_ST;
    plane_curr_x = LCD_W;
    plane_curr_y = rand() % LCD_H / PLANE_HEIGHT;
    plane_fire_x = rand() % LCD_W;
    plane_explode_me = false;
    plane_missile_fired = false;
    missile = plane_missile;
}

/******************** Plane Control & Tick Functions ********************/

// Trigger the plane to explode.
void plane_explode(void) {
    plane_explode_me = true;
}

// State machine tick function.
void plane_tick(void) {

    switch (plane_curr_state) { // State Transition Logic
        case INIT_ST:
            plane_curr_state = MOVING_ST;
            printf("Missile Starting\n");
            break;
        case MOVING_ST:
            if (plane_explode_me) {
                plane_curr_state = EXPLODE_ST;
            } else if (plane_curr_x < plane_fire_x && !plane_missile_fired) {
                plane_curr_state = FIRE_ST;
            } else if (plane_curr_x < -CONFIG_PLANE_WIDTH) {
                plane_curr_state = IDLE_ST;
                printf("Missile Done\n");
            } else {
                plane_curr_state = MOVING_ST;
            }
            break;
        case FIRE_ST:
            plane_curr_state = MOVING_ST;
            break;
        case EXPLODE_ST:
            plane_curr_state = IDLE_ST;
            printf("Missile Done\n");
            break;
        case IDLE_ST:
            plane_curr_state = IDLE_ST;
            break;
    }

    switch (plane_curr_state) { // State Actions
        case INIT_ST:
            break;
        case MOVING_ST:
            plane_curr_x -= CONFIG_PLANE_DISTANCE_PER_TICK;
            draw_plane();
            break;
        case FIRE_ST:
            missile_init_plane(missile, plane_curr_x, plane_curr_y);
            plane_missile_fired = true;
            plane_curr_x -= CONFIG_PLANE_DISTANCE_PER_TICK;
            draw_plane();
            break;
        case EXPLODE_ST:
            break;
        case IDLE_ST:
            break;
    }
}

/******************** Plane Status Function ********************/

// Return the current plane position through the pointers *x,*y.
void plane_get_pos(coord_t *x, coord_t *y) {
    *x = (coord_t) plane_curr_x;
    *y = (coord_t) plane_curr_y;
}

// Return whether the plane is flying.
bool plane_is_flying(void) {
    return plane_curr_state != IDLE_ST;
}

/************************ Internal Functions *****************/

//Draw plane at current location
void draw_plane(void) {
    lcd_fillTriangle(plane_curr_x + CONFIG_PLANE_WIDTH, plane_curr_y - CONFIG_PLANE_HEIGHT / 2,
        plane_curr_x + CONFIG_PLANE_WIDTH,plane_curr_y + CONFIG_PLANE_HEIGHT / 2,
        plane_curr_x, plane_curr_y,
        CONFIG_COLOR_PLANE);
}
