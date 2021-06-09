#pragma once
#include "ThStructure.h"
void InjectAbCard();
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

