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

#ifndef _NURPROTOCOL_H_
#define _NURPROTOCOL_H_ 1

#ifdef __GNUC__
#define NUR_PACKED __attribute__((packed))
#else
#define NUR_PACKED
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup NURPROTO NUR Protocol
 *  Low level protocol functions
 *  @{
 */


/**
 * NUR module basic commands
 */
enum {
	NUR_BASIC_BASE = 1,
	NUR_CMD_PING = NUR_BASIC_BASE,
	NUR_CMD_VERSION,
	NUR_CMD_RESET,
	NUR_CMD_GETMODE,
	NUR_CMD_CLEARIDBUF, //5
	NUR_CMD_GETIDBUF,
	NUR_CMD_GETMETABUF,
	NUR_CMD_GETSYSTEM,
  	NUR_CMD_GETREADERINFO,
  	NUR_CMD_MCUARCH, //10
  	NUR_CMD_DEVCAPS,
  	NUR_CMD_VERSIONEX,
	NUR_CMD_BEEP,
	NUR_CMD_STOPALLCONT,
	NUR_CMD_CONFIGGPIO, //15
	NUR_CMD_GETGPIO,
	NUR_CMD_SETGPIO,
	NUR_CMD_SENSORS,
	NUR_CMD_FACTORYRESET,
	NUR_CMD_RESTART,
	NUR_CMD_GETETHCONFIG,
	NUR_CMD_SETETHCONFIG,
	NUR_CMD_TITLE_SET,
	NUR_CMD_TITLE_GET,
	NUR_CMD_GETFWINFO = 30,
	NUR_BASIC_LAST
};

/**
 * NUR module control commands
 */
enum {
	NUR_CTL_BASE = 0x20,
	NUR_CMD_SETBDR = NUR_CTL_BASE,
	NUR_CMD_ANTIDTRANSLATION,	// WAS CMD_RESERVED3, IN USE SINCE 4.9-L
	NUR_CMD_LOADSETUP2,
	NUR_CMD_INVREADCONFIG,
	NUR_CMD_GETREGIONINFO,
	NUR_CMD_ANTENNAMAP,		// WAS CMD_RESERVED5, IN USE SINCE 4.9-H
	NUR_CMD_RESERVED4,
	NUR_CMD_ANTENNA,		// Applies to USB table reader
	NUR_CMD_STORESETUP,
	NUR_CMD_CUSTOMHOP,
	NUR_CMD_CUSTOMHOP_EX,
	NUR_CTL_LAST
};

/**
 * NUR module UHF G2 commands
 */
enum {
	NUR_G2_BASE = 0x30,
	NUR_CMD_SCANSINGLE = NUR_G2_BASE,
	NUR_CMD_INVENTORY,
	NUR_CMD_INVENTORYSEL,
	NUR_CMD_READ,
	NUR_CMD_WRITE,
	NUR_CMD_BLWRITE,
	NUR_CMD_LOCK,
	NUR_CMD_KILL,
	NUR_CMD_TRACETAG,
	NUR_CMD_INVENTORYSTREAM,
	NUR_CMD_RESETTARGET,
	NUR_CMD_INVENTORYEX,
	NUR_CMD_CUSTREAD,
	NUR_CMD_CUSTWRITE,
	NUR_CMD_CUSTBLWRITE,
	NUR_CMD_CUSTOMEXCHANGE,
	NUR_CMD_BLKERASE,
	NUR_CMD_INVENTORYREAD,
	NUR_CMD_BLWRITE_EX,
	NUR_CMD_EPCENUM,
	NUR_CMD_PERMALOCK,
	NUR_G2_LAST
};

/**
 * NUR module proprietary extensions commands
 */
enum {
	NUR_PROPR_BASE = 0x50,
	NUR_NXPCMD_RDPROTECT = NUR_PROPR_BASE,
	NUR_NXPCMD_EAS,
	NUR_NXPCMD_EASALARM,
	NUR_MZ4CMD_QT,
	NUR_NXPCMD_EASALARMSTREAM,
	NUR_BLE_EXT, // 0x55
	NUR_PROPR_LAST
};

/**
 * NUR module test commands
 */
enum {
	NUR_TEST_BASE = 0x60,
	NUR_CMD_GETREFPOWER = NUR_TEST_BASE,
	NUR_CMD_CONTCARR,
	NUR_CMD_CARRIER,
	NUR_CMD_SCANCHANNELS,
	NUR_CMD_RFSETTINGS,
	NUR_CMD_RFRESERVED1,
	NUR_CMD_TUNEANTENNA,
	NUR_CMD_GETREFPOWEREX,
	NUR_CMD_RESERVED2,
	NUR_CMD_RESERVED3,
	NUR_CMD_SETCHANNEL,
	NUR_TEST_LAST
};

/**
 * NUR module bootloader commands
 */
enum {
	NUR_BOOT_BASE = 0x70,
	NUR_CMD_UNLOCKALL = NUR_BOOT_BASE,
	NUR_CMD_PAGEWRITE,
	NUR_CMD_PAGEREAD,
	NUR_CMD_ENTERBOOT,
	NUR_CMD_APPVALIDATE,
	NUR_CMD_QUERYCRC,
	NUR_CMD_PRODUCTION_CFG,
	NUR_CMD_BLVALIDATE,
	NUR_CMD_CRYPTOBYPASS,
	NUR_CMD_SCRATCHDATA,
	NUR_BOOT_LAST
};

/**
 * NUR module notifications
 */
enum {
	NUR_NOTIFY_BOOT = 0x80,
	NUR_NOTIFY_IOCHANGE,
	NUR_NOTIFY_INVENTORY,
	NUR_NOTIFY_TT_INVENTORY,
	NUR_NOTIFY_TRACETAG,
	NUR_NOTIFY_TRIGGERREAD,
	NUR_NOTIFY_HOPEVENT,
	NUR_NOTIFY_DEBUGMSG,
	NUR_NOTIFY_INVENTORYEX,
	NUR_NOTIFY_NXPALARM,
	NUR_NOTIFY_EPCENUM,
	NUR_NOTIFY_EXTIN,
	NUR_NOTIFY_GENERAL,
	NUR_NOTIFY_AUTOTUNE,
	NUR_NOTIFY_WLAN_SEARCH,
	NUR_NOTIFY_AR_NOTIFY,
	NUR_NOTIFY_BLE_READER,
	NUR_NOTIFY_LAST
};

/**
 * NUR module register commands
 */
enum {
	NUR_REG_BASE = 0x90,
	NUR_CMD_READREG = NUR_REG_BASE,
	NUR_CMD_WRITEREG,
	NUR_CMD_MAKEPERMANENT,
	NUR_CMD_WRITELONG,
	NUR_CMD_REGDUMP,
	NUR_CMD_GENSETUP,
	NUR_CMD_READCONT,
	NUR_CMD_READALL,
	NUR_REG_LAST
};

/*
  Standalone and commands for ethernet Sampo
*/
enum {

	NUR_ETH_BASE = 0xA0,
	NUR_CMD_STANDALONE_GET_1 = NUR_ETH_BASE,
	NUR_CMD_STANDALONE_SET_1,
	NUR_CMD_STANDALONE_GET_2,
	NUR_CMD_STANDALONE_SET_2,
	NUR_CMD_EXTIO,
	NUR_CMD_ETHCLOCK,
	NUR_CMD_ALLOW_NOTIFY,
	NUR_CMD_GET_HCR,
	CMD_GET_BUFFEREDDATA,
	CMD_ACK_BUFFEREDDATA,
	NUR_ETH_LAST
};

// Internal FLASH Page Size: 256 bytes
#define NUR_FLASH_PAGE_SIZE		(256)
// Internal FLASH Page Size: 256/4 uint32_ts
#define NUR_FLASH_PAGE_SIZE_DW	(256/4)

#define ISEL_INVERT (1<<0)
#define ISEL_64ADDR (1<<1)

#define RW_SEC (1<<0)
#define RW_SBP (1<<1)
#define RW_EA1 (1<<2)
#define RW_EA2 (1<<3)

#define PACKET_FLAG_UNSOL  (1<<0)
#define PACKET_FLAG_IRDATA (1<<1)
#define PACKET_FLAG_ACK    (1<<2)

#define NUR_MAX_SENSORS		2
#define NUR_SZ_SENSOR_CONF  2

#define NUR_SENSOR_TAP		(1<<0)
#define NUR_SENSOR_LIGHT	(1<<1)

#define NUR_SENSOR_EVENT_FLAG     (1<<7)
#define NUR_SENSOR_EVENT_MASK     (~(1<<7))
#define NUR_TAP_EVENT_VALUE       0
#define NUR_LIGHT_EVENT_VALUE     1
#define NUR_SENSOR_EVENT_TAP      (SENSOR_EVENT_FLAG | TAP_EVENT_VALUE)
#define NUR_SENSOR_EVENT_LIGHT    (SENSOR_EVENT_FLAG | LIGHT_EVENT_VALUE)

#define NUR_READERINFO_VERSION1		0x52444901

#define NUR_MAX_SEND_SZ				((2*1024)-1)
#define NUR_MAX_RCV_SZ				((8*1024)-1)

/** Minimum number of bytes to write into a scratch page. */
#define MIN_SCRATCHBYTES	1
/** Maximum number of bytes to write into a scratch page. */
#define MAX_SCRATCHBYTES	256

#define CRYPTO_PERMISSION_LENGTH    32


#ifdef _WIN32
#pragma pack(push, 1)
#endif

struct NUR_HEADER
{
	uint8_t start;
	uint16_t payloadlen;
	uint16_t flags;
	uint8_t checksum;
} NUR_PACKED;

#define PACKET_START    0xA5
#define CS_STARTBYTE    0xFF
#define HDR_SIZE sizeof(struct NUR_HEADER)

struct NUR_IDBUFFER_ENTRY
{
	int8_t rssi;
	uint8_t scaledRssi;
	uint16_t timestamp;
	uint32_t freq;
	uint8_t dataLen;
	uint16_t pc;
	uint8_t channel;
	uint8_t antennaId;
	uint8_t epcLen;
	/* EPC + associated data */
	uint8_t *epcData;
} NUR_PACKED;

/////////////////////////////////////////////////////////////////////////////
// PARAMETERS

struct NUR_CMD_BAUDRATE_PARAMS
{
	uint8_t setting;
} NUR_PACKED;

struct NUR_CMD_PAGEREAD_PARAMS
{
	uint16_t pagetoread;
	uint8_t cPermission[CRYPTO_PERMISSION_LENGTH];
} NUR_PACKED;

struct NUR_CMD_PAGEWRITE_PARAMS
{
	uint16_t pagetowrite;
	uint32_t crc;
	uint8_t data[NUR_FLASH_PAGE_SIZE];
} NUR_PACKED;

struct NUR_CMD_SCRATCHDATA_PARAMS
{
	uint8_t read;
	uint8_t page;
	uint8_t offset;
	uint16_t dataLen;
	uint32_t dwReserved;
	uint8_t data[MIN_SCRATCHBYTES];
} NUR_PACKED;

struct NUR_CMD_APPVALIDATE_PARAMS
{
	uint32_t appsize;
	uint32_t appcrc;
} NUR_PACKED;

struct NUR_CMD_SCANSINGLE_PARAMS
{
	uint16_t timeout;
} NUR_PACKED;

struct NUR_CMD_INVENTORY_PARAMS
{
	uint8_t Q;
	uint8_t session;
	uint8_t rounds;
} NUR_PACKED;

struct NUR_CMD_INVENTORYEX_FILTER
{
	uint8_t truncate;
	uint8_t target;
	uint8_t action;
	uint8_t bank;
	uint32_t address;
	uint8_t maskbitlen;
	uint8_t maskdata[NUR_MAX_SELMASK];
} NUR_PACKED;

struct NUR_CMD_INVENTORYEX_PARAMS
{
	uint8_t flags;
	uint8_t Q;
	uint8_t session;
	uint8_t rounds;
	uint16_t transitTime;
	uint8_t inventoryTarget;
	uint8_t inventorySelState;
	uint8_t filterCount;
	struct NUR_CMD_INVENTORYEX_FILTER filters[NUR_MAX_FILTERS];
} NUR_PACKED;

struct NUR_CMD_IRCONFIG_PARAMS
{
  uint8_t active;	// On / off control; 1 means on.
  uint8_t type;		// Type for inventory + read: 0 = EPC + data, 1 = data only (returned in the EPC field).
  uint8_t bank;
  uint32_t wAddress;
  uint8_t wLength;
} NUR_PACKED;

struct NUR_SINGULATIONBLOCK
{
	uint8_t bytestofollow;
	uint8_t bank;
	union {
		uint32_t address32;
		uint64_t address64;
	};
	uint16_t maskbitlen;
	uint8_t maskdata[NUR_MAX_SELMASK];
} NUR_PACKED;

struct NUR_READBLOCK
{
	uint8_t bytestofollow;
	uint8_t bank;
	union {
		uint32_t address32;
		uint64_t address64;
	};
	uint8_t wordcount;
} NUR_PACKED;

struct NUR_WRITEBLOCK
{
	uint8_t bytestofollow;
	uint8_t bank;
	union {
		uint32_t address32;
		uint64_t address64;
	};
	uint8_t wordcount;
	uint8_t data[255];
} NUR_PACKED;

struct NUR_WRITEBLOCK_EX
{
	uint8_t bytestofollow;
	uint8_t bank;
	union {
		uint32_t address32;
		uint64_t address64;
	};
	uint8_t wordcount;
	uint8_t blSize;
	uint8_t data[255];
} NUR_PACKED;

struct NUR_LOCKBLOCK
{
	uint8_t bytestofollow;
	uint16_t mask;
	uint16_t action;
} NUR_PACKED;

struct NUR_PERMALOCKBLOCK
{
	uint8_t lock;
	uint8_t bank;
	uint32_t addr;
	uint8_t range;
	uint16_t wMask[255];
} NUR_PACKED;

struct NUR_SINGULATED_CMD_PARAMS
{
	uint8_t flags;
	uint32_t passwd;
	struct NUR_SINGULATIONBLOCK sb;
} NUR_PACKED;

struct NUR_CMD_READ_PARAMS
{
	uint8_t flags;
	uint32_t passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_READBLOCK rb;
} NUR_PACKED;

struct NUR_CMD_WRITE_PARAMS
{
	uint8_t flags;
	uint32_t passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_WRITEBLOCK wb;
} NUR_PACKED;

struct NUR_CMD_BLWRITEEX_PARAMS
{
	uint8_t flags;
	uint32_t passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_WRITEBLOCK_EX wbe;
} NUR_PACKED;

struct NUR_CMD_BLKERASE_PARAMS
{
	uint8_t flags;
	uint32_t passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_READBLOCK erb;
} NUR_PACKED;

struct NUR_CMD_LOCK_PARAMS
{
	uint8_t flags;
	uint32_t passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_LOCKBLOCK lb;
} NUR_PACKED;

struct NUR_CMD_PERMALOCK_PARAM
{
	uint8_t flags;
	uint32_t passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_PERMALOCKBLOCK plb;
} NUR_PACKED;

struct NUR_CMD_KILL_PARAMS
{
	uint8_t flags;
	uint32_t passwd;

	struct NUR_SINGULATIONBLOCK sb;
} NUR_PACKED;

struct NUR_NXP_PARAMS
{
	uint8_t flags;
	uint32_t passwd;
	int32_t set;

	struct NUR_SINGULATIONBLOCK sb;
} NUR_PACKED;

struct NUR_MONZAQTPARAMS
{
	uint8_t flags;
	uint32_t passwd;
	int32_t write;
	// N/ for the time being.
	//int32_t persist;
	int32_t reduce;
	int32_t pubmem;
	struct NUR_SINGULATIONBLOCK sb;
} NUR_PACKED;

struct NUR_CMD_TRACETAG_PARAMS
{
	uint8_t flags;
	uint8_t bank;
	union {
		uint32_t address32;
		uint64_t address64;
	};
	uint8_t maskbitlen;
	uint8_t maskdata[NUR_MAX_SELMASK];
} NUR_PACKED;

struct NUR_CMD_SENSORS_PARAMS
{
	uint8_t flags;
	uint8_t config[NUR_MAX_SENSORS * NUR_SZ_SENSOR_CONF];
} NUR_PACKED;

struct NUR_GPIO_SETUP
{
	uint8_t enabled;
	uint8_t type;
	uint8_t edge;
	uint8_t action;
} NUR_PACKED;

struct NUR_CMD_CONFIGGPIO_PARAMS
{
	uint8_t flags;
	struct NUR_GPIO_SETUP config[NUR_MAX_GPIO];
} NUR_PACKED;

struct NUR_GPIO_SETSTATE
{
	uint8_t state;
} NUR_PACKED;

struct NUR_CMD_SETGPIO_PARAMS
{
	uint8_t flags;
	union {
		struct NUR_GPIO_SETSTATE state;
		struct NUR_GPIO_SETSTATE states[NUR_MAX_GPIO];
	};
} NUR_PACKED;

struct NUR_CMD_GETGPIO_PARAMS
{
	uint8_t flags;
} NUR_PACKED;

struct NUR_SETUPLOAD_PARAMS
{
	uint16_t flags;
	uint8_t regionId;
	uint32_t linkfreq;
	uint8_t miller;
	uint8_t txLevel;
	uint8_t modulation;
} NUR_PACKED;

/**
 * Tag RSSI filter.
 * @sa NurApiGetModuleSetup(), NurApiSetModuleSetup()
 */
struct NUR_RSSI_FILTER
{
	int8_t min; /**< Minimum accepted RSSI in dBm. Use 0 to disable filtering. */
	int8_t max; /**< Maximum accepted RSSI in dBm. Use 0 to disable filtering. */
} NUR_PACKED;

/**
 * Module's autotune setup. Supported from FW 5.0-A in L2 module.
 *
 * @sa AUTOTUNE_MODE_ENABLE, AUTOTUNE_MODE_THRESHOLD_ENABLE
*/
struct NUR_AUTOTUNE_SETUP
{
	uint8_t mode;			/**< Autotune mode setting, bits: @sa AUTOTUNE_MODE_ENABLE, AUTOTUNE_MODE_THRESHOLD_ENABLE. */
	int8_t threshold_dBm;	/**< Low level threshold in dBm. Do autotune if current reflected power worse than 'threshold_dBm' */
};

struct NUR_CMD_LOADSETUP_PARAMS
{
	uint32_t flags;
	uint32_t linkFreq;
	uint8_t rxDecoding; // Miller
	uint8_t txLevel;
	uint8_t txModulation;
	uint8_t regionId;
	uint8_t inventoryQ;
	uint8_t inventorySession;
	uint8_t inventoryRounds;
	uint8_t antennaMask;
	uint16_t scanSingleTriggerTimeout;
	uint16_t inventoryTriggerTimeout;
	uint8_t selectedAntenna;
	uint32_t opFlags;
	uint8_t inventoryTarget;
	uint8_t inventoryEpcLength;
	struct NUR_RSSI_FILTER readRssiFilter;
	struct NUR_RSSI_FILTER writeRssiFilter;
	struct NUR_RSSI_FILTER inventoryRssiFilter;

	uint16_t readTO;
	uint16_t writeTO;
	uint16_t lockTO;
	uint16_t killTO;

	/** Defines how the periodic auto-inventory power saving is configured.
	 * When in use, reader will be power save mode while no tags in view.
	 */
	uint16_t periodSetup;

	/** DEPRECATED! Per antenna specific power levels. Use antPowerEx instead. */
	uint8_t antPower[NUR_MAX_ANTENNAS];

	/** Modify power levels' offset values (-1, 0, 1) NOTE: First entry in array used for all antennas! Rest of the entries are discarded. */
	int8_t powerOffset[NUR_MAX_ANTENNAS];

	/** Bitmask of enabled antennas, support up to 32 antennas. Value 0x1 - 0xFFFFFFFF.
	 * Example: Value 0x4 means that only antenna 3 is enabled.
	 * Example: Value 0x30300 means that antennas 9,10,17,18 are enabled.
	 */
	uint32_t antennaMaskEx;

	/** Runtime auto tuning settings. */
	struct NUR_AUTOTUNE_SETUP autotune;

	/** Per antenna specific tx levels. Array of the per antenna tx level values. Range is 0..19; Defaults to -1. If set to -1 default tx level is used from the module setup. */
	int8_t antPowerEx[NUR_MAX_ANTENNAS_EX];

	/** The receiver sensitivity field. 0 = Nominal, 1 = Low, 2 = High */
	uint8_t rxSensitivity;

	// ADDED NUR2 7.0
	uint8_t rfProfile;

	// ADDED NUR2 7.5, NanoNur 10.2
	uint16_t toSleepTime;
} NUR_PACKED;

struct NUR_CMD_BEEP_PARAMS
{
	uint32_t freq;
	uint32_t time;
	uint8_t duty;
} NUR_PACKED;

struct NUR_CMD_RESETTARGET_PARAMS
{
	uint8_t session;
	uint8_t targetIsA;
} NUR_PACKED;

#define NUR_CMD_CUSTOMHOP_PARAMS	NUR_CUSTOMHOP_PARAMS

struct NUR_CMD_ETHDEV_PARAMS
{
	uint8_t titleLength;
	uint8_t title[32];
	uint8_t mask[4];	//Subnet mask (used in static IP mode)
	uint8_t gw[4];		//Gateway	(used in static IP)
	uint8_t addrType; //Address type 0=DHCP(default) 1=STATIC
	uint8_t staticip[4];
	uint8_t serverPort[2];
	uint8_t hostmode;	 //Mode: 0=Server (default) 1=Client
	uint8_t hostip[4]; //Client host IP (if Mode=Client)
	uint8_t hostPort[2];	//Client mode port (if Mode=Client)
	uint8_t reserved[8]; //For future usage
} NUR_PACKED;

struct NUR_CMD_EPCENUMPARAM
{
	uint8_t ant;			/* Selected antenna */
	uint8_t twAddr;		/* Word address of the TID data i.e. where to read the TID info */
	uint8_t twLen;			/* TID data word length. */
	uint8_t useBlWrite;	/* Block write type if any: 0 = no, 1 = one word, 2 = two words. */
	uint8_t startVal[8];	/* Start value of the enumerated value in the EPC : equals to "unsigned long long" i.e. 64-bit unsigned integer. */
	uint8_t epcLen;		/* EPC length in words; 4...8 */
	uint8_t modAddr;		/* Bit address where the enumerated value is located in the EPC memory */
	uint8_t bitLen;		/* Bit length of the modified value: 1...64 */
	uint8_t bReset;		/* If non-zero then the current TID information is cleared */
	uint8_t baseEPC[MAX_EE_EPCLEN];	/* Base EPC to be used & modified. */
} NUR_PACKED;

#define PRODUCTION_TUNE_MAGICLEN    8

struct NUR_CMD_TUNECOMMANDPARAM
{
	uint32_t type;
	uint32_t antenna;
	uint32_t band;
	uint32_t userSave;
	int32_t goodEnough;
	uint8_t code[PRODUCTION_TUNE_MAGICLEN];
} NUR_PACKED;

/////////////////////////////////////////////////////////////////////////////
// RESPONSES

struct NUR_TUNERESULT
{
	int32_t I;
	int32_t Q;
	int32_t dBm;
} NUR_PACKED;

struct NUR_CMD_TUNEANTENNA_RESP
{
	int32_t antenna;
	int32_t resrv[3];
	struct NUR_TUNERESULT bands[NR_TUNEBANDS];
} NUR_PACKED;

struct NUR_SINGLETUNE_RESP
{
	int32_t antenna;
	int32_t band;
	struct NUR_TUNERESULT result;
} NUR_PACKED;

struct NUR_CMD_BAUDRATE_RESP
{
	uint8_t setting;
} NUR_PACKED;

struct NUR_CMD_PING_RESP
{
	char status[2];
} NUR_PACKED;

struct NUR_CMD_GETMODE_RESP
{
	char type;
} NUR_PACKED;

struct NUR_CMD_SCANSINGLE_RESP
{
	uint8_t antennaID;
	int8_t rssi;
	uint8_t scaledRssi;
	uint8_t epcdata[NUR_MAX_EPC_LENGTH];
	uint8_t epcLen; // NOTE: Does not come in response from module, calculated from packet size
} NUR_PACKED;

struct NUR_CMD_INVENTORY_RESP
{
	uint16_t numTagsFound;
	uint16_t numTagsMem;
	uint8_t roundsDone;
	uint16_t collisions;
	uint8_t Q;
} NUR_PACKED;

enum NUR_NXPCMD_EASALARM_FLAGS
{
	NUR_EASALARM_ARMED = (1<<0),
	NUR_EASALARM_STOPPPED = (1<<1)
};

enum MONZA4_QTBITS
{
	QT_MEM = (1<<14),
	QT_SR = (1<<15)
};

struct NUR_CMD_MZ4_RESP
{
	uint16_t qtParam;
} NUR_PACKED;

struct NUR_CMD_TRACETAG_RESP
{
	int8_t rssi;
	uint8_t scaledRssi;
	uint8_t antennaID;
	uint8_t epcdata[NUR_MAX_EPC_LENGTH];
	uint8_t epcLen; // NOTE: Does not come in response from module, calculated from packet size
} NUR_PACKED;


#define RINFO_NAME_LENGTH		16
#define RINFO_SERIAL_LEN		RINFO_NAME_LENGTH
#define RINFO_ALTSERIAL_LEN		RINFO_NAME_LENGTH
#define RINFO_HWVERSION_LEN		8
#define RINFO_FCCID_LEN			48

struct NUR_CMD_READERINFO_RESP
{
	uint32_t version;
	uint8_t serialLen;
	char serial[RINFO_SERIAL_LEN];
	uint8_t altSerialLen;
	char altSerial[RINFO_ALTSERIAL_LEN];
	uint8_t nameLen;
	char name[RINFO_NAME_LENGTH];
	uint8_t fccIdLen;
	char fccId[RINFO_FCCID_LEN];
	uint8_t hwVersionLen;
	char hwVersion[RINFO_HWVERSION_LEN];
	uint8_t swVersion[3];
	uint8_t numGpio;
	uint8_t numSensors;
	uint8_t numRegions;
	uint8_t numAntennas;
	uint8_t maxAntennas; // Added in 4.9
} NUR_PACKED;

#define SZ_DEVCAPS		128
struct NUR_CMD_DEVCAPS_RESP
{
	uint32_t dwSize;
	uint32_t flagSet1;
	uint32_t flagSet2;

	int32_t maxTxdBm;
	int32_t txAttnStep;
	uint16_t maxTxmW;
	uint16_t txSteps;

	uint16_t szTagBuffer;
	uint16_t curCfgMaxAnt;
	uint16_t curCfgMaxGPIO;

	uint16_t chipVersion;
	uint16_t moduleType;
	uint32_t moduleConfigFlags;
	uint8_t res[SZ_DEVCAPS - 4*sizeof(uint32_t) - 2*sizeof(int) - 7*sizeof(uint16_t)];
} NUR_PACKED;

struct NUR_CMD_REGIONINFO_RESP
{
	uint8_t regionId;
	uint32_t baseFreq;
	uint32_t channelSpacing;
	uint8_t channelCount;
	uint32_t channelTime;
	uint8_t nameLen;
	char name[64];
} NUR_PACKED;

#define NUR_CMD_SENSORS_RESP		NUR_CMD_SENSORS_PARAMS
#define NUR_CMD_CONFIGGPIO_RESP		NUR_CMD_CONFIGGPIO_PARAMS

struct NUR_GPIO_PIN_STATE
{
  uint8_t number;
  uint8_t state;
} NUR_PACKED;

struct NUR_CMD_SETGPIO_RESP
{
  uint8_t mask;
  union {
    struct NUR_GPIO_PIN_STATE state;
    struct NUR_GPIO_PIN_STATE states[NUR_MAX_GPIO];
  };
} NUR_PACKED;

struct NUR_GPIO_PIN_STATE_RESULT
{
	uint8_t number;
	uint8_t enabled;
	uint8_t type;
	uint8_t state;
} NUR_PACKED;

struct NUR_CMD_GETGPIO_RESP
{
  uint8_t mask;
  union {
    struct NUR_GPIO_PIN_STATE_RESULT state;
    struct NUR_GPIO_PIN_STATE_RESULT states[NUR_MAX_GPIO];
  };
} NUR_PACKED;

struct NUR_CMD_VERSION_RESP
{
	uint8_t mode;
	uint8_t vMajor;
	uint8_t vMinor;
	uint8_t vBuild;
	uint8_t otherMajor;
	uint8_t otherMinor;
	uint8_t otherBuild;
} NUR_PACKED;

struct NUR_CMD_GETREFPOWER_RESP
{
	int32_t iPart;
	int32_t qPart;
	int32_t div;
} NUR_PACKED;

struct NUR_CMD_GETREFPOWEREX_RESP
{
	int32_t iPart;
	int32_t qPart;
	int32_t div;
	uint32_t freqKhz;
} NUR_PACKED;

struct NUR_CMD_PERMALOCK_RD_RESP
{
	uint8_t bank;			/* From bank */
	uint32_t addr;			/* Starting from addr */
	uint8_t nMask;			/* Number of mask words */
	uint16_t wResp[255];	/* Max range is 1...255, BYTE value. */
};

/**
 * NUR_NOTIFICATION_HOPEVENT data.
 */
struct NUR_HOPEVENT_DATA
{
	uint8_t hopTableId;	/**< Current hop table region id */
	uint8_t freqIdx;		/**< Index of frequency in hop table */
	uint32_t freqKhz;		/**< Frequency in kHz */
} NUR_PACKED;

/**
 * NUR_NOTIFICATION_TUNEEVENT data.
 */
struct NUR_TUNEEVENT_DATA
{
	uint8_t cap1;				/**< Tuning capacitor 1 value */
	uint8_t cap2;				/**< Tuning capacitor 2 value */
	int32_t reflPower_dBm;		/**< Reflected power in dBm*1000 */
	uint8_t antenna;			/**< Antenna ID */
	uint32_t freqKhz;			/**< Frequency in kHz */
} NUR_PACKED;

/**
 * Contains the custom hoptable configuration, extended version: + LBT + max TX level.
 * @sa NurApiGetCustomHoptableEx()
 */
struct NUR_CUSTOMHOP_PARAMS_EX
{
	uint32_t count;        /**< Number of channels in this table. */
	uint32_t chTime;       /**< Channel time in milliseconds. */
	uint32_t silentTime;   /**< Silent time in milliseconds, if any, between channel change. */
	uint32_t maxBLF;       /**< Maximum link frequency. */
	uint32_t Tari;         /**< Tari: 1=12.5 and 2 = 25. */
	int32_t lbtThresh;      /**< LBT threshold; minimum value is -90. */
	uint32_t maxTxLevel;   /**< Maximum TX level; range is 0...19. */
	uint32_t freqs[NUR_MAX_CUSTOM_FREQS];  /**< Frequencies in kHz. Number of populated antries is stated with the 'count'. */
} NUR_PACKED;

struct NUR_CMD_RESP
{
	uint8_t cmd;
	uint8_t status;

	union {
		struct NUR_CMD_PING_RESP			ping;
		struct NUR_CMD_GETMODE_RESP		    mode;
		struct NUR_CMD_INVENTORY_RESP	    inventory;
		struct NUR_CMD_LOADSETUP_PARAMS		loadsetup;
		struct NUR_CMD_BAUDRATE_RESP	    baudrate;
		struct NUR_CMD_SCANSINGLE_RESP	    scansingle;
		struct NUR_CMD_TRACETAG_RESP		tracetag;
		struct NUR_CMD_REGIONINFO_RESP		regioninfo;
		struct NUR_CMD_READERINFO_RESP		readerinfo;
		struct NUR_CMD_SENSORS_RESP			sensors;
		struct NUR_CMD_CONFIGGPIO_RESP		gpioconfig;
		struct NUR_CMD_SETGPIO_RESP			gpioset;
		struct NUR_CMD_GETGPIO_RESP			gpioget;
		struct NUR_CMD_TUNEANTENNA_RESP		antennatune;
		struct NUR_CMD_MZ4_RESP				mz4;
    	struct NUR_CMD_VERSION_RESP			versions;
    	struct NUR_SINGLETUNE_RESP			tuneres;
		struct NUR_CMD_DEVCAPS_RESP			devcaps;
		struct NUR_CMD_IRCONFIG_PARAMS		irconfig;
		struct NUR_CUSTOMHOP_PARAMS_EX		customhop;
		struct NUR_CMD_GETREFPOWER_RESP		getrefpower;
		struct NUR_CMD_GETREFPOWEREX_RESP	getrefpowerex;
		struct NUR_HOPEVENT_DATA			hopeventdata;
		struct NUR_TUNEEVENT_DATA			tuneeventdata;
		struct NUR_CMD_PERMALOCK_RD_RESP	permalock;

		uint8_t rawdata[1];
	};
} NUR_PACKED;

#ifdef _WIN32
#pragma pack(pop)
#endif

////////////////////////////////////////////////////////

#define PacketByte(Buf, b, Ptr) { \
	(Buf)[(*(Ptr))++] = b; \
}

