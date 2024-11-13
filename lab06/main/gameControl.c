
#include <stdio.h>
#include <stdlib.h> // rand

#include "hw.h"
#include "lcd.h"
#include "cursor.h"
#include "sound.h"
#include "pin.h"
#include "missile.h"
#include "plane.h"
#include "gameControl.h"
#include "config.h"

// sound support
#include "missileLaunch.h"

// M3: Define stats constants
#define SHOT_STRING_SIZE 10
#define IMPACTED_STRING_SIZE 15

#define SHOT_X 10
#define SHOT_Y 10

#define IMPACTED_X LCD_W / 2 + 10
#define IMPACTED_Y 10

// All missiles
missile_t missiles[CONFIG_MAX_TOTAL_MISSILES];

// Alias into missiles array
missile_t *enemy_missiles = missiles+0;
missile_t *player_missiles = missiles+CONFIG_MAX_ENEMY_MISSILES;
missile_t *plane_missile = missiles+CONFIG_MAX_ENEMY_MISSILES+
									CONFIG_MAX_PLAYER_MISSILES;

// M3: Declare stats variables
uint32_t missiles_fired = 0;
uint32_t missiles_impacted = 0;


// Initialize the game control logic.
// This function initializes all missiles, planes, stats, etc.
void gameControl_init(void)
{
	// Initialize missiles
	for (uint32_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++)
		missile_init_enemy(enemy_missiles+i);
	for (uint32_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++)
		missile_init_idle(player_missiles+i);
	missile_init_idle(plane_missile);

	// Initialize plane
	plane_init(plane_missile);

	// Initialize stats
	missiles_fired = 0;
	missiles_impacted = 0;

	// M3: Set sound volume
	sound_set_volume(CONFIG_SOUND_VOLUME);
}

// Update the game control logic.
// This function calls the missile & plane tick functions, reinitializes
// idle enemy missiles, handles button presses, fires player missiles,
// detects collisions, and updates statistics.
void gameControl_tick(void) {

	// Tick missiles in one batch and check if exploded by other missiles
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) { //Cycle through all missiles
		missile_tick(missiles+i);
		if ((missiles+i)->type == MISSILE_TYPE_PLAYER && (missiles+i)->length >= (missiles+i)->total_length) { // Missile is player - explode
			missile_explode(missiles+i);
		} else if (missile_get_type((missiles+i)) != MISSILE_TYPE_PLAYER) { // Missile is not player, only explode if in radius
			for (uint32_t k = 0; k < CONFIG_MAX_TOTAL_MISSILES; k++) { // Check all missiles
				if(missile_is_colliding(missiles+k, (missiles+i)->x_current, (missiles+i)->y_current)) { // Explode if in radius of any missile
					missile_explode(missiles+i);
					break;
				}
			}
		}
	}

	// Reinitialize idle enemy missiles
	for (uint32_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
		if (missile_is_idle(enemy_missiles+i)) {
			missile_init_enemy(enemy_missiles+i);
		}
	}

	//Fire Missiles when pressed

	static bool button_flag = false;
	coord_t cursor_x, cursor_y;
	uint64_t btns= ~pin_get_in_reg() & HW_BTN_MASK;
	if (!button_flag && btns) { // Button has been pressed and not yet activated
		button_flag = true;
		cursor_get_pos(&cursor_x, &cursor_y);
		for (int32_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) { // Check for idle missile
			if (missile_is_idle(player_missiles+i)) { // if idle, fire then break
				missiles_fired++;
				missile_init_player(player_missiles+i, cursor_x, cursor_y);
				sound_start(missileLaunch, MISSILELAUNCH_SAMPLES, false);
				break;
			}
		}
	} else if (button_flag && !btns) {
		button_flag = false;
	}

	for (int32_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) {
		if (missile_is_impacted(missiles + i)) {
			missiles_impacted += 1;
		}
	}

	//Tick plane
	static uint32_t cnt = 0;

	plane_tick();
	if (!plane_is_flying()) {
		cnt++;
	}
	if (cnt > CONFIG_PLANE_IDLE_TIME_TICKS) { // Wait if plane is idle for idle time
		plane_init(plane_missile);
		cnt = 0;
	}

	// Check for explosion
	coord_t plane_x, plane_y;
	plane_get_pos(&plane_x, &plane_y);
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) { // Cycle through all missiles to check
		if (missile_is_colliding(missiles + i, plane_x, plane_y)) {
			plane_explode();
			break;
		}
	}

	//Draw Stats
	char shot_string[SHOT_STRING_SIZE];
	char impacted_string[IMPACTED_STRING_SIZE];

	sprintf(shot_string, "Shot: %ld", missiles_fired);
	sprintf(impacted_string, "Impacted: %ld", missiles_impacted);

	lcd_drawString(SHOT_X, SHOT_Y, shot_string, CONFIG_COLOR_STATUS);
	lcd_drawString(IMPACTED_X, IMPACTED_Y, impacted_string, CONFIG_COLOR_STATUS);

}
