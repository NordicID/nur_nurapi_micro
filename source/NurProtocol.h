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
// Internal FLASH Page Size: 256/4 DWORDs
#define NUR_FLASH_PAGE_SIZE_DW	(256/4)

#define ISEL_INVERT (1<<0)
#define ISEL_64ADDR (1<<1)

#define RW_SEC (1<<0)
#define RW_SBP (1<<1)
#define RW_EA1 (1<<2)
#define RW_EA2 (1<<3)

#define PACKET_FLAG_UNSOL  (1<<0)
#define PACKET_FLAG_IRDATA (1<<1)

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
	BYTE start;
	WORD payloadlen;
	WORD flags;
	BYTE checksum;
} NUR_PACKED;

#define PACKET_START    0xA5
#define CS_STARTBYTE    0xFF
#define HDR_SIZE sizeof(struct NUR_HEADER)

struct NUR_IDBUFFER_ENTRY
{
	char rssi;
	char scaledRssi;
	WORD timestamp;
	DWORD freq;
	BYTE dataLen;
	WORD pc;
	BYTE channel;
	BYTE antennaId;	
	BYTE epcLen;
	/* EPC + associated data */
	BYTE *epcData;
} NUR_PACKED;

/////////////////////////////////////////////////////////////////////////////
// PARAMETERS

struct NUR_CMD_BAUDRATE_PARAMS
{
	BYTE setting;
} NUR_PACKED;

struct NUR_CMD_PAGEREAD_PARAMS
{
	WORD pagetoread;
	BYTE cPermission[CRYPTO_PERMISSION_LENGTH];
} NUR_PACKED;

struct NUR_CMD_PAGEWRITE_PARAMS
{
	WORD pagetowrite;
	DWORD crc;
	BYTE data[NUR_FLASH_PAGE_SIZE];
} NUR_PACKED;

struct NUR_CMD_SCRATCHDATA_PARAMS
{	
	BYTE read;
	BYTE page;
	BYTE offset;
	WORD dataLen;
	DWORD dwReserved;
	BYTE data[MIN_SCRATCHBYTES];
} NUR_PACKED;

struct NUR_CMD_APPVALIDATE_PARAMS
{
	DWORD appsize;
	DWORD appcrc;	
} NUR_PACKED;

struct NUR_CMD_SCANSINGLE_PARAMS
{
	WORD timeout;
} NUR_PACKED;

struct NUR_CMD_INVENTORY_PARAMS
{
	BYTE Q;
	BYTE session;
	BYTE rounds;
} NUR_PACKED;

struct NUR_CMD_INVENTORYEX_FILTER
{
	BYTE truncate;
	BYTE target;
	BYTE action;
	BYTE bank;	
	DWORD address;	
	BYTE maskbitlen;
	BYTE maskdata[NUR_MAX_SELMASK];	
} NUR_PACKED;

struct NUR_CMD_INVENTORYEX_PARAMS
{
	BYTE flags;
	BYTE Q;
	BYTE session;
	BYTE rounds;
	WORD transitTime;
	BYTE inventoryTarget;
	BYTE inventorySelState;
	BYTE filterCount;
	struct NUR_CMD_INVENTORYEX_FILTER filters[NUR_MAX_FILTERS];
} NUR_PACKED;

struct NUR_CMD_IRCONFIG_PARAMS
{
  BYTE active;
  BYTE type;
  BYTE bank;
  DWORD wAddress;
  BYTE wLength;
} NUR_PACKED;

struct NUR_SINGULATIONBLOCK
{
	BYTE bytestofollow;
	BYTE bank;
	union {
		DWORD address32;
		ULONGLONG address64;
	};
	WORD maskbitlen;
	BYTE maskdata[NUR_MAX_SELMASK];
} NUR_PACKED;

struct NUR_READBLOCK
{
	BYTE bytestofollow;
	BYTE bank;
	union {
		DWORD address32;
		ULONGLONG address64;
	};
	BYTE wordcount;
} NUR_PACKED;

