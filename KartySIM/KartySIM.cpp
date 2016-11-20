#include <iostream >
#include <winscard.h>

#pragma comment ( lib, "Winscard.lib") 

using namespace std;

int main() {
	// Sprawdzenie polaczenia z czytnikiem
	SCARDCONTEXT cardContext; 
	// Zmiena co robi wszystko, czyli za kazdym razem trzyma potrzebny nam status
	LONG cardFunctionReturnValue; 

	/**********************************************************************************************/
	// Laczenie z czytnikiem
	cardFunctionReturnValue = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &cardContext);

	if (SCARD_S_SUCCESS != cardFunctionReturnValue) 
	{
		printf("Nie ma polaczenia z czytnikiem :c\nKod bledu: %d\n",cardFunctionReturnValue);
	}
	else {
		printf("Dziala jak natura chciala :>\n");
	}
	/**********************************************************************************************/
	
	// No to teraz cos pobierzmy z czytnika
	SCARDHANDLE cardHandle;
	DWORD dwAP; 
	LPTSTR Nazwa = NULL; 
	DWORD alocateType = SCARD_AUTOALLOCATE; 

	/**********************************************************************************************/
	// Czytamy czytniki
	printf("\nLista Czytnikow: \n");
	cardFunctionReturnValue = SCardListReaders(cardContext, NULL, (LPTSTR)&Nazwa, &alocateType);
	if (cardFunctionReturnValue == SCARD_S_SUCCESS) {
		LPTSTR pReader = Nazwa;
		while ('\0' != *pReader)
		{
			cout << "Czytnik :  " << pReader << " ";
			printf("(%s )", pReader);
			cout << endl;
			pReader = pReader + wcslen((wchar_t *)pReader) + 1;
		}
	}
	// A jak bedzie blad to to
	else {
		printf("Nie ma polaczenia z czytnikiem :c\nKod bledu: %d\n", cardFunctionReturnValue);
		system("PAUSE");
		exit(1);
	}
	/**********************************************************************************************/
	// A jakby cos z karty pobrac?
	/**********************************************************************************************/
	// Laczenie z karta
	cout << "\nLaczenie z karta:" << endl;
	cardFunctionReturnValue = SCardConnect(cardContext, Nazwa, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &cardHandle, &dwAP);
	if (SCARD_S_SUCCESS != cardFunctionReturnValue) {
		printf("Nie ma polaczenia z karta :c\nKod bledu: %d\n", cardFunctionReturnValue);
		system("PAUSE");
		exit(1);
	}
	else {
		printf("Polaczono z karta. Hip hip Hura!\n\n");
	}
	/**********************************************************************************************/

	// A co dalej? Protokoly!
	switch (dwAP)
	{
	case SCARD_PROTOCOL_T0:
		//bajty
		printf("Aktywny protokol T0\nKod protokolu: %d\n",cardFunctionReturnValue);
		break;
	case SCARD_PROTOCOL_T1:
		//bloki
		printf("Aktywny protokol T1\nKod protokolu: %d\n", cardFunctionReturnValue);
		break;
	case SCARD_PROTOCOL_UNDEFINED:
	default:
		printf("Aktywny protokol Nieznany(Czyli nie T0 albo T1 :>)\nKod protokolu: %d\n", cardFunctionReturnValue);
		break;
	}
	/**********************************************************************************************/


	/**********************************************************************************************/
	// A teraz pogrzebiemy w karcie
	// wybor EF ADN czyli szukamy numeru na Policje
	/*BYTE message[] = { 0xA0, 0xA4, 0x00, 0x00, 0x02, 0x6F, 0x3A }; 
	BYTE buffer[1000];
	DWORD len = 1000;
	printf("\nWysylanie komend:\n");
	for (int i = 0; i < 7; i++){ // wyswietlenie wyslanej wiadomosci
		cout << hex << int(message[i]) << " ";
	}*/





	printf("\n\nRozlaczenie vel. Koniec\n");
	system("PAUSE");
	return 0;
}
