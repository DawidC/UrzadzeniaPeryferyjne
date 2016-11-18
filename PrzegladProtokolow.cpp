// Link to ws2_32.lib
#include <winsock2.h>
#include <Ws2bth.h>
// Link to Bthprops.lib
#include <BluetoothAPIs.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "Bthprops.lib") 

// callback for BluetoothSdpEnumAttributes()
BOOL __stdcall callback(ULONG uAttribId, LPBYTE pValueStream, ULONG cbStreamSize, LPVOID pvParam)
{
	SDP_ELEMENT_DATA element;
	// Just a verification, uncomment to see the output!!!
	//printf("Callback() uAttribId: %ul\n", uAttribId);
	//printf("Callback() pValueStream: %d\n ", pValueStream);
	//printf("Callback() cbStreamSize: %ul\n ", cbStreamSize);

	if (BluetoothSdpGetElementData(pValueStream, cbStreamSize, &element) != ERROR_SUCCESS)
	{
		// Just a verification
		// printf("BluetoothSdpGetElementData() failed with error code %ld\n", WSAGetLastError());
		return FALSE;
	}
	else
	{
		// Just a verification
		// printf("BluetoothSdpGetElementData() is OK!\n");
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

	// Load the winsock2 library
	if (WSAStartup(MAKEWORD(2, 2), &m_data) == 0)
	{
		printf("WSAStartup() should be fine!\n");

		// Create a blutooth socket
		s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if (s == INVALID_SOCKET)
		{
			printf("Failed to get bluetooth socket with error code %ld\n", WSAGetLastError());
			return 1;
		}
		else
			printf("socket() is OK!\n");

		protocolInfoSize = sizeof(protocolInfo);

		// Get the bluetooth device info using getsockopt()
		if (getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&protocolInfo, &protocolInfoSize) != 0)
		{
			printf("getsockopt(SO_PROTOCOL_INFO) failed with error code %ld\n", WSAGetLastError());
			return 1;
		}
		else
			printf("getsockopt(SO_PROTOCOL_INFO) is OK!\n");

		// Query set criteria
		memset(&querySet, 0, sizeof(querySet));
		querySet.dwSize = sizeof(querySet);
		querySet.dwNameSpace = NS_BTH;

		// Set the flags for query
		flags = LUP_RETURN_NAME | LUP_CONTAINERS | LUP_RETURN_ADDR | LUP_FLUSHCACHE |
			LUP_RETURN_TYPE | LUP_RETURN_BLOB | LUP_RES_SERVICE;

		// Start a device in range query...
		result = WSALookupServiceBegin(&querySet, flags, &hLookup);

		// If OK
		if (result == 0)
		{
			printf("          WSALookupServiceBegin() is OK!\n");
			i = 0;

			while (result == 0)
			{
				bufferLength = sizeof(buffer);
				pResults = (WSAQUERYSET *)&buffer;

				// Next query...
				result = WSALookupServiceNext(hLookup, flags, &bufferLength, pResults);
				if (result != 0)
				{
					printf("          WSALookupServiceNext() failed with error code %ld\n", WSAGetLastError());
				}
				else
				{
					// Get the device info, name, address etc
					printf("          WSALookupServiceNext() is OK!\n");
					printf("          The service instance name is %S\n", pResults->lpszServiceInstanceName);
					pCSAddr = (CSADDR_INFO *)pResults->lpcsaBuffer;
					pDeviceInfo = (BTH_DEVICE_INFO *)pResults->lpBlob;
					memset(&querySet2, 0, sizeof(querySet2));
					querySet2.dwSize = sizeof(querySet2);
					protocol = L2CAP_PROTOCOL_UUID;
					querySet2.lpServiceClassId = &protocol;
					querySet2.dwNameSpace = NS_BTH;

					addressSize = sizeof(addressAsString);

					// Print the local bluetooth device address...
					if (WSAAddressToString(pCSAddr->LocalAddr.lpSockaddr, pCSAddr->LocalAddr.iSockaddrLength,
						&protocolInfo, (LPWSTR)addressAsString, &addressSize) == 0)
					{
						printf("          WSAAddressToString() for local address is fine!\n");
						printf("          The local address: %S\n", addressAsString);
					}
					else
						printf("          WSAAddressToString() for local address failed with error code %ld\n", WSAGetLastError());

					addressSize = sizeof(addressAsString);

					// Print the remote bluetooth device address...
					if (WSAAddressToString(pCSAddr->RemoteAddr.lpSockaddr, pCSAddr->RemoteAddr.iSockaddrLength,
						&protocolInfo, (LPWSTR)addressAsString, &addressSize) == 0)
					{
						printf("          WSAAddressToString() for remote address is fine!\n");
						printf("          The remote device address: %S\n", addressAsString);
					}
					else
						printf("          WSAAddressToString() for remote address failed with error code %ld\n", WSAGetLastError());

					// Prepare for service query set
					querySet2.lpszContext = (LPWSTR)addressAsString;

					flags = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_TYPE |
						LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;

					// Start service query
					result = WSALookupServiceBegin(&querySet2, flags, &hLookup2);
					if (result == 0)
					{
						printf("          WSALookupServiceBegin() is OK!\n");
						while (result == 0)
						{
							bufferLength1 = sizeof(buffer1);
							pResults = (WSAQUERYSET *)&buffer1;

							// Next service query
							result = WSALookupServiceNext(hLookup2, flags, &bufferLength1, pResults);

							if (result == 0)
							{
								// Populate the service info
								printf("          WSALookupServiceNext() is OK!\n");
								printf("          WSALookupServiceNext() - service instance name: %S\n",
									pResults->lpszServiceInstanceName);
								printf("          WSALookupServiceNext() - comment (if any): %s\n", pResults->lpszComment);
								pCSAddr = (CSADDR_INFO *)pResults->lpcsaBuffer;

								// Extract the sdp info
								if (pResults->lpBlob)
								{
									pBlob = (BLOB*)pResults->lpBlob;
									if (!BluetoothSdpEnumAttributes(pBlob->pBlobData, pBlob->cbSize, callback, 0))
									{
										printf("BluetoothSdpEnumAttributes() failed with error code %ld\n", WSAGetLastError());
									}
									else
									{
										printf("BluetoothSdpEnumAttributes() #%d is OK!\n", i++);
									}
								}
							}
							else
							{
								printf("          WSALookupServiceNext() failed with error code %ld\n", WSAGetLastError());
								printf("          Error code = 11011 ~ WSA_E_NO_MORE ~ No more device!\n");
							}
						}

						// Close the handle to service query
						if (WSALookupServiceEnd(hLookup2) == 0)
							printf("WSALookupServiceEnd(hLookup2) is fine!\n", WSAGetLastError());
						else
							printf("WSALookupServiceEnd(hLookup2) failed with error code %ld\n");
					}
					else
						printf("WSALookupServiceBegin() failed with error code %ld\n", WSAGetLastError());
				}
			}

			// Close handle to the device query
			if (WSALookupServiceEnd(hLookup) == 0)
				printf("WSALookupServiceEnd(hLookup) is fine!\n", WSAGetLastError());
			else
				printf("WSALookupServiceEnd(hLookup) failed with error code %ld\n");
		}
		else
		{
			printf("WSALookupServiceBegin() failed with error code %ld\n", WSAGetLastError());
		}// end WSALookupServiceBegin()

		 // Cleanup the winsock library startup
		if (WSACleanup() == 0)
			printf("WSACleanup() pretty fine!\n");
		else
			printf("WSACleanup() failed with error code %ld\n", WSAGetLastError());
	} // end WSAStartup()
	system("pause");
	return 0;
}