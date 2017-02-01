/* 
  Copyright © 2014 Nordic ID 
  NORDIC ID DEMO SOFTWARE DISCLAIMER

  You are about to use Nordic ID Demo Software ("Software"). 
  It is explicitly stated that Nordic ID does not give any kind of warranties, 
  expressed or implied, for this Software. Software is provided "as is" and with 
  all faults. Under no circumstances is Nordic ID liable for any direct, special, 
  incidental or indirect damages or for any economic consequential damages to you 
  or to any third party.

  The use of this software indicates your complete and unconditional understanding 
  of the terms of this disclaimer. 
  
  IF YOU DO NOT AGREE OF THE TERMS OF THIS DISCLAIMER, DO NOT USE THE SOFTWARE.  
*/

/*
	Default NUR uApi configuration file.
*/

#ifndef _NURAPICONFIG_H_
#define _NURAPICONFIG_H_	1

// Default timeouts for NUR functions
#define DEF_TIMEOUT 3000
#define DEF_LONG_TIMEOUT 10000

// Comment out to use 'memset' instead.
#define HAVE_NUR_MEMSET

// Comment out to use 'strncpy' instead.
#define HAVE_NUR_STRNCPY

// Comment out to use 'memcpy' instead.
#define HAVE_NUR_MEMCPY

// Comment out to implement own CRC-16
// Prototype is in NurMicroApi.h: extern WORD NurCRC16(WORD crc, BYTE *buf, DWORD len);
#define IMPLEMENT_CRC16

/*
	Generic function inclusion.
*/
/* Whether to have the read function with free selection parameters. */
#define CONFIG_GENERIC_READ
/* Whether to have the write function with free selection parameters. */
#define CONFIG_GENERIC_WRITE

// Comment out to use calculation instead of lookup with CRC-16.
//#define HAVE_CRC16_LOOKUP

// Comment out to leave out error messages.
//#define HAVE_ERROR_MESSAGES

/*
	For architectures that do not support unaligned structure member accesses.
*/
/* Define this in order to use specific, byte-by-byte copy.*/
//#define CONFIG_MEMBER_ACCESS	

#endif
