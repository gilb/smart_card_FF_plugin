/*
* SmartC : Firefox plugin for smart cards. 
* Copyright (C) 2010  Gilles Bernabe.
*
* Sources: reuse of some examples for Winscard from <http://msdn.microsoft.com/en-us/library/aa379793(VS.85).aspx/>.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SmartC.h"
#include "nsCOMPtr.h"
#include "nsEmbedString.h"
#include "nsMemory.h"
#include "nsIObserverService.h"
#include "nsIHttpChannel.h"
#include "nsIProperties.h"
#include "nsIIOService.h"
#include "nsServiceManagerUtils.h"


NS_IMPL_ISUPPORTS1(SmartC, ISmartC)

SmartC::SmartC()
{
  /* member initializers and constructor code */
}

SmartC::~SmartC()
{
  /* destructor code */
}

NS_IMETHODIMP SmartC::SendAPDU(PRUint32 ct, PRInt32 *valueArray, PRUint32* count, PRInt32** retv)
{
		PRUint32 i;
		int  c = 0;

		nsresult rv;
		nsCAutoString urlServer("https://");
		// urlServer.Append(ToNewCString(host));

		nsCOMPtr<nsIIOService> newio = 
		do_GetService("@mozilla.org/network/io-service;1",&rv);
		// NS_ENSURE_SUCCESS(rs, rs);

		nsCOMPtr<nsIURI> uri ;
		rv=newio->NewURI(urlServer,nsnull,nsnull,getter_AddRefs(uri));
		// NS_ENSURE_SUCCESS(rs, rs);

		nsCOMPtr<nsIObserverService> obsSvc = 
		do_GetService("@mozilla.org/observer-service;1", &rv);


		LONG scarderr = SCARD_S_SUCCESS; // Error code returned by the called functions
		SCARDCONTEXT hContext = 0; // Handle to the established resource manager context

		// SCardEstablishContext()
		scarderr = SCardEstablishContext(SCARD_SCOPE_USER, // Scope of the resource manager context 
											NULL, // Reserved for future use, and must be NULL
											NULL, // Reserved for future use, and must be NULL
											&hContext); // Handle to the established resource manager context

		if(scarderr != SCARD_S_SUCCESS) // if an error occurred
		{ 
			c = 1;
			goto quit;
		}

		//list all available readers via SCardListReaders
		
		LPTSTR mszReaders = NULL; // Multi-string that lists the card readers
		DWORD dwReaders = SCARD_AUTOALLOCATE; // Length of the mszReaders buffer

		scarderr = SCardListReaders(hContext, // Handle that identifies the resource manager context
									NULL, // Names of the reader groups defined to the system ( NULL = all )
									(LPTSTR)&mszReaders, // Multi-string that lists the card readers within the supplied reader groups
									&dwReaders); // Length of the mszReaders buffer in characters
	    
		if(scarderr != SCARD_S_SUCCESS) // if an error occurred
		{  
			if(scarderr == SCARD_E_NO_READERS_AVAILABLE)
			{
				c=2;
			} 
			else
				{
					c=3;
				}

			SCardReleaseContext(hContext); // Handle that identifies the resource manager context
			goto quit;
		}
		
		//display all reader names

		LPTSTR mszCurrentReader = NULL; // current reader name within the multi-string list
		unsigned int idx = 1;
		int temp=0;

		mszCurrentReader = mszReaders;
		while ('\0' != *mszCurrentReader) 
		{				
			if(idx==2 && ((string)mszCurrentReader).compare(23,6,"SDI010")!= 0) // verify that it's not a dual interface reader with RFID, in this case SCM SDI010
			{
				temp=1;
			} 
			//printf("%u. Reader: %s\n", idx, mszCurrentReader );
			mszCurrentReader = mszCurrentReader + strlen(mszCurrentReader) + 1;
			idx++;
		}

		// unsigned int uiReader = getc - 0x30
		unsigned int uiReader;
		if (temp==1)
		{
			uiReader =2;
		} 
		else
			{ 
				uiReader =1;
			}

		mszCurrentReader = mszReaders;

		for (idx=1; idx<uiReader; idx++)
		{
			mszCurrentReader = mszCurrentReader + strlen(mszCurrentReader) + 1;
		}

		SCARDHANDLE hCard = 0; // Handle that identifies the connection to the smart card in the designated reader
		DWORD dwActiveProtocol = 0; // Flag that indicates the established active protocol

		scarderr = SCardConnect(hContext, // Handle that identifies the resource manager context
								mszCurrentReader, // Name of the reader containing the target card
								SCARD_SHARE_SHARED, // ShareMode - Flag that indicates whether other applications may form connections to the card
								SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1, // Bit mask of acceptable protocols for the connection
								&hCard, // Handle that identifies the connection to the smart card
								&dwActiveProtocol); // Flag that indicates the established active protocol
        
		if(scarderr != SCARD_S_SUCCESS) 
		{		
			// printf("SCardConnect returned error %#010x \n", scarderr);
			c=4;
			SCardFreeMemory(hContext, mszReaders); // Memory block to be released
			SCardReleaseContext(hContext); // Handle that identifies the resource manager context
			goto quit;
		}

		// ATR of the smartcard
		LPBYTE pbAtr = NULL; // Pointer to a buffer that receives the attribute (ATR)
		DWORD dwAtr = 0; // Length of the pbAtr buffer

		// length of ATR
		scarderr = SCardGetAttrib(hCard, // Handle returned from SCardConnect
									SCARD_ATTR_ATR_STRING,  // Identifier for the attribute to get (ATR)
									NULL, // Pointer to a buffer that receives the attribute
									&dwAtr); // Length of the pbAttr buffer in bytes

		if (scarderr != SCARD_S_SUCCESS)
		{  
			// printf("SCardGetAttrib returned error %#010x \n", scarderr)
			SCardDisconnect(hCard, SCARD_LEAVE_CARD); // Action to take on the card in the connected reader on close
			SCardReleaseContext(hContext); // Handle that identifies the resource manager context
			goto quit;
		}

		// allocate memory
		pbAtr = new byte[dwAtr];

		// get ATR
		scarderr = SCardGetAttrib(hCard, // Handle returned from SCardConnect
									SCARD_ATTR_ATR_STRING, // Identifier for the attribute to get (ATR)
									pbAtr, // Pointer to a buffer that receives the attribute
									&dwAtr); // Length of the pbAttr buffer in bytes

		if (scarderr != SCARD_S_SUCCESS)   
		{   
			//printf("SCardGetAttrib returned error %#010x \n", scarderr)
			SCardDisconnect(hCard, SCARD_LEAVE_CARD); // Action to take on the card in the connected reader on close
			SCardReleaseContext(hContext); // Handle that identifies the resource manager context
			goto quit;
		}

		// free memory again
		delete [] pbAtr;

		// transaction locking
		scarderr = SCardBeginTransaction(hCard); // Handle returned from SCardConnect
		if (scarderr != SCARD_S_SUCCESS) // if an error occurred
		{ 
			//printf("SCardBeginTransaction returned error %#010x \n", scarderr)
			SCardDisconnect(hCard, SCARD_LEAVE_CARD); // Action to take on the card in the connected reader on close
			SCardReleaseContext(hContext); // Handle that identifies the resource manager context
			c=6;
			goto quit;
		}	

		// send command sequence
		SCARD_IO_REQUEST ioSend;// protocol header structure for the instruction
		ioSend.dwProtocol = dwActiveProtocol;
		ioSend.cbPciLength = sizeof(SCARD_IO_REQUEST);
	
		byte* arrayAPDU;
		arrayAPDU=new byte[ct]; // dynamic array

		if (valueArray && ct)
		{
			PRUint32 j;
			for(j= 0; j < ct; j++)
			{		
				arrayAPDU[j]=(byte) valueArray[j];
			}
		}

		else 
			goto quit;

		//byte test[] = {0x00, 0xa4, 0x04, 0x00, 0x0a, 0xa0, 0x00, 0x00, 0x00, 0x62, 0x03, 0x01, 0x0c, 0x06, 0x01};    
		//00 A4 04 00 0A A0 00 00 00 62 03 01 0C 06 01      
		//byte test2[] = {0x00, 0x20, 0x00, 0x02, 0x06, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36};  
		//byte test3[] = {0x00, 0xb0, 0x00, 0x00, 0x80};
        
		byte responseAPDU[256];        
		DWORD dwLenR = sizeof(responseAPDU); // Length of Response Buffer with paddding(static)
		
		scarderr = SCardTransmit(hCard, // Handle returned from SCardConnect
								&ioSend, // Protocol header structure for the instruction
								arrayAPDU, // Pointer to the actual data to be sent to the card
								ct, // Length (in bytes) of the SendBuffer parameter
								NULL, // Protocol header structure for the instruction
								responseAPDU, // Pointer to any data returned from the card
								&dwLenR); // Supplies the length of the responseAPDU parameter (in bytes) and receives the actual number of bytes received from the smart card

		if (scarderr != SCARD_S_SUCCESS)  
		{
			delete[] arrayAPDU;
			goto free;
		}

		*count= dwLenR; // size of the response without the padding
		*retv = (PRInt32*)nsMemory::Alloc(*count * sizeof(PRInt32)); // nsMemory: XPCOM class to allocate scriptable array

		for(i=0; i<*count; i++)
		{
			(*retv)[i] = responseAPDU[i];		
		}

		delete[] arrayAPDU;
		goto free;


quit: 
      
		// *_retval = c;
		return NS_OK; // XPCOM: return to scriptable function

free:

		// call SCardDisconnect()
		scarderr = SCardDisconnect(hCard, // Handle returned from SCardConnect
		SCARD_LEAVE_CARD); // Action to take on the card in the connected reader on close
       
		if(scarderr != SCARD_S_SUCCESS)   // if an error occurred
		{  
			// printf("SCardDisconnect returned error %#010x \n", scarderr);
			goto quit;
		}
        
		// call SCardReleaseContext()
		scarderr = SCardReleaseContext(hContext); // Handle that identifies the resource manager context
		if (scarderr != SCARD_S_SUCCESS) // if an error occurred
		{  
			// printf("SCardReleaseContext returned error %#010x \n", scarderr);
			goto quit;
		}	

	return NS_OK; // XPCOM return to scriptable function 
}

