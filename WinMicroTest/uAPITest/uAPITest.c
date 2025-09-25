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

#define _CRT_SECURE_NO_WARNINGS	1

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "NurMicroApi.h"

// #define PRINT_DIAG_UNSOL_EVENT

int32_t open_serial(struct NUR_API_HANDLE *hApi, int number, uint32_t baudrate);
void close_serial();

static int32_t gConnected = FALSE;

// The API's communication buffers.
static uint8_t gRxBuffer[NUR_MAX_RCV_SZ];
static uint8_t gTxBuffer[NUR_MAX_SEND_SZ];

static int32_t gDevcapsValid = FALSE;
struct NUR_CMD_DEVCAPS_RESP gDevcaps;

/*
In this approach we are using a static API structure.
*/
static struct NUR_API_HANDLE gApi =
{
	NULL,	// void *UserData;
	NULL,	// TransportReadDataFunction
	NULL,	// TransportWriteDataFunction
	NULL,	// UnsolEventHandler;
	NULL,	// UnexpectedCmdHandler;
	NULL,	// IgnoredByteHandler;

	NULL,	// uint8_t *TxBuffer;
	0,		// uint32_t TxBufferLen;

	NULL,	// uint8_t *RxBuffer;
	0,		// uint32_t RxBufferLen;
	0,		// uint32_t RxBufferUsed;

	0,		// uint32_t respLen;
	NULL	// struct NUR_CMD_RESP *resp;
};

static struct NUR_API_HANDLE *hApi = &gApi;

void print_diag_report(struct NUR_DIAG_REPORT* report)
{
#define PRINT_MEMBER(x) printf(" ." #x " = %d (0x%x)\n", report->x, report->x)

	printf("NUR_DIAG_REPORT\n");
	PRINT_MEMBER(flags);        /**< Report flags. see enum NUR_DIAG_REPORT_FLAGS */
	PRINT_MEMBER(uptime);       /**< Uptime in milliseconds */
	PRINT_MEMBER(rfActiveTime); /**< RF on time in milliseconds */
	PRINT_MEMBER(temperature);  /**< Temperature in celcius. 1000 if not supported */
	PRINT_MEMBER(bytesIn);      /**< Number of bytes in to module */
	PRINT_MEMBER(bytesOut);     /**< Number of bytes out from module */
	PRINT_MEMBER(bytesIgnored); /**< Number of ignored (invalid) bytes */
	PRINT_MEMBER(antennaErrors); /**< Number of bad antenna errors */
	PRINT_MEMBER(hwErrors);     /**< Number of automatically recovered internal HW failures */
	PRINT_MEMBER(invTags);      /**< Number of successfully inventoried tags */
	PRINT_MEMBER(invColl);      /**< Number of collisions during inventory */
	PRINT_MEMBER(readTags);     /**< Number of successfully read tag commands */
	PRINT_MEMBER(readErrors);   /**< Number of failed read tag commands */
	PRINT_MEMBER(writeTags);    /**< Number of successfully write tag commands */
	PRINT_MEMBER(writeErrors);  /**< Number of failed write tag commands */
	PRINT_MEMBER(errorConds);   /**< Number of temporary error conditions (over temp, low voltage) occured */
	PRINT_MEMBER(setupErrs);    /**< Number of invalid setup errors */
	PRINT_MEMBER(invalidCmds);  /**< Number of invalid (not supported) commands received */
}

// Handle unsol events
void UnsolEventHandler(struct NUR_API_HANDLE *hNurApi)
{
	switch(hNurApi->resp->cmd)
	{
	case NUR_NOTIFY_DEBUGMSG:
		{
			char tmpStr[255];
			uint32_t tmpStrCnt = 0;
			for (uint32_t n = 0; n < hNurApi->respLen; n++)
			{
				char ch = hNurApi->resp->rawdata[n];
				if (ch == '\r') {
					continue;
				}

				if (ch == '\n' || tmpStrCnt == sizeof(tmpStr)) {
					tmpStr[tmpStrCnt] = '\0';
					puts(tmpStr);
					tmpStrCnt = 0;
				}

				if (ch != '\n') {
					tmpStr[tmpStrCnt++] = ch;
				}
			}
			if (tmpStrCnt > 0) {
				tmpStr[tmpStrCnt] = '\0';
				puts(tmpStr);
			}
			break;
		}

#ifdef PRINT_DIAG_UNSOL_EVENT
	case NUR_NOTIFY_DIAG:
		{
			if (hNurApi->respLen != sizeof(hNurApi->resp->diagreport)) {
				printf("WARNING diagnostics notification report size diff; %d != %d", hNurApi->respLen, sizeof(hNurApi->resp->diagreport));
			}
			print_diag_report(&hNurApi->resp->diagreport);
			break;
		}
#endif

	case NUR_NOTIFY_HOPEVENT:
		{
			printf("NUR_NOTIFY_HOPEVENT: freqIdx=%d, freqKhz=%f\n",
				hNurApi->resp->hopeventdata.freqIdx,
				(double)hNurApi->resp->hopeventdata.freqKhz / 1000);
			break;
		}

	case NUR_NOTIFY_AUTOTUNE:
		{
			printf("NUR_NOTIFY_AUTOTUNE: antenna=%d, reflPower_dBm=%f\n",
				hNurApi->resp->tuneeventdata.antenna,
				(double)hNurApi->resp->tuneeventdata.reflPower_dBm/1000);
			break;
		}
	}
}

static int NurApiEnsureMode(struct NUR_API_HANDLE *hNurApi, char desiredMode)
{
	int error;
	int retries = 5;
	char curMode = 'X';

	// Get current mode
	error = NurApiGetMode(hNurApi, &curMode);

	if (curMode != desiredMode) {
		printf("NurApiEnsureMode(%c) Switching mode from '%c' to '%c'\n", desiredMode, curMode, desiredMode);
	}

	while (retries-- > 0 && curMode != desiredMode) {

		// Attempt to switch mode
		NurApiEnterBoot(hNurApi);

		// NOTE: If this is USB device, you'll need to reconnect serial here!

		// Wait for boot event (2 sec)
		NurApiWaitEvent(hNurApi, 2000);

		// Get current mode
		error = NurApiGetMode(hNurApi, &curMode);
	}

	if (curMode != desiredMode) {
		printf("NurApiEnsureMode(%c) ERROR (%d) Could not change mode to '%c', current mode '%c'\n", desiredMode, error, desiredMode, curMode);
	}

	//printf("NurApiEnsureMode(%c) DONE (%d); curMode %c\n", desiredMode, error, curMode);

	return (curMode != desiredMode) ? NUR_ERROR_GENERAL : error;
}

