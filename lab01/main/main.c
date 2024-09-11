#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lcd.h"
#include "pac.h"

static const char *TAG = "lab01";

#define delayMS(ms) \
	vTaskDelay(((ms)+(portTICK_PERIOD_MS-1))/portTICK_PERIOD_MS)

//----------------------------------------------------------------------------//
// Car Implementation - Begin
//----------------------------------------------------------------------------//

// Car constants
#define CAR_CLR rgb565(220,30,0)
#define WINDOW_CLR rgb565(180,210,238)
#define TIRE_CLR BLACK
#define HUB_CLR GRAY
#define CAR_W 60
#define CAR_H 32

#define BODY_X0 0
#define BODY_Y0 12
#define BODY_X1 59
#define BODY_Y1 24

#define CAB_X0 1
#define CAB_Y0 0
#define CAB_X1 39
#define CAB_Y1 11

#define WINDOW_CORNER_RADIUS 2

#define WINDOW_1_X0 3
#define WINDOW_1_Y0 1
#define WINDOW_1_X1 18
#define WINDOW_1_Y1 8

#define WINDOW_2_X0 21
#define WINDOW_2_Y0 1
#define WINDOW_2_X1 37
#define WINDOW_2_Y1 8

#define HOOD_X0 40
#define HOOD_Y0 9
#define HOOD_X1 40
#define HOOD_Y1 11
#define HOOD_X2 59
#define HOOD_Y2 11

#define WHEEL_INNER_RADIUS 4
#define WHEEL_OUTER_RADIUS 7

#define WHEEL_1_CENTER_X 11
#define WHEEL_1_CENTER_Y 24

#define WHEEL_2_CENTER_X 48
#define WHEEL_2_CENTER_Y 24

/**
 * @brief Draw a car at the specified location.
 * @param x      Top left corner X coordinate.
 * @param y      Top left corner Y coordinate.
 * @details Draw the car components relative to the anchor point (top, left).
 */
void drawCar(coord_t x, coord_t y) {
    //Body
    lcd_fillRect2(x + BODY_X0, y + BODY_Y0, x + BODY_X1, y + BODY_Y1,CAR_CLR);

    //Cab
    lcd_fillRect2(x + CAB_X0, y + CAB_Y0, x + CAB_X1, y + CAB_Y1,CAR_CLR);

    //Windows
    lcd_fillRoundRect2(x + WINDOW_1_X0, y + WINDOW_1_Y0, x + WINDOW_1_X1, y + WINDOW_1_Y1,WINDOW_CORNER_RADIUS,
                       WINDOW_CLR);
    lcd_fillRoundRect2(x + WINDOW_2_X0, y + WINDOW_2_Y0, x + WINDOW_2_X1, y + WINDOW_2_Y1,WINDOW_CORNER_RADIUS,
                       WINDOW_CLR);

    //Hood
    lcd_fillTriangle(x + HOOD_X0, y + HOOD_Y0, x + HOOD_X1, y + HOOD_Y1, x + HOOD_X2, y + HOOD_Y2, CAR_CLR);

    //Wheels
    lcd_fillCircle(x + WHEEL_1_CENTER_X, y + WHEEL_1_CENTER_Y, WHEEL_OUTER_RADIUS, TIRE_CLR);
    lcd_fillCircle(x + WHEEL_2_CENTER_X, y + WHEEL_2_CENTER_Y, WHEEL_OUTER_RADIUS, TIRE_CLR);

    //RIMS
    lcd_fillCircle(x + WHEEL_1_CENTER_X, y + WHEEL_1_CENTER_Y, WHEEL_INNER_RADIUS, HUB_CLR);
    lcd_fillCircle(x + WHEEL_2_CENTER_X, y + WHEEL_2_CENTER_Y, WHEEL_INNER_RADIUS, HUB_CLR);
}

