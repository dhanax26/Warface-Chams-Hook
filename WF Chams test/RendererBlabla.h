#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <d3d9.h>
#include "d3dx9.h"
#include <string>

#include <string.h>
#include <vector>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


//Disable Critical Warnings
#pragma warning (disable: 4244) 
#pragma warning (disable: 4996)
#define _CRT_SECURE_NO_DEPRECATE

//Internal Devices
HMODULE Hand;
LPDIRECT3DDEVICE9 npDevice;

/*ViewPort etc*/
D3DVIEWPORT9 Viewport;

//DX9Font
LPD3DXFONT xFont;

//For SnapLines...
LPD3DXLINE xLine;


//Options...
bool MenuOpen = false;


VOID FillRGB(INT x, INT y, INT w, INT h, D3DCOLOR Color, LPDIRECT3DDEVICE9 pDevice)
{
	D3DRECT rec = { x, y, x + w, y + h };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, Color, 0, 0);
}

void DrawBorder(IDirect3DDevice9 *pDevice, INT x, INT y, INT w, INT h, INT px, D3DCOLOR BorderColor)
{
	FillRGB(x, (y + h - px), w, px, BorderColor, pDevice);
	FillRGB(x, y, px, h, BorderColor, pDevice);
	FillRGB(x, y, w, px, BorderColor, pDevice);
	FillRGB(x + w - px, y, px, h, BorderColor, pDevice);
}

void DrawBox(IDirect3DDevice9 *pDevice, float x, float y, float w, float h, D3DCOLOR Color, D3DCOLOR BorderColor)
{
	DrawBorder(pDevice, x, y, w, h, 2, BorderColor);

	const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

	struct Vertex
	{
		float x, y, z, ht;
		DWORD Color;
	}
	V[4] = { { x, y + h, 0.0f, 0.0f, Color },{ x, y, 0.0f, 0.01f, Color },
	{ x + w, y + h, 0.0f, 0.0f, Color },{ x + w, y, 0.0f, 0.0f, Color } };
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->SetTexture(0, NULL);
	pDevice->SetPixelShader(0);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, D3DZB_TRUE);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_FOGENABLE, D3DZB_FALSE);

	pDevice->SetFVF(FVF);
	pDevice->SetTexture(0, NULL);
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, V, sizeof(Vertex));
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
}

void WriteText(int x, int y, DWORD color, char *text)
{
	RECT rect;
	SetRect(&rect, x, y, x, y);
	xFont->DrawTextA(0, text, -1, &rect, DT_NOCLIP | DT_LEFT, color);
}

HRESULT __stdcall DrawString(LPD3DXFONT pFont, INT X, INT Y, DWORD dColor, CONST PCHAR cString, ...)
{
	HRESULT hRet;

	if (pFont && X < Viewport.Width && Y < Viewport.Height)
	{
		CHAR buf[512] = { NULL };
		va_list ArgumentList;
		va_start(ArgumentList, cString);
		_vsnprintf_s(buf, sizeof(buf), sizeof(buf) - strlen(buf), cString, ArgumentList);
		va_end(ArgumentList);

		RECT rc[2];
		SetRect(&rc[0], X, Y, X, 0);
		SetRect(&rc[1], X, Y, X + 50, 50);

		hRet = D3D_OK;

		if (SUCCEEDED(hRet))
		{
			pFont->DrawTextA(NULL, buf, -1, &rc[0], DT_NOCLIP, 0xFF000000);
			hRet = pFont->DrawTextA(NULL, buf, -1, &rc[1], DT_NOCLIP, dColor);
		}
	}
	return hRet;
}