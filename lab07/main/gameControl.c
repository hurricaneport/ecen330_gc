//
// Created by dansi on 12/9/2024.
//

#include "gameControl.h"

#include <config.h>
#include <pipe.h>
#include <sfx_hit.h>
#include <sound.h>
#include <stdio.h>

#include "bird.h"
#include "pin.h"
#include "sfx_point.h"

bird_type_t bird;
pipe_type_t pipes[CONFIG_PIPE_MAX_PIPES];
bool pipes_scored[CONFIG_PIPE_MAX_PIPES];
bool pipes_progressed[CONFIG_PIPE_MAX_PIPES];

uint16_t current_score = 0;
// Initialize game logic
void gameControl_init(void)
{
    bird_init(&bird);
    for (int i = 0; i < CONFIG_PIPE_MAX_PIPES; i++)
    {
        pipes_scored[i] = false;
        pipes_progressed[i] = false;
        pipe_init(&pipes[i]);
    }

    start_pipe(&pipes[0]);
}

// Tick the game control function. This ticks each individual state machine.
void gameControl_tick(void)
{
    //check if bird is dead
    static bool game_over;
    game_over = bird_is_dead(bird);

    //Check for button presses
    static bool jump_flag = false;
    if (!pin_get_level(HW_BTN_A) && !jump_flag)
    {
        jump_bird(&bird);
        jump_flag = true;
    } else if (pin_get_level(HW_BTN_A))
    {
        jump_flag = false;
    }

    //Pipe drawing Logic
    for (int i = 0; i < CONFIG_PIPE_MAX_PIPES; i++)
    {
        //Tick and draw all pipes
        tick_pipe(&pipes[i]);
        draw_pipe(pipes[i]);

        //Check if pipe has scored since last tick and add it to total score
        if (pipe_scored(pipes[i]) && !pipes_scored[i])
        {
            sound_start(sfx_point, sizeof(sfx_hit), false);
            pipes_scored[i] = true;
            current_score++;
        }

        //Check if pipe has progressed far enough for new pipe to spawn since last tick
        if (get_pipe_position(pipes[i]) < LCD_W - CONFIG_PIPE_HORIZ_SPACING && !pipes_progressed[i] && !pipe_is_idle(pipes[i]))
        {
            pipes_progressed[i] = true;
            for (int j = 0; j < CONFIG_PIPE_MAX_PIPES; j++)
            {
                if (pipe_is_idle(pipes[j]))
                {
                    start_pipe(&pipes[j]);
                    break;
                }
            }
        }

        //Check if pipe is collided with bird
        if (check_collision(pipes[i], get_bird_location(bird)) || game_over)
        {
            kill_bird(&bird);
            for (int j = 0; j < CONFIG_PIPE_MAX_PIPES; j++)
            {
                stop_pipe(&pipes[j]);
            }
        }

        //Check if pipe is idled, and reset flags
        if (pipe_is_idle(pipes[i]))
        {
            pipes_scored[i] = false;
            pipes_progressed[i] = false;
        }
    }

    //Tick and draw bird
    tick_bird(&bird);
    draw_bird(bird);

    //Draw game over screen
    if (game_over)
    {
        lcd_setFontSize(CONFIG_TITLE_FONT_SIZE);
        lcd_drawString(CONFIG_GAME_OVER_X, CONFIG_GAME_OVER_Y, "Game Over!", CONFIG_COLOR_FONT);

        char score[13];
        sprintf(score, "Score: %d", current_score);
        lcd_setFontSize(CONFIG_GAME_OVER_SCORE_FONT_SIZE);
        lcd_drawString(CONFIG_GAME_OVER_SCORE_X, CONFIG_GAME_OVER_SCORE_Y, score, CONFIG_COLOR_FONT);

        //Restart
        if (!pin_get_level(HW_BTN_START))
        {
            gameControl_init();
            current_score = 0;
            game_over = false;
        }
    } else
    {
        char score[6];
        sprintf(score, "%d", current_score);
        lcd_setFontSize(CONFIG_SCORE_FONT_SIZE);
        lcd_drawString(CONFIG_SCORE_X, CONFIG_SCORE_Y, score, CONFIG_COLOR_FONT);
    }

}

