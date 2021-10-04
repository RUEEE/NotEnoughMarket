#include "pch.h"
#include "M_Misc.h"

void (__cdecl* initMemory)(void* ptmem, unsigned __int8 size, unsigned int val)=
    (void(__cdecl *)(void* ptmem, unsigned __int8 size, unsigned int val))(0x48F8F0);
LPVOID (__cdecl* allocMemory)(DWORD sz)=(LPVOID(__cdecl * )(DWORD sz))(0x48DC71);
void (__thiscall* hide_PlayerOpANM)(Player* pl)=(void(__thiscall * )(Player*))(0x416CD0);

DWORD* (__stdcall* insert_StageEff)(LPVOID eff)=(DWORD * (__stdcall * )(LPVOID eff))(0x401530);

int (__thiscall * setMenuSelection)(StageMenu* thiz, int default_menu) = (int (__thiscall*)(StageMenu * thiz, int default_menu))(0x416BA0);

bool M_InitShop(Shop* sp,vector3f* pos)
{
    {
        DWORD dShop = (DWORD)sp;

        auto v3 = (int)insert_StageEff((LPVOID)0x417CC0);
        VALUED(v3 + 4) |= 2u;
        VALUED(v3 + 36) = dShop;

        int(__stdcall * sub_401180)(DWORD a1, int a2) = (int(__stdcall*)(DWORD a1, int a2))(0x401180);

        sub_401180(v3, 12);
        VALUED(dShop + 4) = v3;
        auto v4 = (int)insert_StageEff((LPVOID)0x418C30);
        VALUED(v4 + 4) |= 2u;
        VALUED(v4 + 36) = dShop;

        int(__stdcall * sub_401230)(DWORD a1, int a2) = (int(__stdcall*)(DWORD a1, int a2))(0x401230);
        sub_401230(v4, 81);

        auto v5 = VALUED(dShop + 4);
        VALUED(dShop + 8) = v4;
        VALUED(v5 + 4) |= 2u;
        VALUED(VALUED(dShop + 8) + 4) |= 2u;
        auto v6 = VALUED(dShop + 460);
        if ((v6 & 1) == 0)
        {
            VALUED(dShop + 0x1C8) = 0;
            VALUED(dShop + 460) = v6 | 1;
        }
        VALUED(dShop + 0x1C0) = 0;
        VALUED(dShop + 0x1C4) = 0;
        VALUED(dShop + 0x1BC) = -1;
        auto v7 = VALUED(dShop + 516);
        if ((v7 & 1) == 0)
        {
            VALUED(dShop + 512) = 0;
            VALUED(dShop + 516) = v7 | 1;
        }
        VALUED(dShop + 504) = 0;
        VALUED(dShop + 508) = 0;
        VALUED(dShop + 500) = -1;
        auto v8 = VALUED(dShop + 536);
        if ((v8 & 1) == 0)
        {
            VALUED(dShop + 532) = 0;
            VALUED(dShop + 536) = v8 | 1;
        }
        VALUED(dShop + 524) = 0;
        VALUED(dShop + 528) = 0;
        VALUED(dShop + 520) = -1;
        *(vector3f*)(dShop + 464) = *pos;
        VALUED(dShop + 20) = 2;
        
        VALUED(dShop + 0xDC) = 1;
    }
    
    for (UINT i = 0; i < 56; i++)
    {
        *sp->abCardInfo(i) = GetpCardInfoFromId(i);
        *sp->num_abCard()+=1;
    }
    *(sp->menu()->max_Selectable())= *sp->num_abCard();
    setMenuSelection(sp->menu(), 0);
    return 0;
}

P_AbCardShopInfo GetpCardInfoFromId(int id)
{
    for (UINT i = 0; i < 56; i++)
    {
        if (VALUED(i * 0x34 + 0x4C53C4) == id)
        {
            return (P_AbCardShopInfo)(i * 0x34 + 0x4C53C0);
        }
    }
    return (P_AbCardShopInfo)(VALUED(0x4C5F20));
}

bool IsCardEquipped(int id)
{
    DWORD cardIt = VALUED(VALUED(0x4CF298)+ 0x1C);
    if (cardIt)
    {
        while (VALUED(VALUED(cardIt)+ 4) != id)
        {
            cardIt = VALUED(cardIt+4);
            if (!cardIt)
                return false;
        }
        return true;
    }
    return false;
}

void M_OpenShop()
{
    vector3f v = {448,32,0};
    DWORD shop = (DWORD)allocMemory(0xE3C);
    VALUED(shop + 0x98) = 0;
    VALUED(shop + 0xC) = 0;
    VALUED(shop + 0xE0) = 0;
    VALUED(shop + 0xDC) = 1;
    VALUED(shop + 0x14) = 999;
    VALUED(shop + 0x170) = 0;
    VALUED(shop + 0xE4) = 0;
    VALUED(shop + 0x1B8) = 0;
    VALUED(shop + 0x1B4) = 1;
    VALUED(shop + 0xEC) = 999;
    VALUED(shop + 0x1CC) &= 0xFFFFFFFE;
    VALUED(shop + 0x204) &= 0xFFFFFFFE;
    VALUED(shop + 0x218) &= 0xFFFFFFFE;
    VALUED(shop + 0x228) = 0;
    memset((void*)(shop + 0x22C), 0, 0x400u);
    memset((void*)(shop + 0x62C), 0, 0x400u);
    initMemory((void*)shop, 0, 0xE3Cu);
    VALUED(0x4CF2A4) = shop;
    VALUED(shop) |= 2u;
    if (PPLAYER2)
        hide_PlayerOpANM(PPLAYER2);
    if (M_InitShop((Shop*)shop, &v))
    {
        int* (__thiscall * sub_4176E0)(int thiz)=0;
        VALUED(&sub_4176E0) = 0x4176E0;
        sub_4176E0(shop);
    }
}

void InjectShop()
{

}