struct NUR_WRITEBLOCK
{
	BYTE bytestofollow;
	BYTE bank;
	union {
		DWORD address32;
		ULONGLONG address64;
	};
	BYTE wordcount;
	BYTE data[255];
} NUR_PACKED;

struct NUR_WRITEBLOCK_EX
{
	BYTE bytestofollow;
	BYTE bank;
	union {
		DWORD address32;
		ULONGLONG address64;
	};	
	BYTE wordcount;
	BYTE blSize;
	BYTE data[255];
} NUR_PACKED;

struct NUR_LOCKBLOCK
{
	BYTE bytestofollow;
	WORD mask;
	WORD action;
} NUR_PACKED;

struct NUR_PERMALOCKBLOCK
{
	BYTE lock;
	BYTE bank;
	DWORD addr;
	BYTE range;
	WORD wMask[255];
} NUR_PACKED;

struct NUR_SINGULATED_CMD_PARAMS
{
	BYTE flags;
	DWORD passwd;
	struct NUR_SINGULATIONBLOCK sb;
} NUR_PACKED;

struct NUR_CMD_READ_PARAMS
{
	BYTE flags;
	DWORD passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_READBLOCK rb;
} NUR_PACKED;

struct NUR_CMD_WRITE_PARAMS
{
	BYTE flags;
	DWORD passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_WRITEBLOCK wb;
} NUR_PACKED;

struct NUR_CMD_BLWRITEEX_PARAMS
{
	BYTE flags;
	DWORD passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_WRITEBLOCK_EX wbe;
} NUR_PACKED;

struct NUR_CMD_BLKERASE_PARAMS
{
	BYTE flags;
	DWORD passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_READBLOCK erb;
} NUR_PACKED;

struct NUR_CMD_LOCK_PARAMS
{
	BYTE flags;
	DWORD passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_LOCKBLOCK lb;
} NUR_PACKED;

struct NUR_CMD_PERMALOCK_PARAM
{
	BYTE flags;
	DWORD passwd;

	struct NUR_SINGULATIONBLOCK sb;
	struct NUR_PERMALOCKBLOCK plb;
} NUR_PACKED;

struct NUR_CMD_KILL_PARAMS
{
	BYTE flags;
	DWORD passwd;

	struct NUR_SINGULATIONBLOCK sb;
} NUR_PACKED;

struct NUR_NXP_PARAMS
{
	BYTE flags;
	DWORD passwd;
	BOOL set;
    
	struct NUR_SINGULATIONBLOCK sb;
} NUR_PACKED;

struct NUR_MONZAQTPARAMS
{
	BYTE flags;
	DWORD passwd;
	BOOL write;
	// N/ for the time being.
	//BOOL persist;
	BOOL reduce;
	BOOL pubmem;
	struct NUR_SINGULATIONBLOCK sb;  
} NUR_PACKED;

struct NUR_CMD_TRACETAG_PARAMS
{
	BYTE flags;
	BYTE bank;
	union {
		DWORD address32;
		ULONGLONG address64;
	};
	BYTE maskbitlen;
	BYTE maskdata[NUR_MAX_SELMASK];
} NUR_PACKED;

struct NUR_CMD_SENSORS_PARAMS
{
	BYTE flags;
	BYTE config[NUR_MAX_SENSORS * NUR_SZ_SENSOR_CONF];
} NUR_PACKED;

struct NUR_GPIO_SETUP
{
	BYTE enabled;
	BYTE type;
	BYTE edge;
	BYTE action;
} NUR_PACKED;

struct NUR_CMD_CONFIGGPIO_PARAMS
{
	BYTE flags;
	struct NUR_GPIO_SETUP config[NUR_MAX_GPIO];
} NUR_PACKED;

struct NUR_GPIO_SETSTATE
{  
	BYTE state;
} NUR_PACKED;

