#include <stdio.h>

#include "esp_log.h"
#include "driver/gptimer.h"
#include "hw.h"
#include "lcd.h"
#include "pin.h"
#include "watch.h"

#define A_PIN 32
#define B_PIN 33
#define START_PIN 39

#define TIMER_RESOLUTION 1000000

static const char *TAG = "lab03";

//Timer Alarm
static bool on_timer_alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *user_data) {

    return false;
}

// Main application
void app_main(void)
{
    ESP_LOGI(TAG, "Starting");

    //IO Setup
    pin_reset(A_PIN);
    pin_reset(B_PIN);
    pin_reset(START_PIN);

    pin_input(A_PIN, true);
    pin_input(B_PIN, true);
    pin_input(START_PIN, true);

    //Prepare Timer
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION
    };

    gptimer_new_timer(&config, &gptimer);

    gptimer_event_callbacks_t cbs = {
    .on_alarm = on_timer_alarm};

    gptimer_register_event_callbacks(gptimer, &cbs, NULL);



    gptimer_event_callbacks_t cbs = {
        .on_alarm = NULL
    };


}