static int ProgramProgressFunction(struct NUR_API_HANDLE *hNurApi, struct NUR_PRGPROGRESS_DATA *prg)
{
	if (prg->error != 0) {
		// Programming error
		printf("\nFATAL programming error: %d\n", prg->error);
	}
	else if (prg->curPage == -1) {
		// Programming start
		printf("\n");
	} else if (prg->curPage == prg->totalPages) {
		// Programming done
		printf("\rProgramming 100 %% - SUCCESS\n");
	} else {
		printf("\rProgramming %d %%", prg->curPage * 100 / prg->totalPages);
	}
	return 0;
}

static int NurApiProgramFile(struct NUR_API_HANDLE *hNurApi, uint16_t startPage, uint8_t validateCmd, const char *fname)
{
	int error = 0;
	FILE *fp;
	uint8_t *fileBuffer = NULL;
	uint32_t fileLen = 0;

	printf("NurApiProgramFile(%s)\n", fname);

	fp = fopen(fname, "rb");
	if (fp == NULL) {
		printf("NurApiProgramFile(%s), cannot open file\n", fname);
		return NUR_ERROR_FILE_NOT_FOUND;
	}

	fseek(fp, 0, SEEK_END);
	fileLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fileBuffer = (uint8_t *) malloc(fileLen);
	if (!fileBuffer) {
		fclose(fp);
		printf("NurApiProgramFile(%s), FATAL: file buffer ALLOCATION failed!\n", fname);
		return NUR_ERROR_GENERAL;
	}

	if (fread(fileBuffer, sizeof(uint8_t), fileLen, fp) != fileLen) {
		fclose(fp);
		printf("NurApiProgramFile(%s), FATAL: file buffer READ failed!\n", fname);
		return NUR_ERROR_GENERAL;
	}
	fclose(fp);

	error = NurApiProgramBuffer(hApi, ProgramProgressFunction, startPage, validateCmd, fileBuffer, fileLen);

	free(fileBuffer);
	return error;
}

static int NurApiProgramAppFile(struct NUR_API_HANDLE *hNurApi, const char *fname)
{
	int error = NurApiEnsureMode(hNurApi, 'B');
	if (error == NUR_SUCCESS)
		error = NurApiProgramFile(hNurApi, NUR_APP_FIRST_PAGE, NUR_CMD_APPVALIDATE, fname);
	if (error == NUR_SUCCESS)
		error = NurApiEnsureMode(hNurApi, 'A');
	return error;
}

static int NurApiProgramBootloaderFile(struct NUR_API_HANDLE *hNurApi, const char *fname)
{
	int error = NurApiEnsureMode(hNurApi, 'B');
	if (error == NUR_SUCCESS)
		error = NurApiProgramFile(hNurApi, NUR_BL_FIRST_PAGE, NUR_CMD_BLVALIDATE, fname);
	if (error == NUR_SUCCESS)
		error = NurApiEnsureMode(hNurApi, 'A');
	return error;
}

int InitNurApiHandle(struct NUR_API_HANDLE *hApi)
{
	// Init RX buffer
	hApi->RxBuffer = gRxBuffer;
	hApi->RxBufferLen = sizeof(gRxBuffer);

	// Init TX buffer
	hApi->TxBuffer = gTxBuffer;
	hApi->TxBufferLen = sizeof(gTxBuffer);

	hApi->UnsolEventHandler = UnsolEventHandler;

	return hApi->RxBufferLen;
}

BOOL ask_epc(struct NUR_SINGULATED_CMD_PARAMS* params, const char* prompt)
{
	char epcBuf[32] = "";
	int epcBufLen;

	// Ask EPC from user
	printf("%s", prompt);
	if (fgets(epcBuf, sizeof(epcBuf), stdin) == NULL) {
		return FALSE;
	}

	// Remove trailing newline if present
	epcBuf[strcspn(epcBuf, "\r\n")] = 0;

	if (strlen(epcBuf) > 0 && (strlen(epcBuf) % 2) != 0) {
		printf("EPC must be pairs of two hex chars\n");
		return FALSE;
	}
	if (strlen(epcBuf) > 0) {
		// Use singulation
		params->flags |= RW_SBP;
		params->sb.bank = NUR_BANK_EPC;
		params->sb.address32 = 32;
		epcBufLen = HexStringToBin(epcBuf, params->sb.maskdata, strlen(epcBuf));
		params->sb.maskbitlen = epcBufLen * 8;
	}
	else
	{
		// Access tag without singulation
		params->flags &= ~RW_SBP;
	}
	return TRUE;
}

BOOL ask_password(struct NUR_SINGULATED_CMD_PARAMS* params, const char* prompt)
{
	char input[32]; // Buffer for user input

	// Ask password from user
	printf("%s", prompt);
	if (fgets(input, sizeof(input), stdin) == NULL) {
		goto INVALID_INPUT;
	}

	// Remove newline character
	input[strcspn(input, "\r\n")] = '\0';

	// If input is empty, set password to 0
	if (strlen(input) == 0) {
		goto NO_PASSWORD;
	}

	// Convert input to uint32_t
	char* endptr;
	long value = strtol(input, &endptr, 10);

	// Check if input is a valid decimal number and within uint32_t range
	if (*endptr != '\0' || value < 0 || value > UINT32_MAX) {
		goto INVALID_INPUT;
	}
	else {
		// If input is valid, set the password and secure flag
		params->passwd = (uint32_t)value;
		params->flags |= params->passwd ? RW_SEC : 0;
	}
	return TRUE;

NO_PASSWORD:
	// If no password is provided, set password to 0
	// and clear the secure flag
	params->passwd = 0;
	params->flags &= ~RW_SEC;
	return TRUE;

INVALID_INPUT:
	// If input is invalid, set password to 0
	// and clear the secure flag
	params->passwd = 0;
	params->flags &= ~RW_SEC;
	return FALSE;
}

void wait_key()
{
	printf("Hit any key...");
	_getch();
	printf("\n");
}

void cls()
{
	system("cls");
}

