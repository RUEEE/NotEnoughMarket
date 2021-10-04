#pragma once
#include "ThStructure.h"
void InjectAbCard();
class AbCardBase
{
public:
	virtual int Init() { return 0; };//+0
	virtual int Init2(){ return 0; };//+4
	virtual int __fastcall OnActivate(int a2){ return 0; };//+8
	virtual int OnDying(int a2){ return 0; };//+C
	virtual int Unk1(){ return 0; };//+10
	virtual int OnDied(){ return 0; };//+14
	virtual int OnGetOp(){ return 0; };//+18
	virtual int OnPressZ(int time_pressZ_mod_15, int time_pressZ_mod_120){ return 0; };//+1C
	virtual int Init3(){ return 0; };//+20
	virtual int OnEnterFrame(){ return 0; };//+24
	virtual int OnShotOp(ShotAttObj* shotAttobj){ return 0; };//+28
	virtual int OnEnterFrame2(){ return 0; };//+2C
	virtual int Unk2(){return 0; };//+30
	virtual int Unk3(){return 0; };//+34
	virtual int Unk4(){return 0; };//+38
	virtual int Unk5(){return 0; };//+3C
	virtual int Unk6(){return 0; };//+40
	virtual int Unk7(){return 0; };//+44
	virtual int OnRendering(){ return 0; };//+48
	virtual int Unk8(){ return 0; };//+4C
	virtual int EraseCard(int is_del){ return 0; };//+50
public:
	UINT32 id;//+4
	UINT32 count;//+8
	UINT32 unk1;//+C
	UINT32 unk2;//+10
	UINT32 unk3;//+14
	UINT32 unk4;//+18
	HANM hanm;//+1C
	TimeObj timePassed;//+20,24,28,2C
	UINT32 unk5;//+30
	TimeObj timeCD;//+34,38,3C,40
	UINT32 unk6;//+44
	UINT32 totCD;//+48
	UINT32 unk7;//+4C
	UINT32 cardTypeFlag;//+50
};

class M_Card_global
{
public:
	static bool is_use_riceBall;
	static bool is_use_magicScroll;
	static bool is_use_ufo;
	static bool is_use_sakuyaOp;
	static bool is_use_moon;
	static bool is_use_autobomb;
};

inline void M_CardVtbl(AbCardBase* card,DWORD cardVtblBaseAddr,DWORD offs_func)
{
	VALUED(cardVtblBaseAddr)=VALUED((*(BYTE*)card)+offs_func);
}