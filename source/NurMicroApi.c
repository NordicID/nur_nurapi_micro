/* 
Copyright (c) 2017 Nordic ID.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "NurApiConfig.h"
#include "NurMicroApi.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifdef HAVE_NUR_STRNCPY
char *nurStrncpy(char *destination, const char *source, int num)
{	
	int i;
	int sourceEnd = 0;
	for (i=0; i<num; i++)
	{
		if (sourceEnd == 0 && source[i] == '\0')
			sourceEnd = 1;

		if (sourceEnd)
			destination[i] = '\0';
		else
			destination[i] = source[i];
	}
	return destination;
}
#endif

#ifdef HAVE_NUR_MEMSET
void *nurMemset(void *destination, int val, int num)
{	
	unsigned char *dstPtr = (unsigned char*)destination;
	while (num--)
	{
		*dstPtr++ = (unsigned char)val;
	}
	return destination;
}
#endif

#ifdef HAVE_NUR_MEMCPY
void *nurMemcpy(void *destination, const void *source, int num)
{	
	unsigned char *srcPtr = (unsigned char*)source;
	unsigned char *dstPtr = (unsigned char*)destination;
	while (num--)
	{
		*dstPtr++ = *srcPtr++;
	}
	return destination;
}
#endif

static int TranslateTagError(BYTE tagError)
{
	switch (tagError)
	{
		case 3: return NUR_ERROR_G2_TAG_MEM_OVERRUN;
		case 4: return NUR_ERROR_G2_TAG_MEM_LOCKED;
		case 0xB: return NUR_ERROR_G2_TAG_INSUF_POWER;
		case 0xF: return NUR_ERROR_G2_TAG_NON_SPECIFIC;
		default: break;
	}
	return NUR_ERROR_G2_TAG_RESP;
}

#ifdef HAVE_ERROR_MESSAGES
struct NUR_ERRORMSG
{
	int error;
	const char *msg;
};

static struct NUR_ERRORMSG errors[] = 
{
	{ NUR_SUCCESS, "Call succeeded" },
	{ NUR_NO_ERROR, "Call succeeded" },
	{ NUR_ERROR_INVALID_COMMAND, "Invalid command sent to module" },
	{ NUR_ERROR_INVALID_LENGTH, "Invalid packet length sent to module" },
	{ NUR_ERROR_PARAMETER_OUT_OF_RANGE, "Command parametr(s) out of range" },
	{ NUR_ERROR_RECEIVE_TIMEOUT, "Data receive timeout" },
	{ NUR_ERROR_INVALID_PARAMETER, "Invalid command parameter(s); Invalid function parameter(s)" },
	{ NUR_ERROR_PROGRAM_FAILED, "Programming failure" },
	{ NUR_ERROR_PARAMETER_MISMATCH, "Parameter mismatch" },
	{ NUR_ERROR_HW_MISMATCH, "HW mismatch" },
	{ NUR_ERROR_RESERVED1, "RESERVED1" },
	{ NUR_ERROR_PAGE_PROGRAM, "Page programming failure" },
	{ NUR_ERROR_CRC_CHECK, "Memory check failed" },
	{ NUR_ERROR_CRC_MISMATCH, "CRC mismatch in parameter" },
	{ NUR_ERROR_NOT_READY, "Device not ready or region that is being programmed is not unlocked" },
	{ NUR_ERROR_APP_NOT_PRESENT, "Module application not present" },
	{ NUR_ERROR_GENERAL, "Generic, non-interpreted / unexpected error" },
	{ NUR_ERROR_NO_TAG, "No tag(s) found" },
	{ NUR_ERROR_RESP_AIR, "Air error" },
	{ NUR_ERROR_G2_SELECT, "G2 select error" },
	{ NUR_ERROR_MISSING_SELDATA, "G2 select data missing" },
	{ NUR_ERROR_G2_ACCESS, "G2 access error" },
	{ NUR_ERROR_G2_READ, "G2 Read error, unspecified" },
	{ NUR_ERROR_G2_RD_PART, "G2 Partially successful read" },
	{ NUR_ERROR_G2_WRITE, "G2 Write error, unspecified" },
	{ NUR_ERROR_G2_WR_PART, "G2 Partially successful write" },
	{ NUR_ERROR_G2_TAG_RESP, "G2 Tag read responded w/ error" },
	{ NUR_ERROR_G2_SPECIAL, "Special error; Some additional debug data is returned with this error" },
	{ NUR_ERROR_READER_HW, "HW error" },
	{ NUR_ERROR_INVALID_HANDLE, "Invalid handle passed to function" },
	{ NUR_ERROR_TRANSPORT, "Transport error" },
	{ NUR_ERROR_TR_NOT_CONNECTED, "Transport not connected" },
	{ NUR_ERROR_TR_TIMEOUT, "Transport timeout" },
	{ NUR_ERROR_BUFFER_TOO_SMALL, "Buffer too small" },
	{ NUR_ERROR_NOT_SUPPORTED, "Functionality not supported" },
	{ NUR_ERROR_NO_PAYLOAD, "Packet contains no payload" },
	{ NUR_ERROR_INVALID_PACKET, "Packet is invalid" },
	{ NUR_ERROR_PACKET_TOO_LONG, "Packet too long" },
	{ NUR_ERROR_PACKET_CS_ERROR, "Packet Checksum failure" },
	{ NUR_ERROR_NOT_WORD_BOUNDARY, "Data not in WORD boundary" },
	{ NUR_ERROR_FILE_NOT_FOUND, "File not found" },
	{ NUR_ERROR_FILE_INVALID, "File format is invalid" },
	{ NUR_ERROR_MCU_ARCH, "MCU architecture mismatch" },
	{ NUR_ERROR_G2_TAG_MEM_OVERRUN, "The specified memory location does not exists or the EPC length field is not supported by the tag" },
	{ NUR_ERROR_G2_TAG_MEM_LOCKED, "The specified memory location is locked and/or permalocked and is either not writeable or not readable" },
	{ NUR_ERROR_G2_TAG_INSUF_POWER, "The tag has insufficient power to perform the memory-write operation" },
	{ NUR_ERROR_G2_TAG_NON_SPECIFIC, "The tag does not support error-specific codes" },
	{ NUR_ERROR_TR_SUSPENDED, "Transport suspended error" },
};
static int errorsSize = sizeof(errors) / sizeof(errors[0]);

const char * NURAPICONV NurApiGetErrorMessage(int error)
{
	int n;
	for (n=0; n<errorsSize; n++) {
		if (errors[n].error == error) {
			return errors[n].msg;			
		}
	}
	return "Unknown Error";
}

#endif

#ifdef IMPLEMENT_CRC16

#define CRC16_START				0xFFFF
#define MSG_CCITT_CRC_POLY		0x1021

#ifdef HAVE_CRC16_LOOKUP

static WORD crctable[256];
static BOOL init = FALSE;

static WORD NurCRC16(WORD crc, BYTE *buf, DWORD len)
{
	if (!init)
	{
		int i, j;
		ULONG c;

		for (i = 0; i < 256; i++) {
			c = i << 8;
			for ( j = 0; j < 8; j++ ) {
				c = (c & 0x8000) ? MSG_CCITT_CRC_POLY ^ (c << 1) : (c << 1);
			}
			crctable[i] = (WORD)c;
		}

		init = TRUE;
	}

	while (len--) {
		crc = (crc << 8) ^ crctable[(crc >> 8) ^ *buf++];
	}
	return crc;
}

#else

// Inlining the update call.
#define INLINE_CRC16_UPDATE

#ifdef INLINE_CRC16_UPDATE
static WORD __inline crc_16_update(WORD crc, BYTE data)
#else
static WORD crc_16_update(WORD crc, BYTE data)
#endif
{
	int i;

	crc = crc ^ ((WORD)data << 8);
	for (i=0; i<8; i++)
	{
		if (crc & 0x8000)
			crc = (crc << 1) ^ MSG_CCITT_CRC_POLY;
		else
			crc <<= 1;
	}

	return crc;
}

static WORD NurCRC16(WORD crc, BYTE *buf, DWORD len)
{
	WORD wCrc = crc;

	while (len--) {
		wCrc = crc_16_update(wCrc, *buf++);
	}

	return wCrc;
}

#endif	// CRC-16 lookup / not.

#endif	// CRC-16 included.

#define TxHeaderDataPtr		(hNurApi->TxBuffer)
#define TxHeaderPtr			((struct NUR_HEADER *)TxHeaderDataPtr)
#define TxPayloadCmdPtr		(TxHeaderDataPtr + (HDR_SIZE)) // + header
#define TxPayloadDataPtr	(TxHeaderDataPtr + (HDR_SIZE+1)) // + header + cmd

#define RxHeaderDataPtr		(hNurApi->RxBuffer)
#define RxHeaderPtr			((struct NUR_HEADER *)RxHeaderDataPtr)
#define RxPayloadCmdPtr		(RxHeaderDataPtr + (HDR_SIZE)) // + header
#define RxPayloadDataPtr	(RxHeaderDataPtr + (HDR_SIZE+1)) // + header + cmd
#define RxPayloadLen		(RxHeaderPtr->payloadlen - 2 - 1 - 1) // - CRC - cmd - status

#define STATE_IDLE 			1
#define STATE_HDR 			2
#define STATE_PAYLOAD 		3
#define STATE_PACKETREADY	4

static BYTE packetHandlerState = STATE_IDLE;

int NurApiHandlePacketData(struct NUR_API_HANDLE *hNurApi, DWORD bytesToProcess)
{
	BYTE *trBuf = hNurApi->TxBuffer;

	while (bytesToProcess-- > 0) 
	{
		hNurApi->RxBuffer[hNurApi->RxBufferUsed++] = *trBuf++;

		switch (packetHandlerState)
		{
		case STATE_IDLE:
			if (hNurApi->RxBuffer[0] == 0xA5)
			{
				// Got header marker
				packetHandlerState = STATE_HDR;
			}
			else 
			{
				// Invalid header, just ignore data
				hNurApi->RxBufferUsed = 0;
			}
			break;

		case STATE_HDR:
			// Wait for header completely received
			if (hNurApi->RxBufferUsed == HDR_SIZE) 
			{
				// Calculate header checksum
				BYTE headerChecksum = 0xFF;
				headerChecksum ^= (	hNurApi->RxBuffer[0] ^ 
									hNurApi->RxBuffer[1] ^ 
									hNurApi->RxBuffer[2] ^ 
									hNurApi->RxBuffer[3] ^ 
									hNurApi->RxBuffer[4]);

				// Validate checksum
				if (headerChecksum == RxHeaderPtr->checksum)
				{
					// Valid header received, go to payload state
					packetHandlerState = STATE_PAYLOAD;
				}
				else 
				{
					// Invalid header checksum, return to idle..
					hNurApi->RxBufferUsed = 0;
					packetHandlerState = STATE_IDLE;
				}
			}
			break;

		case STATE_PAYLOAD:
			// Wait for packet completely received
			if (hNurApi->RxBufferUsed == (DWORD)(RxHeaderPtr->payloadlen + HDR_SIZE))
			{
				WORD calcCRC = 0xFFFF;
				WORD packetCRC = 0;
				BYTE *payloadBuffer = RxPayloadCmdPtr;

				// Calculate payload CRC, last WORD contains CRC16 calculated by module
				calcCRC = NurCRC16(calcCRC, payloadBuffer, RxHeaderPtr->payloadlen - 2);

				// Validate CRC
				packetCRC = BytesToWord(&payloadBuffer[RxHeaderPtr->payloadlen-2]);
				if (packetCRC == calcCRC)
				{
					// Payload valid, we're ready
					packetHandlerState = STATE_PACKETREADY;

					// Store response info
					hNurApi->respLen = RxHeaderPtr->payloadlen-1-1-2; // - cmd - status - CRC
					hNurApi->resp = (struct NUR_CMD_RESP *)RxPayloadCmdPtr;
				}
				else
				{
					// Invalid payload CRC, return to idle..
					hNurApi->RxBufferUsed = 0;
					packetHandlerState = STATE_IDLE;
				}
			}
			break;

		case STATE_PACKETREADY:
			// Packet ready, buffer data
			break;
		}

		if (packetHandlerState == STATE_PACKETREADY)
			break;
	}

	if (packetHandlerState == STATE_PACKETREADY)
	{
		// Packet ready, return to idle
		hNurApi->RxBufferUsed = 0;
		packetHandlerState = STATE_IDLE;
		return STATE_PACKETREADY;
	}

	return packetHandlerState;
}

static BYTE CalculateHeaderCheckSum(BYTE *buf)
{
	int len = HDR_SIZE - 1;
	BYTE checksum = CS_STARTBYTE;
	int i;
	for (i=0; i<len; i++) {
		checksum ^= buf[i];
	}
	return checksum;
}

int NURAPICONV NurApiSetupPacket(struct NUR_API_HANDLE *hNurApi, BYTE cmd, WORD payloadLen, WORD flags, WORD *packetLen)
{
	WORD payloadCRC;
	WORD payloadLenWithoutCRC;

	// Setup packet header
	TxHeaderPtr->start = PACKET_START;
	TxHeaderPtr->flags = flags;
	TxHeaderPtr->payloadlen = payloadLen + 1 + 2; // + cmd + CRC	

	if (TxHeaderPtr->payloadlen > NUR_MAX_SEND_SZ) 
		return NUR_ERROR_PACKET_TOO_LONG;

	TxHeaderPtr->checksum = CalculateHeaderCheckSum(TxHeaderDataPtr);

	TxPayloadCmdPtr[0] = cmd;

	// Calculate CRC for whole payload, including CMD
	payloadLenWithoutCRC = payloadLen + 1;
	payloadCRC = NurCRC16(CRC16_START, TxPayloadCmdPtr, payloadLenWithoutCRC);

	// Store CRC
	PacketWordPos(TxPayloadDataPtr, payloadCRC, payloadLenWithoutCRC-1);

	// Whole packet length, including header, cmd, payload and CRC
	*packetLen = (HDR_SIZE + TxHeaderPtr->payloadlen);

	return NUR_SUCCESS;
}

int NURAPICONV NurApiXchPacket(struct NUR_API_HANDLE *hNurApi, BYTE cmd, WORD payloadLen, int timeout)
{
	int error;
	DWORD bytesOutput = 0;
	int packetState = STATE_IDLE;
	WORD packetLen;
	//BYTE tmpRxBuf[32];

	if (cmd != 0)
	{
		error = NurApiSetupPacket(hNurApi, cmd, payloadLen, 0, &packetLen);
		if (error != NUR_SUCCESS) 
			return error;

		// Write packet to module
		// TODO: Handle fragmented write
		error = hNurApi->TransportWriteDataFunction(hNurApi, hNurApi->TxBuffer, packetLen, &bytesOutput);
		if (error != NUR_SUCCESS)
			return error;
	}

WAITMORE:
	//nurMemset(hNurApi->TxBuffer, 0, hNurApi->TxBufferLen);

	packetHandlerState = STATE_IDLE;
	packetState = STATE_IDLE;
	hNurApi->RxBufferUsed = 0;

	// Wait and read response from module
	// NurApiHandlePacketData() function handles fragmented packet validation
	while (timeout-- > 0)
	{
		// Read data
		bytesOutput = 0;
		error = hNurApi->TransportReadDataFunction(hNurApi, hNurApi->TxBuffer, hNurApi->TxBufferLen, &bytesOutput);		
		if (error != NUR_SUCCESS && error != NUR_ERROR_TR_TIMEOUT) {
			// Transport error
			return error;
		}

		if (bytesOutput > 0)
		{
			// Handle incoming data.
			// NOTE: Data may come in pieces.
			packetState = NurApiHandlePacketData(hNurApi, bytesOutput);
			if (packetState == STATE_PACKETREADY)
			{
				// We're done
				break;
			}
		}
	}

	if (packetState != STATE_PACKETREADY)
	{
		// Packet was not ready within timeout
		return NUR_ERROR_TR_TIMEOUT;
	}	

	if (RxHeaderPtr->flags & PACKET_FLAG_UNSOL)
	{
		if (hNurApi->UnsolEventHandler)
		{
			hNurApi->UnsolEventHandler(hNurApi);
		}
	}

	if (cmd == 0)
	{
		// Waiting for event's only..
		return 0;
	}

	// Make sure packet is meant for us
	if (hNurApi->resp->cmd != cmd)
	{
		// Hmm. Response was not meant for this cmd.
		if (!(RxHeaderPtr->flags & PACKET_FLAG_UNSOL)) {
			// TODO: Pass to unexpected packet handler
			//printf("unexpected packet handler %d %d; to %d\n", hNurApi->resp->cmd, cmd, timeout);
		}
		goto WAITMORE;
	}

	// Succeeded!
	return hNurApi->resp->status;
}

int NURAPICONV NurApiPing(struct NUR_API_HANDLE *hNurApi)
{
	return NurApiXchPacket(hNurApi, NUR_CMD_PING, 0, DEF_TIMEOUT);
}

int NURAPICONV NurApiWaitEvent(struct NUR_API_HANDLE *hNurApi, int timeout)
{
	return NurApiXchPacket(hNurApi, 0, 0, timeout);
}

int NURAPICONV NurApiGetVersions(struct NUR_API_HANDLE *hApi)
{
	return NurApiXchPacket(hApi, NUR_CMD_VERSIONEX, 0, DEF_TIMEOUT);
}

int NURAPICONV NurApiGetReaderInfo(struct NUR_API_HANDLE *hNurApi)
{
	struct NUR_CMD_READERINFO_RESP ri;
	int error;
	WORD pos;
	BYTE *ptr;	

	// Reset response
	nurMemset(&ri, 0, sizeof(ri));

	// Exchange data with module
	error = NurApiXchPacket(hNurApi, NUR_CMD_GETREADERINFO, 0, DEF_TIMEOUT);
	LOGIFERROR(error);

	if (error != NUR_SUCCESS) 
	{
		return error;
	}

	// Setup read pointers
	pos = 0;
	ptr = hNurApi->resp->rawdata;

	// Read reader info version
	ri.version = BytesToDword(&ptr[pos]);
	if (ri.version == NUR_READERINFO_VERSION1)
	{
		pos += sizeof(DWORD);
	} 
	else 
	{
		// Really old modules sends reader info without version..
		ri.version = 1;
	}

	// Read serial number len 
	ri.serialLen = ptr[pos++];
	// Read serial number string
	nurMemcpy(ri.serial, &ptr[pos], ri.serialLen);
	pos += ri.serialLen;

	if (ri.version == NUR_READERINFO_VERSION1)
	{
		// Version 1 and up ontains ALT serial also
		ri.altSerialLen = ptr[pos++];
		nurMemcpy(ri.altSerial, &ptr[pos], ri.altSerialLen);
		pos += ri.altSerialLen;
	} 

	// Read module name len
	ri.nameLen = ptr[pos++];
	// Read module name string
	nurMemcpy(ri.name, &ptr[pos], ri.nameLen);
	pos += ri.nameLen;

	// Read FCCID len
	ri.fccIdLen = ptr[pos++];
	// Read FCCID string
	nurMemcpy(ri.fccId, &ptr[pos], ri.fccIdLen);
	pos += ri.fccIdLen;

	// Read HW version len
	ri.hwVersionLen = ptr[pos++];
	// Read HW version string
	nurMemcpy(ri.hwVersion, &ptr[pos], ri.hwVersionLen);
	pos += ri.hwVersionLen;

	// Read SW version
	nurMemcpy(&ri.swVersion, &ptr[pos], 3);
	pos += 3;

	// Read rest of the info
	ri.numGpio = ptr[pos++];
	ri.numSensors = ptr[pos++];
	ri.numRegions = ptr[pos++];
	ri.numAntennas = ptr[pos++];

	// Copy back to response struct
	nurMemcpy(&hNurApi->resp->readerinfo, &ri, sizeof(ri));

	return error;
}

int NURAPICONV NurApiGetRegionInfo(struct NUR_API_HANDLE *hNurApi, BYTE regionId)
{
	int error;
	WORD payloadLen = 0;

	if ((int)regionId != -1) {
		TxPayloadDataPtr[0] = regionId;
		payloadLen = 1;
	}

	error = NurApiXchPacket(hNurApi, NUR_CMD_GETREGIONINFO, payloadLen, DEF_TIMEOUT);
	LOGIFERROR(error);

	if (error == NUR_SUCCESS) 
	{
		// NULL terminate string
		hNurApi->resp->regioninfo.name[hNurApi->resp->regioninfo.nameLen] = '\0';
	}

	return error;
}

int NURAPICONV NurApiStoreCurrentSetup(struct NUR_API_HANDLE *hNurApi, BYTE flags)
{
	TxPayloadDataPtr[0] = flags;
	return NurApiXchPacket(hNurApi, NUR_CMD_STORESETUP, 1, DEF_TIMEOUT);
}

int NURAPICONV NurApiSetBaudrate(struct NUR_API_HANDLE *hNurApi, BYTE setting)
{
	TxPayloadDataPtr[0] = setting;
	return NurApiXchPacket(hNurApi, NUR_CMD_SETBDR, 1, DEF_TIMEOUT);	
}

int NURAPICONV NurApiGetBaudrate(struct NUR_API_HANDLE *hNurApi)
{
	return NurApiXchPacket(hNurApi, NUR_CMD_SETBDR, 0, DEF_TIMEOUT);
}

int NURAPICONV NurApiGetFWINFO(struct NUR_API_HANDLE *hNurApi, char *buf, WORD buflen)
{
	int error = NurApiXchPacket(hNurApi, NUR_CMD_GETFWINFO, 0, DEF_TIMEOUT);

	if (error != NUR_NO_ERROR)
		return error;

	if (buflen < RxPayloadLen) {
		return NUR_ERROR_BUFFER_TOO_SMALL;
	}

	nurStrncpy(buf, (const char*)hNurApi->resp->rawdata, RxPayloadLen);
	return error;
}

static void SetupGetMember(DWORD memberFlag, void *memberPtr, int sizeofMember, DWORD respFlags, BYTE *dataPtr, WORD *dataPos)
{
	if ((respFlags & memberFlag) != 0) 
	{
		nurMemcpy(memberPtr, &dataPtr[*dataPos], sizeofMember);
		*dataPos += sizeofMember;				
	}
}

#ifdef _MSC_VER
#define GETMEMBER(fl, name) SetupGetMember(fl, &resp.##name, sizeof(resp.##name), flags, ptr, &pos)
#else
#define GETMEMBER(fl, name) SetupGetMember(fl, &resp.name, sizeof(resp.name), flags, ptr, &pos)
#endif

static void ParseModuleSetupResponse(struct NUR_API_HANDLE *hNurApi, DWORD flags)
{
	struct NUR_CMD_LOADSETUP_PARAMS resp;
	BYTE *ptr = hNurApi->resp->rawdata;
	WORD pos = 0;

	// Returned flags
	resp.flags = flags;
	pos += sizeof(DWORD);

	// Get all setup members in correct order
	GETMEMBER(NUR_SETUP_LINKFREQ, linkFreq);
	GETMEMBER(NUR_SETUP_RXDEC, rxDecoding);
	GETMEMBER(NUR_SETUP_TXLEVEL, txLevel);
	GETMEMBER(NUR_SETUP_TXMOD, txModulation);
	GETMEMBER(NUR_SETUP_REGION, regionId);
	GETMEMBER(NUR_SETUP_INVQ, inventoryQ);
	GETMEMBER(NUR_SETUP_INVSESSION, inventorySession);
	GETMEMBER(NUR_SETUP_INVROUNDS, inventoryRounds);
	GETMEMBER(NUR_SETUP_ANTMASK, antennaMask);
	GETMEMBER(NUR_SETUP_SCANSINGLETO, scanSingleTriggerTimeout);
	GETMEMBER(NUR_SETUP_INVENTORYTO, inventoryTriggerTimeout);
	GETMEMBER(NUR_SETUP_SELECTEDANT, selectedAntenna);
	GETMEMBER(NUR_SETUP_OPFLAGS, opFlags);
	GETMEMBER(NUR_SETUP_INVTARGET, inventoryTarget);		
	GETMEMBER(NUR_SETUP_INVEPCLEN, inventoryEpcLength);				
	GETMEMBER(NUR_SETUP_READRSSIFILTER, readRssiFilter);				
	GETMEMBER(NUR_SETUP_WRITERSSIFILTER, writeRssiFilter);				
	GETMEMBER(NUR_SETUP_INVRSSIFILTER, inventoryRssiFilter);
	GETMEMBER(NUR_SETUP_READTIMEOUT, readTO);
	GETMEMBER(NUR_SETUP_WRITETIMEOUT, writeTO);
	GETMEMBER(NUR_SETUP_LOCKTIMEOUT, lockTO);
	GETMEMBER(NUR_SETUP_KILLTIMEOUT, killTO);
	GETMEMBER(NUR_SETUP_AUTOPERIOD, periodSetup);
	GETMEMBER(NUR_SETUP_PERANTPOWER, antPower);
	GETMEMBER(NUR_SETUP_PERANTOFFSET, powerOffset);
	GETMEMBER(NUR_SETUP_ANTMASKEX, antennaMaskEx);
	GETMEMBER(NUR_SETUP_AUTOTUNE, autotune);
	GETMEMBER(NUR_SETUP_PERANTPOWER_EX, antPowerEx);
	GETMEMBER(NUR_SETUP_RXSENS, rxSensitivity);

	// Copy response back to main response struct
	nurMemcpy(&hNurApi->resp->loadsetup, &resp, sizeof(resp));
}

static void SetupAddMember(DWORD memberFlag, void *memberPtr, int sizeofMember, DWORD paramsFlags, BYTE *dataPtr, WORD *dataPos)
{
	if ((paramsFlags & memberFlag) != 0) {
		PacketBytes(dataPtr, (BYTE*)memberPtr, sizeofMember, dataPos);
	}
}

#ifdef _MSC_VER
#define ADDMEMBER(fl, name) SetupAddMember(fl, &params->##name, sizeof(params->##name), params->flags, TxPayloadDataPtr, &payloadSize)
#else
#define ADDMEMBER(fl, name) SetupAddMember(fl, &params->name, sizeof(params->name), params->flags, TxPayloadDataPtr, &payloadSize)
#endif

int NURAPICONV NurApiSetModuleSetup(struct NUR_API_HANDLE *hNurApi, struct NUR_CMD_LOADSETUP_PARAMS *params)
{
	int error;
	WORD payloadSize = 0;

	if ((params->flags & NUR_SETUP_ALL) == 0) {
		return NUR_ERROR_INVALID_PARAMETER;
	}

	PacketDword(TxPayloadDataPtr, GET_DWORD(params->flags), &payloadSize);

	// Add all setup members in correct order
	ADDMEMBER(NUR_SETUP_LINKFREQ, linkFreq);
	ADDMEMBER(NUR_SETUP_RXDEC, rxDecoding);
	ADDMEMBER(NUR_SETUP_TXLEVEL, txLevel);
	ADDMEMBER(NUR_SETUP_TXMOD, txModulation);
	ADDMEMBER(NUR_SETUP_REGION, regionId);
	ADDMEMBER(NUR_SETUP_INVQ, inventoryQ);
	ADDMEMBER(NUR_SETUP_INVSESSION, inventorySession);
	ADDMEMBER(NUR_SETUP_INVROUNDS, inventoryRounds);
	ADDMEMBER(NUR_SETUP_ANTMASK, antennaMask);
	ADDMEMBER(NUR_SETUP_SCANSINGLETO, scanSingleTriggerTimeout);
	ADDMEMBER(NUR_SETUP_INVENTORYTO, inventoryTriggerTimeout);
	ADDMEMBER(NUR_SETUP_SELECTEDANT, selectedAntenna);
	ADDMEMBER(NUR_SETUP_OPFLAGS, opFlags);
	ADDMEMBER(NUR_SETUP_INVTARGET, inventoryTarget);		
	ADDMEMBER(NUR_SETUP_INVEPCLEN, inventoryEpcLength);				
	ADDMEMBER(NUR_SETUP_READRSSIFILTER, readRssiFilter);
	ADDMEMBER(NUR_SETUP_WRITERSSIFILTER, writeRssiFilter);				
	ADDMEMBER(NUR_SETUP_INVRSSIFILTER, inventoryRssiFilter);				
	ADDMEMBER(NUR_SETUP_READTIMEOUT, readTO);
	ADDMEMBER(NUR_SETUP_WRITETIMEOUT, writeTO);				
	ADDMEMBER(NUR_SETUP_LOCKTIMEOUT, lockTO);				
	ADDMEMBER(NUR_SETUP_KILLTIMEOUT, killTO);
	ADDMEMBER(NUR_SETUP_AUTOPERIOD, periodSetup);
	ADDMEMBER(NUR_SETUP_PERANTPOWER, antPower);
	ADDMEMBER(NUR_SETUP_PERANTOFFSET, powerOffset);
	ADDMEMBER(NUR_SETUP_ANTMASKEX, antennaMaskEx);
	ADDMEMBER(NUR_SETUP_AUTOTUNE, autotune);
	ADDMEMBER(NUR_SETUP_PERANTPOWER_EX, antPowerEx);
	ADDMEMBER(NUR_SETUP_RXSENS, rxSensitivity);

	error = NurApiXchPacket(hNurApi, NUR_CMD_LOADSETUP2, payloadSize, DEF_TIMEOUT);	
	if (error == NUR_SUCCESS || error == NUR_ERROR_INVALID_PARAMETER)
	{
		ParseModuleSetupResponse(hNurApi, params->flags);
	}

	return error;
}

int NURAPICONV NurApiGetModuleSetup(struct NUR_API_HANDLE *hNurApi, DWORD setupFlags)
{
	int error;	

	if ((setupFlags & NUR_SETUP_ALL) == 0) {
		return NUR_ERROR_INVALID_PARAMETER;
	}

	PacketDwordPos(TxPayloadDataPtr, setupFlags, 0);

	error = NurApiXchPacket(hNurApi, NUR_CMD_LOADSETUP2, 4, DEF_TIMEOUT);
	if (error == NUR_NO_ERROR)
	{
		ParseModuleSetupResponse(hNurApi, setupFlags);
	}

	return error;
}

int NURAPICONV NurApiGetDeviceCaps(struct NUR_API_HANDLE *hNurApi)
{
	return NurApiXchPacket(hNurApi, NUR_CMD_DEVCAPS, 0, DEF_TIMEOUT);
}

int NURAPICONV NurApiGetReflectedPowerEx(struct NUR_API_HANDLE *hNurApi, DWORD freq)
{
	WORD payloadLen = 0;
	if (freq > 0)
	{
		PacketDwordPos(TxPayloadDataPtr, freq, 0);
		payloadLen = sizeof(freq);
	}
	return NurApiXchPacket(hNurApi, NUR_CMD_GETREFPOWEREX, payloadLen, DEF_TIMEOUT);
}

int NURAPICONV NurApiInventory(struct NUR_API_HANDLE *hNurApi,
							   struct NUR_CMD_INVENTORY_PARAMS *params)
{
	WORD payloadSize = params ? sizeof(struct NUR_CMD_INVENTORY_PARAMS) : 0;
	if (payloadSize > 0) {
		nurMemcpy(TxPayloadDataPtr, params, payloadSize);
	}
	return NurApiXchPacket(hNurApi, NUR_CMD_INVENTORY, payloadSize, DEF_LONG_TIMEOUT);
}

int NURAPICONV NurApiInventoryEx(struct NUR_API_HANDLE *hNurApi,
								 struct NUR_CMD_INVENTORYEX_PARAMS *params)
{
	WORD payloadSize = params ? sizeof(struct NUR_CMD_INVENTORYEX_PARAMS) : 0;
	if (payloadSize > 0) {
		int n;
		WORD copySize = payloadSize - sizeof(params->filters);
		nurMemcpy(TxPayloadDataPtr, params, copySize);
		payloadSize = copySize;

		for (n=0; n<params->filterCount; n++)
		{
			copySize = 9; // "Header" size.
			// Calculate filter bytes from bit length	
			copySize += ((params->filters[n].maskbitlen / 8) + ((params->filters[n].maskbitlen % 8) != 0));
			nurMemcpy(TxPayloadDataPtr + payloadSize, &params->filters[n], copySize);
			payloadSize += copySize;
		}
	}
	return NurApiXchPacket(hNurApi, NUR_CMD_INVENTORYEX, payloadSize, DEF_LONG_TIMEOUT);	
}

NUR_API int NURAPICONV NurApiGetInventoryReadConfig(struct NUR_API_HANDLE *hNurApi)
{
	return NurApiXchPacket(hNurApi, NUR_CMD_INVENTORYREAD, 0, DEF_TIMEOUT);
}

int NURAPICONV NurApiSetInventoryReadConfig(struct NUR_API_HANDLE *hNurApi,
											struct NUR_CMD_IRCONFIG_PARAMS *params)
{
	WORD payloadSize;
	if (params->active) {
		payloadSize = params ? sizeof(struct NUR_CMD_IRCONFIG_PARAMS) : 0;
		if (payloadSize > 0) {
			nurMemcpy(TxPayloadDataPtr, params, payloadSize);
		}
	} else {
		payloadSize = 1;
		TxPayloadDataPtr[0] = params->active;
	}
	return NurApiXchPacket(hNurApi, NUR_CMD_INVENTORYREAD, payloadSize, DEF_TIMEOUT);
}

int NURAPICONV NurApiClearTags(struct NUR_API_HANDLE *hNurApi)
{
	return NurApiXchPacket(hNurApi, NUR_CMD_CLEARIDBUF, 0, DEF_TIMEOUT);
}

int NURAPICONV NurApiSetExtCarrier(struct NUR_API_HANDLE *hNurApi, BOOL on)
{
	int error;
	PacketDwordPos(TxPayloadDataPtr, on, 0);

	error = NurApiXchPacket(hNurApi, NUR_CMD_CARRIER, 4, DEF_TIMEOUT);
	return error;
}

int NURAPICONV NurApiSetConstantChannelIndex(struct NUR_API_HANDLE *hNurApi, BYTE channelIdx)
{
	TxPayloadDataPtr[0] = (channelIdx & 0xFF);
	return NurApiXchPacket(hNurApi, NUR_CMD_SETCHANNEL, 1, DEF_TIMEOUT);
}

#define SZ_META_PREPEND_IR    12

int NURAPICONV ParseIdBuffer(struct NUR_API_HANDLE *hNurApi, pFetchTagsFunction tagFunc, BYTE *buffer, DWORD bufferLen, BOOL includeMeta, BOOL includeIrData)
{
	DWORD pos = 0;
	struct NUR_IDBUFFER_ENTRY entry;
	int received = 0;

	if (includeIrData) 
		includeMeta = TRUE;

	nurMemset(&entry, 0, sizeof(entry));

	while (pos < bufferLen)
	{
		// Get tag block length
		BYTE blockLen = buffer[pos++];

		// Done?
		if (blockLen == 0)
			break;

		if (includeMeta) 
		{
			if (includeIrData)
			{
				// Copy all members at once
				nurMemcpy(&entry, &buffer[pos], SZ_META_PREPEND_IR);
				blockLen -= SZ_META_PREPEND_IR;
				pos += SZ_META_PREPEND_IR;
			}
			else
			{
				// Copy: rssi, scaledRssi, timestamp, freq
				entry.dataLen = 0;
				nurMemcpy(&entry, &buffer[pos], 8);
				blockLen -= 8;
				pos += 8;

				// Skip dataLen member

				// Copy: pc, channel
				nurMemcpy(&entry.pc, &buffer[pos], 3);
				blockLen -= 3;			  
				pos += 3;				
			}
		}

		// Copy antenna id
		entry.antennaId = buffer[pos];
		pos++;
		blockLen--;

		if (includeMeta && includeIrData)
		{
			// EPC + data
			entry.epcLen = (blockLen - entry.dataLen);
		}
		else
		{
			// EPC only
			entry.epcLen = blockLen;			
		}

		// Set data pointer
		entry.epcData = &buffer[pos];

		// Call tag callback
		if (tagFunc) 
		{
			if (tagFunc(hNurApi, &entry) != NUR_SUCCESS) {
				break;
			}
		}

		// Advance to next tag in buffer
		pos += blockLen;		
		received++;
	}

	return received;
}

int NURAPICONV NurApiFetchTags(struct NUR_API_HANDLE *hNurApi, BOOL includeMeta, BOOL clearModuleTags, int *tagsReceived, pFetchTagsFunction tagFunc)
{
	int error;
	int parseRet = 0;
	WORD payloadSize = 0;

	if (clearModuleTags) {
		TxPayloadDataPtr[0] = 0x1; // Clear id buffer
		payloadSize = 1;
	}

	error = NurApiXchPacket(hNurApi, includeMeta ? NUR_CMD_GETMETABUF : NUR_CMD_GETIDBUF, payloadSize, DEF_TIMEOUT);
	if (error == NUR_SUCCESS) 
	{
		parseRet = ParseIdBuffer(hNurApi, tagFunc, hNurApi->resp->rawdata, RxPayloadLen, includeMeta, (RxHeaderPtr->flags & PACKET_FLAG_IRDATA) != 0);
	}

	if (tagsReceived) 
		*tagsReceived = parseRet;

	return error;
}

int NURAPICONV NurApiFetchTagAt(struct NUR_API_HANDLE *hNurApi, BOOL includeMeta, int tagNum, pFetchTagsFunction tagFunc)
{
	int error;
	WORD payloadSize = 0;

	PacketDword(TxPayloadDataPtr, tagNum, &payloadSize);

	error = NurApiXchPacket(hNurApi, includeMeta ? NUR_CMD_GETMETABUF : NUR_CMD_GETIDBUF, payloadSize, DEF_TIMEOUT);
	if (error == NUR_SUCCESS) 
	{
		ParseIdBuffer(hNurApi, tagFunc, hNurApi->resp->rawdata, RxPayloadLen, includeMeta, (RxHeaderPtr->flags & PACKET_FLAG_IRDATA) != 0);
	}

	return error;
}

int NURAPICONV NurApiTraceTag(struct NUR_API_HANDLE *hNurApi, struct NUR_CMD_TRACETAG_PARAMS *params)
{
	int error;
	BYTE *payloadBuffer = TxPayloadDataPtr;
	WORD payloadSize = 0;
	BYTE maskdataLen;

	if (params->maskdata == NULL) {
		RETLOGERROR(NUR_ERROR_INVALID_PARAMETER);
	}

	// BitLengthToByteLength
	maskdataLen = ((params->maskbitlen / 8) + ((params->maskbitlen % 8) != 0));

	PacketByte(payloadBuffer, params->flags, &payloadSize);
	PacketByte(payloadBuffer, params->bank, &payloadSize);
	if (params->flags & RW_EA1) {
		PacketQword(payloadBuffer, GET_QWORD(params->address64), &payloadSize);
	} else {
		PacketDword(payloadBuffer, GET_DWORD(params->address32), &payloadSize);
	}
	PacketByte(payloadBuffer, params->maskbitlen, &payloadSize);
	PacketBytes(payloadBuffer, params->maskdata, maskdataLen, &payloadSize);

	error = NurApiXchPacket(hNurApi, NUR_CMD_TRACETAG, payloadSize, DEF_TIMEOUT);
	if (error == NUR_ERROR_G2_TAG_RESP)
	{
		error = TranslateTagError(hNurApi->resp->rawdata[0]);
	}

	LOGIFERROR(error);

	if (error == NUR_SUCCESS) 
	{
		// Calculate epc data length
		hNurApi->resp->tracetag.epcLen = (BYTE)(RxPayloadLen - 3); // - rssi, scaledRssi, antennaID
	}

	return error;
}

static void WriteCommonSingulationBlock(struct NUR_SINGULATED_CMD_PARAMS *params, BYTE *payloadBuffer, WORD *payloadSize)
{
	int hdrSize;

	if (!(params->flags & RW_SEC)) 
		params->passwd = 0;

	// Common RW
	PacketByte(payloadBuffer, params->flags, payloadSize);
	//PacketDword(payloadBuffer, params->passwd, payloadSize);
	PacketDword(payloadBuffer, GET_DWORD(params->passwd), payloadSize);

	if (params->flags & RW_SBP) 
	{
		// Singlation block present
		// Calculate bytes to follow from bit length	
		params->sb.bytestofollow = (BYTE)(((params->sb.maskbitlen / 8) + ((params->sb.maskbitlen % 8) != 0)));

		hdrSize = (params->flags & RW_EA1) ? 11 : 7;
		params->sb.bytestofollow += hdrSize;

		PacketByte(payloadBuffer, params->sb.bytestofollow, payloadSize);
		PacketByte(payloadBuffer, params->sb.bank, payloadSize);
		if (params->flags & RW_EA1) {
			PacketQword(payloadBuffer, GET_QWORD(params->sb.address64), payloadSize);
		} else {
			PacketDword(payloadBuffer, GET_DWORD(params->sb.address32), payloadSize);
		}
		PacketWord(payloadBuffer, GET_WORD(params->sb.maskbitlen), payloadSize);
		PacketBytes(payloadBuffer, params->sb.maskdata, params->sb.bytestofollow - hdrSize, payloadSize);
	}
}

#ifdef CONFIG_GENERIC_READ
int NURAPICONV NurApiReadTag(struct NUR_API_HANDLE *hNurApi, 
							 struct NUR_CMD_READ_PARAMS *params, 
							 BYTE *rdBuffer)
{
	int error;
	BYTE *payloadBuffer = TxPayloadDataPtr;
	WORD payloadSize = 0;
	DWORD rdByteCount = params->rb.wordcount * 2;
	struct NUR_READBLOCK *rb = &params->rb;

	if (rdByteCount < 2 || rdByteCount > 510 || ((rdByteCount &1) != 0)) {
		RETLOGERROR(NUR_ERROR_INVALID_PARAMETER);
	}

	// Write "Common RW" block and "Singulation" block to payload buffer
	WriteCommonSingulationBlock((struct NUR_SINGULATED_CMD_PARAMS*)params, payloadBuffer, &payloadSize);

	// Read block
	rb->bytestofollow = (params->flags & RW_EA2) ? 10 : 6;

	PacketByte(payloadBuffer, rb->bytestofollow, &payloadSize);
	PacketByte(payloadBuffer, rb->bank, &payloadSize);
	if (params->flags & RW_EA2) {
		PacketQword(payloadBuffer, GET_QWORD(rb->address64), &payloadSize);
	} else {
		PacketDword(payloadBuffer, GET_DWORD(rb->address32), &payloadSize);
	}
	PacketByte(payloadBuffer, rb->wordcount, &payloadSize);	

	error = NurApiXchPacket(hNurApi, NUR_CMD_READ, payloadSize, DEF_LONG_TIMEOUT);
	if (error == NUR_ERROR_G2_TAG_RESP)
	{
		error = TranslateTagError(hNurApi->resp->rawdata[0]);
	}

	LOGIFERROR(error);

	if (error == NUR_SUCCESS) {
		nurMemcpy(rdBuffer, hNurApi->resp->rawdata, RxPayloadLen);
	}

	return error;
}
#endif

#ifdef CONFIG_GENERIC_WRITE
int NURAPICONV NurApiWriteTag(struct NUR_API_HANDLE *hNurApi, struct NUR_CMD_WRITE_PARAMS *params)
{
	int error;
	BYTE *payloadBuffer = TxPayloadDataPtr;
	WORD payloadSize = 0;
	int hdrSize = 0;
	struct NUR_WRITEBLOCK *wb = &params->wb;

	if (params->wb.wordcount < 1 || params->wb.wordcount > 127) {
		RETLOGERROR(NUR_ERROR_INVALID_PARAMETER);
	}

	// Write "Common RW" block and "Singulation" block to payload buffer
	WriteCommonSingulationBlock((struct NUR_SINGULATED_CMD_PARAMS*)params, payloadBuffer, &payloadSize);

	// Write block
	// Calculate bytes to follow from word count
	wb->bytestofollow = (wb->wordcount * 2);
	hdrSize = (params->flags & RW_EA2) ? 10 : 6;
	wb->bytestofollow += hdrSize;

	PacketByte(payloadBuffer, wb->bytestofollow, &payloadSize);
	PacketByte(payloadBuffer, wb->bank, &payloadSize);
	if (params->flags & RW_EA2) {
		PacketQword(payloadBuffer, GET_QWORD(wb->address64), &payloadSize);
	} else {
		PacketDword(payloadBuffer, GET_DWORD(wb->address32), &payloadSize);
	}
	PacketByte(payloadBuffer, wb->wordcount, &payloadSize);		
	PacketBytes(payloadBuffer, wb->data, wb->bytestofollow - hdrSize, &payloadSize);

	error = NurApiXchPacket(hNurApi, NUR_CMD_WRITE, payloadSize, DEF_LONG_TIMEOUT);
	if (error == NUR_ERROR_G2_TAG_RESP)
	{
		error = TranslateTagError(hNurApi->resp->rawdata[0]);
	}
	LOGIFERROR(error);
	return error;
}
#endif

int NURAPICONV NurApiScanSingle(struct NUR_API_HANDLE *hNurApi, WORD timeout)
{	
	int error;

	PacketWordPos(TxPayloadDataPtr, timeout, 0);
	error = NurApiXchPacket(hNurApi, NUR_CMD_SCANSINGLE, 2, timeout + 1000);
	if (error == NUR_ERROR_G2_TAG_RESP)
	{
		// Tag error 
		error = TranslateTagError(hNurApi->resp->rawdata[0]);
	}
	LOGIFERROR(error);

	if (error == NUR_SUCCESS) 
	{
		hNurApi->resp->scansingle.epcLen = (BYTE)(RxPayloadLen - 3);
	}
	return error;
}

int NURAPICONV NurApiTuneAntenna(struct NUR_API_HANDLE *hNurApi, int antenna, BOOL wideTune, BOOL bSaveResults, int *dBmResults)
{
	int error, i;
	struct NUR_CMD_TUNECOMMANDPARAM *cmd = (struct NUR_CMD_TUNECOMMANDPARAM *)TxPayloadDataPtr;
	struct NUR_CMD_TUNEANTENNA_RESP *resp;

	cmd->type = wideTune ? 2 : 0;
	cmd->antenna = antenna;
	cmd->band = (DWORD)-1;	/* All */
	cmd->userSave = bSaveResults ? 1 : 0;
	cmd->goodEnough = -100;
	nurMemset((void *)cmd->code, 0, PRODUCTION_TUNE_MAGICLEN);

	error = NurApiXchPacket(hNurApi, NUR_CMD_TUNEANTENNA, sizeof(*cmd), 25000);
	LOGIFERROR(error);

	if (error == NUR_SUCCESS) 
	{
		if (dBmResults!=NULL)
		{
			resp = (struct NUR_CMD_TUNEANTENNA_RESP *)hNurApi->resp->rawdata;
			for (i=0; i<NR_TUNEBANDS; i++)
				*dBmResults++ = resp->bands[i].dBm;
		}
	}
	return error;
}

