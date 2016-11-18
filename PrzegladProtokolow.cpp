// Wypluwamy wszystkie serwisy jakie mamy w rękawie


#include <winsock2.h>
#include <Ws2bth.h>
#include <BluetoothAPIs.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "Bthprops.lib") 

BOOL __stdcall callback(ULONG uAttribId, LPBYTE pValueStream, ULONG cbStreamSize, LPVOID pvParam)
{
	SDP_ELEMENT_DATA element;
	/*****************************************************************************************************/
	// Just a verification, uncomment to see the output!!!
	//printf("Callback() uAttribId: %ul\n", uAttribId);
	//printf("Callback() pValueStream: %d\n ", pValueStream);
	//printf("Callback() cbStreamSize: %ul\n ", cbStreamSize);
	/*****************************************************************************************************/

	if (BluetoothSdpGetElementData(pValueStream, cbStreamSize, &element) != ERROR_SUCCESS)
	{
		/*****************************************************************************************************/
		// Just a verification
		// printf("BluetoothSdpGetElementData() failed with error code %ld\n", WSAGetLastError());
		/*****************************************************************************************************/
		return FALSE;
	}
	else
	{
		/*****************************************************************************************************/
		// Just a verification
		// printf("BluetoothSdpGetElementData() is OK!\n");
		/*****************************************************************************************************/
		return TRUE;
	}
}

int main(int argc, char** argv)
{
	WSADATA m_data;
	SOCKET s;
	WSAPROTOCOL_INFO protocolInfo;
	int protocolInfoSize;
	WSAQUERYSET querySet, *pResults, querySet2;
	HANDLE hLookup, hLookup2;
	int result;
	static int i;
	BYTE buffer[1000];
	BYTE buffer1[2000];
	DWORD bufferLength, flags, addressSize, bufferLength1;
	CSADDR_INFO *pCSAddr;
	BTH_DEVICE_INFO *pDeviceInfo;
	char addressAsString[2000];
	BLOB *pBlob;
	GUID protocol;

	if (WSAStartup(MAKEWORD(2, 2), &m_data) == 0)
	{
		printf("Tworzenie Socketu na blutacza!\n");

		s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if (s == INVALID_SOCKET)
		{
			printf("UPs popsulo sie%ld\n", WSAGetLastError());
			return 1;
		}
		else
			printf("Mamy sokecik!\n");

		protocolInfoSize = sizeof(protocolInfo);

		if (getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&protocolInfo, &protocolInfoSize) != 0)
		{
			printf("Smutny kod:  %ld\n", WSAGetLastError());
			return 1;
		}
		else
			printf(":)!\n");

		// Kryteria zapytania
		memset(&querySet, 0, sizeof(querySet));
		querySet.dwSize = sizeof(querySet);
		querySet.dwNameSpace = NS_BTH;

		// Flagi zapytania
		flags = LUP_RETURN_NAME | LUP_CONTAINERS | LUP_RETURN_ADDR | LUP_FLUSHCACHE | LUP_RETURN_TYPE | LUP_RETURN_BLOB | LUP_RES_SERVICE;

		// No i lecimy!
		result = WSALookupServiceBegin(&querySet, flags, &hLookup);

		// ok
		if (result == 0)
		{
			printf("\tZaczynamy:\n");
			i = 0;

			while (result == 0)
			{
				bufferLength = sizeof(buffer);
				pResults = (WSAQUERYSET *)&buffer;

				// Next query...
				result = WSALookupServiceNext(hLookup, flags, &bufferLength, pResults);
				if (result != 0)
				{
					printf("\tNie pyklo :c%ld\n", WSAGetLastError());
				}
				else
				{
					// Get the device info, name, address etc
					printf("\tNastepna usluga\n");
					printf("\tNazwa: %S\n", pResults->lpszServiceInstanceName);
					pCSAddr = (CSADDR_INFO *)pResults->lpcsaBuffer;
					pDeviceInfo = (BTH_DEVICE_INFO *)pResults->lpBlob;
					memset(&querySet2, 0, sizeof(querySet2));
					querySet2.dwSize = sizeof(querySet2);
					protocol = L2CAP_PROTOCOL_UUID;
					querySet2.lpServiceClassId = &protocol;
					querySet2.dwNameSpace = NS_BTH;

					addressSize = sizeof(addressAsString);

					// Adres Lokalny
					if (WSAAddressToString(pCSAddr->LocalAddr.lpSockaddr, pCSAddr->LocalAddr.iSockaddrLength,
						&protocolInfo, (LPWSTR)addressAsString, &addressSize) == 0)
					{
						printf("\tAdres lokalny: %S\n", addressAsString);
					}
					else
						printf("\t:c %ld\n", WSAGetLastError());

					addressSize = sizeof(addressAsString);

					// Adres Zdalny
					if (WSAAddressToString(pCSAddr->RemoteAddr.lpSockaddr, pCSAddr->RemoteAddr.iSockaddrLength,
						&protocolInfo, (LPWSTR)addressAsString, &addressSize) == 0)
					{
						printf("\tAdres zdalny: %S\n", addressAsString);
					}
					else
						printf("\t:c: %ld\n", WSAGetLastError());

					// Przygotuj serie zapytan
					querySet2.lpszContext = (LPWSTR)addressAsString;

					flags = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_TYPE | LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;

					// Lecimy
					result = WSALookupServiceBegin(&querySet2, flags, &hLookup2);
					if (result == 0)
					{
						printf("Protokoly:\n");
						while (result == 0)
						{
							bufferLength1 = sizeof(buffer1);
							pResults = (WSAQUERYSET *)&buffer1;

							// Dalej?
							result = WSALookupServiceNext(hLookup2, flags, &bufferLength1, pResults);

							if (result == 0)
							{
								// Zapelniamy
								printf("\tNazwa protokolu: %S\n",
									pResults->lpszServiceInstanceName);
								printf("\tJakis komentarz:: %s\n", pResults->lpszComment);
								pCSAddr = (CSADDR_INFO *)pResults->lpcsaBuffer;

								if (pResults->lpBlob)
								{
									pBlob = (BLOB*)pResults->lpBlob;
									if (!BluetoothSdpEnumAttributes(pBlob->pBlobData, pBlob->cbSize, callback, 0))
									{
										printf("Suabo %ld\n", WSAGetLastError());
									}
									else
									{
										printf("Protokul #%d is OK!\n", i++);
									}
								}
							}
							else
							{
								printf("\tKoniec!\n");
							}
						}
					}
				}
			}
		}
	} 
	system("pause");
	return 0;
}