#include <stdbool.h>
#include <stdint.h>

#include "lcd.h"
#include "missile.h"

enum {
    INIT_ST
    MOVING_ST
    EXPLODE_GROW_ST
    EXPLODE_SHRINK_ST
    IMPACT_ST
    IDLE_ST
}

/******************** Missile Init Functions ********************/

// Different _init_ functions are used depending on the missile type.

// Initialize the missile as an idle missile. If initialized to the idle
// state, a missile doesn't appear nor does it move.
void missile_init_idle(missile_t *missile);

// Initialize the missile as a player missile. This function takes an (x, y)
// destination of the missile (as specified by the user). The origin is the
// closest "firing location" to the destination (there are three firing
// locations evenly spaced along the bottom of the screen).
void missile_init_player(missile_t *missile, coord_t x_dest, coord_t y_dest);

// Initialize the missile as an enemy missile. This will randomly choose the
// origin and destination of the missile. The origin is somewhere near the
// top of the screen, and the destination is the very bottom of the screen.
void missile_init_enemy(missile_t *missile);

// Initialize the missile as a plane missile. This function takes the (x, y)
// location of the plane as an argument and uses it as the missile origin.
// The destination is randomly chosen along the bottom of the screen.
void missile_init_plane(missile_t *missile, coord_t x_orig, coord_t y_orig);

/******************** Missile Control & Tick Functions ********************/

// Used to indicate that a moving missile should be detonated. This occurs
// when an enemy or a plane missile is located within an explosion zone.
void missile_explode(missile_t *missile);

// Tick the state machine for a single missile.
void missile_tick(missile_t *missile);

/******************** Missile Status Functions ********************/

// Return the current missile position through the pointers *x,*y.
void missile_get_pos(missile_t *missile, coord_t *x, coord_t *y) {
    *x = missile->x_current;
    *y = missile->y_current;
}

// Return the missile type.
missile_type_t missile_get_type(missile_t *missile) {
    return missile->type;
}

// Return whether the given missile is moving.
bool missile_is_moving(missile_t *missile);

// Return whether the given missile is exploding. If this missile
// is exploding, it can explode another intersecting missile.
bool missile_is_exploding(missile_t *missile) {
    return missile->state == EXPLODE_GROW_ST || missile->state == EXPLODE_SHRINK_ST
}

// Return whether the given missile is idle.
bool missile_is_idle(missile_t *missile);

// Return whether the given missile is impacted.
bool missile_is_impacted(missile_t *missile) {
    return missile->y >= LCD_H;
}

// Return whether an object (e.g., missile or plane) at the specified
// (x,y) position is colliding with the given missile. For a collision
// to occur, the missile needs to be exploding and the specified
// position needs to be within the explosion radius.
bool missile_is_colliding(missile_t *missile, coord_t x, coord_t y);