static void handle_connection()
{
	if (gConnected) {
		close_serial();
		gConnected = FALSE;
	}
	else
	{
		int num = -1;
		printf("Enter COM number (baud 115200): " );
		if (scanf("%d%*c", &num) == 1) {
			if (open_serial(hApi, num, 115200)) {
				gConnected = TRUE;
				printf("Connected.\n");
			}
			else {
				printf("Connection error\n");
				wait_key();
			}
		}
		else {
			printf("Canceled.\n");
			wait_key();
		}
	}
}

static void handle_versions(int doCls)
{
	struct NUR_CMD_VERSION_RESP *vr;
	int rc;

	if (!gConnected)
		return;

	if (doCls)
		cls();

	rc = NurApiGetVersions(hApi);
	vr = &hApi->resp->versions;

	if (rc == NUR_SUCCESS) {
		printf("Versions, mode = '%c':\n", vr->mode);
		printf(" - primary   : %d.%d-%c\n", vr->vMajor, vr->vMinor, vr->vBuild);
		printf(" - secondary : %d.%d-%c\n", vr->otherMajor, vr->otherMinor, vr->otherBuild);
	}
	else {
		printf("Version error. Code = %d.\n", rc);
	}

	wait_key();
}

static void handle_readerinfo()
{
	struct NUR_CMD_READERINFO_RESP *ri;
	char tmpStr[50];
	int rc;

	if (!gConnected)
		return;
	cls();

	rc = NurApiGetReaderInfo(hApi);

	if (rc == NUR_SUCCESS) {
		ri = &hApi->resp->readerinfo;
		printf("* Reader information *\n");

		// Name
		memcpy(tmpStr, ri->name, ri->nameLen);
		tmpStr[ri->nameLen] = 0;
		printf("Name: %s\n", tmpStr);
		// Serial
		memcpy(tmpStr, ri->serial, ri->serialLen);
		tmpStr[ri->serialLen] = 0;
		printf("Serial: %s\n", tmpStr);
		// FCC ID
		memcpy(tmpStr, ri->fccId, ri->fccIdLen);
		tmpStr[ri->fccIdLen] = 0;
		printf("FCC ID: %s\n", tmpStr);
		// Antennas
		printf("# antennas: %d\n", ri->numAntennas);
		// Regions
		printf("# regions: %d\n", ri->numRegions);
		// GPIO
		printf("# GPIOs: %d\n", ri->numGpio);
		// Sensors
		printf("# sensors: %d\n", ri->numSensors);
	}
	else {
		printf("NUR reader info error. Code = %d.\n", rc);
	}

	wait_key();
}

static void handle_ping()
{
	int rc;
	if (!gConnected)
		return;

	cls();
	printf("* Ping *\n");
	rc = NurApiPing(hApi);
	if (rc == NUR_SUCCESS)
		printf("Ping was OK.\n");
	else
		printf("Ping error. Code = %d.\n", rc);

	wait_key();
}

static char *RxDecToStr(int rxd)
{
	static char buf[50];

	switch (rxd)
	{
	case 0: return "FM0";
	case 1:
	case 2:
	case 3:
		sprintf(buf, "M-%d", (1<<rxd));
		break;
	default:
		sprintf(buf, "unknown RX decoding (%d)", rxd);
		break;
	}

	return buf;
}

static void handle_setup_get()
{
	struct NUR_CMD_LOADSETUP_PARAMS *setup;
	int flags, rc;

	if (!gConnected)
		return;

	flags = NUR_SETUP_TXLEVEL | NUR_SETUP_LINKFREQ | NUR_SETUP_RXDEC;

	cls();
	printf("* Get basic setup *\n");
	rc = NurApiGetModuleSetup(hApi, flags);
	if (rc == NUR_SUCCESS) {
		setup = &hApi->resp->loadsetup;
		printf("Setup:\n");
		printf("TX level: %d\n", setup->txLevel);
		printf("Link frequency: %d\n", GET_DWORD(setup->linkFreq));
		printf("RX decoding: %s\n", RxDecToStr(setup->rxDecoding));
	}
	else
		printf("GetModuleSetup error. Code = %d.\n", rc);

	wait_key();
}

static void handle_setup_set_txlevel()
{
	struct NUR_CMD_LOADSETUP_PARAMS params;
	int txlevel, rc;

	if (!gConnected)
		return;

	cls();
	if (gDevcapsValid)
		printf("Set TX level (0 - %d): ", gDevcaps.txSteps - 1);
	else
		printf("Set TX level (0 - ???): ");
	if (scanf("%d", &txlevel) == 1)
	{
		params.flags = NUR_SETUP_TXLEVEL;
		params.txLevel = txlevel;

		rc = NurApiSetModuleSetup(hApi, &params);

		if (rc == NUR_SUCCESS) {
			if (gDevcapsValid) {
				int dBm = gDevcaps.maxTxdBm - (txlevel * gDevcaps.txAttnStep);
				int mW = (int)round(pow(10, (double)dBm / 10));
				printf("Tx level (%d): %d dBm / %d mW\n", txlevel, dBm, mW);
			}
			printf("OK\n");
		}
		else
		{
			printf("SetModuleSetup error. Code = %d.\n", rc);
		}
	}
	else
	{
		printf("Invalid input\n");
	}

	wait_key();
}

int FetchTagsFunction(struct NUR_API_HANDLE *hNurApi, struct NUR_IDBUFFER_ENTRY *tag)
{
	int n;

	int xpc_count;
	uint16_t xpc1 = 0, xpc2 = 0;

	xpc_count = NurApiParseTagXPC(tag, &xpc1, &xpc2);

	printf("Antenna %d, RSSI = %d (%d%%) ", tag->antennaId+1, tag->rssi, tag->scaledRssi);
	printf("EPC[%d]: ", tag->epcLen);

	for (n=0; n<tag->epcLen; n++) {
		printf("%02X", tag->epcData[n]);
	}

	if (tag->dataLen > 0) {
		printf(", DATA[%d]: ", tag->dataLen);

		for (n = 0; n < tag->dataLen; n++) {
			printf("%02X", tag->epcData[tag->epcLen + n]);
		}
	}

	if (xpc_count >= 1) {
		printf(", XPC W1: 0x%04X", xpc1);
	}
	if (xpc_count >= 2) {
		printf(", XPC W2: 0x%04X", xpc2);
	}
	printf("\n");

	return NUR_SUCCESS; // non-zero terminates tag buffer parsing
}

