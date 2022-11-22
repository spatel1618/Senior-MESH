/*
Copyright (c) 2015, NeoCortec A/S
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "NcApi.h"
#include "NeoParser.h"


// This example configures one instance of NcApi using dedicated call back functions.
// It shows a bare minimum implementation of what is required in order to receive and send messages.
// As this example actually is part of the unit-test tool for NcApi, some printing and logging of actions and messages is included in the callbacks as well.
// These should, of course, be left out in a real-world implementation.
// It is recommened to walk through ExampleSetupNcApi() and ExampleSendAcknowledged() defined at the end of this file.


typedef union
{
tNcApiHostAckNack 		NcApiAckNack;
tNcApiHostData 			NcApiHostData;
tNcApiHostDataHapa 		NcApiHostDataHapa;
tNcApiHostUappData 		NcApiHostDataUapp;
tNcApiHostUappDataHapa 	NcApiHostDataUappHapa;
tNcApiWesStatus 		NcApiWesStatus;
tNcApiNodeInfoReply		NcApiNodeInfo;
tNcApiNeighborListReply	NcApiNeighborList;
tNcApiNetCmdReply		NcApiNetCmdResponse;
tNcApiWesSetupRequest	NcApiWesSetupRequest;
}tNcApiAllMessages;

//


// Global declarations expected by NcApi
tNcApi g_ncApi[1];
uint8_t g_numberOfNcApis = 1;

// Global functions expected by NcApi
NcApiErrorCodes NcApiSupportTxData(uint8_t n, uint8_t * finalMsg, uint8_t finalMsgLength)
{
	// Write to particular UART indexed by 
	return NCAPI_OK;
}

void NcApiSupportMessageWritten(uint8_t n, void * callbackToken, uint8_t * finalMsg, uint8_t finalMsgLength)
{
	
}

void NcApiSupportMessageReceived(uint8_t n,void * callbackToken, uint8_t * msg, uint8_t msgLength)
{
tNcApiAllMessages NcApiMsg;	

	switch(msg[0])
	{
	case HostAckEnum:
		NcApiGetMsgAsHostAck(msg, &NcApiMsg.NcApiAckNack);
		break;
	case HostNAckEnum:
		NcApiGetMsgAsHostAck(msg, &NcApiMsg.NcApiAckNack);
		break;
	case HostDataEnum:
		NcApiGetMsgAsHostData(msg, &NcApiMsg.NcApiHostData);
		break;
	case HostDataHapaEnum:
		NcApiGetMsgAsHostDataHapa(msg, &NcApiMsg.NcApiHostDataHapa);
		break;
	case HostUappDataEnum:
		NcApiGetMsgAsHostUappData(msg, &NcApiMsg.NcApiHostDataUapp);
		break;
		
	case HostUappDataHapaEnum:
		NcApiGetMsgAsHostUappDataHapa(msg, &NcApiMsg.NcApiHostDataUappHapa);
		break;
	case NodeInfoReplyEnum:
		NcApiGetMsgAsNodeInfoReply(msg, &NcApiMsg.NcApiNodeInfo);
		break;
	case NeighborListReplyEnum:
		NcApiGetMsgAsNeighborListReply(msg, &NcApiMsg.NcApiNeighborList);
		break;
	case NetCmdReplyEnum:
		NcApiGetMsgAsNetCmdResponse(msg, &NcApiMsg.NcApiNetCmdResponse);
		break;
	case WesStatusEnum:
		NcApiGetMsgAsWesStatus(msg, &NcApiMsg.NcApiWesStatus);
		break;
	case WesSetupRequestEnum:
		NcApiGetMsgAsWesSetupRequest(msg, &NcApiMsg.NcApiWesSetupRequest);
		break;
		
		
	default:
		// unhandled 
		break;
	}
	
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
	NcApiInit();
}
