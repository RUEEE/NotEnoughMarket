#include "pch.h"
#include "M_AbilityCard.h"
#include "Address.h"

#include <set>


DWORD (__cdecl *prob_new_anmObj)()= (DWORD (__cdecl*)()) (0x489320);
int(__fastcall* attach_anm)(int pt_anm_file, int edx, int thiz, int script_id)
	=(int(__fastcall *)(int,int,int,int))(0x407420);
int (__fastcall *anm_ins_switch)(int thiz, int edx)
	=(int(__fastcall*)(int, int))(0x478580);
int(__stdcall* insertAnmObj)(int* _OUT_handle, int anmObj)
	= (int(__stdcall*)(int*, int))(0x488350);
void(__stdcall* setAnmToNextStep)(int handle)
	= (void(__stdcall*)(int))(0x488CF0);

int(__fastcall* delBullet)(int pbullet, int a2, int type)
= (int(__fastcall*)(int,int,int))(0x428E90);
//type==0:nothing,type=1:greenpoint,type==4:hammerhit

extern int(__stdcall* playSE)(int id, int a2);

int(__fastcall* getAnmObjFromHandle)(int anm_list,int edx, int handle)
	= (int(__fastcall*)(int, int, int))(0x488B40);

//int __userpurge sb_45DE40_summon_attack_circle@<eax>(int pplayer@<ecx>, float radiox@<xmm2>, float radioy@<xmm3>, vector3F* pos, int lifespan, int attack);
int summon_attack_circle(int pplayer,float radiox,float radioy, vector3f* pos, int lifespan, int damage)
{
	int id_att;
	__asm
	{
		push damage;
		push lifespan;
		push pos;
		movss xmm3, radioy;
		movss xmm2, radiox;
		mov ecx, pplayer;
		mov eax, 0x0045DE40;
		call eax;
		mov id_att,eax
	}
	return id_att - 1;
}

typedef std::pair<std::set<int>, std::vector<int>> TYPE_HD;