#define PacketBytes(Buf, Src, n, Ptr) { \
	if ((n) > 0) { \
		nurMemcpy((void*)&(Buf)[*(Ptr)], (void*)(Src), (n)); \
		(*(Ptr)) += (n); \
	} \
}

#define PacketWordPos(Buf, w, Pos) { \
	(Buf)[(Pos)+1] = (uint8_t)(((uint16_t)(w)>>8) & 0xFF); \
	(Buf)[(Pos)+0] = (uint8_t)((uint16_t)(w) & 0xFF); \
}

#define PacketWord(Buf, w, Ptr) { \
	PacketWordPos(Buf, w, *Ptr); \
	(*(Ptr)) += 2; \
}

#define PacketDwordPos(Buf, dw, Pos) { \
	(Buf)[(Pos)+3] = (uint8_t)(((uint32_t)(dw)>>24) & 0xFF); \
	(Buf)[(Pos)+2] = (uint8_t)(((uint32_t)(dw)>>16) & 0xFF); \
	(Buf)[(Pos)+1] = (uint8_t)(((uint32_t)(dw)>>8) & 0xFF); \
	(Buf)[(Pos)+0] = (uint8_t)((uint32_t)(dw) & 0xFF); \
}

#define PacketDword(Buf, dw, Ptr) { \
	PacketDwordPos(Buf, dw, *Ptr); \
	(*(Ptr)) += 4; \
}

