//
// Created by dansi on 12/9/2024.
//

#include "pipe.h"
#include "esp_random.h"

#include <config.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
    INIT_ST,
    MOVE_ST,
    STOP_ST,
    IDLE_ST,
};

//Init pipe
void pipe_init(pipe_type_t *pipe)
{
    pipe->current_state = IDLE_ST;
    pipe->scored = false;
    pipe->stop_me = false;
    pipe->current_x = 0;
    pipe->center_y = 0;
}

//Tick pipe
void tick_pipe(pipe_type_t *pipe)
{
    //State Transitions
    switch (pipe->current_state)
    {
        case INIT_ST:
            pipe->current_state = MOVE_ST;
            break;
    case MOVE_ST:
            if (pipe->stop_me)
            {
                pipe->current_state = STOP_ST;
            } else if ((coord_t) pipe->current_x < -CONFIG_PIPE_WIDTH / 2)
            {
                pipe->current_state = IDLE_ST;
                pipe->scored = false;
            } else
            {
                pipe->current_state = MOVE_ST;
            }
            break;
        case STOP_ST:
            pipe->current_state = STOP_ST;
            break;
        case IDLE_ST:
            pipe->current_state = IDLE_ST;
            break;
        default:
            pipe->current_state = IDLE_ST;
            break;
    }

    //state actions
    switch (pipe->current_state)
    {
        case INIT_ST:
            break;
        case MOVE_ST:
            pipe->current_x -= CONFIG_PIPE_SPEED_TICKS;
            if (pipe->current_x < (float) CONFIG_BIRD_X_LOCATION - CONFIG_BIRD_W / 2.0f)
            {
                pipe->scored = true;
            }
            break;
        case STOP_ST:
            break;
        case IDLE_ST:
            break;
        default:
            break;
    }
}

//Draw the current pipe
void draw_pipe(pipe_type_t pipe)
{
    if (pipe.current_state == IDLE_ST)
    {
        return;
    }

    // Main Pipes
    lcd_fillRect2((coord_t) pipe.current_x - CONFIG_PIPE_WIDTH / 2, 0, (coord_t) pipe.current_x + CONFIG_PIPE_WIDTH / 2,
        pipe.center_y - CONFIG_PIPE_SPACING / 2 - CONFIG_PIPE_LIP_HEIGHT, CONFIG_COLOR_PIPE);
    lcd_fillRect2((coord_t) pipe.current_x - CONFIG_PIPE_WIDTH / 2, LCD_H, (coord_t) pipe.current_x + CONFIG_PIPE_WIDTH / 2,
        pipe.center_y + CONFIG_PIPE_SPACING / 2 + CONFIG_PIPE_LIP_HEIGHT, CONFIG_COLOR_PIPE);

    // Lips
    lcd_fillRect2((coord_t) pipe.current_x - CONFIG_PIPE_LIP_WIDTH / 2, pipe.center_y - CONFIG_PIPE_SPACING / 2 - CONFIG_PIPE_LIP_HEIGHT,
        (coord_t) pipe.current_x + CONFIG_PIPE_LIP_WIDTH / 2, pipe.center_y - CONFIG_PIPE_SPACING / 2, CONFIG_COLOR_PIPE);
    lcd_fillRect2((coord_t) pipe.current_x - CONFIG_PIPE_LIP_WIDTH / 2, pipe.center_y + CONFIG_PIPE_SPACING / 2 + CONFIG_PIPE_LIP_HEIGHT,
        (coord_t) pipe.current_x + CONFIG_PIPE_LIP_WIDTH / 2, pipe.center_y + CONFIG_PIPE_SPACING / 2, CONFIG_COLOR_PIPE);
}


//Start the pipe
void start_pipe(pipe_type_t *pipe)
{
    pipe->current_state = INIT_ST;
    pipe->center_y = (coord_t) (esp_random() % (LCD_H - 2 * CONFIG_PIPE_PADDING - CONFIG_PIPE_SPACING)) + CONFIG_PIPE_PADDING + CONFIG_PIPE_SPACING / 2;
    pipe->current_x = LCD_W;
    pipe->scored = false;
    pipe->stop_me = false;
}

//Function to stop pipe
void stop_pipe(pipe_type_t *pipe)
{
    pipe->stop_me = true;
}

//Get pipes x position
coord_t get_pipe_position(pipe_type_t pipe)
{
    return (coord_t) pipe.current_x;
}

//Check if bird has collided with pipe
bool check_collision(pipe_type_t pipe, coord_t y)
{
    if (pipe.current_state == IDLE_ST)
    {
        return false;
    }
    const bool x_collision = (coord_t) pipe.current_x <  CONFIG_BIRD_X_LOCATION + CONFIG_BIRD_W / 2 + CONFIG_PIPE_WIDTH / 2
        && (coord_t) pipe.current_x > CONFIG_BIRD_X_LOCATION - CONFIG_BIRD_W / 2 - CONFIG_PIPE_WIDTH / 2;
    const bool y_collision = y > pipe.center_y + CONFIG_PIPE_SPACING / 2 - CONFIG_BIRD_W / 2 || y < pipe.center_y - CONFIG_PIPE_SPACING / 2 + CONFIG_BIRD_W / 2;
    return x_collision && y_collision;
}

//Check if pipe has scored
bool pipe_scored(pipe_type_t pipe)
{
    return pipe.scored;
}

//Check if pipe is idle
bool pipe_is_idle(pipe_type_t pipe)
{
    return pipe.current_state == IDLE_ST;
}