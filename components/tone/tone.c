#include "tone.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>


uint8_t * g_buffer;
uint32_t g_sample_hz;

// Initialize the tone driver. Must be called before using.
// May be called again to change sample rate.
// sample_hz: sample rate in Hz to playback tone.
// Return zero if successful, or non-zero otherwise.
int32_t tone_init(uint32_t sample_hz) {
    if (sample_hz < LOWEST_FREQ * 2) return 1; //Nyquist Frequency too low
    if(sound_init(sample_hz)) return 1; //Return if error
    if((g_buffer = (uint8_t *) malloc(sizeof (uint8_t) * sample_hz/LOWEST_FREQ)) == NULL) return 1; //return if malloc not successful

    g_sample_hz = sample_hz;

    return 0;

}

// Free resources used for tone generation (DAC, etc.).
// Return zero if successful, or non-zero otherwise.
int32_t tone_deinit(void) {
    if (sound_deinit()) return 1;
    free(g_buffer);

    return 0;
}

// Start playing the specified tone.
// tone: one of the enumerated tone types.
// freq: frequency of the tone in Hz.
void tone_start(tone_t tone, uint32_t freq) {
    uint32_t period_samples = g_sample_hz / freq;

    for (int x = 0; x < period_samples; x++) {
        switch(tone) {
            case SINE_T:
                g_buffer[x] = (float) x / (float) g_sample_hz;
                printf("x:%d buffer;%d\n", x, g_buffer[x]);
                break;
            case SQUARE_T:
                break;
            case TRIANGLE_T:
                break;
            case SAW_T:
                break;
            case LAST_T:
                break;
        }
    }
}
