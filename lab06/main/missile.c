#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "missile.h"

#include <config.h>
#include <stdio.h>

#define EXPONENT_SQUARED 2
#define ENEMY_MISSILE_ORIG_HEIGHT_FACTOR 5
#define AMT_FIRE_LOCATIONS 3

enum {
    INIT_ST,
    MOVING_ST,
    EXPLODE_GROW_ST,
    EXPLODE_SHRINK_ST,
    IMPACT_ST,
    IDLE_ST,
};

/******************** Missile Init Functions ********************/

// Different _init_ functions are used depending on the missile type.

// Initialize the missile as an idle missile. If initialized to the idle
// state, a missile doesn't appear nor does it move.
void missile_init_idle(missile_t *missile) {
    missile->currentState = IDLE_ST;
    missile->explode_me = false;
}

// Initialize the missile as a player missile. This function takes an (x, y)
// destination of the missile (as specified by the user). The origin is the
// closest "firing location" to the destination (there are three firing
// locations evenly spaced along the bottom of the screen).
void missile_init_player(missile_t *missile, coord_t x_dest, coord_t y_dest) {
    coord_t x_orig;
    coord_t y_orig;
    get_closest_firing(x_dest, y_dest, &x_orig, &y_orig);

    missile_init_idle(missile);
    init_positioning(missile, x_dest, y_dest, x_orig, y_orig);

    missile->type = MISSILE_TYPE_PLAYER;

}

// Initialize the missile as an enemy missile. This will randomly choose the
// origin and destination of the missile. The origin is somewhere near the
// top of the screen, and the destination is the very bottom of the screen.
void missile_init_enemy(missile_t *missile) {
    coord_t x_dest;
    coord_t y_dest;
    coord_t x_orig;
    coord_t y_orig;
    get_random_position(&x_dest, &y_dest, &x_orig, &y_orig);

    missile_init_idle(missile);
    init_positioning(missile, x_dest, y_dest, x_orig, y_orig);

    missile->type = MISSILE_TYPE_ENEMY;

}

// Initialize the missile as a plane missile. This function takes the (x, y)
// location of the plane as an argument and uses it as the missile origin.
// The destination is randomly chosen along the bottom of the screen.
void missile_init_plane(missile_t *missile, coord_t x_orig, coord_t y_orig) {
    coord_t x_dest;
    coord_t y_dest;
    coord_t x_temp;
    coord_t y_temp;
    get_random_position(&x_dest, &y_dest, &x_temp, &y_temp);

    missile_init_idle(missile);
    init_positioning(missile, x_dest, y_dest, x_orig, y_orig);

    missile->type = MISSILE_TYPE_PLANE;

}

/******************** Missile Control & Tick Functions ********************/

// Used to indicate that a moving missile should be detonated. This occurs
// when an enemy or a plane missile is located within an explosion zone.
void missile_explode(missile_t *missile) {
    missile->explode_me = true;
}

// Used to move the given missile
void move_missile(missile_t *missile) {
    uint16_t missile_distance_per_tick;
    switch (missile->type) { // Choose correct missile speed
        case MISSILE_TYPE_PLAYER:
            missile_distance_per_tick = CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK;
            break;
        case MISSILE_TYPE_ENEMY:
            missile_distance_per_tick = CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
            break;
        case MISSILE_TYPE_PLANE:
            missile_distance_per_tick = CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
            break;
        default:
            missile_distance_per_tick = 0;
            break;
    }

    missile->length += missile_distance_per_tick;
    double fraction = missile->length / missile->total_length;

    missile->x_current = missile->x_origin + fraction * (missile->x_dest - missile->x_origin);
    missile->y_current = missile->y_origin + fraction * (missile->y_dest - missile->y_origin);
}

void get_missile_color(missile_t *missile, color_t *path_color) {
    switch (missile->type) { // Choose correct path color
        case MISSILE_TYPE_PLAYER:
            *path_color = CONFIG_COLOR_PLAYER_MISSILE;
            break;
        case MISSILE_TYPE_ENEMY:
            *path_color = CONFIG_COLOR_ENEMY_MISSILE;
            break;
        case MISSILE_TYPE_PLANE:
            *path_color = CONFIG_COLOR_PLANE_MISSILE;
            break;
        default:
            *path_color = BLACK;
            break;
    }
}

// Used to draw the given missile
void draw_missile_path(missile_t *missile) {
    color_t path_color;
    get_missile_color(missile, &path_color);

    lcd_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, path_color);
}

void expand_explode(missile_t *missile) {
    missile->radius += CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;

    color_t explode_color;
    get_missile_color(missile, &explode_color);
    lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, explode_color);
}

