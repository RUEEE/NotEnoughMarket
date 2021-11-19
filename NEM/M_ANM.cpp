#include "pch.h"
#include "M_ANM.h"
#include "Address.h"
#include "THFunction.h"
#include <d3d9.h>

int __fastcall DrawANM_Switch_Case28(DWORD thiz,AnmObj* pAnmObj)
{
	int renderFlag; // eax
	renderFlag = *pAnmObj->renderFlag();
	if ((renderFlag & 1) == 0 || (renderFlag & 2) == 0 || !*(BYTE*)((DWORD)pAnmObj + 0x527))
		return -1;
	if (*(DWORD*)(thiz + 51514988))
		DrawANM_inQueue(thiz);
	int v7; // ecx
	v7 = *(DWORD*)(*(DWORD*)(*((DWORD*)VALUED(0x51F65C) + *(DWORD*)((DWORD)pAnmObj + 32) + 12878283) + 284)
		+ 68 * *(DWORD*)((DWORD)pAnmObj + 36)+ 8);
	auto pdevice=VALUEV(0x4CCDF8, IDirect3DDevice9*);
	if (*(DWORD*)(thiz + 0x3120E04) != v7)
	{
		*(DWORD*)(thiz + 0x3120E04) = v7;
		pdevice->SetTexture(0,*(IDirect3DBaseTexture9**)(*(DWORD*)(*(DWORD*)(thiz + 4 * (v7 >> 8) + 0x312072C) + 0x124)+ 24 * (unsigned __int8)v7));
	}
	if (*(BYTE*)(thiz + 0x3120E0A) != 3)
	{
		pdevice->SetFVF(0x144);
		pdevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
		pdevice->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
		*(BYTE*)(thiz + 51514890) = 3;
	}
	Anm_SetRenderState(thiz, pAnmObj);
	if (*((BYTE*)VALUED(0x51F65C) + 0x3120E0F) != 1)
	{
		pdevice->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
		pdevice->SetTextureStageState(0, D3DTSS_COLOROP, 4);
		*((BYTE*)VALUED(0x51F65C) + 0x3120E0F) = 1;
	}

	UINT num = *pAnmObj->anm_globalVar_10000_10003(0);
	void* vertice_arr=*pAnmObj->p_verticeArr();
	//if (*pAnmObj->anm_globalVar_10000_10003(1) == 999999)
	//{
	//	pdevice->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 0, 0);//!!!
	//}
	pdevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, num, vertice_arr, 0x1C);
	return 0;
}

int __declspec(naked) DrawANM_Switch_InjectCall()
{
	__asm
	{
		mov ecx, esi//this
		mov edx, edi
		mov eax, DrawANM_Switch_Case28
		call eax
		pop edi
		pop esi
		mov ecx, [esp + 0x74]
		xor ecx, esp
		mov eax, 0x0048DC60
		call eax
		mov esp, ebp
		pop ebp
		ret 0x4
	}
}

void Inject_DrawANM_Switch()
{
	//the switch in 481294
	static BYTE jmpTable[29] = { 
		00,0x01,0x02,0x01,0x03,0x04,0x05,
		0x06,0x07,0x08,0x0E,0x09,0x08,0x08,
		0x08,0x0A,0x0B,0x0C,0x0C,0x0C,0x0B,
		0x0B,0x0B,0x0E,0x0D,0x0D,0x0B,0x0B,

		15
	};//original:28
	//add type28
	static DWORD jmpTable2[16] = 
	{ 0x0048129B,0x004812E8,0x004816B6,0x00481331,0x00481363,
		0x004813C2,0x004813F3,0x004815C0,0x00481638,0x00481662,
		0x004815F1,0x004816ED,0x00481A5C,0x0048168C,0x00481C73,
		
		(DWORD)DrawANM_Switch_InjectCall 
	};//original:15

	Address<BYTE>(0x00481286).SetValue(29);
	Address<DWORD>(0x00481290).SetValue((DWORD)jmpTable);
	Address<DWORD>(0x00481290).SetValue((DWORD)jmpTable);
	Address<DWORD>(0x00481297).SetValue((DWORD)jmpTable2);
	//use type 28 to draw CustomVertex1 with triangle list, num of point=[10000]*3
	//if [10001]==999999,the bg will be clear
}

void InjectANM()
{
	Inject_DrawANM_Switch();
}
