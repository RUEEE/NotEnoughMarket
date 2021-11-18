#include "pch.h"
#include "M_Bomb.h"
#include "Address.h"
#include "THFunction.h"
#include <set>


std::set<int> M_marisaBomb_enm_set;
std::set<std::pair<DWORD,int>> M_marisaBomb_hanm_set;
int __fastcall M_MarisaBombRelease(DWORD thiz)
{
	/*VALUEV(thiz+0x14,vector3f)=VALUEV(PPLAYER+0x620, vector3f);
	DWORD pAnmObj = prob_new_anmObj();
	DWORD pAnmFile = VALUED(PPLAYER + 0xC);
	++VALUED(pAnmFile + 0x4D);
	attach_anm(pAnmFile, pAnmObj, 37);
	HANM hAnm;
	insertAnmObj(&hAnm, pAnmObj);
	anm_ins_switch(pAnmObj);
	VALUED(thiz + 0x5C) = hAnm;
	VALUEV(pAnmObj + 0x5F0, vector3f) = VALUEV(thiz + 0x14, vector3f);
	SetInvincibleTime(120);
	return 0;*/

	SetInvincibleTime(240);
	M_marisaBomb_enm_set.clear();
	M_marisaBomb_hanm_set.clear();

	VALUEV(thiz + 0x14, vector3f) = VALUEV(PPLAYER + 0x620, vector3f);
	DWORD pAnmFile = VALUED(PPLAYER + 0xC);
	++VALUED(pAnmFile + 0x4D);
	AnmObj* pAnmObj = AllocateAnmObj();
	AttachAnm(pAnmFile, pAnmObj, 50);//bk
	HANM hAnm;
	InsertAnmObj(&hAnm, pAnmObj);
	AnmInsSwitch(pAnmObj);
	VALUED(thiz + 0x64) = hAnm;
	VALUEV(pAnmObj + 0x5F0, vector3f) = VALUEV(thiz + 0x14, vector3f);
	return 0;
}

int __fastcall M_MarisaBombFunc(DWORD thiz)
{
	{
		//float up_time = 480.0f;
		//float resize_time = 60.0f;
		//float size = 50.0f;
		//DWORD pAnmObj = (DWORD)getAnmObjFromHandle(VALUED(0x51F65C), VALUED(thiz + 0x5C));
		//if (!pAnmObj)
		//{
		//	VALUED(thiz + 0x5C) = 0;
		//	return -1;
		//}
		//TimeObj* time = (TimeObj*)(thiz + 0x34);
		//if (time->time_last != time->time_now)
		//{
		//	if (time->time_now_f >= up_time)
		//	{
		//		float ratio = (1.0f - (time->time_now_f - up_time) / resize_time);
		//		VALUEV(pAnmObj + 0x54, vector2f).x = ratio;
		//		VALUEV(pAnmObj + 0x54, vector2f).y = ratio;
		//		size *= ratio;
		//	}
		//	VALUEV(pAnmObj + 0x5F0, vector3f) = VALUEV(thiz + 0x14, vector3f);
		//	VALUEV(pAnmObj + 0x5F0, vector3f).y -= time->time_now_f * 0.5f;
		//	erase_bullet_round(size, (vector3f*)(pAnmObj + 0x5F0), 4, 99999, 0);
		//}
		//if (time->time_now_f >= resize_time + up_time)
		//{
		//	anmReturn_byHANM(VALUED(thiz + 0x5C));
		//	VALUED(thiz + 0x5C) = 0;
		//	return -1;//end
		//}
		//return 0;//not end
	}
	DWORD enmBase = *(DWORD*)(0x004CF2D0);
	TimeObj* time = (TimeObj*)(thiz + 0x34);
	if (time->time_last != time->time_now && time->time_now < 90)
	{
		float time_remain = 90.0f, max_radius = 600.0f;
		float r_now = time->time_now_f * (max_radius / time_remain);
		EraseBullet_round(r_now, (vector3f*)(thiz + 0x14), 0, 99999, 0);
		if (enmBase)
		{
			DWORD iter = VALUED(enmBase + 0x18C);
			while (iter)
			{
				DWORD pEnm = VALUED(iter);
				{
					DWORD flag = VALUED(pEnm + 0x635C);
					if ((flag & 0x0000021) == 0)//available
					{
						vector3f posE=VALUEV(pEnm + 0x1270, vector3f);
						vector3f posH = VALUEV(thiz + 0x14, vector3f);
						float dist = (posE.x - posH.x)* (posE.x - posH.x);
						dist+= (posE.y- posH.y) * (posE.y - posH.y);
						if (dist<r_now*r_now)
						{
							int id = VALUED(pEnm + 0x6830);
							if (!M_marisaBomb_enm_set.count(id))
							{
								M_marisaBomb_enm_set.insert(id);
								AnmObj* pAnmObj = AllocateAnmObj();
								DWORD pAnmFile = VALUED(PPLAYER + 0xC);
								++VALUED(pAnmFile + 0x4D);
								AttachAnm(pAnmFile, pAnmObj, 47);
								HANM hAnm;
								InsertAnmObj(&hAnm, pAnmObj);
								VALUEV(pAnmObj+0x5F0, vector3f)= VALUEV(pEnm + 0x1270, vector3f);
								AnmInsSwitch(pAnmObj);
								M_marisaBomb_hanm_set.insert(std::make_pair(id,hAnm));
							}
						}
					}
				}
				iter = VALUED(iter + 0x4);
			}
		}
	}
	for (auto& i : M_marisaBomb_hanm_set)
	{
		DWORD pAnm = (DWORD)GetAnmObjFromHandle(VALUED(0x51F65C), i.second);
		if (pAnm)
		{
			int id = i.first;
			DWORD pEnm=(DWORD)GetEnmObjFromId(&id);
			if (pEnm)
			{
				VALUEV(pAnm + 0x5F0, vector3f) = VALUEV(pEnm + 0x1270, vector3f);
			}
		}
	}
	if (time->time_now == 120)
	{
		for (auto& i : M_marisaBomb_hanm_set)
		{
			DWORD pAnm = (DWORD)GetAnmObjFromHandle(VALUED(0x51F65C), i.second);
			if (pAnm)
			{
				int n=SummonAttackCircle(PPLAYER2, 10.0f, 0.5f, (vector3f*)(pAnm + 0x5F0), 90, 500);
				EraseBullet_round(100.0f, (vector3f*)(pAnm + 0x5F0), 0, 99999, 0);
				VALUED(PPLAYER + 0x204D8 + n * 0x9C+0x98) = 3;//fire eff
			}
			AnmReturn_byHANM(i.second);
		}
		AnmReturn_byHANM(VALUED(thiz + 0x64));
	}
	if (time->time_now >= 150)
	{
		return -1;
	}
	return 0;
}



void Inject_Bomb()
{
	//Address<DWORD>(0x4B6314).SetValue((DWORD)M_MarisaBombRelease);
	//Address<DWORD>(0x4B6318).SetValue((DWORD)M_MarisaBombFunc);
}