struct NUR_CMD_SETGPIO_PARAMS
{
	BYTE flags;
	union {
		struct NUR_GPIO_SETSTATE state;    
		struct NUR_GPIO_SETSTATE states[NUR_MAX_GPIO];    
	};
} NUR_PACKED;

struct NUR_CMD_GETGPIO_PARAMS
{
	BYTE flags;
} NUR_PACKED;

struct NUR_SETUPLOAD_PARAMS
{
	WORD flags;
	BYTE regionId;
	DWORD linkfreq;
	BYTE miller;
	BYTE txLevel;
	BYTE modulation;
} NUR_PACKED;

/**
 * Tag RSSI filter.
 * @sa NurApiGetModuleSetup(), NurApiSetModuleSetup()
 */
struct NUR_RSSI_FILTER
{
	char min; /**< Minimum accepted RSSI in dBm. Use 0 to disable filtering. */
	char max; /**< Maximum accepted RSSI in dBm. Use 0 to disable filtering. */
} NUR_PACKED;

/**
 * Module's autotune setup. Supported from FW 5.0-A in L2 module.
 * 
 * @sa AUTOTUNE_MODE_ENABLE, AUTOTUNE_MODE_THRESHOLD_ENABLE
*/
struct NUR_AUTOTUNE_SETUP
{
	BYTE mode;			/**< Autotune mode setting, bits: @sa AUTOTUNE_MODE_ENABLE, AUTOTUNE_MODE_THRESHOLD_ENABLE. */
	char threshold_dBm;	/**< Low level threshold in dBm. Do autotune if current reflected power worse than 'threshold_dBm' */
};

struct NUR_CMD_LOADSETUP_PARAMS
{
	DWORD flags;
	DWORD linkFreq;
	BYTE rxDecoding; // Miller
	BYTE txLevel;
	BYTE txModulation;
	BYTE regionId;
	BYTE inventoryQ;
	BYTE inventorySession;
	BYTE inventoryRounds;
	BYTE antennaMask;
	WORD scanSingleTriggerTimeout;
	WORD inventoryTriggerTimeout;
	BYTE selectedAntenna;
	DWORD opFlags;
	BYTE inventoryTarget;
	BYTE inventoryEpcLength;
	struct NUR_RSSI_FILTER readRssiFilter;
	struct NUR_RSSI_FILTER writeRssiFilter;
	struct NUR_RSSI_FILTER inventoryRssiFilter;

	WORD readTO;
	WORD writeTO;
	WORD lockTO;
	WORD killTO;

	/** Defines how the periodic auto-inventory power saving is configured.
	 * When in use, reader will be power save mode while no tags in view.
	 */
	WORD periodSetup;

	/** DEPRECATED! Per antenna specific power levels. Use antPowerEx instead. */
	BYTE antPower[NUR_MAX_ANTENNAS];

	/** Modify power levels' offset values (-1, 0, 1) NOTE: First entry in array used for all antennas! Rest of the entries are discarded. */
	char powerOffset[NUR_MAX_ANTENNAS];

	/** Bitmask of enabled antennas, support up to 32 antennas. Value 0x1 - 0xFFFFFFFF.
	 * Example: Value 0x4 means that only antenna 3 is enabled.
	 * Example: Value 0x30300 means that antennas 9,10,17,18 are enabled.
	 */
	DWORD antennaMaskEx;

	/** Runtime auto tuning settings. */
	struct NUR_AUTOTUNE_SETUP autotune;

	/** Per antenna specific tx levels. Array of the per antenna tx level values. Range is 0..19; Defaults to -1. If set to -1 default tx level is used from the module setup. */
	char antPowerEx[NUR_MAX_ANTENNAS_EX];

	/** The receiver sensitivity field. 0 = Nominal, 1 = Low, 2 = High */
	BYTE rxSensitivity;
} NUR_PACKED;

struct NUR_CMD_BEEP_PARAMS
{
	DWORD freq;
	DWORD time;
	BYTE duty;
} NUR_PACKED;

struct NUR_CMD_RESETTARGET_PARAMS
{
	BYTE session;
	BYTE targetIsA;
} NUR_PACKED;

