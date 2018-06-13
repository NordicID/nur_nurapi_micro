#include <NurMicroApi.h>
#include <SoftwareSerial.h>

/*
  NurExample
  Does tag inventory in loop every 1000ms.

  This example code is in the public domain.
*/

// NUR serial port baud rate.
// NOTE: By default NUR module is configured to 115200 baudrate.
//       If using something else baudrate (e.g. 38400) you'll need to reconfigure NUR for other baudrate using Nordic ID RFID Configurator app.
#define NUR_SERIAL_BAUDRATE   (38400)

// Print serial port baudrate. If not used, leave undefined
#define PRINT_SERIAL_BAUDRATE (38400)

// Software serial
SoftwareSerial swSerial(10, 11); // RX, TX

// In this setup NUR is connected to software serial and print data in HW serial
#define NurSerial swSerial
#define PrintSerial Serial

// In this setup NUR is connected to HW serial and print data in software serial
//#define NurSerial Serial
//#define PrintSerial swSerial

#ifndef PRINT_SERIAL_BAUDRATE
  #undef PrintSerial
#endif

// The API's communication buffers. Adjust if needed
static BYTE ApiRxBuffer[256];
static BYTE ApiTxBuffer[128];

// True if NUR module detected in setup()
BOOL NurAvailable = FALSE;

// NurMicroApi handle
static struct NUR_API_HANDLE gApi =
{
  NULL, // void *UserData;
  NULL, // TransportReadDataFunction
  NULL, // TransportWriteDataFunction
  NULL, // UnsolEventHandler;

  NULL, // BYTE *TxBuffer;
  0,    // DWORD TxBufferLen;

  NULL, //BYTE *RxBuffer;
  0,    // DWORD RxBufferLen;
  0,    // DWORD RxBufferUsed;

  0,    // DWORD respLen;
  NULL  // struct NUR_CMD_RESP *resp;
};

// Read buffer from NUR serial
// If no data available, this function should take 500us - 1000us
int nur_serial_read(struct NUR_API_HANDLE *hNurApi, BYTE *buffer, DWORD bufferLen, DWORD *bytesRead)
{
  DWORD dwRead = 0;
  DWORD retryCount = 200;

  // Wait for data
  while (dwRead == 0 && retryCount-- > 0)
  {
    // Read all data available
    while (NurSerial.available()) {
      buffer[dwRead++] = NurSerial.read();
      if (dwRead == bufferLen)
        break;
    }
  }

  if (dwRead == 0) {
    return NUR_ERROR_TR_TIMEOUT;
  }

  *bytesRead = dwRead;

  return NUR_SUCCESS;
}

// Write buffer to NUR serial
int nur_serial_write(struct NUR_API_HANDLE *hNurApi, BYTE *buffer, DWORD bufferLen, DWORD *bytesWritten)
{
  DWORD dwWritten = 0;

  while (dwWritten < bufferLen)
  {
    NurSerial.write(buffer[dwWritten++]);
  }

  *bytesWritten = dwWritten;
  return NUR_SUCCESS;
}

// Init Nur api buffers and transport
void nur_init_handle(struct NUR_API_HANDLE *hApi)
{
  // Init RX buffer
  hApi->RxBuffer = ApiRxBuffer;
  hApi->RxBufferLen = sizeof(ApiRxBuffer);

  // Init TX buffer
  hApi->TxBuffer = ApiTxBuffer;
  hApi->TxBufferLen = sizeof(ApiTxBuffer);

  // Init transport functions
  hApi->TransportReadDataFunction = nur_serial_read;
  hApi->TransportWriteDataFunction = nur_serial_write;
}

// Print NUR module mode (A = app, B = bootloader) and versions
static void nur_print_versions()
{
#ifdef PrintSerial
  struct NUR_CMD_VERSION_RESP *vr;
  int rc = NurApiGetVersions(&gApi);
  vr = &gApi.resp->versions;

  if (rc == NUR_SUCCESS) {
    PrintSerial.print(F("Versions, mode "));
    PrintSerial.print((char)vr->mode);
    PrintSerial.println("");

    PrintSerial.print(F(" - primary   : "));
    PrintSerial.print(vr->vMajor, DEC);
    PrintSerial.print(F("."));
    PrintSerial.print(vr->vMinor, DEC);
    PrintSerial.print(F("-"));
    PrintSerial.print((char)vr->vBuild);
    PrintSerial.println("");

    PrintSerial.print(F(" - secondary : "));
    PrintSerial.print(vr->otherMajor, DEC);
    PrintSerial.print(F("."));
    PrintSerial.print(vr->otherMinor, DEC);
    PrintSerial.print(F("-"));
    PrintSerial.print((char)vr->otherBuild);
    PrintSerial.println("");
  }
  else {
    PrintSerial.print(F("Version error: "));
    PrintSerial.print(rc, DEC);
    PrintSerial.println("");
  }
#endif
}

