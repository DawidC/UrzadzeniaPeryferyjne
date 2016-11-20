#include <windows.h>
#include <stdio.h>
#include <conio.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>

using namespace std;

LPDIRECTINPUT8 DirectInput;
LPDIRECTINPUTDEVICE8 GamePad;

// Odpowiedz zwrotna Joysticka
BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
{
	HRESULT Pom;

	// Tworzenie i inicjalizacja urzadzenia 
	Pom = DirectInput->CreateDevice(instance->guidInstance, &GamePad, NULL);

	// Nie udalo sie pobrac GamePada, w sensie ze nie 
	// zostal on znaleziony jako uzadzenie podlaczone do 
	// kompa
	if (FAILED(Pom)) {
		return DIENUM_CONTINUE;
	}

	return DIENUM_STOP;
}

// Potrzebne do zwracania wartosci Osi X i Y
BOOL CALLBACK enumAxesCallback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context)
{
	HWND hDlg = (HWND)context;

	DIPROPRANGE propRange;
	propRange.diph.dwSize = sizeof(DIPROPRANGE);
	propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propRange.diph.dwHow = DIPH_BYID;
	propRange.diph.dwObj = instance->dwType;
	propRange.lMin = -1000;
	propRange.lMax = +1000;

	// Zakres układu
	if (FAILED(GamePad->SetProperty(DIPROP_RANGE, &propRange.diph))) {
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

// Funkcja poll potrzebna nam jest do uzyskania
// informacji na temat stanu naszych przyciskow
// i naszej pozycji(joysticka) w przestrzeni
HRESULT poll(DIJOYSTATE *js)
{
	HRESULT  Pom;

	if (GamePad == NULL) {
		return S_OK;
	}

	// Odczyt GamePada (czy mozemy go dalej uzywac)
	Pom = GamePad->Poll();
	if (FAILED(Pom)) {

		// Przywrocenie polaczenia jesli strumien input zostal przerwany
		Pom = GamePad->Acquire();
		while (Pom == DIERR_INPUTLOST) {
			Pom = GamePad->Acquire();
		}

		// Na wypadek fatal errora
		if ((Pom == DIERR_INVALIDPARAM) || (Pom == DIERR_NOTINITIALIZED)) {
			return E_FAIL;
		}

		// Jesli inna aplikacja ma kontrole nad GamePadiem, jest ok
		// polaczenie zostanie przywrocone gdy GamePad sie zwolni
		if (Pom == DIERR_OTHERAPPHASPRIO) {
			return S_OK;
		}
	}

	// Odczyt stanu urzadzenia
	if (FAILED(Pom = GamePad->GetDeviceState(sizeof(DIJOYSTATE), js))) {
		return Pom;
	}

	return S_OK;
}

int main() {

	HRESULT Pom;

	// Inicjalizacja urzadzenia DirectInput
	if (FAILED(Pom = DirectInput8Create(GetModuleHandle(NULL),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&DirectInput, NULL))) {
		return 1;
	}

	// Szukanie pierwszego GamePada
	if (FAILED(Pom = DirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumCallback,
		NULL, DIEDFL_ATTACHEDONLY))) {
		return 1;
	}

	// Nie znaleziono GamePada
	if (GamePad == NULL) {
		printf("Nie znaleziono urzadzenia. :C\n");
		return E_FAIL;
	}
	// Nazwa Pada i inne informacje o nim dostpne w strukturze DIDEVICEINSTANCE 
	else {
		printf("Znaleziono GamePad! :)\n");
		DIDEVICEINSTANCE info;
		info.dwSize = sizeof(DIDEVICEINSTANCE);
		GamePad->GetDeviceInfo(&info);
		wchar_t* NazwaPada = info.tszProductName;
		wprintf(NazwaPada);
		printf("\n");
		printf("Typ uzadzenia w postaci numeru nadanego przez Nasz Komputer: %ld", info.dwDevType);
		printf("\n");
		printf("Typ uzadzenia w postaci numeru seryjnego nadanego przez Prodycenta: %ld", info.guidProduct);
		printf("\n");
	}

	DIDEVCAPS capabilities;

	if (FAILED(Pom = GamePad->SetDataFormat(&c_dfDIJoystick))) {
		return 1;
	}

	// sprawdzanie ile osi ma GamePad
	capabilities.dwSize = sizeof(DIDEVCAPS);
	if (FAILED(Pom = GamePad->GetCapabilities(&capabilities))) {
		return 1;
	}

	if (FAILED(Pom = GamePad->EnumObjects(enumAxesCallback, NULL, DIDFT_AXIS))) {
		return 1;
	}

	DIJOYSTATE state;
	POINT cursor;
	int klik = -1;

	LONG osX = 0;
	LONG osY = 0;
	LONG osZ = 0;

	// Oczekiwanie na akcje uzytkownika,
	// czyli klik klawisza czy zmiana
	// kursora(przesuniecie)
	printf("Uzyj GamePada:\n");
	printf("Poruszaj czy cos takiego. O! Guziczki wcisnij\n");
	int koniec = 0;
	BlockInput(true);
	while (!poll(&state)) {
		for (int i = 0; i < 32; i++) {
			if (state.rgbButtons[klik] == 0) {
				// Podniesienie lewego przycisku myszy do gory
				if (klik == 0) {
					GetCursorPos(&cursor);
					mouse_event(MOUSEEVENTF_LEFTUP, cursor.x, cursor.y, 0, 0);
				}
				klik = -1;
			}
			if (state.rgbButtons[i] && klik != i) {
				printf("Wcisnieto przycisk o numerze:  %d\n", i + 1);
				klik = i;
				if (i == 0) {
					// Wcisniecie i przytrzymanie lewego przycisku myszy
					GetCursorPos(&cursor);
					mouse_event(MOUSEEVENTF_LEFTDOWN, cursor.x, cursor.y, 0, 0);
				}
				// Prawy przycisk Myszy
				else if (i == 1) {
					GetCursorPos(&cursor);
					mouse_event(MOUSEEVENTF_RIGHTUP, cursor.x, cursor.y, 0, 0);
				}
				// Wyjscie po wcisnieciu przycisku numer 3
				else if (i == 2){
					koniec++;
				}
			}
		}
		// Wypisywanie pozycji kursora
		// Na osiach X, Y, Z
		if (osX != state.lX || osY != state.lY || osZ != state.lZ) {

			printf("X: %d Y: %d  Z: %d\n", state.lX, state.lY, state.lZ);
			osX = state.lX;
			osY = state.lY;
			osZ = state.lZ;
		}

		GetCursorPos(&cursor);
		SetCursorPos(cursor.x + state.lX / 100, cursor.y + state.lY / 100);

		// Czekanie na ponowienie akcji w clu ustalenia stanu
		// i zapewnienia plynnosci
		Sleep(10);
		if (koniec != 0){
			break;
		}
	}
	BlockInput(0);
	// Odlaczenie Pada
	if (GamePad) {
		GamePad->Unacquire();
	}
	printf("Ale fajne, milego dnia :*\nPad juz nie dziala jak co\n");

	system("pause");

	return 1;
}