int NURAPICONV NurApiSetGPIOConfig(struct NUR_API_HANDLE *hNurApi, struct NUR_CMD_CONFIGGPIO_PARAMS *gpioParams)
{
	DWORD dwSize = 1; // flags
	int i, gpioFlag = 1;

	for (i=0; i<NUR_MAX_GPIO; i++) 
	{
		if (gpioFlag & gpioParams->flags) {
			dwSize += sizeof(struct NUR_GPIO_SETUP);
		}
		gpioFlag <<= 1;
	}

	nurMemcpy(TxPayloadDataPtr, gpioParams, dwSize);
	return NurApiXchPacket(hNurApi, NUR_CMD_CONFIGGPIO, (WORD)dwSize, DEF_TIMEOUT);
}

int NURAPICONV NurApiGetGPIOConfig(struct NUR_API_HANDLE *hNurApi)
{
	return NurApiXchPacket(hNurApi, NUR_CMD_CONFIGGPIO, 0, DEF_TIMEOUT);
}

int NURAPICONV NurApiSetGPIOStatus(struct NUR_API_HANDLE *hNurApi, int gpio, BOOL state)
{
	int error;
	DWORD dwSize;
	struct NUR_CMD_SETGPIO_PARAMS *setState = (struct NUR_CMD_SETGPIO_PARAMS *)TxPayloadDataPtr;

	setState->flags = (1<<gpio);
	setState->state.state = (BYTE)state;

	dwSize = sizeof(setState->flags) + sizeof(setState->state);

	error = NurApiXchPacket(hNurApi, NUR_CMD_SETGPIO, (WORD)dwSize, DEF_TIMEOUT);
	if (error==NUR_NO_ERROR) 
	{
		if (state != hNurApi->resp->gpioset.state.state) {
			error = NUR_ERROR_INVALID_PARAMETER;
		}
	}

	return error;
}

