// No to tak. Jak masz parę dongli z BT to wybieramy pierwszy
// skanujemy ile wlezie, wyświetlamy i idziemy do kolejnego dongla.
// Do tego dla znalezionych Dongli szukamy urzadzen Nimi.
// ELo
// I tyle :>

#include <stdlib.h>
#include <stdio.h>
#include <Winsock2.h>
#include <Ws2bth.h>
#include <BluetoothAPIs.h>

#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "Bthprops.lib") 

BLUETOOTH_FIND_RADIO_PARAMS MamyDongiel_find_radio = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };

BLUETOOTH_RADIO_INFO MamyDongiel_info = { sizeof(BLUETOOTH_RADIO_INFO),0, };

BLUETOOTH_DEVICE_SEARCH_PARAMS m_search_params = {
	sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),1,0,1,1,1,15,NULL
};

BLUETOOTH_DEVICE_INFO m_device_info = { sizeof(BLUETOOTH_DEVICE_INFO),0, };

// Uwaga:
// Radio - To jest dongiel
// Urzadzenie - To jest z czym sie ten dongiel łączy

int main()
{
	HANDLE dongiel = NULL;
	HBLUETOOTH_RADIO_FIND MamyDongiel = NULL;
	HBLUETOOTH_DEVICE_FIND MamyUrzadzenie = NULL;
	int Dongiel_ID;
	int Urzadzenie_ID;
	DWORD Info;

/*****************************************************************************************************/
	//while (TRUE)
	//{
/*****************************************************************************************************/
		MamyDongiel = BluetoothFindFirstRadio(&MamyDongiel_find_radio, &dongiel);

		if (MamyDongiel != NULL)
			printf("Mamy Dongiel\n");
		else
			printf("No cos sie posypalo. Kod bledu: %d\n", GetLastError());

		Dongiel_ID = 0;

/*****************************************************************************************************/
		//do {
/*****************************************************************************************************/
			Info = BluetoothGetRadioInfo(dongiel, &MamyDongiel_info);
			if (Info == ERROR_SUCCESS)
				printf("No to info zebrane!\n");
			else
				printf("No cos sie posypalo. Kod bledu: %d\n", Info);

			wprintf(L"Radio %d:\r\n", Dongiel_ID);
			wprintf(L"\tNazwa: %s\r\n", MamyDongiel_info.szName);
			wprintf(L"\tAdres: %02X:%02X:%02X:%02X:%02X:%02X\r\n", MamyDongiel_info.address.rgBytes[5],
				MamyDongiel_info.address.rgBytes[4], MamyDongiel_info.address.rgBytes[3], MamyDongiel_info.address.rgBytes[2],
				MamyDongiel_info.address.rgBytes[1], MamyDongiel_info.address.rgBytes[0]);
			wprintf(L"\tKlasa: 0x%08x\r\n", MamyDongiel_info.ulClassofDevice);
			wprintf(L"\tWykonawca Dongla: 0x%04x\r\n", MamyDongiel_info.manufacturer);

			m_search_params.hRadio = dongiel;
			ZeroMemory(&m_device_info, sizeof(BLUETOOTH_DEVICE_INFO));
			m_device_info.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);

			// Szukanie Urzadzonek
			MamyUrzadzenie = BluetoothFindFirstDevice(&m_search_params, &m_device_info);

			if (MamyUrzadzenie != NULL)
				printf("\nA dziala tu jakies urzadzenie?\n");
			else
				printf("\nCos sie popsulo i nie bylo mnie slychac %d\n", GetLastError());

			Dongiel_ID++;
			Urzadzenie_ID = 0;
			
			// Info o urzadzonku
			do
			{
				wprintf(L"\n\tUrzadzenie %d:\r\n", Urzadzenie_ID);
				wprintf(L"  \tNazwa: %s\r\n", m_device_info.szName);
				wprintf(L"  \tAdres: %02X:%02X:%02X:%02X:%02X:%02X\r\n", m_device_info.Address.rgBytes[5],
					m_device_info.Address.rgBytes[4], m_device_info.Address.rgBytes[3], m_device_info.Address.rgBytes[2],
					m_device_info.Address.rgBytes[1], m_device_info.Address.rgBytes[0]);
				wprintf(L"  \tKlasa: 0x%08x\r\n", m_device_info.ulClassofDevice);
				wprintf(L"  \tPolaczony?: %s\r\n", m_device_info.fConnected ? L"true" : L"false");
				wprintf(L"  \tAutentykacja?: %s\r\n", m_device_info.fAuthenticated ? L"true" : L"false");
				wprintf(L"  \tZapamietany?: %s\r\n", m_device_info.fRemembered ? L"true" : L"false");
				Urzadzenie_ID++;

				// Super rzeczy bulwo
				if (!BluetoothFindNextDevice(MamyUrzadzenie, &m_device_info))
					break;

			} while (BluetoothFindNextDevice(MamyUrzadzenie, &m_device_info));

			// Zamykamy bo urzadzen nie mamy
			if (BluetoothFindDeviceClose(MamyUrzadzenie) == TRUE)
				printf("\nBluetoothFindDeviceClose(MamyUrzadzenie) is OK!\n");
			else
				printf("\nBluetoothFindDeviceClose(MamyUrzadzenie) failed with error code %d\n", GetLastError());
/*****************************************************************************************************/
		//} while (BluetoothFindNextRadio(&MamyDongiel_find_radio, &dongiel));
/*****************************************************************************************************/

		// Zamykamy bo urzadzen nie mamy
		if (BluetoothFindRadioClose(MamyDongiel) == TRUE)
			printf("BluetoothFindRadioClose(MamyDongiel) is OK!\n");
		else
			printf("BluetoothFindRadioClose(MamyDongiel) failed with error code %d\n", GetLastError());

/*****************************************************************************************************/
		// Exit the outermost WHILE and BluetoothFindXXXXRadio loops if there is no more radio
		/*if (!BluetoothFindNextRadio(&MamyDongiel_find_radio, &dongiel))
			break;*/

		// Give some time for the 'signal' which is a typical for crap wireless devices
		//Sleep(1000);
	//}
/*****************************************************************************************************/
	system("pause");
	return 0;
}