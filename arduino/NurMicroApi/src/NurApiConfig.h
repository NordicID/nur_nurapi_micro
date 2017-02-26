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