int NURAPICONV NurApiGetGPIOStatus(struct NUR_API_HANDLE *hNurApi, int gpio)
{
	struct NUR_CMD_GETGPIO_PARAMS *getState = (struct NUR_CMD_GETGPIO_PARAMS *)TxPayloadDataPtr;
	getState->flags = (1<<gpio);
	return NurApiXchPacket(hNurApi, NUR_CMD_GETGPIO, sizeof(*getState), DEF_TIMEOUT);	
}

int NURAPICONV NurApiEnterBoot(struct NUR_API_HANDLE *hNurApi) 
{
	return NurApiXchPacket(hNurApi, NUR_CMD_ENTERBOOT, 0, DEF_TIMEOUT);
}

int NURAPICONV NurApiModuleRestart(struct NUR_API_HANDLE *hNurApi) 
{
	return NurApiXchPacket(hNurApi, NUR_CMD_RESTART, 0, DEF_TIMEOUT);
}

int NURAPICONV NurApiGetMode(struct NUR_API_HANDLE *hNurApi, char *mode) 
{
	int error = NurApiXchPacket(hNurApi, NUR_CMD_GETMODE, 0, DEF_TIMEOUT);
	if (error == NUR_SUCCESS) {
		*mode = hNurApi->resp->mode.type;
	}
	return error;
}

