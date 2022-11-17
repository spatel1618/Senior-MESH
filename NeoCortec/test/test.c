/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "NcApi.h"


/// \tag::hello_uart[]

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define UART_CTS_PIN 2
#define UART_RTS_PIN 3

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE


// Global declarations expected by NcApi
tNcApi g_ncApi[1];
uint8_t g_numberOfNcApis = 1;
tNcApiRxHandlers ncRx;

/*Declarations*/

void ExampleSetupNcApi();

/**************/


int main() {
    
	picoSetup();
    ExampleSetupNcApi();

	char msg[5] = {'h', 'e', 'l', 'l', 'o'};
	uint8_t  msgHex[5] = {0x68, 0x65, 0x6C, 0x6C, 0x6F};
	uint8_t msgLen = 5;
	char data[7] = {0,0,0,0,0,0,0};
	uint8_t nodeId = 0x0020;

	uint8_t getNodeInfo[1] = {0x08};
	uint8_t getListReq[1] = {0x09};
	uint8_t getAckPacket[1] = {0x03};
	uint8_t getUnackPacket[1] = {0x02};
	uint8_t lenOne = 1;

	uint8_t destPort = 0x00;	
	uint8_t ackDestNodeId[2] = {0x00, 0x20}; // {MSB, LSB} 

	uint8_t fullAckMsg[10] = {getAckPacket[0], 0x08, ackDestNodeId[0], ackDestNodeId[1], destPort, msgHex[0], msgHex[1], msgHex[2], msgHex[3], msgHex[4]};
	//sending chars DOESNT WORK
	//uint8_t fullAckTestMsg[6] = {0x03, 0x04, 0x00, 0x20, 0x00, 0x23}; //THIS WORKS!!!!!!!!!!!!!

	while(1)
	{
		//sleep_ms(5000); //sleep 5 seconds
		//NcApiSupportTxData(0, getNodeInfo, lenOne);                         //send command for getting node id
		//NcApiSupportTxData(0, getListReq, lenOne);						  //send command to get neighboor list

		//sleep_ms(5000); //sleep 5 seconds

		//NcApiSupportTxData(0, fullAckMsg, 10);

		//NcApiSupportTxData(0, fullAckTestMsg, 6); //THIS WORKS!!!!!!!!!!!

		uart_read_blocking(UART_ID, data, 7);

		for(int i = 0; i<7; i++)
		{
			printf("%c", data[i]);
		}


	}


}

/// \end::hello_uart[]

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

    uart_set_hw_flow(uart0, true, true);

	uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

	// Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

}


/*************************************************/

// Global functions expected by NcApi
NcApiErrorCodes NcApiSupportTxData(uint8_t n, uint8_t * finalMsg, uint8_t finalMsgLength)
{
	// Write to particular UART indexed by n
	for(int i = 0; i<finalMsgLength; i++)
	{
		uart_putc_raw(uart0, finalMsg[i]);
	}

	return NCAPI_OK;
}

void NcApiSupportMessageWritten(uint8_t n, void * callbackToken, uint8_t * finalMsg, uint8_t finalMsgLength)
{
	printf("Message successfully sent to UART %i", n);
}

void NcApiSupportMessageReceived(uint8_t n,void * callbackToken, uint8_t * msg, uint8_t msgLength)
{
	NcApiExecuteCallbacks(n,msg,msgLength);

	printf("Message successfully received by node");
}
 

// Handle CTS and RX interrupts
void ExamplePassthroughCts()
{
	NcApiCtsActive(0);
}

void ExamplePassthroughRx(uint8_t byte)
{
	NcApiRxData(0, byte);
	
}

void ExamplePassthroughNwu()
{
	NcApiCallbackNwuActive(0);
}


// Sample implementation of generic message received handler merely providing the bytes.
// Always called before any of the strongly typed message handlers are called.
void ExampleReadCallback(uint8_t n, uint8_t * msg, uint8_t msgLength)
{
	uint16_t i;
	printf("Raw UART data received: ");
	for (i=0; i<msgLength; i++)
		printf( "%02x ", msg[i] );
	printf("\r\n");
}

void ExampleHostAckCallback(uint8_t n, tNcApiHostAckNack * p)
{
	printf( "My previous package to node node %04x was successfully delivered\r\n", p->originId );
}

