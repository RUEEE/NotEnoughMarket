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

#define ANM_VERTICEARR_OFFSET VALUEV(0x0056ACBC,vector2i)
#define BACKGROUND_ANMOBJ VALUEV(0x004CCFAC,AnmObj*)
#define STAGE_BACKGROUND_ANMOBJ VALUEV(0x004CCFB0,AnmObj*)
#define STAGE_ANMOBJ VALUEV(0x004CCFB4,AnmObj*)
#define STAGE_FINAL_ANMOBJ VALUEV(0x004CCFB4,AnmObj*)

#define ANM_TOWIND_SCALE VALUEF(0x0056ACA0) // when 1280x960 it's 2

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

struct CardInShopInfo
{
	//4C53C4(+4)	第n号卡的卡面id(商店卡号不等于卡面id)
	//4C53D0(+10)	(int) floor(价格 / 30)
	//4C53D4(+14)	投放入卡池的数量, 若为0则为商店必出卡
	//4C53D8(+18)	为n则第n面必出现(? ), 受sub 416E10控制
	//; bool 416E10(CardInShop*)返回1则为必出卡, 2则为特有卡
	//4C53DC(+1C)	一次性获得卡(不显示在右侧的卡)为1
	const char* cardName;//+0
	UINT cardId;//+4
	DWORD unk_a8;//+8
	DWORD cardType;//+C
	DWORD price_div30_floor;//+10
	UINT numInPool;//+14
	UINT prob_stage_to_get = 0;//+18
	UINT is_immediateCard;//+1C
	DWORD isableEquipInMenu;
	DWORD unk_a24;
	DWORD unk_a28;
	DWORD spriteId_memu;
	DWORD spriteId_stage;
	DWORD unk_a34;
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

struct ShotAttObj;
class AbCardBase
{
public:
	AbCardBase()
	{
		this->hanm = 0;
		this->unk5_1 &= 0xFFFFFFFE;
		this->unk6_1 &= 0xFFFFFFFE;
		memset(this, 0x0, 0x54);
		id = -1;
		pThis = this;
		pNext = pPrev = nullptr;
		unk4 = 0;
		unk6_1 = 1;
	}
public:
	virtual int Init() { return 0; };//+0
	virtual int Init2() { return 0; };//+4
	virtual int __fastcall OnActivate(int a2) { return 0; };//+8
	virtual int OnDying(int a2) { return 0; };//+C
	virtual int OnDieBomb() { return 0; };//+10
	virtual int OnDied() { return 0; };//+14
	virtual int OnGetOp() { return 0; };//+18
	virtual int OnPressZ(int time_pressZ_mod_15, int time_pressZ_mod_120) { return 0; };//+1C
	virtual int Init3() { return 0; };//+20
	virtual int OnEnterFrame() { return 0; };//+24
	virtual int OnShotOp(ShotAttObj* shotAttobj) { return 0; };//+28
	virtual int OnEnterFrame2() { return 0; };//+2C
	virtual int OnDrop(vector2f* pos, UINT dropArray[]) { return 0; };//+30
	virtual int Unk3() { return 0; };//+34
	virtual int SetCDRemain(int time_remain) {
		int v2; // eax
		v2 = this->unk6_1;
		if ((v2 & 1) == 0) {
			this->timeCD.flag = 0; this->unk6_1 = v2 | 1;
		}
		this->timeCD.time_now = time_remain;
		this->timeCD.time_last = time_remain - 1;
		this->timeCD.time_now_f = (float)time_remain;
		return 0;
	};//+38
	virtual UINT32 GetCD() { return this->totCD; };//+3C
	virtual UINT IsOpCard() { return (this->cardTypeFlag >> 5) & 1; };//+40
	virtual int OnChangeStage(HANM hanm) { return 0; };//+44
	virtual int OnRendering() { return 0; };//+48
	virtual int Unk8() { return 0; };//+4C
	virtual ~AbCardBase() = default;//+50
public:
	//LPVOID VPTR +0
	UINT32 id;//+4
	UINT32 count;//+8
	AbCardBase* pThis;//+C
	AbCardBase* pNext;//+10
	AbCardBase* pPrev;//+14
	UINT32 unk4;//+18
	HANM hanm;//+1C
	TimeObj timePassed;//+20,24,28,2C
	UINT32 unk5_1;//+30    always 1
	TimeObj timeCD;//+34,38,3C,40
	UINT32 unk6_1;//+44    always 1
	UINT32 totCD;//+48
	CardInShopInfo* pCardInfo;//+4C
	UINT32 cardTypeFlag;//+50
	//sizeof(AbCardBase)=0x54
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
ARRTYPESZ(0x47800,vector2i,iPosition_prev,0x8)\
STYPE(0x477C4,vector2f,fVelocity_fixed)\
STYPE(0x4799C,float,hitbox_highspeed)\
STYPE(0x479A0,float,hitbox_focused)\

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
STYPE(0x6830,DWORD,id)\

#include "StructGenerator.h"


#define STRUCT_NAME AnmObj
#define STRUCT_BODY \
STYPE(0x18,DWORD,layer)\
STYPE(0x24,DWORD,spriteId)\
ARRTYPESZ(0x30,float,anmGlobalVarFloat,0x4)\
STYPE(0x30,vector3f,posAbs)\
STYPE(0x3C,vector3f,rotation)\
STYPE(0x54,vector2f,scaleRel)\
STYPE(0x6C,vector2f,uv_scale)\
STYPE(0x74,vector2f,scaleAbs)\
STYPE(0x7C,vector2f,uv_trans)\
STYPE(0x3CC,vector2f,uv_moveVelocity)\
ARRTYPESZ(0x4A4,INT32,anm_globalVar_10000_10003,0x4)\
ARRTYPESZ(0x4B4,float,anm_globalVar_10004_10007,0x4)\
STYPE(0x524,DWORD,color1)\
STYPE(0x528,DWORD,color2)\
STYPE(0x534,DWORD,renderFlag)\
STYPE(0x538,DWORD,layerFlag)\
STYPE(0x544,HANM,handle)\
STYPE(0x560,TimeObj,time)\
STYPE(0x5F0,vector3f,pos)\
STYPE(0x5C8,void*,p_verticeArr)\
STYPE(0x5CC,UINT,verticeNum)\
STYPE(0x4E4,vector3f,pos2)\

#include "StructGenerator.h"



#define STRUCT_NAME StageMenu
#define STRUCT_BODY \
STYPE(0x0,DWORD,id_nowSel)\
STYPE(0x4,DWORD,id_nowSel2)\
STYPE(0x8,DWORD,max_Selectable)\

#include "StructGenerator.h"


#define STRUCT_NAME Shop
#define STRUCT_BODY \
STYPE(0xE4,StageMenu,menu)\
ARRTYPESZ(0x22C,HANM,disPlayCardHanm,0x4)\
STYPE(0xA2C,int,num_abCard)\
ARRTYPESZ(0xA30,CardInShopInfo*,abCardInfo,0x4)\
STYPE(0xE38,DWORD,state)\

#include "StructGenerator.h"