#define NUR_CMD_CUSTOMHOP_PARAMS	NUR_CUSTOMHOP_PARAMS

struct NUR_CMD_ETHDEV_PARAMS
{
	BYTE titleLength;
	BYTE title[32];
	BYTE mask[4];	//Subnet mask (used in static IP mode)
	BYTE gw[4];		//Gateway	(used in static IP)
	BYTE addrType; //Address type 0=DHCP(default) 1=STATIC
	BYTE staticip[4];
	BYTE serverPort[2];
	BYTE hostmode;	 //Mode: 0=Server (default) 1=Client
	BYTE hostip[4]; //Client host IP (if Mode=Client)
	BYTE hostPort[2];	//Client mode port (if Mode=Client)
	BYTE reserved[8]; //For future usage
} NUR_PACKED;

struct NUR_CMD_EPCENUMPARAM
{
	BYTE ant;			/* Selected antenna */
	BYTE twAddr;		/* Word address of the TID data i.e. where to read the TID info */
	BYTE twLen;			/* TID data word length. */
	BYTE useBlWrite;	/* Block write type if any: 0 = no, 1 = one word, 2 = two words. */
	BYTE startVal[8];	/* Start value of the enumerated value in the EPC : equals to "unsigned long long" i.e. 64-bit unsigned integer. */
	BYTE epcLen;		/* EPC length in words; 4...8 */
	BYTE modAddr;		/* Bit address where the enumerated value is located in the EPC memory */
	BYTE bitLen;		/* Bit length of the modified value: 1...64 */
	BYTE bReset;		/* If non-zero then the current TID information is cleared */	
	BYTE baseEPC[MAX_EE_EPCLEN];	/* Base EPC to be used & modified. */
} NUR_PACKED;

#define PRODUCTION_TUNE_MAGICLEN    8

struct NUR_CMD_TUNECOMMANDPARAM
{
	DWORD type;
	DWORD antenna;
	DWORD band;
	DWORD userSave;
	int goodEnough;	
	BYTE code[PRODUCTION_TUNE_MAGICLEN];
} NUR_PACKED;

/////////////////////////////////////////////////////////////////////////////
// RESPONSES

struct NUR_TUNERESULT
{
	int I;
	int Q;
	int dBm;
} NUR_PACKED;

struct NUR_CMD_TUNEANTENNA_RESP
{
	int antenna;
	int resrv[3];
	struct NUR_TUNERESULT bands[NR_TUNEBANDS];
} NUR_PACKED;

struct NUR_SINGLETUNE_RESP
{
	int antenna;
	int band;
	struct NUR_TUNERESULT result;
} NUR_PACKED;

struct NUR_CMD_BAUDRATE_RESP
{
	BYTE setting;
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
	BYTE antennaID;
	char rssi;
	char scaledRssi;
	BYTE epcdata[NUR_MAX_EPC_LENGTH];
	BYTE epcLen; // NOTE: Does not come in response from module, calculated from packet size
} NUR_PACKED;

struct NUR_CMD_INVENTORY_RESP
{
	WORD numTagsFound;
	WORD numTagsMem;
	BYTE roundsDone;
	WORD collisions;
	BYTE Q;
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
	WORD qtParam;
} NUR_PACKED;

struct NUR_CMD_TRACETAG_RESP
{
	char rssi;
	char scaledRssi;
	BYTE antennaID;
	BYTE epcdata[NUR_MAX_EPC_LENGTH];	
	BYTE epcLen; // NOTE: Does not come in response from module, calculated from packet size
} NUR_PACKED;


#define RINFO_NAME_LENGTH		16
#define RINFO_SERIAL_LEN		RINFO_NAME_LENGTH
#define RINFO_ALTSERIAL_LEN		RINFO_NAME_LENGTH
#define RINFO_HWVERSION_LEN		8
#define RINFO_FCCID_LEN			48