// Perform inventory using NurApiInventoryEx() and select tag population based on EPC mask.
// epcMask:				EPC filter, beginning of the EPC code
// epcMaskByteLen:		Length of the supplied epcMask in bytes
// Example:
// uint8_t epcMask[] = { 0xAB, 0xCD }; // Select tags EPC starting with "ABCD"
// handle_inventoryex_epcmask(epcMask, sizeof(epcMask));
static void handle_inventoryex_epcmask(uint8_t *epcMask, int epcMaskByteLen)
{
	int rc, n;

	struct NUR_CMD_INVENTORYEX_PARAMS params;

	if (!gConnected)
		return;

	cls();
	printf("* InventoryEx *\n");

	// Clear tag buffer
	rc = NurApiClearTags(hApi);
	if (rc == NUR_SUCCESS)
	{
		if (epcMask && epcMaskByteLen > 0)
		{
			printf("EPC Filter: ");
			for (n=0; n<epcMaskByteLen; n++)
			{
				printf("%02X", epcMask[n]);
			}
			printf("\n");

			// Setup InventoryEx params
			params.inventorySelState = NUR_SELSTATE_SL; // Only tags with SL asserted responds
			params.inventoryTarget = NUR_INVTARGET_A;   // Query tags with inventoried flag set to A
			params.Q = 0;								// Auto Q
			params.rounds = 0;							// Auto Rounds
			params.session = NUR_SESSION_S0;			// Session 0
			params.transitTime = 0;						// No transit time
			params.flags = 0;							// No flags, single inventory
			// Setup filters
			params.filterCount = 1;
			params.filters[0].action = NUR_FACTION_0;	// Matching tags: assert SL or inventoried session flag -> A. Non-matching: deassert SL or inventoried session flag -> B.
			params.filters[0].address = 32;				// Bit address to start of EPC. EPC starts after CRC + PC words
			params.filters[0].bank = NUR_BANK_EPC;		// EPC bank
			params.filters[0].maskbitlen = epcMaskByteLen * 8; // Set bit length
			memcpy(params.filters[0].maskdata, epcMask, epcMaskByteLen); // Copy EPC mask
			params.filters[0].target = NUR_SESSION_SL;	// Set SL flag
			params.filters[0].truncate = 0;				// Always 0

			rc = NurApiInventoryEx(hApi, &params);
		} else {
			// No epcMask defined, just perform normal inventory
			rc = NurApiInventory(hApi, NULL);
		}
		if (rc == NUR_SUCCESS)
		{
			int tagCount;
#if 1
			// Fetch tags one by one (needs less memory)
			int n;
			tagCount = hApi->resp->inventory.numTagsMem;
			for (n=0; n<tagCount; n++)
			{
				rc = NurApiFetchTagAt(hApi, TRUE, n, FetchTagsFunction);
				if (rc != NUR_SUCCESS) {
					break;
				}
			}
#else
			// Fetch all tags at once (needs more memory)
			rc = NurApiFetchTags(hApi, TRUE, TRUE, &tagCount, FetchTagsFunction);
#endif
			printf("\nInventory done; total %d tags\n", tagCount);
		}
		else {
			printf("Inventory error. Code = %d.\n", rc);
		}
	}
	else
	{
		printf("ClearTags error. Code = %d.\n", rc);
	}

	wait_key();
}

static void handle_inventory(uint8_t enable_inventory_read)
{
	int rc;
	struct NUR_CMD_IRCONFIG_PARAMS ir_params;

	if (!gConnected)
		return;

	cls();
	printf("* Inventory *\n");

	if (enable_inventory_read) {
		// Setup and enable inventory read
		printf(" * InventoryRead enabled; Read TID bank address 0, length 4 words *\n");
		ir_params.active = 1;
		ir_params.bank = NUR_BANK_TID;
		ir_params.type = 0; // 0 = EPC + data
		ir_params.wAddress = 0;
		ir_params.wLength = 4;
		NurApiSetInventoryReadConfig(hApi, &ir_params);
	}

	// Clear tag buffer
	rc = NurApiClearTags(hApi);
	if (rc == NUR_SUCCESS)
	{
		rc = NurApiInventory(hApi, NULL); // Pass NULL as params, uses default inventory settings from module setup
		if (rc == NUR_SUCCESS)
		{
			int tagCount;
#if 1
			// Fetch tags one by one (needs less memory)
			int n;
			tagCount = hApi->resp->inventory.numTagsMem;
			for (n=0; n<tagCount; n++)
			{
				rc = NurApiFetchTagAt(hApi, TRUE, n, FetchTagsFunction);
				if (rc != NUR_SUCCESS) {
					break;
				}
			}
#else
			// Fetch all tags at once (needs more memory)
			rc = NurApiFetchTags(hApi, TRUE, TRUE, &tagCount, FetchTagsFunction);
#endif
			printf("\nInventory done; total %d tags\n", tagCount);
		}
		else {
			printf("Inventory error. Code = %d.\n", rc);
		}
	}
	else
	{
		printf("ClearTags error. Code = %d.\n", rc);
	}

	if (enable_inventory_read) {
		// Disable inventory read
		ir_params.active = 0;
		NurApiSetInventoryReadConfig(hApi, &ir_params);
	}

	wait_key();
}

static void handle_tune()
{
	int rc, ant, i;
	int res[NR_TUNEBANDS];
	struct NUR_CMD_LOADSETUP_PARAMS params;

	if (!gConnected)
		return;

	cls();
	printf("* Antenna Tune *\n");
	printf(" NOTE: Make sure antenna is in open space\n\n");

	printf("Enter Antenna ID (0 = first): " );
	if (scanf("%d", &ant) == 1)
	{
		printf("Tune in progress, may take up to 25 secs\n");
		rc = NurApiTuneAntenna(hApi, ant, TRUE, TRUE, res);

		if (rc == NUR_SUCCESS) {
			printf("Tune OK\n");
			for (i=0; i<NR_TUNEBANDS; i++)
			{
				printf(" Band[%d] = %.2f dBm\n", i, (float)res[i]/1000.0f);
			}
			printf("\n");
		}
		else
		{
			printf("Tune error. Code = %d.\n", rc);
		}

		if (rc == NUR_SUCCESS)
		{
			printf("Enablind AutoTone with -15 threshold\n");
			params.flags = NUR_SETUP_AUTOTUNE;
			params.autotune.mode = AUTOTUNE_MODE_ENABLE | AUTOTUNE_MODE_THRESHOLD_ENABLE;
			params.autotune.threshold_dBm = -15;
			rc = NurApiSetModuleSetup(hApi, &params);

			if (rc == NUR_SUCCESS) {
				printf("OK\n");
			}
			else
			{
				printf("SetModuleSetup error. Code = %d.\n", rc);
				printf("Make sure that the module/firmware supports AutoTune feature\n");
				printf("(FW 5.0-A or higher in L2 module)\n");
			}
		}
	}
	else
	{
		printf("Invalid input\n");
	}

	printf("\n");
	wait_key();
}

