#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

/* #################################################################################################################
    Description: This script does the following:
                -Collects Raspberry Pi Pico sensor data with 3 ADCs (Pins GP26-GP28)
                -Sends readable sensor data to a NeoMesh node via UART (Tx = GP4, Rx = GP5)
                -Repeats this process over a specified time interval

    ADC References:
    - https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__adc.html#ga5d3b8875947df2f4ba022cb6aa40594b
################################################################################################################# */
 
int main() {
    stdio_init_all();
    printf("ADC Example, measuring GPIO26\n");
 
    adc_init();
 
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);

    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    // Set Round-Robin ADC reading scheme for ADC0-ADC2
    // This scheme will cycle through adc_select_input and select all of the ADC inputs
    adc_set_round_robin(0x7);

 
    while(59) {

        uint8_t i;
        for(i = 0; i < 3; i++) {
            // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
            const float conversion_factor = 3.3f / (1 << 12);
            uint16_t result = adc_read();
            printf("Collecting ADC%d\n", i);
            printf("Raw value: 0x%03x, voltage: %f V\n", result, result*conversion_factor);
            sleep_ms(500);
        }
        sleep_ms(1000);
    }
}