struct NUR_CMD_READERINFO_RESP
{
	DWORD version;
	BYTE serialLen;
	char serial[RINFO_SERIAL_LEN];
	BYTE altSerialLen;
	char altSerial[RINFO_ALTSERIAL_LEN];	
	BYTE nameLen;
	char name[RINFO_NAME_LENGTH];
	BYTE fccIdLen;
	char fccId[RINFO_FCCID_LEN];
	BYTE hwVersionLen;
	char hwVersion[RINFO_HWVERSION_LEN];
	BYTE swVersion[3];
	BYTE numGpio;
	BYTE numSensors;
	BYTE numRegions;
	BYTE numAntennas;
} NUR_PACKED;

#define SZ_DEVCAPS		128
struct NUR_CMD_DEVCAPS_RESP
{
	DWORD dwSize;
	DWORD flagSet1;
	DWORD flagSet2;
	
	int maxTxdBm;
	int txAttnStep;	
	WORD maxTxmW;
	WORD txSteps;
	
	WORD szTagBuffer;
	WORD curCfgMaxAnt;
	WORD curCfgMaxGPIO;
	
	WORD chipVersion;
	WORD moduleType;
	DWORD moduleConfigFlags;
	BYTE res[SZ_DEVCAPS - 4*sizeof(DWORD) - 2*sizeof(int) - 7*sizeof(WORD)];
} NUR_PACKED;

struct NUR_CMD_REGIONINFO_RESP
{
	BYTE regionId;
	DWORD baseFreq;
	DWORD channelSpacing;
	BYTE channelCount;
	DWORD channelTime;
	BYTE nameLen;
	char name[64];
} NUR_PACKED;

#define NUR_CMD_SENSORS_RESP		NUR_CMD_SENSORS_PARAMS
#define NUR_CMD_CONFIGGPIO_RESP		NUR_CMD_CONFIGGPIO_PARAMS

struct NUR_GPIO_PIN_STATE
{  
  BYTE number;
  BYTE state;
} NUR_PACKED;

struct NUR_CMD_SETGPIO_RESP
{
  BYTE mask;
  union {
    struct NUR_GPIO_PIN_STATE state;
    struct NUR_GPIO_PIN_STATE states[NUR_MAX_GPIO];
  };
} NUR_PACKED;

struct NUR_GPIO_PIN_STATE_RESULT
{
	BYTE number;
	BYTE enabled;
	BYTE type;
	BYTE state;
} NUR_PACKED;

struct NUR_CMD_GETGPIO_RESP
{
  BYTE mask;
  union {
    struct NUR_GPIO_PIN_STATE_RESULT state;
    struct NUR_GPIO_PIN_STATE_RESULT states[NUR_MAX_GPIO];
  };
} NUR_PACKED;

struct NUR_CMD_VERSION_RESP
{
	BYTE mode;
	BYTE vMajor;
	BYTE vMinor;
	BYTE vBuild;
	BYTE otherMajor;
	BYTE otherMinor;
	BYTE otherBuild;	
} NUR_PACKED;

struct NUR_CMD_GETREFPOWER_RESP
{
	int iPart;
	int qPart;
	int div;
} NUR_PACKED;

struct NUR_CMD_GETREFPOWEREX_RESP
{
	int iPart;
	int qPart;
	int div;
	DWORD freqKhz;
} NUR_PACKED;

struct NUR_CMD_RESP
{
	BYTE cmd;
	BYTE status;

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
		struct NUR_CMD_GETREFPOWER_RESP		getrefpower;
		struct NUR_CMD_GETREFPOWEREX_RESP	getrefpowerex;

		BYTE rawdata[1];
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
	(Buf)[(Pos)+1] = (BYTE)(((WORD)(w)>>8) & 0xFF); \
	(Buf)[(Pos)+0] = (BYTE)((WORD)(w) & 0xFF); \
}

#define PacketWord(Buf, w, Ptr) { \
	PacketWordPos(Buf, w, *Ptr); \
	(*(Ptr)) += 2; \
}

