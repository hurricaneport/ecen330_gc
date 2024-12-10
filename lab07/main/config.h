//
// Created by dansi on 12/8/2024.
//

#ifndef CONFIG_H
#define CONFIG_H

#include "lcd.h"

#define CONFIG_GAME_TIMER_PERIOD 40.0E-3f
#define CONFIG_GAME_TIMER_TICKS_PER_SECOND 1/CONFIG_GAME_TIMER_PERIOD

#define CONFIG_BIRD_MAX_VELOCITY 200
#define CONFIG_BIRD_MAX_VELOCITY_TICKS CONFIG_BIRD_MAX_VELOCITY * CONFIG_GAME_TIMER_PERIOD
#define CONFIG_BIRD_ACCELERATION 30
#define CONFIG_BIRD_ACCELERATION_TICKS CONFIG_BIRD_ACCELERATION * CONFIG_GAME_TIMER_PERIOD
#define CONFIG_BIRD_JUMP_VELOCITY -165
#define CONFIG_BIRD_JUMP_VELOCITY_TICKS CONFIG_BIRD_JUMP_VELOCITY * CONFIG_GAME_TIMER_PERIOD

#define CONFIG_BIRD_X_LOCATION LCD_W / 4
#define CONFIG_BIRD_Y_START_LOCATION LCD_H / 2.0f

#define CONFIG_BIRD_H 25
#define CONFIG_BIRD_W 25

#define CONFIG_PIPE_PADDING 30
#define CONFIG_PIPE_WIDTH 50
#define CONFIG_PIPE_LIP_WIDTH 60
#define CONFIG_PIPE_SPACING 70

#define CONFIG_PIPE_SPEED 100
#define CONFIG_PIPE_SPEED_TICKS CONFIG_PIPE_SPEED * CONFIG_GAME_TIMER_PERIOD

#define CONFIG_COLOR_BACKGROUND rgb565(3, 202, 252)
#define CONFIG_COLOR_BIRD YELLOW
#define CONFIG_COLOR_BIRD_DEATH rgb565(128, 113, 3)
#define CONFIG_COLOR_PIPE rgb565(0, 209, 242)

#endif //CONFIG_H