int __fastcall M_abKozuchi_8(AbCardBase* thiz, int a2)
{
	if (!VALUED((DWORD)thiz + 0x54) && VALUEV((DWORD)thiz + 0x38,int) <= 0)// not using
	{
		DWORD ppl = PPLAYER;

		thiz->pos_use = VALUEV(ppl+ 0x620, vector3f);
		DWORD anmFile=VALUED(VALUED(0x4CF298)+0xC);
		
		DWORD pAnmObj = prob_new_anmObj();
		attach_anm(anmFile,0, pAnmObj, 68);
		VALUED(pAnmObj + 0x18) = 13;
		VALUED(pAnmObj + 0x538) |= 0x101000;
		VALUEV(pAnmObj + 0x5F0, vector3f) = thiz->pos_use;
		anm_ins_switch(pAnmObj, 0);
		VALUED(pAnmObj + 0x520) = 0;
		int handle;
		insertAnmObj(&handle, pAnmObj);

		thiz->handle_anm = (DWORD)(new TYPE_HD());

		((TYPE_HD*)(thiz->handle_anm))->second.push_back(handle);
		thiz->card_process_flag = 1;
		thiz->now_passed_time = TimeObj{ 0,0,0,0 };
	}
	
	return 0;
}
int __fastcall M_abKozuchi_C(AbCardBase* thiz,int a2, int a3)
{
	{
		CancelFlag(&thiz->cardFlag, 0x20);
		thiz->card_process_flag = 0;
		//free the bullet
		auto st = (TYPE_HD*)thiz->handle_anm;
		if (st == 0)
			return 0;
		for (auto iter = st->second.begin(); iter != st->second.end(); iter++)
		{
			setAnmToNextStep(*iter);
		}
		DWORD pbtx = VALUED(0x004CF2BC);
		if (pbtx)
		{
			DWORD iter = VALUED(pbtx + 0xC0);
			while (iter)
			{
				DWORD pbt = VALUED(iter);
				if (st->first.count(pbt))//have the bullet
				{
					summon_attack_circle(PPLAYER, 50.0f, 50.0f, (vector3f*)(pbt + 0x638), 10, 500);
					delBullet(pbt, 0, 0);
				}
				iter = VALUED(iter + 0x4);
			}
		}
		playSE(51, 0);
		//delete the set
		delete (TYPE_HD*)thiz->handle_anm;
		thiz->handle_anm = 0;
		return 0;
	}
	return 0;
}
int __fastcall M_abKozuchi_2C(AbCardBase* thiz)
{
	if (thiz->card_process_flag == 0)
		return 0;//the ability is not using
	SetFlag(&thiz->cardFlag, 0x20);
	thiz->now_passed_time.IncTime();
	if (thiz->now_passed_time.time_now >= 100)
	{
		CancelFlag(&thiz->cardFlag, 0x20);
		thiz->card_process_flag = 0;
		//free the bullet
		auto st = (TYPE_HD*)thiz->handle_anm;
		for (auto iter = st->second.begin(); iter != st->second.end(); iter++)
		{
			setAnmToNextStep(*iter);
		}
		DWORD pbtx = VALUED(0x004CF2BC);
		if (pbtx)
		{
			DWORD iter = VALUED(pbtx + 0xC0);
			while (iter)
			{
				DWORD pbt = VALUED(iter);
				if (st->first.count(pbt))//have the bullet
				{
					summon_attack_circle(PPLAYER, 10.0f, 10.0f, (vector3f*)(pbt + 0x638), 10, 5000);
					delBullet(pbt, 0, 4);
				}
				iter = VALUED(iter + 0x4);
			}
		}
		//delete the set
		delete (TYPE_HD*)thiz->handle_anm;
		thiz->handle_anm = 0;
		return 0;
	}
	{
		DWORD pbtx = VALUED(0x004CF2BC);
		if (pbtx)
		{
			DWORD iter = VALUED(pbtx + 0xC0);
			while (iter)
			{
				DWORD pbt = VALUED(iter);
				vector3f pos_pbt= VALUEV(pbt+0x638,vector3f);
				float dx = pos_pbt.x - thiz->pos_use.x,
					dy = pos_pbt.y - thiz->pos_use.y,
					d = thiz->now_passed_time.time_now_f*5;
				auto st = (TYPE_HD*)thiz->handle_anm;
				if (VALUEF(pbt + 0x658)!=0.0f && dx * dx + dy * dy < d*d)
				{
					for(auto iter2=st->second.begin();iter2!=st->second.end();iter2++)
					{
						DWORD pAnmObj = getAnmObjFromHandle(VALUED(0x51F65C),0,*iter2);
						if (!pAnmObj)
							break;
						if (VALUED(pAnmObj + 0x560) <= 2)
							continue;
						vector3f pos_anmObj = VALUEV(pAnmObj + 0x5F0, vector3f);
						float dx2 = pos_pbt.x - pos_anmObj.x;
						float dy2 = pos_pbt.y - pos_anmObj.y;
						if (dx2 * dx2 + dy2 * dy2 <= 40.0f*40.0f)
						{
							VALUEV(pbt + 0x644, vector2f) = vector2f{ 0,0 };//stop the bullet
							VALUEF(pbt + 0x658) = 0.0f;//delete the hitbox
							VALUED(pbt + 0x67C) = 24;//stop the trans
							VALUED(pbt + 0x684) = 0;//stop the trans
							DWORD anmFile = VALUED(VALUED(0x4CF298) + 0xC);
							DWORD pAnmObj = prob_new_anmObj();
							attach_anm(anmFile, 0, pAnmObj, 68);
							VALUED(pAnmObj + 0x18) = 13;
							VALUED(pAnmObj + 0x538) |= 0x101000;
							VALUEV(pAnmObj + 0x5F0, vector3f) = VALUEV(pbt + 0x638, vector3f);
							anm_ins_switch(pAnmObj, 0);
							VALUED(pAnmObj + 0x520) = 0;
							int handle;
							insertAnmObj(&handle, pAnmObj);
							playSE(61, 0);
							st->first.insert(pbt);
							st->second.push_back(handle);
							break;
						}
					}
				}
				iter = VALUED(iter + 0x4);
			}
		}
	}
	return 0;
}
void InjectAbCard()
{
	Address<DWORD>(0x004B50F8).SetValue((DWORD)M_abKozuchi_8);
	Address<DWORD>(0x004B50FC).SetValue((DWORD)M_abKozuchi_C);
	Address<DWORD>(0x004B511C).SetValue((DWORD)M_abKozuchi_2C);

}