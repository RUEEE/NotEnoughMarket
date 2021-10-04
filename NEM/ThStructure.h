#pragma once

#include "..\imgui\imgui.h"
#include "..\imgui\imgui_impl_dx9.h"
#include "..\imgui\imgui_impl_win32.h"

#define VALUED(x) (*(DWORD*)(x))
#define VALUEF(x) (*(float*)(x))
#define VALUEV(x,T) (*(T*)(x))

#define DIST_VEC2(a,b) hypotf(a.x-b.x,a.y-b.y)

//the bullet defs
#define PPLAYER VALUED(0x004CF410)
#define PPLAYER2 VALUEV(0x004CF410,Player*)
#define POWER VALUED(0x004CCD38)
#define GAME_SPD VALUEF(VALUED(0x004B35C0))
#define CARDCD_SCALE VALUEF(VALUED(0x4CF298)+0xC58)
#define IS_ABLE_COLDDOWN_CARD (VALUED(0x4CF2E0)&& !VALUED(VALUED(0x4CF2E0) + 0x1B0)&&VALUED(0x4CF2D0)&&VALUED(VALUED(0x4CF2D0) + 0x198))

constexpr auto PI = 3.141592653589793;

enum class Drop
{
	smallP = 1, gold, bigP, lifePeice, life, bombPeice, bomb, largeF, smallG, midG, largeG, bombPeice2
};

typedef ImVec2 vector2f;
typedef DWORD HANM;

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
	void DecTime()
	{
		float gms = GAME_SPD;
		time_last = time_now;
		if (gms <= 0.99f || gms >= 1.01f)
		{
			time_now_f -= gms;
			time_now = (int)time_now_f;
		}
		else
		{
			time_now_f--;
			time_now--;
		}
	}
};

struct EnemyInfo_init
{
	vector3f pos;
	float fC;
	Drop drop;
	int hp;
	DWORD flag_Rsh_19;//enm.flag=(flag << 19)) & 0x80000
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
	float att_radio;//+4
	float att_radio_acc;//+8
	float angle;//+C
	int unk1;//+10
	vector2f att_box;//+14,18
	vector3f pos;//+1C,20,24
	int unk2[0xF];
	TimeObj time;//+64,68,6C,70
	int damage;//+74
	int damage_count;//+78
	int max_damage;//+7C
	int rate;//+80
	int unk3[0x3];
	DWORD pShotAttObj;//+90
	int unk4[3];//+94,98,9C
	bool is_used() { return flag & 1; }
	bool is_round_hitbox() { return flag & 2; }
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
	BYTE rate			;//+0�������(<=15)
	BYTE delay			;//+1�����ӳ�(<=15)
	WORD damage			;//+2ÿ���ӵ�������
	vector2f pos		;//+4�������������
	vector2f hitBox		;//+C�ӵ��ж��ĳ��Ϳ�
	float angle			;//+14���䷽��,��>=1000.0����playerOp��angle֮�ϼ���ɢ��Ч��(.text 45E569)���ٶ����Ϊԭ�ٶ�+-1,��995.0-1000.0ʱ����PlayerOp��angle(+A8),��Ȼ�ƺ�һֱΪ0(.text 45E646)
	float speed			;//+18�����ٶ�
	DWORD unknown1		;//+1C fsl֮�����
	BYTE option			;//+20�ӻ���,Ϊ0���Դ�����λ�÷���(����/������������ӻ�)
	BYTE shotFlag		;
	//+21���Ϊ7������ܱ�ɳ��ܵı����˺�
	//Ϊ2����㸽�����ĸ��ӻ���ʱʹ��optionֵ
	//���Ϊ4,5,6��᲻�����ʱ���ӵ��ٶȼ����������(45E6F6)
	WORD ANM			;//+22�Ի�����ͼ
	WORD SE				;//+24��Ч
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
	WORD  maxPower				;//+0 Ĭ��4
	WORD  totalOffset			;//+2 ���offset����
	float d_hitBox				;//+4 �ж�(����)
	float d_powerAttractSpeed	;//+8 ��p����ٶ�(����)
	float d_powerAttractHitBox	;//+C ��p����ж�(����)(����)
	float normalSpeed			;//+10 �����ٶ�
	float forcedSpeed			;//+14 �����ٶ�
	float normalDiagonalSpeed	;//+18 б�����(ָ���䵽xy���ٶ�)
	float forcedDiagonalSpeed	;//+1C б�����
	WORD  maxPowerLevel			;//+20 �����ӻ���
	WORD  unknown1				;//+22 ��֪��
	DWORD powerVar				;//+24 powerҪ�޸ĵ�����(40)
	DWORD maxDamage				;//+28 ��֡�ӵ�����˺�
	DWORD unknown[5]			;//+2C ��֪��
};

struct Custom_vertex1
{
	vector3f pos;
	float rhw;
	DWORD spec;
	vector2f uv;
};
#define FVF_VERTEX_1 (D3DFVF_TEX1|D3DFVF_DIFFUSE|D3DFVF_XYZRHW)

struct Custom_vertex2
{
	vector3f pos;
	float rhw;
	DWORD spec;
};
#define FVF_VERTEX_2 (D3DFVF_DIFFUSE|D3DFVF_XYZRHW)
struct Custom_vertex3
{
	vector3f pos;
	DWORD spec;
	vector2f uv;
};

