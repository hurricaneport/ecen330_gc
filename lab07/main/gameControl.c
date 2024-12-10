//
// Created by dansi on 12/9/2024.
//

#include "gameControl.h"
#include "bird.h"
#include "pin.h"

bird_type_t bird;
// Initialize game logic
void gameControl_init(void)
{
    bird_init(&bird);
}

// Tick the game control function. This ticks each individual state machine.
void gameControl_tick(void)
{
    static bool game_over = false;
    if (!game_over)
    {
        static bool jump_flag = false;
        if (!pin_get_level(HW_BTN_A) && !jump_flag)
        {
            jump_bird(&bird);
            jump_flag = true;
        } else if (pin_get_level(HW_BTN_A))
        {
            jump_flag = false;
        }
        tick_bird(&bird);
        draw_bird(bird);
        game_over = bird_is_dead(bird);

    } else
    {
        tick_bird(&bird);
        draw_bird(bird);
    }
}

