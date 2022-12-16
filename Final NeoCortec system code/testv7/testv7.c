/* Description: This script programs a NeoCortec coordinator/sink node
                such that it will do the following:
				- Read hopped data
				- Convert the raw ADC data from hopped data into readable information
				- Send the readable information to an SD card.
				
				Users must specify the YOUR_LENGTH and ARRAY_LENGTH defines so that 
				uart_read_blocking does not infinitely hang while waiting for data.
				
				Users may also want to edit the raw ADC data to voltage handling;
				The current setup ensures that soil data, then temperature data, then light data is received.
				Specifically, soil[15:8], soil[7:0], temp[15:7]... is the order */
				
// UART and Api includes
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

// SD Card includes
#include "sd_card.h"
#include "ff.h"
#include "hw_config.h"
#include "diskio.h"

// UART
#define UART_ID      uart0
#define BAUD_RATE    115200
#define UART_TX_PIN  0
#define UART_RX_PIN  1
#define UART_CTS_PIN 18 // 2
#define UART_RTS_PIN 19 // 3
#define DATA_BITS    8
#define STOP_BITS    1
#define PARITY       UART_PARITY_NONE

/* Make this the length of your payload;
   This is used for parsing when printing
   Our debugging: We read 3 ADCs into 16-bit values -> YOUR_LENGTH = 6 */

#define YOUR_LENGTH 6 //3 

/* Make this the appropriate number for uart_read_blocking;
   1 byte payload -> arrayLength = 8 */
#define ARRAY_LENGTH 8 + YOUR_LENGTH // 7

// Array for holding data received from the mesh
char data[ARRAY_LENGTH];
uint16_t sensorData[YOUR_LENGTH];
char nodeData[2];

// SD card
FRESULT fr;
FATFS fs;
FIL fil;
int ret;
char buf[50]; // edit this to prevent stalling
char filename[] = "test02.txt";

// Function Prototypes
void picoSetup();
void SDCardInit();
void retCheck();
void openFile();
void closeFile();
void fillData();

int main() {
    
	// Initialize the Pico ad SDCard read/write
	picoSetup();
	SDCardInit();

	// Variables for raw ADC readings
	uint16_t soil_raw, temp_raw, light_raw = 0;
	uint8_t temp_final_int, temp_final_decimal;
	float soil_final, light_final, temp_final = 0;

	uint8_t count=0;

	// 12-bit ADC
	const float conversion_factor = 3.3f / (float)4096;//(1 << 12);

	sleep_ms(5000);

	while(59) {
		//fills data array, sorted sensor data into sensorData array, pulls out nodeData info
		fillData();

		openFile();

		for(int i = 0; i<ARRAY_LENGTH; i++) {
			// Print out every byte of data received. This is useful for validation but can be removed
			ret = f_printf(&fil,"Data[%d]: %x \n", i, data[i]);
			
			// This check is repeatedly performed below to ensure that the data is valid.
			// It could probably be removed since uart_read_blocking waits until all data is received
			retCheck();
		}
		
		ret = f_printf(&fil, "\n");
		retCheck();
		
		// Identify the node of the sensor that is hopping data
		ret = f_printf(&fil,"The following data is from Node %x%x: \n", nodeData[0], nodeData[1]);
		retCheck();

		/* Convert the payload data (raw ADC readings) into voltages
		   and write meaningful messages to the SD card 
		
		soil_raw |= (data[7] << 8) | (data[8]);
		soil_final = soil_raw * conversion_factor;

		temp_raw |= (data[9] << 8) | (data[10]);
		temp_raw *= conversion_factor;
		temp_final = ((float)temp_raw - 0.5f) / 0.01f;
		
		temp_final_int = (uint8_t) temp_final;
		temp_final_decimal = (uint8_t)((temp_final - temp_final_int)*100);

		light_raw |= (data[11] << 8) | (data[12]);
		light_final = light_raw * conversion_factor; */

		soil_raw |= (sensorData[0] << 8) | (sensorData[1]);
		soil_final = (float)soil_raw * conversion_factor;

		temp_raw |= (sensorData[2] << 8) | (sensorData[3]);
		temp_final = (((float)temp_raw * conversion_factor) - 0.5f) / 0.01f;
		printf("temp_final = %f\n", temp_final);
		
		temp_final_int = (uint8_t)(floor(temp_final));
		temp_final_decimal = (uint8_t)(floor((temp_final - temp_final_int)*100));
		

		light_raw |= (sensorData[4] << 8) | (sensorData[5]);
		light_final = (float)light_raw * conversion_factor;


		// Temperature message
		ret = f_printf(&fil,"Temperature: %d.%d°C \n", temp_final_int, temp_final_decimal);
		retCheck();
		
		// Soil message
		ret = f_printf(&fil,"Soil sensor raw reading: 0x%x \n", soil_raw);
		retCheck();

		// Light message
		if (light_final < 0.2) {
			ret = f_printf(&fil,"Light level: Dark \n");
			retCheck();
		}

		else if (light_final >= 0.2 && light_final < 0.6) {
			ret = f_printf(&fil,"Light level: Medium-Bright \n");
			retCheck();
		}

		else if (light_final >= 0.6) {
			ret = f_printf(&fil,"Light level: Bright \n");
			retCheck();
		}

		// Make clear separations between each data reception
		// This can probably be written better
		
		ret = f_printf(&fil,"\n");
		retCheck();
		ret = f_printf(&fil,"****************************\n");
		retCheck();

/*		//clear variables for next transmission
		soil_raw = 0;
		temp_raw = 0;
		light_raw = 0;
*/

/*		//keeps track of transmission count for debugging
		ret = f_printf(&fil,"count=%d\n", count);
		retCheck();
		count++;
		
*/
		closeFile();
		
		// Unmount drive
		//f_unmount("0:");

	} 

}

