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
    coord_t current_x;
    coord_t current_y;

    uint16_t current_velocity;

    bool hit_me;
} bird_type_t;

#endif //BIRD_H
