#pragma once
#include <Windows.h>
#include <iostream>
#include <winternl.h>
#include <ntstatus.h>
#include <atomic>
#include <mutex>
#include <TlHelp32.h>
#include <d3d9.h>
#include <xmmintrin.h>


typedef enum _KEY_VALUE_INFORMATION_CLASS {
	KeyValueBasicInformation,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64,
	KeyValueLayerInformation,
	MaxKeyValueInfoClass  // MaxKeyValueInfoClass should always be the last enum
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_FULL_INFORMATION {
	ULONG   TitleIndex;
	ULONG   Type;
	ULONG   DataOffset;
	ULONG   DataLength;
	ULONG   NameLength;
	WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, * PKEY_VALUE_FULL_INFORMATION;


#ifdef __cplusplus
extern "C++"
{
	char _RTL_CONSTANT_STRING_type_check(const char* s);
	char _RTL_CONSTANT_STRING_type_check(const WCHAR* s);
	// __typeof would be desirable here instead of sizeof.
	template <size_t N> class _RTL_CONSTANT_STRING_remove_const_template_class;
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(char)> { public: typedef  char T; };
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(WCHAR)> { public: typedef WCHAR T; };
#define _RTL_CONSTANT_STRING_remove_const_macro(s) \
    (const_cast<_RTL_CONSTANT_STRING_remove_const_template_class<sizeof((s)[0])>::T*>(s))
}
#else
char _RTL_CONSTANT_STRING_type_check(const void* s);
#define _RTL_CONSTANT_STRING_remove_const_macro(s) (s)
#endif
#define RTL_CONSTANT_STRING(s) \
{ \
    sizeof( s ) - sizeof( (s)[0] ), \
    sizeof( s ) / sizeof(_RTL_CONSTANT_STRING_type_check(s)), \
    _RTL_CONSTANT_STRING_remove_const_macro(s) \
}

extern "C" {
	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwClose(
			_In_ HANDLE Handle
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwOpenKey(
			_Out_ PHANDLE KeyHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwSetValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_opt_ ULONG TitleIndex,
			_In_ ULONG Type,
			_In_reads_bytes_opt_(DataSize) PVOID Data,
			_In_ ULONG DataSize
		);

	NTSYSAPI NTSTATUS ZwCreateKey(
		PHANDLE            KeyHandle,
		ACCESS_MASK        DesiredAccess,
		POBJECT_ATTRIBUTES ObjectAttributes,
		ULONG              TitleIndex,
		PUNICODE_STRING    Class,
		ULONG              CreateOptions,
		PULONG             Disposition
	);
}

namespace RegistryUtils
{
    template <typename T>
    T ReadRegistry(const std::wstring& regPath, const std::wstring& key)
    {
        // Open the registry key
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        {
            return T();
        }

        // Query the value size
        DWORD type = 0;
        DWORD dataSize = 0;
        if (RegQueryValueExW(hKey, key.c_str(), NULL, &type, NULL, &dataSize) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return T();
        }

        // Allocate buffer for the value
        std::vector<BYTE> data(dataSize);

        // Query the value data
        if (RegQueryValueExW(hKey, key.c_str(), NULL, &type, data.data(), &dataSize) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return T();
        }

        // Close the registry key
        RegCloseKey(hKey);

        // Convert the data to the return type
        if (type != GetRegType<T>())
        {
            return T();
        }

        return ConvertRegValue<T>(data.data(), dataSize);
    }

    // Helper function to get the registry value type for a given type T
    template <typename T>
    DWORD GetRegType()
    {
        if (std::is_same<T, std::wstring>::value)
        {
            return REG_SZ;
        }
        else if (std::is_same<T, DWORD>::value)
        {
            return REG_DWORD;
        }
        else if (std::is_same<T, QWORD>::value)
        {
            return REG_QWORD;
        }
        else
        {
            static_assert(false, "Unsupported registry value type");
            return 0;
        }
    }

    // Helper function to convert the registry value data to a given type T
    template <typename T>
    T ConvertRegValue(const BYTE* data, DWORD dataSize)
    {
        if (std::is_same<T, std::wstring>::value)
        {
            return std::wstring(reinterpret_cast<const wchar_t*>(data), dataSize / sizeof(wchar_t));
        }
        else if (std::is_same<T, DWORD>::value)
        {
            return *reinterpret_cast<const DWORD*>(data);
        }
        else if (std::is_same<T, QWORD>::value)
        {
            return *reinterpret_cast<const QWORD*>(data);
        }
        else
        {
            static_assert(false, "Unsupported registry value type");
            return T();
        }
    }
}

DWORD Daimkey = VK_RBUTTON;		//aimkey
int aimheight = 46;				//aim height value
unsigned int asdelay = 90;		//use x-999 (shoot for xx millisecs, looks more legit)
bool IsPressed = false;			//
DWORD astime = timeGetTime();	//autoshoot timer

//init only once
bool firstTime = true;

//viewport
UINT vps = 1;
D3D11_VIEWPORT viewport;
float ScreenCenterX;
float ScreenCenterY;

//vertex
ID3D11Buffer *veBuffer;
UINT Stride = 24;
UINT veBufferOffset = 0;
D3D11_BUFFER_DESC vedesc;

//index
ID3D11Buffer *inBuffer;
DXGI_FORMAT inFormat;
UINT        inOffset;
D3D11_BUFFER_DESC indesc;

//rendertarget
ID3D11Texture2D* RenderTargetTexture;
ID3D11RenderTargetView* RenderTargetView = NULL;

//shader
ID3D11PixelShader* psRed = NULL;
ID3D11PixelShader* psGreen = NULL;

//pssetshaderresources
UINT pssrStartSlot;
D3D11_SHADER_RESOURCE_VIEW_DESC  Descr;
ID3D11ShaderResourceView* ShaderResourceView;
D3D11_TEXTURE2D_DESC texdesc;

//psgetConstantbuffers
ID3D11Buffer *pcsBuffer;
D3D11_BUFFER_DESC pscdesc;
UINT pscStartSlot;

//vsgetconstantbuffers
ID3D11Buffer *mConstantBuffers;
UINT vsConstant_StartSlot;

UINT psStartSlot = 0, vsStartSlot = 0;

const auto pcall_present_discord = Helper::PatternScan(Discord::GetDiscordModuleBase(), "FF 15 ? ? ? ? 8B D8 E8 ? ? ? ? E8 ? ? ? ? EB 10");

const auto presentSceneAdress = Helper::PatternScan(Discord::GetDiscordModuleBase(), "56 57 53 48 83 EC 30 44 89 C6");

DISCORD.HookFunction(presentSceneAdress, reinterpret_cast<uintptr_t>(PresentHook), reinterpret_cast<uintptr_t>(&PresentOriginal));

DISCORD.HookFunction(presentSceneAdress, reinterpret_cast<uintptr_t>(ResizeHook), reinterpret_cast<uintptr_t>(&PresentOriginal));