/*************************************************/

void picoSetup()
{

	// Set up our UART with the required speed.
	stdio_init_all(); // NEED FOR COM PORT TO SHOW UP!!!!!!!!!!!!!!!!!!!
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_CTS_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RTS_PIN, GPIO_FUNC_UART);

    uart_set_hw_flow(UART_ID, true, true);

	uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

	// Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

}

/*************************************************/


//****************SD Card Functions****************

void spi0_dma_isr();

// Hardware Configuration of SPI "objects"
// Note: multiple SD cards can be driven by one SPI if they use different slave
// selects.
static spi_t spis[] = {  // One for each SPI.
    {
        .hw_inst = spi0,  // SPI component
        .miso_gpio = 4,//16, // GPIO number (not pin number)
        .mosi_gpio = 3, //19,
        .sck_gpio = 2, // 18,

        /* The choice of SD card matters! SanDisk runs at the highest speed. PNY
           can only mangage 5 MHz. Those are all I've tried. */
        //.baud_rate = 400 * 1000,
        .baud_rate = 1000 * 1000,
        //.baud_rate = 12500 * 1000,  // The limitation here is SPI slew rate.
        //.baud_rate = 25 * 1000 * 1000, // Actual frequency: 20833333. Has
        // worked for me with SanDisk.

        .dma_isr = spi0_dma_isr
    }
};

// Hardware Configuration of the SD Card "objects"
static sd_card_t sd_cards[] = {  // One for each SD card
    {
        .pcName = "0:",           // Name used to mount device
        .spi = &spis[0],          // Pointer to the SPI driving this card
        .ss_gpio = 5, //17,            // The SPI slave select GPIO for this SD card
        .use_card_detect = true,
        .card_detect_gpio = 22,   // Card detect
        .card_detected_true = 1,  // What the GPIO read returns when a card is
                                  // present. Use -1 if there is no card detect.
        .m_Status = STA_NOINIT
    }
};

void spi0_dma_isr() { 
	spi_irq_handler(&spis[0]); 
}

/* ********************************************************************** */
size_t sd_get_num() { return count_of(sd_cards); }
sd_card_t *sd_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &sd_cards[num];
    } else {
        return NULL;
    }
}
size_t spi_get_num() { return count_of(spis); }
spi_t *spi_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &spis[num];
    } else {
        return NULL;
    }
}

/************Custom**************/

void SDCardInit()
{
	// Initialize SD card
	if (!sd_init_driver()) {
		printf("ERROR: Could not initialize SD card\r\n");
		while (true);
	}

	// Mount drive
	fr = f_mount(&fs, "0:", 1);
	if (fr != FR_OK) {
		printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
		while (true);
	}
}

void retCheck()
{
	if (ret < 0) {
				printf("ERROR: Could not write to file (%d)\r\n", ret);
				f_close(&fil);
				while (true);
			}
}

void openFile()
{
	// Open file for writing ()
		fr = f_open(&fil, filename, FA_WRITE | FA_OPEN_APPEND);
		if (fr != FR_OK) {
			printf("ERROR: Could not open file (%d)\r\n", fr);
			while (true);
		}
}

void closeFile()
{
	// Close file
		fr = f_close(&fil);
		if (fr != FR_OK) {
			printf("ERROR: Could not close file (%d)\r\n", fr);
			while (true);
		} 
}

void fillData()
{
	// Have the Pico read data from the Neocortec node's pin
	printf("The Pico is waiting for UART data. \n");

	uart_read_blocking(UART_ID, data, ARRAY_LENGTH);

	printf("The Pico has read all the data. \n");

	if(data[0] == 0x52){ //in a normal transmission, 0x52 is the first byte sent
		for(int i=0; i<YOUR_LENGTH; i++){
			sensorData[i] = (uint16_t)data[i+7];
		}
		nodeData[0] = data[2];
		nodeData[1] = data[3];
	}
	else if(data[0] == 0xFF){ //transmission issues arise where the last byte is sent as the first transmission to the Pico, so we need a case for this where all data is one element off
		for(int i=0; i<YOUR_LENGTH; i++){
			sensorData[i] = (uint16_t)data[i+8];
		}
		nodeData[0] = data[3];
		nodeData[1] = data[4];
	}
	else{
		printf("ERROR in reading and sorting data.");
		for(int i=0; i<YOUR_LENGTH; i++){
			sensorData[i] = 0;
		}
		nodeData[0] = 0;
		nodeData[1] = 0;
	}

	//for debugging, but also does not print to SD card with out this forsome reason
	for(int i=0; i<YOUR_LENGTH; i++){
		printf("sensorData[%d] = %x\n", i, sensorData[i]);
	}

	printf("nodeData[0] = %x\n", nodeData[0]);
	printf("nodeData[1] = %x\n", nodeData[1]);

}