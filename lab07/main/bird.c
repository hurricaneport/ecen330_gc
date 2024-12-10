//
// Created by dansi on 12/9/2024.
//


#include "bird.h"
#include "config.h"

enum
{
    INIT_ST,
    FLYING_ST,
    JUMP_ST,
    DEATH_FALL_ST,
    DEATH_IDLE_ST,
};

//Initialize bird logic before game
void bird_init(bird_type_t *bird)
{
    bird->current_state = INIT_ST;
    bird->current_x = CONFIG_BIRD_X_LOCATION;
    bird->current_y = CONFIG_BIRD_Y_START_LOCATION;
    bird->current_velocity = 0;
    bird->hit_me = false;
    bird->jump_me = false;
}

void tick_bird(bird_type_t *bird)
{
    //Bird Transition Logic
    switch (bird->current_state)
    {
        case INIT_ST:
            bird->current_state = FLYING_ST;
            break;
        case FLYING_ST:
            if (bird->hit_me)
            {
                bird->current_state = DEATH_FALL_ST;
                bird->hit_me = false;
            } else if (bird->jump_me)
            {
                bird->current_state = JUMP_ST;
                bird->jump_me = false;
            } else
            {
                bird->current_state = FLYING_ST;
            }
            break;
        case JUMP_ST:
            if (bird->hit_me)
            {
                bird->current_state = DEATH_FALL_ST;
                bird->hit_me = false;
            } else
            {
                bird->current_state = FLYING_ST;
            }
            break;
        case DEATH_FALL_ST:
            if ((coord_t) bird->current_y > LCD_H - CONFIG_BIRD_H)
            {
                bird->current_state = DEATH_IDLE_ST;
            } else
            {
                bird->current_state = DEATH_FALL_ST;
            }
            break;
        case DEATH_IDLE_ST:
            bird->current_state = DEATH_IDLE_ST;
            break;
        default:
            bird->current_state = DEATH_IDLE_ST;
    }

    //Bird State Actions
    switch (bird->current_state)
    {
        case INIT_ST:
            break;
        case FLYING_ST:
            if ((coord_t) bird->current_y < 0 + CONFIG_BIRD_H || (coord_t) bird->current_y > LCD_H - CONFIG_BIRD_H)
            {
                bird->hit_me = true;
            }
            else
            {
                if (bird->current_velocity < CONFIG_BIRD_MAX_VELOCITY_TICKS)
                {
                    bird->current_velocity += CONFIG_BIRD_ACCELERATION_TICKS;
                } else
                {
                    bird->current_velocity = CONFIG_BIRD_MAX_VELOCITY_TICKS;
                }

                bird->current_y += bird->current_velocity;
            }
            break;
        case JUMP_ST:
            if ((coord_t) bird->current_y < 0 + CONFIG_BIRD_H || (coord_t) bird->current_y > LCD_H - CONFIG_BIRD_H)
            {
                bird->hit_me = true;
            } else
            {
                bird->current_velocity = CONFIG_BIRD_JUMP_VELOCITY_TICKS;
                bird->current_y += bird->current_velocity;
            }
            break;
        case DEATH_FALL_ST:
            if (bird->current_velocity < CONFIG_BIRD_MAX_VELOCITY_TICKS)
            {
                bird->current_velocity += CONFIG_BIRD_ACCELERATION_TICKS;
            } else
            {
                bird->current_velocity = CONFIG_BIRD_MAX_VELOCITY_TICKS;
            }

            bird->current_y += bird->current_velocity;
            break;
        case DEATH_IDLE_ST:
            bird->current_velocity = 0;
            break;
        default:
            break;

    }
}

//Draw bird at its current location with correct color
void draw_bird(bird_type_t bird)
{
    const color_t bird_color = (bird.current_state == DEATH_FALL_ST || bird.current_state == DEATH_IDLE_ST) ? CONFIG_COLOR_BIRD_DEATH : CONFIG_COLOR_BIRD;
    lcd_fillCircle(bird.current_x, (coord_t) bird.current_y, (coord_t) CONFIG_BIRD_H / 2, bird_color);
}

//Indicate bird should die
void kill_bird(bird_type_t *bird)
{
    bird->hit_me = true;
}

//Indicate bird should jump
void jump_bird(bird_type_t *bird)
{
    bird->jump_me = true;
}

//Get bird's current location
void get_bird_location(bird_type_t bird, coord_t *x, coord_t *y)
{
    *x = bird.current_x;
    *y = (coord_t) bird.current_y;
}

//Get if bird is dead
bool bird_is_dead(bird_type_t bird)
{
    return (bird.current_state == DEATH_FALL_ST || bird.current_state == DEATH_IDLE_ST);
}
