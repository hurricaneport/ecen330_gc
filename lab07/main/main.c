#include <esp_timer.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "pin.h"
#include "config.h"
#include "gameControl.h"
#include "lcd.h"

#define JUMP_PIN HW_BTN_A
#define RESET_PIN HW_BTN_START
#define STOP_PIN HW_BTN_MENU

#define TIME_OUT 500
#define PER_MS ((uint32_t)(CONFIG_GAME_TIMER_PERIOD*1000))

static const char *TAG = "lab07";

TimerHandle_t update_timer;

volatile bool interrupt_flag;

uint32_t isr_triggered_count;
uint32_t isr_handled_count;

// Update ISR
void update()
{
	interrupt_flag = true;
	isr_triggered_count++;
}

// Main application
void app_main(void)
{
	ESP_LOGI(TAG, "Starting");

	// ISR flag and interrupt counters
	interrupt_flag = false;
	isr_triggered_count = 0;
	isr_handled_count = 0;

	// Init
	lcd_init();
	lcd_frameEnable();
	gameControl_init();
	lcd_fillScreen(CONFIG_COLOR_BACKGROUND);

	// IO Setup
	pin_reset(JUMP_PIN);
	pin_reset(RESET_PIN);
	pin_reset(STOP_PIN);

	pin_input(JUMP_PIN, true);
	pin_input(RESET_PIN, true);
	pin_input(STOP_PIN, true);

	// Initialize Update Timer
	update_timer = xTimerCreate(
		"update_timer",
		pdMS_TO_TICKS(PER_MS),
		pdTRUE,
		NULL,
		update);

	if (update_timer == NULL)
	{ // Timer not allocated
		ESP_LOGE(TAG, "Failed to create update_timer");
		return;
	}
	if (xTimerStart(update_timer, pdMS_TO_TICKS(TIME_OUT)) != pdPASS)
	{ // Timer not started
		ESP_LOGE(TAG, "Failed to start update_timer");
		return;
	}

	// Main game loop
	uint64_t t1, t2, t_max = 0; // HW timer values
	while (pin_get_level(STOP_PIN)) // End game when Menu pressed
	{
		// Wait for timer tick
		while (!interrupt_flag) {}
		t1 = esp_timer_get_time();
		interrupt_flag = false;

		// Erase Screen
		lcd_fillScreen(CONFIG_COLOR_BACKGROUND);

		gameControl_tick();

		lcd_writeFrame();

		// Time logging
		isr_handled_count++;
		t2 = esp_timer_get_time();
		if (t2 - t1 > t_max) t_max = t2 - t1;
	}

	printf("Handled %lu of %lu interrupts\n", isr_handled_count, isr_triggered_count);
	printf("WCET us:%llu\n", t_max);
}
