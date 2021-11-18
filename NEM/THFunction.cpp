#include "THFunction.h"
int(__stdcall* PlaySE)(int id, int a2)= (int(__stdcall*)(int, int))(0x476BE0);

int(__thiscall* AddCard)(DWORD thiz, int ab_id, int a3)= (int(__thiscall*)(DWORD, int, int))(0x00411460);
void(__thiscall* ClearCard)(DWORD thiz, int a2)= (void(__thiscall*)(DWORD, int))(0x407DA0);

int(__thiscall* UpdateShot)(DWORD thiz, int a2)= (int(__thiscall*)(DWORD, int))(0x45D5E0);
int(__thiscall* DropItem)(DWORD thiz, Drop type, vector3f* pos, int a4, float dir, float spd, int delay, int a8, int a9)= (int(__thiscall*)(DWORD thiz, Drop type, vector3f * pos, int a4, float dir, float spd, int delay, int a8, int a9))(0x446F40);
BOOL(__thiscall* AddPower)(int thiz, int power_add) = (BOOL(__thiscall*)(int thiz, int power_add))(0x4573F0);
int(__stdcall* ShowPopupText)(vector2f* pos, int text, DWORD color) = (int(__stdcall*)(vector2f * pos, int text, DWORD color))(0x4645F0);
int(__thiscall* AddLifePeice)(int thiz, int a2) = (int(__thiscall*)(int thiz, int a2))(0x457570);
int(__thiscall* AddBomb)(int thiz, int a2) = (int(__thiscall*)(int thiz, int a2))(0x457690);
int(__thiscall* AddLife)(int thiz) = (int(__thiscall*)(int thiz))(0x4575F0);
void(_fastcall* SummonEnm)(int thiz, int edx, const char* name, EnemyInfo_init* info, int a) =(void(_fastcall*)(int thiz, int edx, const  char* name, EnemyInfo_init * info, int a))(0x0042D7D0);
DWORD(__fastcall* ShakeStage)(int type, int time, int fade_in, int fade_out, int a5, int a6) =(DWORD(__fastcall*)(int type, int time, int fade_in, int fade_out, int a5, int a6))(0x476060);


void(__cdecl* InitMemory)(void* ptmem, unsigned __int8 size, unsigned int val) =(void(__cdecl*)(void* ptmem, unsigned __int8 size, unsigned int val))(0x48F8F0);
LPVOID(__cdecl* AllocMemory)(DWORD sz) = (LPVOID(__cdecl*)(DWORD sz))(0x48DC71);
void(__thiscall* HidePlayerOpANM)(Player* pl) = (void(__thiscall*)(Player*))(0x416CD0);
DWORD* (__stdcall* InsertStageEff)(LPVOID eff) = (DWORD * (__stdcall*)(LPVOID eff))(0x401530);
int(__thiscall* SetMenuSelection)(StageMenu* thiz, int default_menu) = (int(__thiscall*)(StageMenu * thiz, int default_menu))(0x416BA0);

AnmObj* (__cdecl* AllocateAnmObj)() = (AnmObj * (__cdecl*)()) (0x489320);
int(__thiscall* AttachAnm)(DWORD pt_anm_file, AnmObj* thiz, int script_id)= (int(__thiscall*)(DWORD, AnmObj*, int))(0x407420);
int(__thiscall* AnmInsSwitch)(AnmObj* thiz)= (int(__thiscall*)(AnmObj*))(0x478580);
HANM* (__stdcall* InsertAnmObj)(HANM* _OUT_handle, AnmObj* anmObj)= (HANM * (__stdcall*)(HANM*, AnmObj*))(0x488350);
void(__stdcall* AnmReturn_byHANM)(HANM handle)= (void(__stdcall*)(HANM))(0x488CF0);
AnmObj* (__stdcall* AnmIns5_switch)(HANM hanm, int sw_case) = (AnmObj * (__stdcall*)(HANM hanm, int sw_cas))(0x488BE0);
void(__fastcall* DelBullet)(DWORD pbullet, int a2, int type)= (void(__fastcall*)(DWORD, int, int))(0x428E90);
AnmObj* (__thiscall* GetAnmObjFromHandle)(int anm_list, HANM handle)= (AnmObj * (__thiscall*)(int, HANM))(0x488B40);
Enemy*(__thiscall* GetEnmObjFromId)(int* pId)= (Enemy*(__thiscall*)(int*))(0x0042E0C0);
unsigned int(__thiscall* GetRndInt)(DWORD thiz)= (unsigned int(__thiscall*)(DWORD thiz))0x402740;

extern int(__thiscall* DrawANM_inQueue)(DWORD thiz)= (int(__thiscall *)(DWORD thiz))0x47E730;
extern int(__thiscall* Anm_SetRenderState)(int thiz, AnmObj* pAnmObj)= (int(__thiscall*)(int thiz, AnmObj * pAnmObj))0x47D910;



void EraseBullet_round(float radius, vector3f* pos, int dropType, int max_bullet_count, int a5)
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

UINT32 EraseLaser_round(float radius, vector3f* pos, int dropType, int is_not_forced_erase)
{
	UINT32 res = 0;
	__asm
	{
		movss xmm2, radius
		push is_not_forced_erase
		push dropType
		push pos;
		mov eax, 0x00449010;
		call eax;
		mov res, eax
	}
	return res;
}


int SummonAttackCircle(Player* pplayer, float radiox, float r_acc, vector3f* pos, int lifespan, int damage)
{
	int id_att = 0;
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

void SetAllPlayerOpOnTickFunc()//fix 2un's playerOp
{
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
		movss xmm0, x
		push id
		mov eax, 0x476C70
		call eax
		mov res, eax
	}
	return res;
}

Enemy* GetEnm_inCircle_attackable(float radius, int* out_id, vector2f* pos)
{
	int* retV = 0;
	__asm
	{
		push pos
		push out_id
		movss xmm3, radius
		mov eax, 0x438CB0
		call eax
		mov retV, eax
	}
	return (Enemy *)retV;
}

Enemy* GeEnmFromId(int id)
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
					return (Enemy*)pEnm;
			}
			iter = VALUED(iter + 0x4);
		}
	}
	return 0;
}


vector2f RotateVec(vector2f pt, vector2f mid, vector2f angle)
{
	vector2f dif{ pt.x - mid.x,pt.y - mid.y };
	float& cost = angle.x;
	float& sint = angle.y;
	return { mid.x + dif.x * cost - dif.y * sint,mid.y + dif.y * cost + dif.x * sint };
}

vector2f RotateVec(vector2f pt, vector2f angle)
{
	float& cost = angle.x;
	float& sint = angle.y;
	return { pt.x * cost - pt.y * sint,pt.y * cost + pt.x * sint };
}