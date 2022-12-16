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

    const float conversion_factor = 3.3f / (1 << 12);
    uint16_t data[3] = {0, 0, 0};
    float soil, temp, light = 0;
 
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);

    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    // Set Round-Robin ADC reading scheme for ADC0-ADC2
    // This scheme will cycle through adc_select_input and select all of the ADC inputs
    adc_set_round_robin(0x7);

    // ADC0 is soil sensor
    // ADC1 is Temp sensor
    // ADC2 is light sensor
 
    while(59) {
        /*
        for(int i = 0; i < 3; i++) {
            // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
            const float conversion_factor = 3.3f / (1 << 12);
            uint16_t result = adc_read();
            printf("Collecting ADC%d\n", i);
            printf("Raw value: 0x%03x, voltage: %f V\n", result, result*conversion_factor);
            sleep_ms(500);
        }
        */

        data[0] = adc_read(); //soil
        sleep_ms(500);
        data[1] = adc_read(); //temp
        sleep_ms(500);
        data[2] = adc_read(); //light
        sleep_ms(500);

        soil = (float)data[0] * conversion_factor;
        printf("Soil moisture = %f\n", soil);
        temp = ( (float)(data[1]* conversion_factor) - 0.5f )  / 0.01f;
        printf("Temp = %f\n", temp);
        light = (float)data[2] * conversion_factor;
        printf("Light reading = %f\n", light);

        sleep_ms(1000);

        for(int i=0; i<3; i++) //returns cursor to beginning of terminal so that PuTTY outputs isn't overwhelming
        {
            printf("\033[F");
        }

    }
}