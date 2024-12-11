//
// Created by dansi on 12/9/2024.
//

#ifndef BIRD_H
#define BIRD_H
#include <stdint.h>

#include "lcd.h" // coord_t

typedef struct
{
    int32_t current_state;
    float current_y;

    float current_velocity;

    bool hit_me;
    bool jump_me;
} bird_type_t;

//Initialize bird logic before game
void bird_init(bird_type_t *bird);

//Tick bird
void tick_bird(bird_type_t *bird);

//Draw bird at its current location with correct color
void draw_bird(bird_type_t bird);

//Indicate bird should die
void kill_bird(bird_type_t *bird);

//Indicate bird should jump
void jump_bird(bird_type_t *bird);

//Get bird's current location
coord_t get_bird_location(bird_type_t bird);

//Get if bird is dead
bool bird_is_dead(bird_type_t bird);

//Check when bird becomes idle
bool bird_is_idle(bird_type_t bird);
#endif //BIRD_H