static void handle_app_update()
{
	cls();
	printf("* Updating app (app_update.bin) *\n");

	if (NurApiProgramAppFile(hApi, "app_update.bin") == NUR_SUCCESS)
	{
		printf("\n");
		handle_versions(0);
	}
	else
	{
		wait_key();
	}
}

static void handle_cont_carrier()
{
	int num=-1;

	cls();

	printf("Enter Channel number: " );
	if (scanf("%d", &num) == 1) {
		int ret = NurApiContCarrier(hApi,num);

		if(ret!=0)
		{
			printf("Fail to set continuous carrier: %d\n",ret );
			wait_key();
			return;

		}

		printf("Carrier on.. hit any key to stop\n");
		_getch();
		printf("\n");
		NurApiStopContCarrier(hApi);
	}
	else
	{
		printf("Invalid input\n");
		wait_key();
	}

}

unsigned char GetBinVal(char ch)
{
	unsigned char value=0;

	if (ch >= '0' && ch <= '9')
            value = ch - '0';
    else if (ch >= 'A' && ch <= 'F')
        value = (ch - 'A' + 10);
    else if (ch >= 'a' && ch <= 'f')
        value = (ch - 'a' + 10);

	return value;
}

int HexStringToBin(char* str,uint8_t* buf,int length)
{
	int strPtr=0;
	int x=0;

	if(length>62) length=62;

	for(x=0;x<length;x++)
	{
		strPtr=x*2;
		buf[x]=GetBinVal(str[strPtr]);
		buf[x]<<=4;
		buf[x]+=GetBinVal(str[strPtr+1]);
	}

	return x/2;
}

void ShowEnabledAntennas(uint32_t antMask)
{
	int x=0;
	printf("EnabledAntennas:\n");

	for(x=0;x<32;x++)
	{
		if((antMask) & (1<<(x)))
		{
			printf("ANT # %d\n",x+1);
		}
	}
}

static void handle_selectAntenna()
{
	struct NUR_CMD_LOADSETUP_PARAMS *setup;
	int flags;
	int antNum, rc;

	if (!gConnected)
		return;

	flags = NUR_SETUP_ANTMASKEX;

	cls();

	rc = NurApiGetModuleSetup(hApi, flags);
	if (rc == NUR_SUCCESS) {
		setup = &hApi->resp->loadsetup;
		ShowEnabledAntennas(setup->antennaMaskEx);
	}
	else
	{
		printf("GetModuleSetup error. Code = %d.\n", rc);
		wait_key();
		return;
	}

	printf("Enable/Disable Antenna # (1 - 16): ");
	if (scanf("%d", &antNum) == 1)
	{
		if(antNum>0 && antNum<=16)
		{
			setup->flags = NUR_SETUP_ANTMASKEX;
			//Toggle specified bit on antennaMaskEx
			setup->antennaMaskEx ^= 1UL << (antNum-1);
			ShowEnabledAntennas(setup->antennaMaskEx);
			//Set new antennaMask to module
			rc = NurApiSetModuleSetup(hApi, setup);

			if (rc == NUR_SUCCESS) {
				printf("OK\n");
			}
			else
			{
				printf("SetModuleSetup error. Code = %d.\n", rc);
			}
		}
		else printf("Invalid input\n");
	}
	else
	{
		printf("Invalid input\n");
	}

	wait_key();
}

static void handle_writeEPC()
{
	int x=0;
	char curepc[32] = "";	//Current EPC as string
	char newepc[32] = "";	//New Epc to write as string
	uint8_t epcBuf[62];
	uint8_t wrBuf[62];
	int epcBufLen,wrBufLen;

	cls();

	printf("Enter Current EPC: " );
	if (scanf("%s", &curepc) == 1)
	if(strlen(curepc) > 1)
	{
		if ((strlen(curepc) % 2) != 0) {
				printf("EPC must be pairs of two hex chars\n");
				goto INVALID_INPUT;
		}

		printf("Enter new EPC: " );
		if (scanf("%s", &newepc) == 1)
		if(strlen(newepc) > 1)
		{
			if ((strlen(newepc) % 2) != 0) {
				printf("EPC must be pairs of two hex chars\n");
				goto INVALID_INPUT;
			}

			//convert HEX string to byte array.
			epcBufLen = HexStringToBin(curepc,epcBuf,strlen(curepc));
			wrBufLen = HexStringToBin(newepc,wrBuf,strlen(newepc));

			//printf("epcLen=%d newEpcLen=%d\n",epcBufLen,wrBufLen);

			x = NurApiWriteEPCByEPC(hApi,0,FALSE,epcBuf,epcBufLen,wrBuf,wrBufLen);

			if(x==4106)
			{
				printf("New EPC must be in word boundaries like: aaaabbbbcccc\n");
				goto INVALID_INPUT;
			}

			if(x==NUR_SUCCESS)
				printf("Write success!\n");
			else printf("Write error = %d\n",x);

			wait_key();
			return;
		}
	}

INVALID_INPUT:

	printf("Invalid input\n");
	wait_key();
}

