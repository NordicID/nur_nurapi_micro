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
	Sample serial transport for WIN32
*/

#define _CRT_SECURE_NO_WARNINGS	1

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "NurMicroApi.h"

// This is anything that the serial transport requires to have; here it is the HANDLE returned by the Win32 API.
static HANDLE gSerial = INVALID_HANDLE_VALUE;

static int serial_read(struct NUR_API_HANDLE *hNurApi, BYTE *buffer, DWORD bufferLen, DWORD *bytesRead);
static int serial_write(struct NUR_API_HANDLE *hNurApi, BYTE *buffer, DWORD bufferLen, DWORD *bytesWritten);

// As we are using the handle here locally, only return the ok/fail status.
BOOL open_serial(struct NUR_API_HANDLE *hApi, int number, DWORD baudrate)
{
	char thePort[20];
	COMMTIMEOUTS to;
	DCB dcb;
	
	if (gSerial != INVALID_HANDLE_VALUE) {
		CloseHandle(gSerial);
		gSerial = INVALID_HANDLE_VALUE;
	}

	if (number < 1 || number > 255)
		return FALSE;	// Not allowed here.

	if (number < 10)
		sprintf(thePort, "COM%d:", number);
	else
		sprintf(thePort, "\\\\.\\COM%d", number);

	gSerial = CreateFile(thePort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (gSerial == INVALID_HANDLE_VALUE ||gSerial == NULL) {
		gSerial = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	/* Setup */
	GetCommState(gSerial, &dcb);
	dcb.fBinary = 1;
	dcb.ByteSize = 8;
	dcb.BaudRate = baudrate;
	SetCommState(gSerial, &dcb);

	// Set "ByteTimeout" to 1ms
	ZeroMemory(&to, sizeof(to));	
	to.ReadIntervalTimeout = MAXDWORD;
	to.ReadTotalTimeoutMultiplier = MAXDWORD;
	to.ReadTotalTimeoutConstant = 1;
	to.WriteTotalTimeoutMultiplier = 1;
	to.WriteTotalTimeoutConstant = 1;
	SetCommTimeouts(gSerial, &to);

	hApi->TransportReadDataFunction = (pTransportReadDataFunction)serial_read;
	hApi->TransportWriteDataFunction = (pTransportWriteDataFunction)serial_write;

	PurgeComm(gSerial, PURGE_RXCLEAR|PURGE_TXCLEAR);	

	return TRUE;
}

void close_serial()
{
	if (gSerial != INVALID_HANDLE_VALUE) {
		CloseHandle(gSerial);
		gSerial = INVALID_HANDLE_VALUE;
	}
}

static int serial_read(struct NUR_API_HANDLE *hNurApi, BYTE *buffer, DWORD bufferLen, DWORD *bytesRead)
{
	DWORD dwRead = 0;
	DWORD dwError;

	if (gSerial == INVALID_HANDLE_VALUE) 
		return NUR_ERROR_TR_NOT_CONNECTED;

	if (!ReadFile(gSerial, buffer, bufferLen, &dwRead, NULL)) 
	{
		dwError = GetLastError();
			
		if (dwError == ERROR_ACCESS_DENIED || 
		    dwError == ERROR_INVALID_HANDLE ||
		    dwError == ERROR_FILE_NOT_FOUND) 
		{
			// Happens when usb converter is unplugged
			return NUR_ERROR_TR_NOT_CONNECTED;
		}
		else if (dwError != ERROR_SUCCESS) {
			return NUR_ERROR_TRANSPORT;
		}
	}

	if (dwRead == 0) {
		return NUR_ERROR_TR_TIMEOUT;
	}

	*bytesRead = dwRead;

	return (gSerial != INVALID_HANDLE_VALUE) ? NUR_SUCCESS : NUR_ERROR_TR_NOT_CONNECTED;	
}

static int serial_write(struct NUR_API_HANDLE *hNurApi, BYTE *buffer, DWORD bufferLen, DWORD *bytesWritten)
{
	DWORD dwWritten = 0;
	DWORD totalWritten = 0;
	DWORD dwError;

	if (gSerial == INVALID_HANDLE_VALUE) 
		return NUR_ERROR_TR_NOT_CONNECTED;
	
	while (totalWritten < bufferLen && gSerial != INVALID_HANDLE_VALUE) 
	{
		if (!WriteFile(gSerial, buffer + totalWritten, bufferLen - totalWritten, &dwWritten, NULL)) 
		{
			dwError = GetLastError();
			
			if (dwError == ERROR_ACCESS_DENIED || 
				dwError == ERROR_INVALID_HANDLE ||
				dwError == ERROR_FILE_NOT_FOUND) 
			{
				// Happens when usb converter is unplugged
				return NUR_ERROR_TR_NOT_CONNECTED;
			}
			else if (dwError != ERROR_SUCCESS) {
				return NUR_ERROR_TRANSPORT;
			}
		}

		totalWritten += dwWritten;

		if (dwWritten == 0) {
			return NUR_ERROR_TR_TIMEOUT;
		}
	}

	FlushFileBuffers(gSerial);

	*bytesWritten = totalWritten;
	return (gSerial != INVALID_HANDLE_VALUE) ? NUR_SUCCESS : NUR_ERROR_TR_NOT_CONNECTED;
}