void ExampleHostNAckCallback(uint8_t n, tNcApiHostAckNack * p)
{
	printf( "My previous package to node node %04x was not delivered\r\n", p->originId );
}

void ExampleHostDataCallback(uint8_t n, tNcApiHostData * p)
{
	printf
	(
		"I have received %d bytes from node %04x. They are %dms old\r\n",
		p->payloadLength,
		p->originId,
		p->packageAge * 125
	);
}

void ExampleHostDataHapaCallback(uint8_t n, tNcApiHostDataHapa * p)
{
	printf
	(
		"I have received %d bytes from node %x4. They are %dms old\r\n",
		p->payloadLength,
		p->originId,
		((double)p->packageAge) / 524.288
	);
}

void ExampleWesSetupRequestCallback(uint8_t n, tNcApiWesSetupRequest * p)
{
	n;
}

void ExampleWesStatusCallback(uint8_t n, tNcApiWesStatus * p)
{
	n;
}

// Sample implementation of how to send one Unacknowledged command message
void ExampleSendUnacknowledged(uint16_t destNodeId, uint8_t port, uint16_t appSeqNo, uint8_t * payload, uint8_t payloadLen)
{
	tNcApiSendUnackParams args;
	NcApiErrorCodes apiStatus;
	args.msg.destNodeId = destNodeId;
	args.msg.destPort = port;
	args.msg.appSeqNo = appSeqNo;
	args.msg.payload = payload;
	args.msg.payloadLength = payloadLen;
	args.callbackToken = &g_ncApi;
	apiStatus = NcApiSendUnacknowledged(0, &args);
	if (apiStatus != NCAPI_OK)
	{
		; // Application specific
	}
}

// Sample implementation of how to send one Acknowledged command message
void ExampleSendAcknowledged(uint16_t destNodeId, uint8_t port, uint8_t * payload, uint8_t payloadLen)
{
	tNcApiSendAckParams args;
	NcApiErrorCodes apiStatus;
	args.msg.destNodeId = destNodeId;
	args.msg.destPort = port;
	args.msg.payload = payload;
	args.msg.payloadLength = payloadLen;
	args.callbackToken = &g_ncApi;
	apiStatus = NcApiSendAcknowledged(0, &args);
	if (apiStatus != NCAPI_OK)
	{
		; // Application specific
	}
}

// Sample implementation of how to send one WesCmd
void ExampleSendWesCmd(NcApiWesCmdValues cmd)
{
	tNcApiWesCmdParams args;
	args.msg.cmd = cmd;
	args.callbackToken = &g_ncApi;
	NcApiSendWesCmd( 0, &args );
}

// Sample implementation of how to send one WesResponse
void ExampleSendWesResponse(uint64_t uid, uint16_t nodeId)
{
	tNcApiWesResponseParams args;
	args.msg.uid[0] = (uid >> 32) & 0xff;
	args.msg.uid[1] = (uid >> 24) & 0xff;
	args.msg.uid[2] = (uid >> 16) & 0xff;
	args.msg.uid[3] = (uid >> 8) & 0xff;
	args.msg.uid[4] = uid & 0xff;
	args.msg.nodeId = nodeId;
	args.callbackToken = &g_ncApi;
	NcApiSendWesResponse( 0, &args );
}

// Sample implementation of how to setup global rx-handlers and initialize NcApi.
void ExampleSetupNcApi()
{
	tNcApiRxHandlers * rxHandlers = &ncRx;
	memset( rxHandlers, 0, sizeof(tNcApiRxHandlers));

	rxHandlers->pfnReadCallback = ExampleReadCallback;
	rxHandlers->pfnHostAckCallback = ExampleHostAckCallback;
	rxHandlers->pfnHostNAckCallback = ExampleHostNAckCallback;
	rxHandlers->pfnHostDataCallback = ExampleHostDataCallback;
	rxHandlers->pfnHostDataHapaCallback = ExampleHostDataHapaCallback;
	rxHandlers->pfnWesSetupRequestCallback = ExampleWesSetupRequestCallback;
	rxHandlers->pfnWesStatusCallback = ExampleWesStatusCallback;
	
	memset( (void*)&g_ncApi[0], 0, sizeof(tNcApi) );
	
	g_ncApi[0].NcApiRxHandlers=&ncRx;
}
