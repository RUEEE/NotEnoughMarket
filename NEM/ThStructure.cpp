#include "pch.h"
#include "ThStructure.h"

AnmObj*(__cdecl* prob_new_anmObj)() = (AnmObj * (__cdecl*)()) (0x489320);
int(__thiscall* attach_anm)(DWORD pt_anm_file, AnmObj* thiz, int script_id)
= (int(__thiscall*)(DWORD, AnmObj*, int))(0x407420);
int(__thiscall* anm_ins_switch)(AnmObj* thiz)
= (int(__thiscall*)(AnmObj*))(0x478580);
HANM* (__stdcall* insertAnmObj)(HANM* _OUT_handle, AnmObj* anmObj)
= (HANM * (__stdcall*)(HANM*, AnmObj*))(0x488350);
void(__stdcall* setAnmToNextStep)(HANM handle)
= (void(__stdcall*)(HANM))(0x488CF0);
AnmObj* (__stdcall *anm_ins5_switch)(HANM hanm, int sw_case) = (AnmObj * (__stdcall *)(HANM hanm, int sw_cas))(0x488BE0);

int(__fastcall* delBullet)(DWORD pbullet, int a2, int type)
= (int(__fastcall*)(DWORD, int, int))(0x428E90);
//type==0:nothing,type=1:greenpoint,type==4:hammerhit

AnmObj* (__thiscall* getAnmObjFromHandle)(int anm_list, HANM handle)
= (AnmObj * (__thiscall*)(int, HANM))(0x488B40);

int(__thiscall* getEnmObjFromId)(int* pId)
= (int(__thiscall*)(int*))(0x0042E0C0);

unsigned int(__thiscall* getRndInt)(DWORD thiz)
= (unsigned int(__thiscall*)(DWORD thiz))0x402740;

unsigned int getRndInt_Rep()
{
	return getRndInt(0x004CF288);
}

float getRndFloat01_Rep()
{
	int i = 796917760;
	return ((double)getRndInt_Rep()) * VALUEF(&i);
}

float getRndFloat11_Rep()
{
	int i = 805306368;
	return ((double)getRndInt_Rep() * VALUEF(&i))-1.0f;
}


void erase_bullet_round(float radius, vector3f* pos, int dropType, int max_bullet_count, int a5)
{
	__asm
	{
		movss xmm2, radius
		push a5
		push max_bullet_count
		push dropType
		push pos;
		mov eax, 0x00429370;
		call eax;
	}
	return;
}

UINT32 erase_laser_round(float radius, vector3f* pos, int dropType, int is_not_forced_erase)
{
	UINT32 res=0;
	__asm
	{
		movss xmm2, radius
		push is_not_forced_erase
		push dropType
		push pos;
		mov eax, 0x00449010;
		call eax;
		mov res,eax
	}
	return res;
}


int summon_attack_circle(Player* pplayer, float radiox, float r_acc, vector3f* pos, int lifespan, int damage)//do not need to dec id
{
	int id_att=0;
	__asm
	{
		push damage;
		push lifespan;
		push pos;
		movss xmm3, r_acc;
		movss xmm2, radiox;
		mov ecx, pplayer;
		mov eax, 0x0045DE40;
		call eax;
		mov id_att, eax
	}
	return id_att - 1;
}


void SetFlag(DWORD* flag_to_set, DWORD flag)
{
	*flag_to_set |= flag;
}

void CancelFlag(DWORD* flag_to_set, DWORD flag)
{
	*flag_to_set &= ~flag;
}

void SetAllPlayerOpOnTickFunc()
{//fix 2un's playerOp
	if (PPLAYER)
	{
		for (int i = 5; i <= 16; i++)
		{
			VALUED(PPLAYER + 0x580 + 0xF0 * i + 0xE8) = 0;
		}
	}
}

int PlaySEWithChannel(float x, int id)
{
	int res{};
	_asm
	{
		movss xmm0,x
		push id
		mov eax, 0x476C70
		call eax
		mov res,eax
	}
	return res;
}

void SetInvincibleTime(int time)
{
	VALUED(PPLAYER + 0x47774) = time - 1;
	VALUED(PPLAYER + 0x47778) = time;
	VALUEF(PPLAYER + 0x4777C) =(float)(time);
	return;
}


int* getEnm_inCircle_attackable(float radius, int* out_id, vector2f* pos)
{
	int* ret = 0;
	__asm
	{
		push pos
		push out_id
		movss xmm3, radius
		mov eax, 0x438CB0
		call eax
		mov ret, eax
	}
	return ret;
}

DWORD Get_pEnemyFromId(int id)
{
	DWORD penm = *(DWORD*)(0x004CF2D0);
	if (penm)
	{
		DWORD iter = VALUED(penm + 0x18C);
		while (iter)
		{
			DWORD pEnm = VALUED(iter);
			{
				if (VALUED(pEnm + 0x6830) == id)
					return pEnm;
			}
			iter = VALUED(iter + 0x4);
		}
	}
	return 0;
}