struct AsciiToRender
{
	char context[256];
	vector3f pos;
	DWORD color;
	vector3f scale;
	DWORD dword;
	DWORD type_ascii;
	int has_shade_flag;
	int prob_layer;
	int lifespan;
	DWORD x_Posflag_130;
	DWORD y_Posflag_134;
};

#define FVF_VERTEX_3 (D3DFVF_TEX1|D3DFVF_DIFFUSE|D3DFVF_XYZ)

void SetFlag(DWORD* flag_to_set, DWORD flag);
void CancelFlag(DWORD* flag_to_set, DWORD flag);

void SetAllPlayerOpOnTickFunc();
int PlaySEWithChannel(float x,int id);
void SetInvincibleTime(int time);
void erase_bullet_round(float radius, vector3f* pos, int dropType, int max_bullet_count, int a5);
UINT32 erase_laser_round(float radius, vector3f* pos, int dropType, int is_not_forced_erase);
struct Player;
int summon_attack_circle(Player* pplayer, float radiox, float r_acc, vector3f* pos, int lifespan, int damage);
unsigned int getRndInt_Rep();
float getRndFloat11_Rep();
float getRndFloat01_Rep();
int* getEnm_inCircle_attackable(float radius, int* out_id, vector2f* pos);
DWORD Get_pEnemyFromId(int id);


#define STRUCT_NAME ShotAttObj
#define STRUCT_BODY \
STYPE(0x8C,bool,is_used)\
STYPE(0x9C,UINT32,damage)\
STYPE(0xA0,vector2f,hitBox)\
STYPE(0xAC,UINT16,shotGroupId_shotId)\
STYPE(0xB0,UINT32,hitIndex_add1)\
STYPE(0xE8,LPVOID,funcArr)\

#include "StructGenerator.h"


#define STRUCT_NAME Player
#define STRUCT_BODY \
STYPE(0x62C,vector2i,iPosition_fixed)\
STYPE(0x620,vector3f,fPosition)\
STYPE(0x634,TimeObj,nowTime)\
ARRTYPESZ(0x580,PlayerOp,playerOpArr,0xF0)\
ARRTYPESZ(0x1570,ShotAttObj,shotAttObjArr,0xF8)\
ARRTYPESZ(0x20574,AttackObj,attObjArr,0x9C)\
STYPE(0x476AC,UINT32,plState)\
STYPE(0x47800,vector2i,iPosition_prev)\
STYPE(0x477C4,vector2f,fVelocity_fixed)\

#include "StructGenerator.h"


#define STRUCT_NAME Bullet
#define STRUCT_BODY \
STYPE(0x28,DWORD,pAnm)\
STYPE(0x638,vector3f,position)\
STYPE(0x644,vector3f,velocity)\
STYPE(0x658,vector2f,hitbox)\
STYPE(0xF80,TimeObj,nowTime)\
STYPE(0x654,float,anmAngle)\
STYPE(0xF98,UINT16,type)\
STYPE(0xF9A,UINT16,color)\

#include "StructGenerator.h"

#define STRUCT_NAME Enemy
#define STRUCT_BODY \
STYPE(0x1270,vector2f,positionFin)\
STYPE(0x12B4,vector2f,positionAbs)\
STYPE(0x12F8,vector2f,positionRel)\
STYPE(0x133C,vector2f,hitbox_hurt)\
STYPE(0x1344,vector2f,hitbox_kill)\
STYPE(0x134C,float,hitboxAngle)\
STYPE(0x14EC,UINT32,time_loc9988)\
STYPE(0x6220,UINT32,nowHp)\
STYPE(0x6224,UINT32,totHp)\
STYPE(0x623C,Drop,basicDrop)\
ARRTYPESZ(0x623C,Drop,dropNum,0x4)\
STYPE(0x62F8,vector2f,dropArea)\
STYPE(0x635C,DWORD,flag)\

#include "StructGenerator.h"


#define STRUCT_NAME AnmObj
#define STRUCT_BODY \
STYPE(0x18,DWORD,layer)\
STYPE(0x24,DWORD,spriteId)\
ARRTYPESZ(0x30,float,anmGlobalVarFloat,0x4)\
STYPE(0x3C,vector3f,angle)\
STYPE(0x54,vector2f,scaleRel)\
STYPE(0x6C,vector2f,uv_scale)\
STYPE(0x74,vector2f,scaleAbs)\
STYPE(0x7C,vector2f,uv_trans)\
STYPE(0x3CC,vector2f,uv_moveVelocity)\
STYPE(0x524,DWORD,color1)\
STYPE(0x528,DWORD,color2)\
STYPE(0x534,DWORD,renderFlag)\
STYPE(0x538,DWORD,layerFlag)\
STYPE(0x544,HANM,handle)\
STYPE(0x560,TimeObj,time)\
STYPE(0x5F0,vector3f,pos)\

#include "StructGenerator.h"



#define STRUCT_NAME StageMenu
#define STRUCT_BODY \
STYPE(0x0,DWORD,id_nowSel)\
STYPE(0x4,DWORD,id_nowSel2)\
STYPE(0x8,DWORD,max_Selectable)\

#include "StructGenerator.h"


typedef DWORD* P_AbCardShopInfo;
#define STRUCT_NAME Shop
#define STRUCT_BODY \
STYPE(0xE4,StageMenu,menu)\
ARRTYPESZ(0x22C,HANM,disPlayCardHanm,0x4)\
STYPE(0xA2C,int,num_abCard)\
ARRTYPESZ(0xA30,P_AbCardShopInfo,abCardInfo,0x4)\
STYPE(0xE38,DWORD,state)\

#include "StructGenerator.h"