static void handle_writeToUserMem()
{
	int x = 0;
	char curepc[32] = "";	//Current EPC as string
	char newuser[32] = "";	//Data to user mem to as string
	uint8_t epcBuf[62];
	uint8_t wrBuf[62];
	int epcBufLen, wrBufLen;

	cls();

	printf("Enter Current EPC: ");
	if (scanf("%s", &curepc) == 1)
		if (strlen(curepc) > 1)
		{
			if ((strlen(curepc) % 2) != 0) {
				printf("EPC must be pairs of two hex chars\n");
				goto INVALID_INPUT;
			}

			printf("Enter data to USER mem: ");
			if (scanf("%s", &newuser) == 1)
				if (strlen(newuser) > 1)
				{
					if ((strlen(newuser) % 2) != 0) {
						printf("USER must be pairs of two hex chars\n");
						goto INVALID_INPUT;
					}

					//convert HEX string to byte array.
					epcBufLen = HexStringToBin(curepc, epcBuf, strlen(curepc));
					wrBufLen = HexStringToBin(newuser, wrBuf, strlen(newuser));

					x = NurApiWriteTagByEPC(hApi, 0, FALSE, epcBuf, epcBufLen, NUR_BANK_USER, 0,wrBufLen, wrBuf);

					if (x == 4106)
					{
						printf("New USER mem valuemust be in word boundaries like: aaaabbbbcccc\n");
						goto INVALID_INPUT;
					}
					else if (x == 4110)
					{
						printf("The specific memory location (USER) not exist\n");
						goto INVALID_INPUT;
					}

					if (x == NUR_SUCCESS)
						printf("Write success!\n");
					else printf("Write error = %d\n", x);

					wait_key();
					return;
				}
		}

INVALID_INPUT:

	printf("Invalid input\n");
	wait_key();
}

static void handle_kill_tag()
{
	int x = 0;
	char curepc[32] = "";	//Current EPC as string
	struct NUR_CMD_KILL_PARAMS params;
	int epcBufLen;

	cls();

	printf("Enter EPC: ");
	if (scanf("%s", &curepc) != 1) {
		goto INVALID_INPUT;
	}

	if (strlen(curepc) == 0 || (strlen(curepc) % 2) != 0) {
		printf("EPC must be pairs of two hex chars\n");
		goto INVALID_INPUT;
	}

	printf("Enter kill password in dec: ");
	if (scanf("%d", &params.passwd) != 1)
	{
		goto INVALID_INPUT;
	}
	params.flags = RW_SEC | RW_SBP;

	params.sb.bank = NUR_BANK_EPC;
	params.sb.address32 = 32;
	//convert HEX string to byte array.

	printf("Kill tag [%s] passwd %d\n", curepc, params.passwd);

	epcBufLen = HexStringToBin(curepc, params.sb.maskdata, strlen(curepc));
	params.sb.maskbitlen = epcBufLen * 8;

	x = NurApiKillTag(hApi, &params);

	if (x == NUR_SUCCESS)
		printf("Kill success!\n");
	else 
		printf("Kill error = %d\n", x);

	wait_key();
	return;

INVALID_INPUT:
	printf("Invalid input\n");
	wait_key();
}

static void handle_lock_tag()
{
	int x = 0;
	char curepc[32] = "";	//Current EPC as string
	struct NUR_CMD_LOCK_PARAMS params;
	uint32_t mask = 0, action = 0;
	int epcBufLen;

	cls();

	printf("Enter EPC: ");
	if (scanf("%s", &curepc) != 1) {
		goto INVALID_INPUT;
	}

	if (strlen(curepc) == 0 || (strlen(curepc) % 2) != 0) {
		printf("EPC must be pairs of two hex chars\n");
		goto INVALID_INPUT;
	}

	printf("Enter access password in dec: ");
	if (scanf("%d", &params.passwd) != 1) {
		goto INVALID_INPUT;
	}

	// For lock mask and action values, please see GS1 gen2 UHF standard section 6.3.2.12.3.5 Lock (mandatory)

	printf("Enter lock mask in dec: ");
	if (scanf("%d", &mask) != 1) {
		goto INVALID_INPUT;
	}

	printf("Enter lock action in dec: ");
	if (scanf("%d", &action) != 1) {
		goto INVALID_INPUT;
	}

	params.lb.action = (uint16_t)action;
	params.lb.mask = (uint16_t)mask;

	params.flags = RW_SEC | RW_SBP;

	params.sb.bank = NUR_BANK_EPC;
	params.sb.address32 = 32;
	//convert HEX string to byte array.

	printf("Lock tag [%s] mask %d, action %d\n", curepc, mask, action);

	epcBufLen = HexStringToBin(curepc, params.sb.maskdata, strlen(curepc));
	params.sb.maskbitlen = epcBufLen * 8;

	x = NurApiSetLockRaw(hApi, &params);

	if (x == NUR_SUCCESS)
		printf("Lock success!\n");
	else
		printf("Lock error = %d\n", x);

	wait_key();
	return;

INVALID_INPUT:
	printf("Invalid input\n");
	wait_key();
}

static void handle_get_device_caps()
{
	struct NUR_CMD_DEVCAPS_RESP* dc;
	int rc;

	if (!gConnected)
		return;
	cls();

	rc = NurApiGetDeviceCaps(hApi);

	if (rc == NUR_SUCCESS) {
		dc = &hApi->resp->devcaps;
		printf("* Device capabilities *\n");
		printf("flagSet1: 0x%x\n", dc->flagSet1);
		printf("flagSet2: 0x%x\n\n", dc->flagSet2);
		printf("Maximum TX power in terms of dBm: %d\n", dc->maxTxdBm);
		printf("TX level attenuation pre step in dBm: %d\n", dc->txAttnStep);
		printf("Maximum TX level in mW: %d\n", dc->maxTxmW);
		printf("Number of TX attenuation levels available: %d\n\n", dc->txSteps);
		printf("Number of 96 - bit EPCs that the module tag buffer can currently hold: %d\n", dc->szTagBuffer);
		printf("Number of maximum possible antennas with current configuration: %d\n", dc->curCfgMaxAnt);
		printf("Number of maximum possible GPIO pins with current configuration: %d\n\n", dc->curCfgMaxGPIO);
		printf("RFID chip version: %d\n", dc->chipVersion);
		printf("Module type: %d\n", dc->moduleType);
		printf("Module configuration flag bits: 0x%x\n", dc->moduleConfigFlags);
		gDevcaps = hApi->resp->devcaps;
		gDevcapsValid = TRUE;
	}
	else {
		printf("NUR GetDeviceCaps error. Code = %d.\n", rc);
	}

	wait_key();
}

