#include <pin.h>
#include <stdint.h>
#include <driver/uart.h>
#include <hal/uart_types.h>
#include "hw_gc.h"

#define UART_NUM UART_NUM_2
#define BAUD_RATE 115200

#define TX_PIN HW_EX8
#define RX_PIN HW_EX7

#define RX_BUFFER_SIZE UART_HW_FIFO_LEN(PORT_NUM)*2

// This component is a wrapper around a lower-level communication
// method such as a serial port (UART), Bluetooth, or WiFi. The
// complexities of establishing a communication channel and sending
// bytes through the channel are abstracted away. The read and write
// functions are non-blocking so they can be used in a tick function.

// Initialize the communication channel.
// Return zero if successful, or non-zero otherwise.
int32_t com_init(void) {

    const uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    if (uart_param_config(UART_NUM, &uart_config) != ESP_OK) { return -1; }

    if (uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        return -1;
    }

    if (uart_driver_install(UART_NUM, RX_BUFFER_SIZE, 0, 0, NULL, 0) != ESP_OK) { return -1; }

    pin_pullup(RX_PIN, true);

    return 0;
}

// Free resources used for communication.
// Return zero if successful, or non-zero otherwise.
int32_t com_deinit(void) {
    if (uart_is_driver_installed(UART_NUM)) { //Uninstall if installed
        if (uart_driver_delete(UART_NUM) != ESP_OK) { return -1; }
    }
    return 0;
}

// Write data to the communication channel. Does not wait for data.
// *buf: pointer to data buffer
// size: size of data in bytes to write
// Return number of bytes written, or negative number if error.
int32_t com_write(const void *buf, uint32_t size);

// Read data from the communication channel. Does not wait for data.
// *buf: pointer to data buffer
// size: size of data in bytes to read
// Return number of bytes read, or negative number if error.
int32_t com_read(void *buf, uint32_t size);
