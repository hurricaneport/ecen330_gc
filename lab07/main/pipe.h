//
// Created by dansi on 12/9/2024.
//

#ifndef PIPE_H
#define PIPE_H
#include <lcd.h>
#include <stdbool.h>

typedef struct
{
    int32_t current_state;
    float current_x;
    coord_t center_y;

    bool scored;
    bool stop_me;
} pipe_type_t;

//Init pipe
void pipe_init(pipe_type_t *pipe);

//Tick pipe
void tick_pipe(pipe_type_t *pipe);

//Draw the current pipe
void draw_pipe(pipe_type_t pipe);

//Start the pipe
void start_pipe(pipe_type_t *pipe);

//Function to stop pipe
void stop_pipe(pipe_type_t *pipe);

//Get pipes x position
coord_t get_pipe_position(pipe_type_t pipe);

//Check if bird has collided with pipe
bool check_collision(pipe_type_t pipe, coord_t y);

//Check if pipe has scored
bool pipe_scored(pipe_type_t pipe);

//Check if pipe is idle
bool pipe_is_idle(pipe_type_t pipe);

#endif //PIPE_H