static void handle_get_diag_report()
{
	struct NUR_DIAG_REPORT report;
	int error = NurApiDiagGetReport(hApi, NUR_DIAG_GETREPORT_NONE, &report, sizeof(report));
	if (error == NUR_SUCCESS)
		print_diag_report(&report);
	else
		printf("NurApiDiagGetReport error = %d\n", error);
	wait_key();
}

/* Calculate a reflected power */
static int CalcReflPower(int iPart, int qPart, int div) {
	double dRfdBm;
	double t;

	if (div == 0) {
		return iPart;
	}

	t = (double) ((iPart * iPart) + (qPart * qPart));
	dRfdBm = sqrt(t);

	if (!isnan(dRfdBm)) {
		dRfdBm = dRfdBm / div;
		if (dRfdBm <= 0)
			dRfdBm = 0;
		else
			dRfdBm = log10(dRfdBm) * 20.0f;
	} else {
		dRfdBm = -100;
	}

	return (int) (dRfdBm * 1000.0);
}

static void handle_enable_disable_events()
{
	struct NUR_CMD_LOADSETUP_PARAMS modulesetup;
	int i, rc;

	cls();
	printf("* Enable/disable (and test) events *\n");

	rc = NurApiGetModuleSetup(hApi, NUR_SETUP_OPFLAGS | NUR_SETUP_AUTOTUNE);
	if (rc == NUR_SUCCESS) {
		modulesetup = hApi->resp->loadsetup;
		modulesetup.flags = NUR_SETUP_OPFLAGS;
		// Enable/disable HOPEVENT notification
		modulesetup.opFlags ^= NUR_OPFLAGS_EN_HOPEVENTS;
		printf("NUR_OPFLAGS_EN_HOPEVENTS = %s\n", modulesetup.opFlags & NUR_OPFLAGS_EN_HOPEVENTS ? "Enabled" : "Disabled");
		// Enable/disable TUNEEVENT notification
		modulesetup.opFlags ^= NUR_OPFLAGS_EN_TUNEEVENTS;
		printf("NUR_OPFLAGS_EN_TUNEEVENTS = %s\n", modulesetup.opFlags & NUR_OPFLAGS_EN_TUNEEVENTS ? "Enabled" : "Disabled");
		if ((modulesetup.opFlags & NUR_OPFLAGS_EN_TUNEEVENTS) && !(modulesetup.autotune.mode & AUTOTUNE_MODE_ENABLE))
			printf("!!! Enable also AutoTune if you want to see tune events !!!\n");
		rc = NurApiSetModuleSetup(hApi, &modulesetup);
	}

	// Perform some inventory rounds
	rc = NurApiClearTags(hApi);
	for (i=0; i<10; i++)
	{
		rc = NurApiInventory(hApi, NULL);
	}
	printf("\n");
	wait_key();
}

static void handle_get_reflected_power_ex()
{
	struct NUR_CMD_GETREFPOWEREX_RESP *refpowerex;
	uint32_t middle_frequency;
	uint32_t measure_frequency;
	int reflected_power;

	cls();
	printf("* Get Reflected Power value *\n");

	//reflected_power = CalcReflPower(-32, 63, 71);
	//printf("Reflected Power %.3f\n", (double)reflected_power/1000);

	if (NurApiGetReflectedPowerEx(hApi, 0) == NUR_SUCCESS)
	{
		printf("\n");
		refpowerex = &hApi->resp->getrefpowerex;
		reflected_power = CalcReflPower(refpowerex->iPart, refpowerex->qPart, refpowerex->div);
		printf("Reflected Power 0 => %.3f\n", (double)reflected_power/1000);
		printf(" - iPart   : %d\n", refpowerex->iPart);
		printf(" - qPart   : %d\n", refpowerex->qPart);
		printf(" - div     : %d\n", refpowerex->div);
		printf(" - freqKhz : %d (%.3f MHz)\n", refpowerex->freqKhz, (double)refpowerex->freqKhz/1000);
		printf("\n");
		middle_frequency = refpowerex->freqKhz;

		measure_frequency = middle_frequency - 1000;
		if (NurApiGetReflectedPowerEx(hApi, measure_frequency) == NUR_SUCCESS)
		{
			reflected_power = CalcReflPower(refpowerex->iPart, refpowerex->qPart, refpowerex->div);
			printf("Reflected Power %d => %.3f (%.3f MHz)\n", measure_frequency, (double)reflected_power/1000, (double)refpowerex->freqKhz/1000);
		}

		measure_frequency = middle_frequency + 1000;
		if (NurApiGetReflectedPowerEx(hApi, measure_frequency) == NUR_SUCCESS)
		{
			reflected_power = CalcReflPower(refpowerex->iPart, refpowerex->qPart, refpowerex->div);
			printf("Reflected Power %d => %.3f (%.3f MHz)\n", measure_frequency, (double)reflected_power/1000, (double)refpowerex->freqKhz/1000);
		}
	}

	printf("\n");
	wait_key();
}

static void handle_switch_mode()
{
	int error;

	cls();
	printf("* Switching mode between App <--> Boot*\n");

	// Attempt to switch mode
	error = NurApiEnterBoot(hApi);

	// NOTE: If this is USB device, you'll need to reconnect serial here!

	if (error) {
		printf("Switch error: %d\n", error);
	} else {
		printf("Mode switched, waiting for device boot\n");
		// Wait for boot event (2 sec)
		NurApiWaitEvent(hApi, 2000);
	}

	wait_key();
}

