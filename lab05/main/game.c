#include "game.h"

#include <com.h>
#include <config.h>

#include "pin.h"
#include "graphics.h"


#include "board.h"
#include <hw_gc.h>
#include <lcd.h>
#include <nav.h>
#include <stdio.h>

#define MESSAGE_COLOR WHITE
#define MESSAGE_BG_COLOR GRAY
#define X_COLOR RED
#define O_COLOR BLUE
#define GRID_COLOR WHITE
#define BACKGROUND_COLOR BLACK

#define POS_BITS 4
#define C_MASK 0b00001111


typedef enum {
    INIT_ST,
    NEW_GAME_ST,
    WAIT_MARK_ST,
    MARK_ST,
    CHECK_MARK_ST,
    WAIT_RESTART_ST,
} game_state_t;

mark_t current_turn_g;
mark_t game_winner_g;

game_state_t current_state_g;

uint8_t tx_buffer;
uint8_t rx_buffer;

void start_game(void) {
    lcd_fillRect(0, 0, LCD_W, LCD_H, BACKGROUND_COLOR);
    board_clear();
    graphics_drawGrid(GRID_COLOR);
    graphics_drawMessage("", MESSAGE_COLOR, MESSAGE_BG_COLOR);

    game_winner_g = no_m;
    current_turn_g = X_m;

    while (com_read(&rx_buffer, 1)) {}
    rx_buffer = 0;
}

void wait_mark(void) {
    com_read(&rx_buffer, 1);
}

void mark(void) {
    int8_t curr_r, curr_c;
    if (rx_buffer) {
        curr_r = rx_buffer >> POS_BITS;
        curr_c = rx_buffer & C_MASK;
        rx_buffer = 0;
    } else {
        nav_get_loc(&curr_r, &curr_c);
    }
    if (!board_set(curr_r,curr_c, current_turn_g)) {
        graphics_drawMessage("Space already Occupied", MESSAGE_COLOR, MESSAGE_BG_COLOR);
    } else {
        if (current_turn_g == X_m) {
            graphics_drawX(curr_r, curr_c, X_COLOR);
            current_turn_g = O_m;
        } else {
            graphics_drawO(curr_r, curr_c, O_COLOR);
            current_turn_g = X_m;
        }
        tx_buffer = curr_r << POS_BITS | (curr_c & C_MASK);
        com_write(&tx_buffer, 1);
    }
}

void check_mark_transition(void) {
    if (game_winner_g) {
        current_state_g = WAIT_RESTART_ST;
        const char winner_char = game_winner_g == X_m ? 'X' : 'O';
        char winner_message[33];
        sprintf(winner_message, "%c Wins! Press Start to restart.", winner_char);
        graphics_drawMessage(winner_message, MESSAGE_COLOR, MESSAGE_BG_COLOR);
    } else {
        if (board_mark_count() == CONFIG_BOARD_R * CONFIG_BOARD_C) {
            current_state_g = WAIT_RESTART_ST;
        graphics_drawMessage("Draw! Press Start to restart.", MESSAGE_COLOR, MESSAGE_BG_COLOR);
        } else {
            current_state_g = WAIT_MARK_ST;
            const char current_turn = current_turn_g == X_m ? 'X' : 'O';
            char turn_message[40];
            sprintf(turn_message, "%c's turn. Press A to place your mark.", current_turn);
            graphics_drawMessage(turn_message, MESSAGE_COLOR, MESSAGE_BG_COLOR);
        }
    }
}

// Initialize the game logic.
void game_init(void) {
    current_state_g = INIT_ST;
    current_turn_g = X_m;
    game_winner_g = no_m;
    board_clear();
    com_init();
}

// Update the game logic.
void game_tick(void) {
    //printf("Current state: %d\n", current_state_g);

    switch (current_state_g) { //Transition Logic and Actions
        case INIT_ST:
            current_state_g = NEW_GAME_ST;
            break;
        case NEW_GAME_ST:
            current_state_g = WAIT_MARK_ST;
            const char current_turn = current_turn_g == X_m ? 'X' : 'O';
            char turn_message[40];
            sprintf(turn_message, "%c's turn. Press A to place your mark.", current_turn);
            graphics_drawMessage(turn_message, MESSAGE_COLOR, MESSAGE_BG_COLOR);
            break;
        case WAIT_MARK_ST:
            if (!pin_get_level(HW_BTN_A) || rx_buffer) {
                current_state_g = MARK_ST;
            } else {
                current_state_g = WAIT_MARK_ST;
            }
            break;
        case MARK_ST:
            current_state_g = CHECK_MARK_ST;
            break;
        case CHECK_MARK_ST:
            check_mark_transition();
            break;
        case WAIT_RESTART_ST:
            if (!pin_get_level(HW_BTN_START)) {
                current_state_g = NEW_GAME_ST;
            } else {
                current_state_g = WAIT_RESTART_ST;
            }
            break;
    }

    switch (current_state_g) { //State Actions
        case INIT_ST:
            break;
        case NEW_GAME_ST:
            start_game();
            break;
        case WAIT_MARK_ST:
            wait_mark();
            break;
        case MARK_ST:
            mark();
            break;
        case CHECK_MARK_ST:
            if(board_winner(X_m)) game_winner_g = X_m;
            else if (board_winner(O_m)) game_winner_g = O_m;
            else game_winner_g = no_m;
            break;
        case WAIT_RESTART_ST:
            break;
    }

}