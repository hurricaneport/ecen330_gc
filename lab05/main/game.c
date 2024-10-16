#include "game.h"
#include "pin.h"
#include "graphics.h"


#include "board.h"
#include <hw_gc.h>
#include <lcd.h>
#include <nav.h>

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
void mark() {
    int8_t curr_r, curr_c;
    nav_get_loc(&curr_r, &curr_c);
    if (!board_set(curr_r,curr_c, current_turn_g)) {
        graphics_drawMessage("Space already Occupied", WHITE, GRAY);
    } else {
        if (current_turn_g == X_m) {
            graphics_drawX(curr_r, curr_c, RED);
            current_turn_g = O_m;
        } else {
            graphics_drawO(current_turn_g, curr_r, BLUE);
            current_turn_g = X_m;
        }
    }
}

// Initialize the game logic.
void game_init(void) {
    current_state_g = INIT_ST;
    current_turn_g = X_m;
    game_winner_g = no_m;
}

// Update the game logic.
void game_tick(void) {
    switch (current_state_g) { //Transition Logic
        case INIT_ST:
            current_state_g = NEW_GAME_ST;
            break;
        case NEW_GAME_ST:
            current_state_g = WAIT_MARK_ST;
            break;
        case WAIT_MARK_ST:
            if (!pin_get_level(HW_BTN_A)) {
                current_state_g = MARK_ST;
            } else {
                current_state_g = WAIT_MARK_ST;
            }
            break;
        case MARK_ST:
            current_state_g = CHECK_MARK_ST;
            break;
        case CHECK_MARK_ST:
            if (!game_winner_g) {
                current_state_g = WAIT_MARK_ST;
            } else {
                current_state_g = WAIT_RESTART_ST;
            }
            break;
        case WAIT_RESTART_ST:
            if (!pin_get_level(HW_BTN_START)) {
                current_state_g = NEW_GAME_ST;
            } else {
                current_state_g = WAIT_RESTART_ST;
            }
            break;
    }

    switch (current_state_g) {
        case INIT_ST:
            break;
        case NEW_GAME_ST:
            graphics_drawGrid(WHITE);
            game_winner_g = no_m;
            current_turn_g = X_m;
            break;
        case WAIT_MARK_ST:
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