/*
* SmartC : Firefox plugin for smart cards. 
* Copyright (C) 2010  Gilles Bernabe.
*
* Sources: reuse of Ludovic Rousseau example code included in the source of PCSC-Lite.
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
	
	int l=0;
	LONG rv;
	SCARDCONTEXT hContext;
	DWORD dwReaders;
	LPSTR mszReaders = NULL;
	char *ptr, **readers = NULL;
	int nbReaders;
	SCARDHANDLE hCard;
	DWORD dwActiveProtocol, dwReaderLen, dwState, dwProt, dwAtrLen;
	BYTE pbAtr[MAX_ATR_SIZE] = "";
	char pbReader[MAX_READERNAME] = "";
	int reader_nb;
	unsigned int i;
	SCARD_IO_REQUEST *pioSendPci;
	SCARD_IO_REQUEST pioRecvPci;
	BYTE pbRecvBuffer[256];
	DWORD dwSendLength, dwRecvLength;

	rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
	if (rv != SCARD_S_SUCCESS)
	{
		//printf("SCardEstablishContext: Cannot Connect to Resource Manager %lX\n", rv);
		return EXIT_FAILURE;
	}
	
	rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
	PCSC_ERROR(rv, "SCardListReaders")
	
	mszReaders = (char*) malloc(sizeof(char)*dwReaders);
	if (mszReaders == NULL)
	{
		printf("malloc: not enough memory\n");
		goto end;
	}	
	
	rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
	PCSC_ERROR(rv, "SCardListReaders")

	/* Extract readers from the null separated string and get the total
	 * number of readers */
	nbReaders = 0;
	ptr = mszReaders;
	while (*ptr != '\0')
	{
		ptr += strlen(ptr)+1;
		nbReaders++;
	}

	if (nbReaders == 0)
	{
		//printf("No reader found\n");
		goto end;
	}

	/* allocate the readers table */
	readers =(char **) calloc(nbReaders, sizeof(char *));
	if (NULL == readers)
	{
		//printf("Not enough memory for readers[]\n");
		goto end;
	}

	/* fill the readers table */
	nbReaders = 0;
	ptr = mszReaders;
	while (*ptr != '\0')
	{
		//printf("%d: %s\n", nbReaders, ptr);
		readers[nbReaders] = ptr;
		ptr += strlen(ptr)+1;
		nbReaders++;
	}

	reader_nb = 0;

	/* connect to a card */
	dwActiveProtocol = -1;
	rv = SCardConnect(hContext, readers[reader_nb], SCARD_SHARE_SHARED,
	SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
	//printf(" Protocol: %ld\n", dwActiveProtocol);
	PCSC_ERROR(rv, "SCardConnect")

	/* get card status */
	dwAtrLen = sizeof(pbAtr);
	dwReaderLen = sizeof(pbReader);
	rv = SCardStatus(hCard, /*NULL*/ pbReader, &dwReaderLen, &dwState, &dwProt, pbAtr, &dwAtrLen);
	//printf(" Reader: %s (length %ld bytes)\n", pbReader, dwReaderLen);
	//printf(" State: 0x%lX\n", dwState);
	//printf(" Prot: %ld\n", dwProt);
	//printf(" ATR (length %ld bytes):", dwAtrLen);
	/*for (i=0; i<dwAtrLen; i++)
		printf(" %02X", pbAtr[i]);
	printf("\n");*/
	PCSC_ERROR(rv, "SCardStatus")

	
	switch(dwActiveProtocol)
	{
	case SCARD_PROTOCOL_T0:
		pioSendPci = SCARD_PCI_T0;
		break;
	case SCARD_PROTOCOL_T1:
		pioSendPci = SCARD_PCI_T1;
		break;
        default:
		printf("Unknown protocol\n");
		return -1;
   	}

	/* exchange APDU */

	//byte test[] = {0x00, 0xa4, 0x04, 0x00, 0x0a, 0xa0, 0x00, 0x00, 0x00, 0x62, 0x03, 0x01, 0x0c, 0x06, 0x01};    
	//00 A4 04 00 0A A0 00 00 00 62 03 01 0C 06 01      
	//byte test2[] = {0x00, 0x20, 0x00, 0x02, 0x06, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36};  
	//byte test3[] = {0x00, 0xb0, 0x00, 0x00, 0x80};
        
	BYTE* arrayAPDU;
	arrayAPDU = (BYTE*)malloc(ct * sizeof(BYTE));
	

	if (valueArray && ct)
	{		
		for(l= 0; l < ct; l++)
		{		
			arrayAPDU[l]= (BYTE) valueArray[l];
		}
	}
	else 
		goto end;

	dwSendLength = ct;
	dwRecvLength = sizeof(pbRecvBuffer);

	/*printf("Sending: ");
	for (l=0; l<dwSendLength; l++)
		printf("%02X ", arrayAPDU[l]);
	printf("\n");*/
	rv = SCardTransmit(hCard, pioSendPci, arrayAPDU, dwSendLength, &pioRecvPci, pbRecvBuffer, &dwRecvLength);
	/*printf("Received: ");

	for (l=0; l<dwRecvLength; l++)
		printf("%02X ", pbRecvBuffer[l]);
	printf("\n");*/

	PCSC_ERROR(rv, "SCardTransmit")

	free (arrayAPDU);

	*count= (int)dwRecvLength; //size of the response without the padding
	*retv = (PRInt32*)nsMemory::Alloc(*count * sizeof(PRInt32)); // nsMemory: XPCOM class to allocate scriptable array	

	for(l=0; l<*count; l++)
	{
		(*retv)[l]= pbRecvBuffer[l];
		// printf("%02x \n ", responseAPDU[i]);		
	}

	/* end transaction */
	rv = SCardEndTransaction(hCard, SCARD_LEAVE_CARD);
	PCSC_ERROR(rv, "SCardEndTransaction")

	/* card disconnect */
	rv = SCardDisconnect(hCard, SCARD_UNPOWER_CARD);
	PCSC_ERROR(rv, "SCardDisconnect")
	

	goto end;


end:
	/* We try to leave things as clean as possible */
	rv = SCardReleaseContext(hContext);
	if (rv != SCARD_S_SUCCESS)
		//printf("SCardReleaseContext: %s (0x%lX)\n", pcsc_stringify_error(rv),rv);

	/* free allocated memory */
	if (mszReaders)
		free(mszReaders);
	if (readers)
		free(readers);

	//*_retval = arrayAPDU;
	return NS_OK;	
}

