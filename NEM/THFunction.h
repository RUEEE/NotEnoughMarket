#pragma once
#pragma warning(disable:4244)//float<->int
#pragma warning(disable:4305)//float<->double
#include <Windows.h>
#include <functional>
#include "ThStructure.h"
#include "THFunction.h"
extern LPVOID(__cdecl* AllocMemory)(DWORD sz);
extern void(__cdecl* InitMemory)(void* ptmem, unsigned __int8 size, unsigned int val);

extern int(__stdcall* ShowPopupText)(vector2f* pos, int text, DWORD color);

extern int(__stdcall* PlaySE)(int id, int a2);
extern int(__thiscall* AddCard)(DWORD thiz, int ab_id, int a3);
extern void(__thiscall* ClearCard)(DWORD thiz, int a2);
extern int(__thiscall* UpdateShot)(DWORD thiz, int a2);
extern BOOL(__thiscall* AddPower)(int thiz, int power_add);

extern int(__thiscall* AddLifePeice)(int thiz, int a2);
extern int(__thiscall* AddBomb)(int thiz, int a2);
extern int(__thiscall* AddLife)(int thiz);

extern void(_fastcall* SummonEnm)(int thiz, int edx, const char* name, EnemyInfo_init* info, int a);
extern Enemy* (__thiscall* GetEnmObjFromId)(int* pId);


extern int(__thiscall* SetMenuSelection)(StageMenu* thiz, int default_menu);
extern DWORD* (__stdcall* InsertStageEff)(LPVOID eff);
extern DWORD(__fastcall* ShakeStage)(int type, int time, int fade_in, int fade_out, int a5, int a6);
extern void(__thiscall* HidePlayerOpANM)(Player* pl);

extern int(__thiscall* DropItem)(DWORD thiz, Drop type, vector3f* pos, int a4, float dir, float spd, int delay, int a8, int a9);
extern void(__fastcall* DelBullet)(DWORD pbullet, int a2, int type);


extern AnmObj* (__cdecl* AllocateAnmObj)();
extern int(__thiscall* AttachAnm)(DWORD pt_anm_file, AnmObj* thiz, int script_id);
extern int(__thiscall* AnmInsSwitch)(AnmObj* thiz);
extern HANM* (__stdcall* InsertAnmObj)(HANM* _OUT_handle, AnmObj* anmObj);
extern void(__stdcall* AnmReturn_byHANM)(HANM handle);
extern AnmObj* (__stdcall* AnmIns5_switch)(HANM hanm, int sw_case);
extern AnmObj* (__thiscall* GetAnmObjFromHandle)(int anm_list, HANM handle);


extern unsigned int(__thiscall* GetRndInt)(DWORD thiz);

extern int(__thiscall* DrawANM_inQueue)(DWORD thiz);
extern int(__thiscall* Anm_SetRenderState)(int thiz, AnmObj* pAnmObj);

extern int* (__thiscall* ECL_GetPtArgI_Th)(ECL_ThreadObj* thiz, UINT n);
extern float* (__thiscall* ECL_GetPtArgF_Th)(ECL_ThreadObj* thiz, UINT n);
extern int(__thiscall* ECL_GetArgI_Th)(ECL_ThreadObj* thiz, UINT n);
float inline ECL_GetArgF_Th(ECL_ThreadObj* thiz, UINT n)
{
	float retV=-1;
	__asm
	{
		mov ecx,thiz
		push n
		mov eax, 0x0048D5A0
		call eax
		movss retV,xmm0
	}
	return retV;
}
float inline ECL_GetGlobalArgF(Enemy* thiz, INT32 n)
{
	float retV = -1;
	__asm
	{
		mov ecx, thiz
		push n
		mov eax, 0x00437C90
		call eax
		fstp retV
	}
	return retV;
}
//call with [this] filled
inline AnmObj* GetAnmObjFromHandle_F(HANM hanm)
		{return  GetAnmObjFromHandle(VALUED(0x51F65C), hanm);}
