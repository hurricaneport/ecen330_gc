#include "joy.h"

#include <esp_log.h>

#include "esp_adc/adc_oneshot.h"

#define ADC_CHANNEL_X ADC_CHANNEL_6
#define ADC_CHANNEL_Y ADC_CHANNEL_7

#define NUM_AVG_READS 3


uint16_t g_offset_x = 0;
uint16_t g_offset_y = 0;

adc_oneshot_unit_handle_t * g_adc_handle = NULL;

// Initialize the joystick driver. Must be called before use.
// May be called multiple times. Return if already initialized.
// Return zero if successful, or non-zero otherwise.
int32_t joy_init(void) {



    if (g_adc_handle != NULL) return 0; // ADC already initialized
    g_adc_handle = malloc(sizeof(adc_oneshot_unit_handle_t));
    const adc_oneshot_unit_init_cfg_t oneshot_unit_cfg = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    esp_err_t err = adc_oneshot_new_unit(&oneshot_unit_cfg, g_adc_handle);

    if (err != ESP_OK) return 1; // Not initialized Successfully


    const adc_oneshot_chan_cfg_t oneshot_chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    adc_oneshot_config_channel(*g_adc_handle, ADC_CHANNEL_X, &oneshot_chan_cfg);
    adc_oneshot_config_channel(*g_adc_handle, ADC_CHANNEL_Y, &oneshot_chan_cfg);

    int_fast32_t x_zero [NUM_AVG_READS];
    int_fast32_t y_zero [NUM_AVG_READS];

    uint16_t x_sum = 0;
    uint16_t y_sum = 0;

    for (int i = 0; i < NUM_AVG_READS; i++) { //Read Center Position Multiple Times
        adc_oneshot_read(*g_adc_handle, ADC_CHANNEL_X, &x_zero[i]);
        x_sum += x_zero[i];

        adc_oneshot_read(*g_adc_handle, ADC_CHANNEL_Y, &y_zero[i]);
        y_sum += y_zero[i];
    }

    g_offset_x = x_sum / NUM_AVG_READS;
    g_offset_y = y_sum / NUM_AVG_READS;

    return 0;
}

// Free resources used by the joystick (ADC unit).
// Return zero if successful, or non-zero otherwise.
int32_t joy_deinit(void) {
    if (g_adc_handle == NULL) return 0; // Already freed
    esp_err_t err = adc_oneshot_del_unit(*g_adc_handle);
    if (err == ESP_OK) { // Deleted Successfully
        g_adc_handle = NULL;
        free(g_adc_handle);
        return 0;
    }
    else return 1;
}

// Get the joystick displacement from center position.
// Displacement values range from 0 to +/- JOY_MAX_DISP.
// This function is not safe to call from an ISR context.
// Therefore, it must be called from a software task context.
// *dcx: pointer to displacement in x.
// *dcy: pointer to displacement in y.
void joy_get_displacement(int32_t *dcx, int32_t *dcy) {
    int_fast32_t dcx_read;
    int_fast32_t dcy_read;
    adc_oneshot_read(*g_adc_handle, ADC_CHANNEL_X, &dcx_read);
    adc_oneshot_read(*g_adc_handle, ADC_CHANNEL_Y, &dcy_read);
    *dcx = dcx_read - g_offset_x;
    *dcy = dcy_read - g_offset_y;
}