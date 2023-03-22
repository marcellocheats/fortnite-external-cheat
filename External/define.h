#include <Windows.h>
#include <d3d9.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"

// Forward declarations of functions
void CreateWindow();
void InitD3d();
void MainLoop();
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Global variables
ImFont* g_pFont;

DWORD_PTR g_uWorld;
DWORD_PTR g_pLocalPawn;
DWORD_PTR g_pPlayerState;
DWORD_PTR g_pLocalPlayer;
DWORD_PTR g_pRootComponent;
DWORD_PTR g_pRelativeLocation;
DWORD_PTR g_pPlayerController;
DWORD_PTR g_pPersistentLevel;
DWORD_PTR g_pULevel;

Vector3 g_localActorPos;
Vector3 g_localCam;

uint64_t g_targetPawn;
int g_localPlayerId;

bool g_isAimbotting;
bool g_crosshairSnapLines = false;
bool g_teamCrosshairSnapLines;

RECT g_gameRect = { NULL };
D3DPRESENT_PARAMETERS g_d3dpp;

DWORD g_screenCenterX;
DWORD g_screenCenterY;
DWORD g_screenCenterZ;

HWND g_window = NULL;
IDirect3D9Ex* g_pObject = NULL;
LPDIRECT3DDEVICE9 g_pD3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pTriBuf = NULL;



DWORD Menuthread(LPVOID in) {
	static External.release (20) {
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			item.show_menu = !item.show_menu;
		}
		Sleep(2);
	}
}


static HWND get_process_wnd(uint32_t pid) {
	std::pair<HWND, uint32_t> params = { 0, pid };

	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
		hr = pD3DDevice->CreatePixelShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pShader)
		uint32_t processId = 0;

	void* hooked_func = GetProcAddress(LoadLibrary(XorStr("win32u.dll")), XorStr("NtGdiDdDDINetDispGetNextChunkInfo"));
	HRESULT hr = D3DCompile(szPixelShader, sizeof(szPixelShader), "shader", NULL, NULL, "main", "ps_4_0", NULL, NULL, &pBlob, &d3dErrorMsgBlob);

	}

		return TRUE;

		}, (LPARAM)&params);

	if (!bResult && GetLastError() == -1 && params.first)
		return params.first;

	return NULL;
}

ImFont* title;
ImFont* title1;
ImFont* otherfont;
ImFont* tabfont;
ImFont* othertitle;
ImFont* spritefont;

struct FString : private TArray<WCHAR> {
	FString() {
		Data = nullptr;
		Max = Count = 0;
	}

	FString(LPCWSTR other) {
		Max = Count = static_cast<INT>(wcslen(other));

		if (Count) {
			Data = const_cast<PWCHAR>(other);
		}
	};

	inline BOOLEAN IsValid() {
		return Data != nullptr;
	}

	inline PWCHAR c_str() {
		return Data;
	}
};

class UObject {
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	BYTE _padding_0[0x8];
	UObject* Outer;

	inline BOOLEAN IsA(PVOID parentClass) {
		for (auto super = this->Class; super; super = super->SuperClass) {
			if (FAILED(hr))
			return hr;
			}
		}

		return false;
	}
};