void shrink_explode(missile_t *missile) {
    missile->radius -= CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;

    color_t explode_color;
    get_missile_color(missile, &explode_color);
    lcd_fillCircle(missile->x_current, missile->y_current, missile->radius + CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK, CONFIG_COLOR_BACKGROUND);
    lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, explode_color);
}

// Tick the state machine for a single missile.
void missile_tick(missile_t *missile) {
    switch (missile->currentState) { // State transitions
        case INIT_ST:
            missile->currentState = MOVING_ST;
        break;
        case IDLE_ST:
            break;
        case MOVING_ST:
            if (missile->explode_me) {
                missile->currentState = EXPLODE_GROW_ST;
            } else if (missile_is_impacted(missile)) {
                missile->currentState = IMPACT_ST;
            } else {
                missile->currentState = MOVING_ST;
            }
        break;
        case EXPLODE_GROW_ST:
            if (missile->radius < CONFIG_EXPLOSION_MAX_RADIUS) {
                missile->currentState = EXPLODE_GROW_ST;
            } else {
                missile->currentState = EXPLODE_SHRINK_ST;
            }
        break;
        case EXPLODE_SHRINK_ST:
            if (missile->radius > 0) {
                missile->currentState = EXPLODE_SHRINK_ST;
            } else {
                missile->currentState = IDLE_ST;
            }
        break;
        case IMPACT_ST:
            missile->currentState = IDLE_ST;
            break;
        default:
    }

    switch (missile->currentState) { // State actions
        case INIT_ST:
            break;
        case IDLE_ST:
            break;
        case MOVING_ST:
            move_missile(missile);
            draw_missile_path(missile);
            break;
        case EXPLODE_GROW_ST:
            expand_explode(missile);
            break;
        case EXPLODE_SHRINK_ST:
            shrink_explode(missile);
            break;
        case IMPACT_ST:
            break;
        default:
    }
}

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
bool missile_is_moving(missile_t *missile) {
    return missile->currentState == MOVING_ST;
}

// Return whether the given missile is exploding. If this missile
// is exploding, it can explode another intersecting missile.
bool missile_is_exploding(missile_t *missile) {
    return missile->currentState == EXPLODE_GROW_ST || missile->currentState == EXPLODE_SHRINK_ST;
}

// Return whether the given missile is idle.
bool missile_is_idle(missile_t *missile) {
    return missile->currentState == IDLE_ST;
}

// Return whether the given missile is impacted.
bool missile_is_impacted(missile_t *missile) {
    return missile->y_current >= LCD_H && missile->type != MISSILE_TYPE_PLAYER;
}

// Return whether an object (e.g., missile or plane) at the specified
// (x,y) position is colliding with the given missile. For a collision
// to occur, the missile needs to be exploding and the specified
// position needs to be within the explosion radius.
bool missile_is_colliding(missile_t *missile, coord_t x, coord_t y) {
    bool is_colliding = false;
    //printf("current state%ld\n", missile->currentState);
    if (missile_is_exploding(missile)) {
        is_colliding = pow(missile->x_current - x, EXPONENT_SQUARED) + pow(missile->y_current - y, EXPONENT_SQUARED) < pow(missile->radius, EXPONENT_SQUARED);
    }
    return is_colliding;
}

// Find the closest firing station to any given missile destination
void get_closest_firing(coord_t  x_dest, coord_t  y_dest, coord_t *x_orig, coord_t *y_orig) {
    coord_t x_temp = LCD_W / (AMT_FIRE_LOCATIONS + 1);
    for (uint8_t i = 1; i < AMT_FIRE_LOCATIONS; i++) {
        if (x_dest > i * LCD_W / AMT_FIRE_LOCATIONS) {
            x_temp = (i + 1) * LCD_W / (AMT_FIRE_LOCATIONS + 1);
        } else {
            break;
        }
    }

    *x_orig = x_temp;
    *y_orig = LCD_H;
}

// Get random start point and end point for enemy missile
void get_random_position(coord_t *x_dest, coord_t *y_dest, coord_t *x_orig, coord_t *y_orig) {
    *x_dest = rand() % LCD_W;
    *y_dest = LCD_H;

    *x_orig = rand() % LCD_W;
    *y_orig = rand() % (LCD_H / ENEMY_MISSILE_ORIG_HEIGHT_FACTOR);
}

// Initialize the position parameters in the given missile
void init_positioning(missile_t *missile, coord_t x_dest, coord_t y_dest, coord_t x_orig, coord_t y_orig) {
    missile->x_origin = x_orig;
    missile->y_origin = y_orig;
    missile->x_dest = x_dest;
    missile->y_dest = y_dest;
    missile->x_current = x_orig;
    missile->y_current = y_orig;
    missile->total_length = sqrt(pow(x_dest - x_orig, EXPONENT_SQUARED) + pow(y_dest - y_orig, EXPONENT_SQUARED));
    missile->length = 0;
    missile->currentState = INIT_ST;
}