inline int DropItem_F(Drop type, vector3f* pos, int a4, float dir, float spd, int delay, int a8, int a9)
		{return DropItem(VALUED(0x004CF2EC), type, pos,a4,dir, spd,delay,a8,a9);}
inline void UpdateShot_F() { UpdateShot(PPLAYER + 0x620, 0); }
inline void AddBomb_F(){AddBomb(0x4CCCDC, 0); }
inline void AddLifePeice_F(){AddLifePeice(0x4CCCDC, 0); }
inline void AddLife_F(){AddLife(0x4CCCDC); }


//quick call
constexpr float ToRad(float f) { return f / 180.0f * PI; }
constexpr float ToDeg(float f) { return f * 180.0f / PI; }

void inline SetInvincibleTime(int time)
		{VALUED(PPLAYER + 0x47774) = time - 1;VALUED(PPLAYER + 0x47778) = time;VALUEF(PPLAYER + 0x4777C) = (float)(time);}
UINT32 inline GetRndInt_Rep()
		{return GetRndInt(0x004CF288);}
float inline GetRndFloat01_Rep()
		{int i = 796917760;return ((double)GetRndInt_Rep()) * VALUEF(&i);}
float inline GetRndFloat11_Rep()
		{int i = 805306368;return ((double)GetRndInt_Rep() * VALUEF(&i)) - 1.0f;}
float inline GetRndFloat11_Seed(int seed[2])
{
	int i = 805306368; return ((double)GetRndInt((DWORD)seed) * VALUEF(&i)) - 1.0f;
}
Enemy* GeEnmFromId(int id);
bool IsCardEquipped(UINT id);

vector2f RotateVec(vector2f pt, vector2f mid, vector2f angle);
vector2f RotateVec(vector2f pt, vector2f angle);
vector3f inline ToVec3f(vector2f p) { return { p.x,p.y,0.0f }; }

//functions with ASM
int PlaySEWithChannel(float x, int id);
void EraseBullet_round(float radius, vector3f* pos, int dropType, int max_bullet_count, int a5);
UINT32 EraseLaser_round(float radius, vector3f* pos, int dropType, int is_not_forced_erase);
int SummonAttackCircle(Player* pplayer, float radiox, float r_acc, vector3f* pos, int lifespan, int damage);//do not need to dec id
Enemy* GetEnm_inCircle_attackable(float radius, int* out_id, vector2f* pos);


//fix bug
void SetAllPlayerOpOnTickFunc();


//iterators
typedef DWORD EnemyIter;

inline EnemyIter EnemyIterBegin() {
	DWORD enmBase = *(DWORD*)(0x004CF2D0);
	if (enmBase)
	{
		DWORD iter = VALUED(enmBase + 0x18C);
		return iter;
	}
	return 0;
};
inline EnemyIter EnemyIterNext(EnemyIter& it)
{
	if (it == 0)
		return 0;
	return it=VALUED((DWORD)it + 0x4);
}
inline Enemy* EnemyIterDeRef(EnemyIter it) { 
	if (it == 0)
		return nullptr;
	return VALUEV(it, Enemy*); 
}

//ForAll
inline void For_AllBullets(std::function<void (Bullet*,int)> func)//the pointer of bullet and the index for bullet
{
	int i = 0;
	DWORD pbtx = VALUED(0x004CF2BC);
	if (pbtx)
	{
		DWORD iter = VALUED(pbtx + 0xC0);
		while (iter)
		{
			Bullet* pbt = VALUEV(iter,Bullet*);
			func(pbt, i);
			i++;
			iter = VALUED(iter + 0x4);
		}
	}
}

inline void For_AllEnemies(std::function<void(Enemy*, int)> func)
{
	DWORD enmBase = *(DWORD*)(0x004CF2D0);
	if (enmBase)
	{
		int i = 0;
		DWORD iter = VALUED(enmBase + 0x18C);
		while (iter)
		{
			Enemy* pEnm = VALUEV(iter,Enemy*);
			func(pEnm,i);
			i++;
			iter = VALUED(iter + 0x4);
		}
	}
}