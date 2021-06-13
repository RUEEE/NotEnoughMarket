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
typedef DWORD HANM;

//struct vector2f
//{
//	float x;
//	float y;
//};
struct vector2i
{
	int x;
	int y;
};

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
struct AttackObj//total:0x9C
{
	DWORD flag;//+0
	vector2f att_radio;//+4,+8
	float angle;//+C
	int unk1;//+10
	vector2f att_box;//+14,18
	vector3f pos;//+1C,20,24
	int unk2[0xF];
	TimeObj time;//+64,68,6C,70
	int damage;//+74
	int damage_count;//+78
	int max_damage;//+7C
	int unk3[0x4];
	DWORD pShotAttObj;//+90
	int unk4[3];//+94,98,9C
	bool is_used() { return flag & 1; }
	bool is_round_hitbox() { return flag & 2; }
};
struct AbCardBase
{
	DWORD vtbl;//+0
	DWORD dw0;//+4
	DWORD id_ab;//+8
	DWORD num_ab;//+C
	DWORD unk1[3];//+10
	DWORD handle_anm;//+1C
	TimeObj now_passed_time;//+20
	DWORD unk2;//+30
	TimeObj cd_remain;//+34
	DWORD unk3;//+44
	int tot_cd;//+48
	DWORD unk4;//+4C
	DWORD cardFlag;//+50
	DWORD card_process_flag;//+54
	vector3f pos_use;
};
struct PlayerOp//F0
{
	DWORD flag;//+0
	DWORD gap4[20];
	vector2i i_pos_dest;//+54
	vector2i i_pos_now;//+5C
	vector2i i_highSpd_offs;//+64
	vector2i i_lowSpd_offs;//+6C
	DWORD gap74[13];
	float dwA8_angle;//+A8
	float dwAC_unk;
	HANM hanm;//+B0
	DWORD gapB4[7];
	int num_when_getAbCard;//+D0
	DWORD gapD4[4];
	DWORD dwordE4;
	void (*func_onTick)(PlayerOp*);//+E8
	DWORD dwEC;
};

struct ShotInfo//5C
{
	BYTE rate			;//+0发弹间隔(<=15)
	BYTE delay			;//+1发弹延迟(<=15)
	WORD damage			;//+2每发子弹的威力
	vector2f pos		;//+4发弹点相对坐标
	vector2f hitBox		;//+C子弹判定的长和宽
	float angle			;//+14发射方向,当>=1000.0则在playerOp的angle之上加入散射效果(.text 45E569)且速度随机为原速度+-1,在995.0-1000.0时采用PlayerOp的angle(+A8),虽然似乎一直为0(.text 45E646)
	float speed			;//+18发射速度
	DWORD unknown1		;//+1C fsl之后加入
	BYTE option			;//+20子机号,为0则尝试从其它位置发射(主炮/能力卡制造的子机)
	BYTE shotFlag		;
	//+21如果为7则代表能变成赤蛙的暴风伤害
	//为2则计算附加在哪个子机上时使用option值
	//如果为4,5,6则会不将射击时的子弹速度计入射击坐标(45E6F6)
	WORD ANM			;//+22自机弹贴图
	WORD SE				;//+24音效
	WORD unknown_th18	;//+26
	WORD unknown_th18_2	;//+28
	BYTE rate2			;//+2A rate2
	BYTE delay2			;//+2B delay2
	LPVOID func_init	;//+2C
	LPVOID func_tick	;//+30
	LPVOID func_draw	;//+34
	LPVOID func_hit		;//+38
	DWORD flags[8]		;//+3C
};

struct PlayerInfo
{
		WORD  maxPower				;//+0 默认4
		WORD  totalOffset			;//+2 最大offset数量
		float d_hitBox				;//+4 判定(无用)
		float d_powerAttractSpeed	;//+8 收p点的速度(无用)
		float d_powerAttractHitBox	;//+C 收p点的判定(高速)(无用)
		float normalSpeed			;//+10 高速速度
		float forcedSpeed			;//+14 低速速度
		float normalDiagonalSpeed	;//+18 斜向高速(指分配到xy的速度)
		float forcedDiagonalSpeed	;//+1C 斜向低速
		WORD  maxPowerLevel			;//+20 最大的子机数
		WORD  unknown1				;//+22 不知道
		DWORD powerVar				;//+24 power要修改的内容(40)
		DWORD maxDamage				;//+28 单帧子弹最高伤害
		DWORD unknown[5]			;//+2C 不知道
};


void SetFlag(DWORD* flag_to_set, DWORD flag);
void CancelFlag(DWORD* flag_to_set, DWORD flag);

void SetAllPlayerOpOnTickFunc();