static void handle_block_permalock(BOOL lockBlocks)
{
	int n;
	int temp;
	int error;
	struct NUR_CMD_PERMALOCK_PARAM bpParams;	// BlockPermalock parameters
	bpParams.flags = 0;

	cls();
	if (lockBlocks) {
		printf("* Set BlockPermalock (lock blocks) *\n");
	}
	else {
		printf("* Get BlockPermalock (read blocks) *\n");
	}

	// Ask target EPC from user
	if (!ask_epc(&bpParams, "Enter EPC for singulation (or leave empty): ")) {
		goto INVALID_INPUT;
	}
	// Ask access password from user
	if (!ask_password(&bpParams, "Enter access password in dec (or leave empty): ")) {
		goto INVALID_INPUT;
	}
	// Set up BlockPermalock parameters
	bpParams.plb.lock = lockBlocks;
	printf("Enter MemBank (3 is USER): ");
	if (scanf("%u%*c", &temp) != 1) {
		goto INVALID_INPUT;
	}
	bpParams.plb.bank = (uint8_t)temp;
	printf("Enter BlockPtr (the starting address for Mask, in units of 16 blocks): ");
	if (scanf("%d%*c", &bpParams.plb.addr) != 1) {
		goto INVALID_INPUT;
	}
	printf("Enter BlockRange (specifies the range of Mask, starting at BlockPtr): ");
	if (scanf("%u%*c", &temp) != 1) {
		goto INVALID_INPUT;
	}
	bpParams.plb.range = (uint8_t)temp;
	// Ask mask from user if range is > 0 and lockBlocks is TRUE
	if (bpParams.plb.lock && bpParams.plb.range > 0) {
		printf("Enter mask in hex (e.g. 0001): ");
		for (n = 0; n < bpParams.plb.range; n++) {
			if (scanf("%4hx", &bpParams.plb.wMask[n]) != 1) {
				goto INVALID_INPUT;
			}
		}
	}

	// Example of setting up BlockPermalock parameters
	// This is commented out, but you can use it as a reference
	// The example sets up a permalock for USER memory bank,
	// starting at address 0, range of 2 blocks,
	// and a mask of 0xFFFF and 0x000F.
	// So it would lock the first 20 blocks of USER memory
	// 
	// bpParams.plb.lock = TRUE;
	// bpParams.plb.bank = NUR_BANK_USER;
	// bpParams.plb.addr = 0;
	// bpParams.plb.range = 2;
	// bpParams.plb.wMask[0] = 0xFFFF;
	// bpParams.plb.wMask[1] = 0x000F;

	error = NurApiPermalock(hApi, &bpParams);

	if (error == NUR_SUCCESS)
	{
		if (lockBlocks) {
			printf("BlockPermalock set successfully\n");
		}
		else {
			printf("BlockPermalock read successfully\n");
			printf("Bank: %d, Addr: %d, nMask: %d",
				hApi->resp->permalock.bank,
				hApi->resp->permalock.addr,
				hApi->resp->permalock.nMask);
			for (n = 0; n < hApi->resp->permalock.nMask; n++) {
				printf(", wResp[%d]: %04X", n, hApi->resp->permalock.wResp[n]);
			}
			printf("\n");
		}
	}
	else
	{
		printf("NurApiPermalock Error: %d, Message: %s\n", error, (char*)NurApiGetErrorMessage(error));
	}
	wait_key();
	return;

INVALID_INPUT:
	printf("Invalid input\n");
	wait_key();
}

static void show_block_permalock_menu()
{
	while (TRUE)
	{
		cls();
		printf("* BlockPermalock operations menu *\n");
		printf("[1]\tSet BlockPermalock (lock)\n");
		printf("[2]\tGet BlockPermalock (read)\n");
		printf("\nESC\tReturn\n");
		printf("\nSelection: ");

		int key = _getch();
		printf("\n");

		if (key == 27) // ESC
			return;

		switch (key)
		{
		case '1': handle_block_permalock(TRUE); break;
		case '2': handle_block_permalock(FALSE); break;
		default: break;
		}
	}
}

static void options()
{
	if (gConnected)
	{
		char mode = 'X';
		int error = NurApiGetMode(hApi, &mode);
		if (error == NUR_SUCCESS) {
			if (mode != 'A')
				printf("WARNING: Device is not running in application mode. Press 's' to switch mode\n\n");
		} else {
			printf("WARNING: Could not get device mode, error %d\n\n", error);
		}

		printf("[1]\tDisconnect\n");
		printf("[2]\tPing\n");
		printf("[3]\tVersions\n");
		printf("[4]\tReader info\n");
		printf("[5]\tGet setup\n");
		printf("[6]\tInventory\n");
		printf("[7]\tInventoryEx select SGTIN-96 tags only\n");
		printf("[8]\tTune antenna and enable Auto-Tune feature\n");
		printf("[9]\tSet TX Level\n");
		printf("[d]\tInventoryRead\n");
		printf("[b]\tBlockPermalock\n");
		printf("[e]\tEnable/disable events\n");
		printf("[r]\tGet Reflected Power\n");
		printf("[s]\tSwitch device mode to '%c'\n", mode == 'A' ? 'B' : 'A');
		printf("[u]\tUpdate app (app_update.bin)\n");
		printf("[c]\tContinuous carrier\n");
		printf("[w]\tWrite EPC\n");
		printf("[p]\tWrite to USER mem\n");
		printf("[k]\tKill tag\n");
		printf("[l]\tLock tag\n");
		printf("[a]\tSet antenna\n");
		printf("[z]\tGet device capabilities\n");
		printf("[x]\tGet diagnostics report\n");

	} else {
		printf("[1]\tConnect\n");
	}
	printf("\nESC\tExit\n");

	printf("\nSelection: ");
}

static int32_t do_command()
{
	int key;
	options();
	key = _getch();
	printf("\n");

	if (key == 27)
		return FALSE;

	switch (key)
	{
	case '1': handle_connection(); break;
	case '2': handle_ping(); break;
	case '3': handle_versions(1); break;
	case '4': handle_readerinfo(); break;
	case '5': handle_setup_get(); break;
	case '6': handle_inventory(0); break;
	case 'd': handle_inventory(1); break;
	case '7': {
		uint8_t epcMask[] = { 0x30 }; // Select SGTIN-96 tags (EPC starting with "30")
		handle_inventoryex_epcmask(epcMask, sizeof(epcMask));
		break;
			  }
	case '8': handle_tune(); break;
	case '9': handle_setup_set_txlevel(); break;
	case 'b': show_block_permalock_menu(); break;
	case 'e': handle_enable_disable_events(); break;
	case 'r': handle_get_reflected_power_ex(); break;
	case 's': handle_switch_mode(); break;
	case 'u': handle_app_update(); break;
	case 'c': handle_cont_carrier(); break;
	case 'w': handle_writeEPC(); break;
	case 'p': handle_writeToUserMem(); break;
	case 'a': handle_selectAntenna(); break;
	case 'k': handle_kill_tag(); break;
	case 'l': handle_lock_tag(); break;
	case 'z': handle_get_device_caps(); break;
	case 'x': handle_get_diag_report(); break;

	default: break;
	}

	cls();
	return TRUE;
}

int main(int argc, char *argv[])
{
	InitNurApiHandle(hApi);

	while (do_command());

	close_serial();

	return 0;
}