#define PacketInt PacketDword
#define PacketShort PacketWord

#define PacketQwordPos(Buf, ull, Pos) { \
	(Buf)[(Pos)+7] = (uint8_t)(((uint64_t)(ull)>>56) & 0xFF); \
	(Buf)[(Pos)+6] = (uint8_t)(((uint64_t)(ull)>>48) & 0xFF); \
	(Buf)[(Pos)+5] = (uint8_t)(((uint64_t)(ull)>>40) & 0xFF); \
	(Buf)[(Pos)+4] = (uint8_t)(((uint64_t)(ull)>>32) & 0xFF); \
	(Buf)[(Pos)+3] = (uint8_t)(((uint64_t)(ull)>>24) & 0xFF); \
	(Buf)[(Pos)+2] = (uint8_t)(((uint64_t)(ull)>>16) & 0xFF); \
	(Buf)[(Pos)+1] = (uint8_t)(((uint64_t)(ull)>>8) & 0xFF); \
	(Buf)[(Pos)+0] = (uint8_t)((uint64_t)(ull) & 0xFF); \
}

#define PacketQword(Buf, ull, Ptr) { \
	PacketQwordPos(Buf, ull, *Ptr); \
	(*(Ptr)) += 8; \
}

/**********************/

#define BytesToWord(Buf) \
	((uint16_t)((uint16_t)(Buf)[0] | \
	((uint16_t)(Buf)[1] << 8)))

