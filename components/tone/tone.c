#include "tone.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define DAC_RANGE 256
#define WAVE_AMPLITUDE DAC_RANGE / 2
#define BIAS DAC_RANGE / 2
#define HALF_PERIOD 2
#define QUARTER_PERIOD 4
#define NATURAL_PERIOD 2 * M_PI

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

    //printf("Tone Start\nSample Rate:%ld Frequency:%ld\n", g_sample_hz, freq);
    for (int x = 0; x < period_samples; x++) { // Generate Waveform
        switch(tone) { // Switch logic based on chosen waveform
            case SINE_T:
                g_buffer[x] = WAVE_AMPLITUDE * sinf(NATURAL_PERIOD * (float) freq * (float) x / (float) g_sample_hz) + BIAS;
                break;
            case SQUARE_T:
                if (x < period_samples / HALF_PERIOD) g_buffer[x] = DAC_RANGE - 1;
                else g_buffer[x] = 0;
                break;
            case TRIANGLE_T:
                if (x < period_samples / QUARTER_PERIOD) g_buffer[x] = BIAS + (DAC_RANGE / (period_samples / HALF_PERIOD)) * x ; // First Quarter
                else if (x < period_samples / QUARTER_PERIOD +  period_samples / HALF_PERIOD) g_buffer[x] = BIAS - (DAC_RANGE / (period_samples / HALF_PERIOD)) * (x - period_samples / HALF_PERIOD); // Second and Third Quarters - Shifted
                else g_buffer[x] = BIAS + (DAC_RANGE / (period_samples / HALF_PERIOD)) * (x - period_samples); // Last Quarter - Shifter
                break;
            case SAW_T:
                if (x < period_samples / HALF_PERIOD) g_buffer[x] = BIAS + (DAC_RANGE / period_samples) * x; // First Half
                else g_buffer[x] = BIAS + (DAC_RANGE / period_samples) * (x - period_samples); // Second Half - Shifted
                break;
            case LAST_T:
                break;
        }
        //printf("x:%2d buffer:%d\n", x, g_buffer[x]);
    }
    sound_cyclic(g_buffer, period_samples);
}
