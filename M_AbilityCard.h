#pragma once
#include "ThStructure.h"
void InjectAbCard();
int __fastcall M_AddCard(DWORD thiz, DWORD edx, int ab_id, UINT getType);


inline void M_CardVtbl(AbCardBase* card,DWORD cardVtblBaseAddr,DWORD offs_func)
{
	VALUED(cardVtblBaseAddr)=VALUED((*(BYTE*)card)+offs_func);
}