static const DWORD crc32Table[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

DWORD NURAPICONV NurCRC32(DWORD crc, const BYTE *buf, DWORD len)
{	
	crc ^= 0xFFFFFFFF;
	while (len--) {
		crc = (crc >> 8) ^ crc32Table[(crc ^ *buf++) & 0xFF];
	}
	return crc ^ 0xFFFFFFFF;
}

#define NUR_PROGRAM_RETRIES 5

int NURAPICONV NurApiProgramBuffer(struct NUR_API_HANDLE *hNurApi, pProgramProgressFunction prgFn, WORD startPage, BYTE validateCmd, BYTE *buffer, DWORD bufferLen) 
{
	int error = NUR_SUCCESS;
	DWORD numPages = 0;
	DWORD numPagesReminder = 0;
	DWORD writePos = 0;
	DWORD pageSize;
	DWORD i = 0;
	WORD currentPage = startPage;
	int writeRetries = NUR_PROGRAM_RETRIES;
	DWORD appCRC = 0;

	struct NUR_PRGPROGRESS_DATA notificationData;
	struct NUR_CMD_PAGEWRITE_PARAMS *pagewriteParams = (struct NUR_CMD_PAGEWRITE_PARAMS *)TxPayloadDataPtr;
	struct NUR_CMD_APPVALIDATE_PARAMS *appValidateParams = (struct NUR_CMD_APPVALIDATE_PARAMS *)TxPayloadDataPtr;

	numPages = bufferLen / NUR_FLASH_PAGE_SIZE;
	numPagesReminder = bufferLen % NUR_FLASH_PAGE_SIZE;
	if (numPagesReminder > 0)
		numPages++;	

	notificationData.error = 0;
	notificationData.totalPages = numPages;
	notificationData.curPage = -1;
	if (prgFn && (*prgFn)(hNurApi, &notificationData) != 0) {
		return NUR_ERROR_NOT_READY;
	}

	while (writePos < bufferLen) 
	{
		notificationData.curPage = currentPage - startPage;
		if (prgFn && (*prgFn)(hNurApi, &notificationData) != 0) {
			return NUR_ERROR_NOT_READY;
		}

		pageSize = bufferLen - writePos;
		if (pageSize > NUR_FLASH_PAGE_SIZE)
			pageSize = NUR_FLASH_PAGE_SIZE;

		nurMemcpy(pagewriteParams->data, &buffer[writePos], pageSize);

		if (pageSize < NUR_FLASH_PAGE_SIZE) {
			// Pad with 0xFF
			for (i = pageSize; i < NUR_FLASH_PAGE_SIZE; i++) {
				pagewriteParams->data[i] = 0xFF;
			}
		}

		pagewriteParams->pagetowrite = currentPage;
		pagewriteParams->crc = NurCRC32(0, pagewriteParams->data, NUR_FLASH_PAGE_SIZE);

		// XOR w/ crc
		for (i=0; i<NUR_FLASH_PAGE_SIZE; i += 4) {
			DWORD dwData = BytesToDword(&pagewriteParams->data[i]);
			dwData = (dwData ^ pagewriteParams->crc);
			PacketDwordPos(pagewriteParams->data, dwData, i);
		}

		writeRetries = NUR_PROGRAM_RETRIES;
		while (writeRetries-- > 0) {
			error = NurApiXchPacket(hNurApi, NUR_CMD_PAGEWRITE, sizeof(*pagewriteParams), DEF_TIMEOUT);
			// TEST: Simulated write error @ page 100
			/*if (notificationData.curPage == 100 && writeRetries == NUR_PROGRAM_RETRIES)
			{
			error = NUR_ERROR_PAGE_PROGRAM;
			}*/
			if (error == NUR_SUCCESS) {
				break;
			}
		}

		if (error != NUR_SUCCESS) {
			break;
		}

		currentPage++;
		writePos += pageSize;
	}

	if (error == NUR_SUCCESS && validateCmd != 0) 
	{
		appCRC = NurCRC32(0, buffer, bufferLen);
		appValidateParams->appcrc = appCRC;
		appValidateParams->appsize = bufferLen;

		writeRetries = NUR_PROGRAM_RETRIES;
		while (writeRetries-- > 0) {
			error = NurApiXchPacket(hNurApi, validateCmd, sizeof(*appValidateParams), DEF_TIMEOUT);
			if (error == NUR_SUCCESS) {
				break;
			}
		}
	}

	if (error != NUR_SUCCESS) {
		notificationData.error = error;
	} else {
		notificationData.curPage = notificationData.totalPages;
	}
	if (prgFn)
		(*prgFn)(hNurApi, &notificationData);

	return error;
}

int NURAPICONV NurApiProgramApp(struct NUR_API_HANDLE *hNurApi, pProgramProgressFunction prgFn, BYTE *buffer, DWORD bufferLen) 
{
	return NurApiProgramBuffer(hNurApi, prgFn, NUR_APP_FIRST_PAGE, NUR_CMD_APPVALIDATE, buffer, bufferLen);
}

int NURAPICONV NurApiProgramBootloader(struct NUR_API_HANDLE *hNurApi, pProgramProgressFunction prgFn, BYTE *buffer, DWORD bufferLen)
{
	return NurApiProgramBuffer(hNurApi, prgFn, NUR_BL_FIRST_PAGE, NUR_CMD_BLVALIDATE, buffer, bufferLen);
}

/*
Unaligned member access.
*/

#if defined(CONFIG_MEMBER_ACCESS)

WORD GET_U16(void *wSrc)
{
	WORD w;
	BYTE *b = (BYTE *)wSrc;
	w = b[1];
	w <<= 8;
	w |= b[0];
	return w;
}

DWORD GET_U32(void *dwSrc)
{
	DWORD dw;
	BYTE *b = (BYTE *)dwSrc;
	dw = b[3];
	dw <<= 8;
	dw |= b[2];
	dw <<= 8;
	dw |= b[1];
	dw <<= 8;
	dw |= b[0];
	return dw;	
}

QWORD GET_U64(void *qwSrc)
{
	QWORD qw;	
	nurMemcpy(&qw, qwSrc, 8);	
	return qw;
}

void SET_U16(void *wDst, void *wSrc)
{
	((BYTE *)wDst)[0] = ((BYTE *)wSrc)[0];
	((BYTE *)wDst)[1] = ((BYTE *)wSrc)[1];
}

void SET_U32(void *dwDst, void *dwSrc)
{
	((BYTE *)dwDst)[0] = ((BYTE *)dwSrc)[0];
	((BYTE *)dwDst)[1] = ((BYTE *)dwSrc)[1];
	((BYTE *)dwDst)[2] = ((BYTE *)dwSrc)[2];
	((BYTE *)dwDst)[3] = ((BYTE *)dwSrc)[3];
}

void SET_U64(void *qwDst, void *qwSrc)
{
	nurMemcpy(qwDst, qwSrc, 8);
}

#endif