//----------------------------------------------------------------------------//
// Car Implementation - End
//----------------------------------------------------------------------------//

// Main display constants
#define BACKGROUND_CLR rgb565(0,60,90)
#define TITLE_CLR GREEN
#define STATUS_CLR WHITE
#define STR_BUF_LEN 12 // string buffer length
#define FONT_SIZE 2
#define FONT_W (LCD_CHAR_W*FONT_SIZE)
#define FONT_H (LCD_CHAR_H*FONT_SIZE)
#define STATUS_W (FONT_W*3)

#define WAIT 2000 // milliseconds
#define DELAY_EX3 20 // milliseconds

// Object position and movement
#define OBJ_X 100
#define OBJ_Y 100
#define OBJ_MOVE 3 // pixels


void app_main(void) {
    ESP_LOGI(TAG, "Start up");
    lcd_init();
    lcd_fillScreen(BACKGROUND_CLR);
    lcd_setFontSize(FONT_SIZE);
    lcd_drawString(0, 0, "Hello World! (lcd)", TITLE_CLR);
    printf("Hello World! (terminal)\n");
    delayMS(WAIT);

    // Exercise 1 - Draw car in one location.
    lcd_fillScreen(BACKGROUND_CLR);
    lcd_drawString(0, 0, "Exercise 1", TITLE_CLR);
    drawCar(OBJ_X, OBJ_Y);
    delayMS(WAIT);

    // Exercise 2 - Draw moving car (Method 1), one pass across display.
    lcd_fillScreen(BACKGROUND_CLR);
    for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) {
        lcd_fillScreen(BACKGROUND_CLR);
        lcd_drawString(0, 0, "Exercise 2", TITLE_CLR);
        drawCar(x, OBJ_Y);

        char str[4];
        sprintf(str, "%3ld", x);
        lcd_drawString(0,LCD_H - FONT_H, str, STATUS_CLR);
    }

    // Exercise 3 - Draw moving car (Method 2), one pass across display.
    lcd_fillScreen(BACKGROUND_CLR);
    lcd_drawString(0, 0, "Exercise 3", TITLE_CLR);
    for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) {
        lcd_fillRect(x - OBJ_MOVE, OBJ_Y, CAR_W, CAR_H, BACKGROUND_CLR);
        drawCar(x, OBJ_Y);

        lcd_fillRect(0, LCD_H - FONT_H, FONT_W * 3, FONT_H, BACKGROUND_CLR);
        char str[4];
        sprintf(str, "%3ld", x);
        lcd_drawString(0, LCD_H - FONT_H, str, STATUS_CLR);
        delayMS(DELAY_EX3);
    }

    // Exercise 4 - Draw moving car (Method 3), one pass across display.
    lcd_frameEnable();

    for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) {
        lcd_fillScreen(BACKGROUND_CLR);
        lcd_drawString(0, 0, "Exercise 4", TITLE_CLR);
        drawCar(x, OBJ_Y);

        char str[4];
        sprintf(str, "%3ld", x);
        lcd_drawString(0, LCD_H - FONT_H, str, STATUS_CLR);

        lcd_writeFrame();
    }

    // Draw an animated Pac-Man moving across the display.
    lcd_frameEnable();
    while (1) {
        uint16_t i = 0;
        const uint8_t pIdx[] = {0, 1, 2, 1};
        for (coord_t x = -PAC_W; x <= LCD_W; x += OBJ_MOVE) {
            lcd_fillScreen(BACKGROUND_CLR);
            lcd_drawString(0, 0, "Exercise 5", TITLE_CLR);

            lcd_drawBitmap(x, OBJ_Y, pac[pIdx[i++ % sizeof(pIdx)]], PAC_W, PAC_H, YELLOW);


            char str[4];
            sprintf(str, "%3ld", x);
            lcd_drawString(0, LCD_H - FONT_H, str, STATUS_CLR);

            lcd_writeFrame();
        }
    }
}