// Configure NUR module
static void nur_configure_module()
{
  int rc = NUR_SUCCESS;
  struct NUR_CMD_LOADSETUP_PARAMS params;
  // Flag settings that you want to chage
  params.flags = NUR_SETUP_TXLEVEL | NUR_SETUP_ANTMASK | NUR_SETUP_SELECTEDANT;
  // Set TxLevel to maximum (500mW/1000mW depending from the module)
  params.txLevel = 0;
  // Enable antenna 0.
  // Use bit operation if you want to enable multiple
  // antennas like antenna 0 and 1 (NUR_ANTENNAMASK_1 | NUR_ANTENNAMASK_2)
  params.antennaMask = NUR_ANTENNAMASK_1;
  // Set antenna selection to auto mode
  params.selectedAntenna = NUR_ANTENNAID_AUTOSELECT;
#ifdef PrintSerial
  PrintSerial.print(F("Configure NUR module"));
#endif
  // Set new module setti
  rc = NurApiSetModuleSetup(&gApi, &params);
  if (rc == NUR_SUCCESS) {
#ifdef PrintSerial
    PrintSerial.print(F("OK"));
#endif
  }
  else
  {
#ifdef PrintSerial
    PrintSerial.print(F("SetModuleSetup error. Code = "));
    PrintSerial.print(rc, DEC);
    PrintSerial.println("");
#endif
  }
}

#ifdef PrintSerial
void print_hex(int val) {
  char tmp[3];
  sprintf(tmp, "%02X", val);
  PrintSerial.print(tmp);
}
#endif

// This function is called for each inventoried tag, see NurApiFetchTagAt in nur_tag_inventory()
int nur_fetch_tags_function(struct NUR_API_HANDLE *hNurApi, struct NUR_IDBUFFER_ENTRY *tag)
{
#ifdef PrintSerial
  int n;
  PrintSerial.print(F("Antenna "));
  PrintSerial.print(tag->antennaId, DEC);
  PrintSerial.print(F(" RSSI "));
  PrintSerial.print(tag->rssi, DEC);
  PrintSerial.print(F(" ("));
  PrintSerial.print(tag->scaledRssi, DEC);
  PrintSerial.print(F("%) EPC: "));

  for (n = 0; n < tag->epcLen; n++) {
    print_hex(tag->epcData[n]);
  }
  PrintSerial.println("");
#endif

  return NUR_SUCCESS; // non-zero terminates tag buffer parsing
}

// Perform tag inventory
// 1. Clear tag buffer
// 2. Perform inventory
// 3. Fetch tags
void nur_tag_inventory()
{
  int rc;

#ifdef PrintSerial
  PrintSerial.println("Inventory begin");
#endif

  // Clear tag buffer
  rc = NurApiClearTags(&gApi);
  if (rc == NUR_SUCCESS)
  {
    // Perform tag inventory
    rc = NurApiInventory(&gApi, NULL); // Pass NULL as params, uses default inventory settings from module setup
    if (rc == NUR_SUCCESS)
    {
      // Fetch tags one by one
      int tagCount, n;
      tagCount = gApi.resp->inventory.numTagsMem;
      for (n = 0; n < tagCount; n++)
      {
        rc = NurApiFetchTagAt(&gApi, TRUE, n, nur_fetch_tags_function);
        if (rc != NUR_SUCCESS) {
          break;
        }
      }
#ifdef PrintSerial
      PrintSerial.println(F("Inventory done"));
#endif
    }
    else {
#ifdef PrintSerial
      PrintSerial.print(F("Inventory error: "));
      PrintSerial.print(rc, DEC);
      PrintSerial.println("");
#endif
    }
  }
  else
  {
#ifdef PrintSerial
    PrintSerial.print(F("ClearTags error: "));
    PrintSerial.print(rc, DEC);
    PrintSerial.println("");
#endif
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

#ifdef PrintSerial
  // Open the print serial port
  PrintSerial.begin(PRINT_SERIAL_BAUDRATE);
  PrintSerial.println(F("Start"));
#endif

  // Open the NUR serial port
  NurSerial.begin(NUR_SERIAL_BAUDRATE);

  // Initialize NUR handle
  nur_init_handle(&gApi);

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

  // Send ping to NUR to test availability
  if (NurApiPing(&gApi) == NUR_SUCCESS) {
    // Got OK response
    NurAvailable = TRUE;
#ifdef PrintSerial
    PrintSerial.println(F("NUR DETECTED"));
#endif
    nur_print_versions();
  }
  else {
    // No response or invalid response
#ifdef PrintSerial
    PrintSerial.println(F("NUR NOT DETECTED"));
#endif
  }
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
}

// the loop function runs over and over again forever
void loop() {
  if (!NurAvailable)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);                       // wait for a 100ms
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(100);                       // wait for a 100ms
#ifdef PrintSerial
    PrintSerial.println(F("ERROR"));
#endif
    return;
  }

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  nur_tag_inventory();               // perform tag inventory
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a 1000ms
}