#define PacketDwordPos(Buf, dw, Pos) { \
	(Buf)[(Pos)+3] = (BYTE)(((DWORD)(dw)>>24) & 0xFF); \
	(Buf)[(Pos)+2] = (BYTE)(((DWORD)(dw)>>16) & 0xFF); \
	(Buf)[(Pos)+1] = (BYTE)(((DWORD)(dw)>>8) & 0xFF); \
	(Buf)[(Pos)+0] = (BYTE)((DWORD)(dw) & 0xFF); \
}

#define PacketDword(Buf, dw, Ptr) { \
	PacketDwordPos(Buf, dw, *Ptr); \
	(*(Ptr)) += 4; \
}

#define PacketInt PacketDword
#define PacketShort PacketWord

#define PacketQwordPos(Buf, ull, Pos) { \
	(Buf)[(Pos)+7] = (BYTE)(((QWORD)(ull)>>56) & 0xFF); \
	(Buf)[(Pos)+6] = (BYTE)(((QWORD)(ull)>>48) & 0xFF); \
	(Buf)[(Pos)+5] = (BYTE)(((QWORD)(ull)>>40) & 0xFF); \
	(Buf)[(Pos)+4] = (BYTE)(((QWORD)(ull)>>32) & 0xFF); \
	(Buf)[(Pos)+3] = (BYTE)(((QWORD)(ull)>>24) & 0xFF); \
	(Buf)[(Pos)+2] = (BYTE)(((QWORD)(ull)>>16) & 0xFF); \
	(Buf)[(Pos)+1] = (BYTE)(((QWORD)(ull)>>8) & 0xFF); \
	(Buf)[(Pos)+0] = (BYTE)((QWORD)(ull) & 0xFF); \
}

#define PacketQword(Buf, ull, Ptr) { \
	PacketQwordPos(Buf, ull, *Ptr); \
	(*(Ptr)) += 8; \
}

/**********************/

#define BytesToWord(Buf) \
	((WORD)((WORD)(Buf)[0] | \
	((WORD)(Buf)[1] << 8)))

#define BytesToWordPos(Buf, Pos) \
	((WORD)((WORD)(Buf)[(Pos)+0] | \
	((WORD)(Buf)[(Pos)+1] << 8)))

#define BytesToDword(Buf) \
	((DWORD)((DWORD)(Buf)[0] | \
	((DWORD)(Buf)[1] << 8) | \
	((DWORD)(Buf)[2] << 16) | \
	((DWORD)(Buf)[3] << 24)))

#define BytesToDwordPos(Buf, Pos) \
	((DWORD)((DWORD)(Buf)[(Pos)+0] | \
	((DWORD)(Buf)[(Pos)+1] << 8) | \
	((DWORD)(Buf)[(Pos)+2] << 16) | \
	((DWORD)(Buf)[(Pos)+3] << 24)))

#define BytesToQword(Buf) \
	((QWORD)((QWORD)(Buf)[0] | \
	((QWORD)(Buf)[1] << 8) | \
	((QWORD)(Buf)[2] << 16) | \
	((QWORD)(Buf)[3] << 24) | \
	((QWORD)(Buf)[4] << 32) | \
	((QWORD)(Buf)[5] << 40) | \
	((QWORD)(Buf)[6] << 48) | \
	((QWORD)(Buf)[7] << 56)))

#define BytesToQwordPos(Buf, Pos) \
	((QWORD)((Buf)[(Pos)+0] | \
	((QWORD)(Buf)[(Pos)+1] << 8) | \
	((QWORD)(Buf)[(Pos)+2] << 16) | \
	((QWORD)(Buf)[(Pos)+3] << 24) | \
	((QWORD)(Buf)[(Pos)+4] << 32) | \
	((QWORD)(Buf)[(Pos)+5] << 40) | \
	((QWORD)(Buf)[(Pos)+6] << 48) | \
	((QWORD)(Buf)[(Pos)+7] << 56)))

/** @} */ // end of NURPROTO

#ifdef __cplusplus 
}
#endif

#endif