#define BytesToWordPos(Buf, Pos) \
	((uint16_t)((uint16_t)(Buf)[(Pos)+0] | \
	((uint16_t)(Buf)[(Pos)+1] << 8)))

#define BytesToDword(Buf) \
	((uint32_t)((uint32_t)(Buf)[0] | \
	((uint32_t)(Buf)[1] << 8) | \
	((uint32_t)(Buf)[2] << 16) | \
	((uint32_t)(Buf)[3] << 24)))

#define BytesToDwordPos(Buf, Pos) \
	((uint32_t)((uint32_t)(Buf)[(Pos)+0] | \
	((uint32_t)(Buf)[(Pos)+1] << 8) | \
	((uint32_t)(Buf)[(Pos)+2] << 16) | \
	((uint32_t)(Buf)[(Pos)+3] << 24)))

#define BytesToQword(Buf) \
	((uint64_t)((uint64_t)(Buf)[0] | \
	((uint64_t)(Buf)[1] << 8) | \
	((uint64_t)(Buf)[2] << 16) | \
	((uint64_t)(Buf)[3] << 24) | \
	((uint64_t)(Buf)[4] << 32) | \
	((uint64_t)(Buf)[5] << 40) | \
	((uint64_t)(Buf)[6] << 48) | \
	((uint64_t)(Buf)[7] << 56)))

#define BytesToQwordPos(Buf, Pos) \
	((uint64_t)((Buf)[(Pos)+0] | \
	((uint64_t)(Buf)[(Pos)+1] << 8) | \
	((uint64_t)(Buf)[(Pos)+2] << 16) | \
	((uint64_t)(Buf)[(Pos)+3] << 24) | \
	((uint64_t)(Buf)[(Pos)+4] << 32) | \
	((uint64_t)(Buf)[(Pos)+5] << 40) | \
	((uint64_t)(Buf)[(Pos)+6] << 48) | \
	((uint64_t)(Buf)[(Pos)+7] << 56)))

/** @} */ // end of NURPROTO

#ifdef __cplusplus
}
#endif

#endif
