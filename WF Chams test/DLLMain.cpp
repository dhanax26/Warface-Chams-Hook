#include "Windows.h"
#include "intrin.h"
#include "RendererBlabla.h"


//Hook With Detours
#include "detours.h"
#pragma comment(lib, "Detours.lib")
#pragma intrinsic(_ReturnAddress)

//Hook With MinHook
#include "MinHook/MinHook.h"

#define Player 0x68EBA1
bool bChams = true;


bool Initialize = false;

char dhanax26[20]{ "Hook by dhanax26" };

LPDIRECT3DTEXTURE9 texRed = NULL;

//Global Module Record
static HMODULE		g_hModule = nullptr;

typedef HRESULT(__stdcall * f_EndScene)(IDirect3DDevice9 * pDevice);
f_EndScene oEndScene; // Original Endscene

typedef HRESULT(__stdcall *Reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
HRESULT __stdcall Reset_hook(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
Reset_t Reset_orig = 0;


typedef HRESULT(__stdcall *SetStreamSource_t)(IDirect3DDevice9*, UINT, IDirect3DVertexBuffer9*, UINT, UINT);
SetStreamSource_t SetStreamSource_orig = 0;

typedef HRESULT(__stdcall *SetTexture_t)(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9 *);
SetTexture_t SetTexture_orig = 0;

HRESULT WINAPI Hooked_Present(LPDIRECT3DDEVICE9 Device, CONST RECT *pSrcRect, CONST RECT *pDestRect, HWND hDestWindow, CONST RGNDATA *pDirtyRegion);
HRESULT WINAPI Hooked_DrawIndexedPrimitive(LPDIRECT3DDEVICE9 Device, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
//returns
typedef HRESULT(WINAPI *Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
typedef HRESULT(WINAPI *Prototype_DrawIndexedPrimitive)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);

Prototype_Present Original_Present;
Prototype_DrawIndexedPrimitive Original_DrawIndexedPrimitive;


DWORD FindDevice(DWORD Len)
{
	DWORD dwObjBase = 0;

	dwObjBase = (DWORD)LoadLibrary("d3d9.dll");
	while (dwObjBase++ < dwObjBase + Len)
	{
		if ((*(WORD*)(dwObjBase + 0x00)) == 0x06C7 && (*(WORD*)(dwObjBase + 0x06)) == 0x8689 && (*(WORD*)(dwObjBase + 0x0C)) == 0x8689) {
			dwObjBase += 2; break;
		}
	}
	return(dwObjBase);
}

DWORD GetDeviceAddress(int VTableIndex)
{
	PDWORD VTable;
	*(DWORD*)&VTable = *(DWORD*)FindDevice(0x128000);
	return VTable[VTableIndex];
}

HRESULT GenerateTexture(LPDIRECT3DDEVICE9 pDevice, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
{
	if (FAILED(pDevice->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)))
		return E_FAIL;

	WORD colour16 = ((WORD)((colour32 >> 28) & 0xF) << 12)
		| (WORD)(((colour32 >> 20) & 0xF) << 8)
		| (WORD)(((colour32 >> 12) & 0xF) << 4)
		| (WORD)(((colour32 >> 4) & 0xF) << 0);

	D3DLOCKED_RECT d3dlr;
	(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
	WORD *pDst16 = (WORD*)d3dlr.pBits;

	for (int xy = 0; xy < 8 * 8; xy++)
		*pDst16++ = colour16;

	(*ppD3Dtex)->UnlockRect(0);

	return S_OK;
}


/*Old Main*/
/*BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!bInit) {

			Original_Present = (Prototype_Present)DetourFunction((PBYTE)GetDeviceAddress(17), (PBYTE)Hooked_Present);
			Original_DrawIndexedPrimitive = (Prototype_DrawIndexedPrimitive)DetourFunction((PBYTE)GetDeviceAddress(82), (PBYTE)Hooked_DrawIndexedPrimitive);
			bInit = true;
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		DetourRemove((PBYTE)Original_Present, (PBYTE)Hooked_Present);
		DetourRemove((PBYTE)Original_DrawIndexedPrimitive, (PBYTE)Hooked_DrawIndexedPrimitive);
	}

	return TRUE;
}*/


HRESULT __stdcall SetStreamSource_hook(LPDIRECT3DDEVICE9 Device, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT sStride)
{
	//Your Stream Hook....

	return SetStreamSource_orig(Device, StreamNumber, pStreamData, OffsetInBytes, sStride);
}

HRESULT __stdcall SetTexture_hook(LPDIRECT3DDEVICE9 Device, DWORD Sampler, IDirect3DBaseTexture9 *pTexture)
{

	//Hook Your Textures Here...

	//Example a simple wallhack....

	return SetTexture_orig(Device, Sampler, pTexture);
}


HRESULT __stdcall Hooked_EndScene(IDirect3DDevice9 * Device) // Our hooked endscene
{
	//Your EndScene

	if (Initialize)
	{
		Initialize = false;

	}

	//Example

	//DrawMenuGui(pDevice);

	//DrawBox(Device, 71.0f, 65.0f, 200.0f, 20.0f, D3DCOLOR_ARGB(255, 255, 0, 0), D3DCOLOR_ARGB(255, 255, 0, 0));

	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		MenuOpen = !MenuOpen;
	}

	//etc etc...
	return oEndScene(Device);
}


HRESULT WINAPI Hooked_Present(LPDIRECT3DDEVICE9 Device, CONST RECT *pSrcRect, CONST RECT *pDestRect, HWND hDestWindow, CONST RGNDATA *pDirtyRegion) 
{

	if (xFont == NULL)
		D3DXCreateFont(Device, 14, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &xFont);

	if (!xLine)
		D3DXCreateLine(Device, &xLine);

	if (texRed == NULL) {
		GenerateTexture(Device, &texRed, D3DCOLOR_ARGB(255, 255, 0, 0));
	}

	WriteText(20, 20, D3DCOLOR_ARGB(255, 255, 0, 0), dhanax26); // TEXTO SLIDER

	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		MenuOpen = !MenuOpen;
	}

	return Original_Present(Device, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}


HRESULT __stdcall Reset_hook(IDirect3DDevice9* Device, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	if (xFont)
		xFont->OnLostDevice();

	if (xLine)
		xLine->OnLostDevice();

	HRESULT ResetReturn = Reset_orig(Device, pPresentationParameters);

	if (SUCCEEDED(ResetReturn))
	{

		if (xFont)
			xFont->OnResetDevice();

		if (xLine)
			xLine->OnResetDevice();

		Initialize = true;

	}

	return ResetReturn;
}

HRESULT WINAPI Hooked_DrawIndexedPrimitive(LPDIRECT3DDEVICE9 Device, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {

	void* ReturnAddress = _ReturnAddress();

	if (ReturnAddress == (void*)Player && bChams) {
		Device->SetRenderState(D3DRS_ZENABLE, FALSE);
		Device->SetTexture(0, texRed);
		Original_DrawIndexedPrimitive(Device, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
		Device->SetRenderState(D3DRS_ZENABLE, TRUE);
		Device->SetTexture(0, texRed);
	}

	return Original_DrawIndexedPrimitive(Device, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

DWORD WINAPI MainThread(LPVOID param) // Our main thread
{

	if (!Initialize)
	{
		//by dhanax26
		oEndScene = (f_EndScene)DetourFunction((PBYTE)GetDeviceAddress(42), (PBYTE)Hooked_EndScene);
		Reset_orig = (Reset_t)DetourFunction((PBYTE)GetDeviceAddress(16), (PBYTE)Reset_hook);
		SetTexture_orig = (SetTexture_t)DetourFunction((PBYTE)GetDeviceAddress(65), (PBYTE)SetTexture_hook);
		SetStreamSource_orig = (SetStreamSource_t)DetourFunction((PBYTE)GetDeviceAddress(100), (PBYTE)SetStreamSource_hook);

		//original
		Original_Present = (Prototype_Present)DetourFunction((PBYTE)GetDeviceAddress(17), (PBYTE)Hooked_Present);
		Original_DrawIndexedPrimitive = (Prototype_DrawIndexedPrimitive)DetourFunction((PBYTE)GetDeviceAddress(82), (PBYTE)Hooked_DrawIndexedPrimitive);
	}

	else
	{
		//by dhanax26
		DetourRemove((PBYTE)oEndScene, (PBYTE)Hooked_EndScene);
		DetourRemove((PBYTE)Reset_orig, (PBYTE)Reset_hook);
		DetourRemove((PBYTE)SetTexture_orig, (PBYTE)SetTexture_hook);
		DetourRemove((PBYTE)SetStreamSource_orig, (PBYTE)SetStreamSource_hook);

		//original
		DetourRemove((PBYTE)Original_Present, (PBYTE)Hooked_Present);
		DetourRemove((PBYTE)Original_DrawIndexedPrimitive, (PBYTE)Hooked_DrawIndexedPrimitive);
	}

	do {
		Sleep(100);
	} while (!(GetAsyncKeyState(VK_DELETE) & 0x1));

	Beep(220, 100);

	FreeLibraryAndExitThread(g_hModule, 0);//Detach By Dhanax26


	/*Or If You Want To Hook With MinHook...*/

/*	if (!Initialize)
	{
		//by dhanax26

		HMODULE dDll = NULL;
		while (!dDll)
		{
			dDll = GetModuleHandleA("d3d9.dll");
			Sleep(100);
		}
		CloseHandle(dDll);

		IDirect3D9* d3d = NULL;
		IDirect3DDevice9* d3ddev = NULL;
		HWND tmpWnd = CreateWindowA("BUTTON", "DX", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, Hand, NULL);
		if (tmpWnd == NULL)
		{
			return 0;
		}

		d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (d3d == NULL)
		{
			DestroyWindow(tmpWnd);
			return 0;
		}

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = tmpWnd;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

		HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
		if (result != D3D_OK)
		{
			d3d->Release();
			DestroyWindow(tmpWnd);
			return 0;
		}

		DWORD* dVtable = (DWORD*)d3ddev;

		for (int i = 0; i < 95; i++)
		{

		}

		if (MH_Initialize() != MH_OK) { return 1; }

		if (MH_CreateHook((DWORD_PTR*)GetDeviceAddress(42), &Hooked_EndScene, reinterpret_cast<void**>(&oEndScene)) != MH_OK) { return 1; }
		if (MH_EnableHook((DWORD_PTR*)GetDeviceAddress(42)) != MH_OK) { return 1; }

		if (MH_CreateHook((DWORD_PTR*)GetDeviceAddress(17), &Hooked_Present, reinterpret_cast<void**>(&Original_Present)) != MH_OK) { return 1; }
		if (MH_EnableHook((DWORD_PTR*)GetDeviceAddress(17)) != MH_OK) { return 1; }


		if (MH_CreateHook((DWORD_PTR*)GetDeviceAddress(82), &Hooked_DrawIndexedPrimitive, reinterpret_cast<void**>(&Original_DrawIndexedPrimitive)) != MH_OK) { return 1; }
		if (MH_EnableHook((DWORD_PTR*)GetDeviceAddress(82)) != MH_OK) { return 1; }

		if (MH_CreateHook((DWORD_PTR*)GetDeviceAddress(100), &SetStreamSource_hook, reinterpret_cast<void**>(&SetStreamSource_orig)) != MH_OK) { return 1; }
		if (MH_EnableHook((DWORD_PTR*)GetDeviceAddress(100)) != MH_OK) { return 1; }

		if (MH_CreateHook((DWORD_PTR*)GetDeviceAddress(65), &SetTexture_hook, reinterpret_cast<void**>(&SetTexture_orig)) != MH_OK) { return 1; }
		if (MH_EnableHook((DWORD_PTR*)GetDeviceAddress(65)) != MH_OK) { return 1; }

		if (MH_CreateHook((DWORD_PTR*)GetDeviceAddress(16), &Reset_hook, reinterpret_cast<void**>(&Reset_orig)) != MH_OK) { return 1; }
		if (MH_EnableHook((DWORD_PTR*)GetDeviceAddress(16)) != MH_OK) { return 1; }

		HMODULE mod = LoadLibrary(TEXT("Kernel32.dll"));

		d3ddev->Release();
		d3d->Release();
		DestroyWindow(tmpWnd);
	}*/

	return true;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, MainThread, hModule, 0, 0);
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}