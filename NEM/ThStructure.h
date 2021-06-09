#pragma once

#include "..\imgui\imgui.h"
#include "..\imgui\imgui_impl_dx9.h"
#include "..\imgui\imgui_impl_win32.h"

#define VALUED(x) (*(DWORD*)(x))
#define VALUEF(x) (*(float*)(x))
#define VALUEV(x,T) (*(T*)(x))

#define PPLAYER VALUED(0x004CF410)
#define GAME_SPD VALUEF(VALUED(0x004B35C0))

enum Drop
{
	smallP = 1, gold, bigP, lifePeice, life, bombPeice, bomb, largeF, smallG, midG, largeG, bombPeice2
};
typedef ImVec2 vector2f;

//struct vector2f
//{
//	float x;
//	float y;
//};
struct vector3f
{
	float x;
	float y;
	float z;
};
struct TimeObj
{
	int time_last;
	int time_now;
	float time_now_f;
	int flag;
	void IncTime()
	{
		float gms = GAME_SPD;
		time_last = time_now;
		if (gms <=0.99f || gms>=1.01f)
		{
			time_now_f += gms;
			time_now = (int)time_now_f;
		}
		else
		{
			time_now_f++;
			time_now++;
		}
	}
};
struct EnemyInfo_init
{
	vector3f pos;
	float fC;
	Drop drop;
	int hp;
	int f18;
	int f1C;
	int loc_9985;
	int loc_9984;
	int loc_9983;
	int loc_9982;

	float loc_9981;
	float loc_9980;
	float loc_9979;
	float loc_9978;
};


void SetFlag(DWORD* flag_to_set, DWORD flag);
void CancelFlag(DWORD* flag_to_set, DWORD flag);