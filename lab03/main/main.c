#include <stdio.h>

#include "esp_log.h"
#include "driver/gptimer.h"
#include "hw.h"
#include "lcd.h"
#include "pin.h"
#include "watch.h"
#include "esp_timer.h"

#define A_PIN 32
#define B_PIN 33
#define START_PIN 39

#define TIMER_RESOLUTION 1000000
#define ALARM_PERIOD .01

static const char *TAG = "lab03";
volatile uint32_t g_timer_ticks;
volatile bool g_running;

volatile uint64_t g_isr_max;
volatile uint32_t g_isr_cnt;

//Timer Alarm
static bool on_timer_alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *user_data) {
    uint64_t start_time = esp_timer_get_time();

    if (!pin_get_level(A_PIN)) g_running = true; //A Pressed
    else if(!pin_get_level(B_PIN)) g_running = false; //B Pressed
    else if (!pin_get_level(START_PIN)) { //Reset
        g_running = false;
        g_timer_ticks = 0;
    }

    if (g_running) g_timer_ticks++;
    uint64_t finish_time = esp_timer_get_time();

    if ((finish_time - start_time) > g_isr_max) g_isr_max = finish_time - start_time;
    g_isr_cnt++;
    return false;
}

// Main application
void app_main(void)
{
    ESP_LOGI(TAG, "Starting");

    uint64_t start_time = esp_timer_get_time();

    //IO Setup
    pin_reset(A_PIN);
    pin_reset(B_PIN);
    pin_reset(START_PIN);

    pin_input(A_PIN, true);
    pin_input(B_PIN, true);
    pin_input(START_PIN, true);

    uint64_t finish_time =esp_timer_get_time();

    printf("Time Elapsed - Pin Config %llu\n", finish_time - start_time);

    //Initialize Global Variables
    g_timer_ticks = 0;
    g_running = false;
    g_isr_max = 0;
    g_isr_cnt = 0;

    start_time = esp_timer_get_time();
    //Prepare Timer
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION
    };
    gptimer_new_timer(&config, &gptimer);

    //Register callback function
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm
    };
    gptimer_register_event_callbacks(gptimer, &cbs, NULL);

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = ALARM_PERIOD * TIMER_RESOLUTION,
        .flags.auto_reload_on_alarm = true
        };
    gptimer_set_alarm_action(gptimer, &alarm_config);

    ESP_LOGI(TAG, "Starting Timer");
    gptimer_enable(gptimer);
    gptimer_start(gptimer);

    finish_time = esp_timer_get_time();

    printf("Time elapsed - Timer Config %llu\n", finish_time - start_time);

    start_time = esp_timer_get_time();
    ESP_LOGI(TAG, "Stopwatch update");
    finish_time = esp_timer_get_time();

    printf("Time elapsed - Log Statement %llu\n", finish_time - start_time);
    lcd_init(); // Initialize LCD display
    watch_init(); // Initialize stopwatch face
    while(true) { // forever update loop
        watch_update(g_timer_ticks);
        if (g_isr_cnt == 500) {
            g_isr_cnt = 0;
            printf("Time elapsed - ISR Execute Time %llu\n", g_isr_max);
        }
    }
}
