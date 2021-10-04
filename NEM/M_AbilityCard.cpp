#include "pch.h"
#include "M_AbilityCard.h"
#include "Address.h"
#include "injection.h"
#include <set>
extern AnmObj* (__cdecl* prob_new_anmObj)();
extern int(__thiscall* attach_anm)(DWORD pt_anm_file, AnmObj* thiz, int script_id);
extern int(__thiscall* anm_ins_switch)(AnmObj* thiz);
extern HANM* (__stdcall* insertAnmObj)(HANM* _OUT_handle, AnmObj* anmObj);
extern void(__stdcall* setAnmToNextStep)(HANM handle);
extern int(__fastcall* delBullet)(DWORD pbullet, int a2, int type);
extern int(__stdcall* playSE)(int id, int a2);
extern AnmObj*(__thiscall* getAnmObjFromHandle)(int anm_list, HANM handle);
extern AnmObj* (__stdcall* anm_ins5_switch)(HANM hanm, int sw_case);
#define HANM_TO_ANMOBJ(hanm) getAnmObjFromHandle(VALUED(0x51F65C),hanm);

extern int(__thiscall* update_shot)(DWORD thiz, int a2);
extern BOOL(__thiscall* addPower)(int thiz, int power_add);
extern int(__stdcall* showPopupText)(vector2f* pos, int text, DWORD color);
extern int(__thiscall* addLifePeice)(int thiz, int a2);
extern int(__thiscall* addLife)(int thiz);
extern int(__thiscall* addBomb)(int thiz, int a2);
extern int(__thiscall* original_DropItem)(DWORD thiz,int type, vector3f* pos, int a4, float dir, float spd, int delay, int a8, int a9);
extern void(_fastcall* SummonEnm)(int thiz, int edx, const char* name, EnemyInfo_init* info, int a);
extern int(__thiscall* dropItem)(int thiz, int type, vector3f* pos, int a4, float dir, float spd, int delay, int a8, int a9);
extern DWORD(__fastcall* shake_stage)(int type, int time, int fade_in, int fade_out, int a5, int a6);

bool M_Card_global::is_use_riceBall = false;
bool M_Card_global::is_use_ufo = false;
bool M_Card_global::is_use_sakuyaOp = false;
bool M_Card_global::is_use_magicScroll = false;
bool M_Card_global::is_use_moon = false;
bool M_Card_global::is_use_autobomb = false;

void Init_M_CardGlobal()
{
	M_Card_global::is_use_riceBall = false;
	M_Card_global::is_use_magicScroll = false;
	M_Card_global::is_use_ufo = false;
	M_Card_global::is_use_sakuyaOp = false;
	M_Card_global::is_use_moon = false;
	M_Card_global::is_use_autobomb = false;
}

class M_Null : public AbCardBase
{
public:
	int Init2()override;
};

int M_Null::Init2()
{
	Init_M_CardGlobal();
	return 0;
}

class M_RiceBall :public AbCardBase
{
public:
	int __fastcall OnActivate(int a2)override;
	int OnEnterFrame2()override;
	static int __fastcall M_DropItem(int thiz, int edx,
		int type, vector3f* pos, int a4, float dir, float spd, int delay, int a8, int a9);
};

int M_RiceBall::OnActivate(int a2)
{
	M_Card_global::is_use_riceBall = true;
	int(__thiscall * original_func)(void* thiz) = nullptr;
	VALUED(&original_func) = 0x410BD0;
	HookCall((LPVOID)0x410BF8,addLifePeice);
	return original_func(this);
}

int M_RiceBall::OnEnterFrame2()
{
	M_Card_global::is_use_riceBall = true;
	int(__thiscall * original_func)(void* thiz) = nullptr;
	VALUED(&original_func) = 0x410A90;
	return original_func(this);
}

int __fastcall M_RiceBall::M_DropItem(int thiz, int edx,int type, vector3f* pos, int a4, float dir, float spd, int delay, int a8, int a9)
{
	if (M_Card_global::is_use_riceBall && type==(int)Drop::smallP)
	{
		if (getRndInt_Rep() % 2 == 0)
		{
			type = (int)Drop::smallG;
		}
	}
	return original_DropItem(thiz, type, pos, a4, dir, spd, delay, a8, a9);
}

class M_BassDrum :public AbCardBase
{
public:
	int OnEnterFrame2()override;
};

int M_BassDrum::OnEnterFrame2()
{
	static float inc_num = 3.0f;
	if (IS_ABLE_COLDDOWN_CARD)
	{
		DWORD enmBase = *(DWORD*)(0x004CF2D0);
		if (enmBase)
		{
			DWORD iter = VALUED(enmBase + 0x18C);
			while (iter)
			{
				Enemy* pEnm = VALUEV(iter, Enemy*);
				if ((*pEnm->flag() & 0xC000021) == 0)
				{
					auto pos = pEnm->positionFin();
					EnemyInfo_init info = { 0 };
					info.drop = Drop::smallG;
					info.hp = 99999;
					info.pos = { pos->x,pos->y,0 };
					info.loc_9982 = *pEnm->time_loc9988();
					SummonEnm(*(DWORD*)(0x004CF2D0), 0, "DanmakuBassDrum", &info, 0);
				}
				iter = VALUED(iter + 0x4);
			}
		}
	}
	Address<DWORD>(0x41023F).SetValue((DWORD)&inc_num);
	int(__thiscall * original_func)(void* thiz) = nullptr;
	VALUED(&original_func) = 0x410110;
	return original_func(this);
}

class M_Kozuchi :public AbCardBase
{
public:
	int OnEnterFrame2()override;
};

int M_Kozuchi::OnEnterFrame2()
{
	static float inc_num = 3.0f;
	float scale = 1.25f;
	float scale_acc = 1.005f;

	DWORD pbtx = VALUED(0x004CF2BC);
	if (pbtx)
	{
		DWORD iter = VALUED(pbtx + 0xC0);
		while (iter)
		{
			Bullet* pbt = VALUEV(iter,Bullet*);
			auto time = pbt->nowTime()->time_now;
			
			auto h=pbt->hitbox();
			if (*(float*)((DWORD)(pbt)+0x7C) < scale)
			{
				h->x *= scale_acc;
				h->y *= scale_acc;
				*(float*)((DWORD)(pbt)+0x7C) *= scale_acc;
				*(float*)((DWORD)(pbt)+0x80) *= scale_acc;
			}
			iter = VALUED(iter + 0x4);
		}
	}

	DWORD enmBase = *(DWORD*)(0x004CF2D0);
	Address<DWORD>(0x40F0AD).SetValue((DWORD)&inc_num);
	int(__thiscall * original_func)(void* thiz) = nullptr;
	VALUED(&original_func) = 0x40EED0;
	return original_func(this);
}

class M_Narumi:public AbCardBase
{
public:
	int OnEnterFrame2()override;
	int OnDied()override;
};

int M_Narumi::OnEnterFrame2()
{
	if (IS_ABLE_COLDDOWN_CARD)
	{
		EnemyInfo_init info = { 0 };
		info.drop = Drop::smallG;
		info.hp = 99999;
		info.pos = { 0.0f,0.0f,0 };
		info.loc_9982 = VALUED(0x004CCCE8);
		SummonEnm(*(DWORD*)(0x004CF2D0), 0, "Narumi", &info, 0);
	}
	return 0;
}

int M_Narumi::OnDied()
{
	addLifePeice(0x4CCCDC, 7);
	return 0;
}

class M_Yuyuko :public AbCardBase
{
public:
	int OnEnterFrame2()override;
	int OnDied()override;
};

int M_Yuyuko::OnEnterFrame2()
{
	if (VALUED(0x4CF2BC))
		VALUED(VALUED(0x4CF2BC)+ 0x7A41F0) = 1;
	DWORD enmBase = *(DWORD*)(0x004CF2D0);
	if (enmBase)
	{
		DWORD iter = VALUED(enmBase + 0x18C);
		while (iter)
		{
			Enemy* pEnm = VALUEV(iter, Enemy*);
			if ((*pEnm->flag() & 0xC800021) == 0)
			{
				if (!VALUED((DWORD)pEnm + 0x67D4))
				{
					const char de[] = "DieBt";
					memcpy((char*)pEnm + 0x67D4, de, sizeof(de));
				}
			}
			iter = VALUED(iter + 0x4);
		}
	}
	return 0;
}

int M_Yuyuko::OnDied()
{
	vector3f pos = *PPLAYER2->fPosition();
	pos.y -= 20.0f;
	dropItem(VALUED(0x004CF2EC),(int)Drop::bomb, &pos,0, -1.57079f,2.0f,20,0,0);
	return 0;
}

class M_MagicScroll :public AbCardBase
{
public:
	int OnShotOp(ShotAttObj* shotAttobj)override;//+28
	int Init3()override;//+28
	static void OnReleaseBomb();
};

void  M_MagicScroll::OnReleaseBomb()
{
	if (M_Card_global::is_use_magicScroll)
	{
		vector3f pos = *PPLAYER2->fPosition();
		pos.y -= 20.0f;
		dropItem(VALUED(0x004CF2EC), (int)Drop::bombPeice, &pos, 0, -1.57079f, 2.0f, 20, 0, 0);
	}
	if (M_Card_global::is_use_autobomb && VALUED(0x004CCD58)>=2)
	{
		VALUED(0x004CCD58)--;
	}
}

int  M_MagicScroll::OnShotOp(ShotAttObj* shotAttobj)
{
	float dmg=*shotAttobj->damage();
	*shotAttobj->damage() = dmg * 0.8f;
	return 0;
}

int  M_MagicScroll::Init3()
{
	VALUED(VALUED(0x4CF2B8)+0xA4)= 1;
	M_Card_global::is_use_magicScroll = true;
	return 0;
}

class M_NueOp :public AbCardBase
{
public:
	int OnEnterFrame()override;
public:
	PlayerOp* plOp;
};

int M_NueOp::OnEnterFrame()
{
	float scale = 7.0f;
	M_Card_global::is_use_ufo = true;
	AnmObj* pAnmObj = getAnmObjFromHandle(VALUED(0x51F65C), plOp->hanm);
	if (pAnmObj->scaleRel()->x < scale)
	{
		pAnmObj->scaleRel()->x *= 1.03f;
		pAnmObj->scaleRel()->y *= 1.03f;
	}
	if (plOp)
	{
		vector3f v{ plOp->i_pos_now.x * 0.0078125f,(plOp->i_pos_now.y) * 0.0078125f,0.0f};
		erase_bullet_round(scale*6.2f, &v, 0, 9999, 1);
	}
	return 0;
}

class M_SakuyaOp:AbCardBase
{
public:
	int OnEnterFrame2()override;
};

int M_SakuyaOp::OnEnterFrame2()
{
	M_Card_global::is_use_sakuyaOp = true;
	if (IS_ABLE_COLDDOWN_CARD)
	{
		DWORD enmBase = *(DWORD*)(0x004CF2D0);
		if (enmBase)
		{
			DWORD iter = VALUED(enmBase + 0x18C);
			while (iter)
			{
				Enemy* pEnm = VALUEV(iter, Enemy*);
				if ((*pEnm->flag() & 0xC000021) == 0)
				{
					auto pos = pEnm->positionFin();
					EnemyInfo_init info = { 0 };
					info.drop = Drop::smallG;
					info.hp = 99999;
					info.pos = { pos->x,pos->y,0 };
					info.loc_9982 = *pEnm->time_loc9988();
					SummonEnm(*(DWORD*)(0x004CF2D0), 0, "DanmakuSakuyaOp", &info, 0);
				}
				iter = VALUED(iter + 0x4);
			}
		}
	}
	return 0;
}

class M_MarisaOp :AbCardBase
{
public:
	static int __fastcall M_abMarisaOp_1C_shot_onTick_flag_2(int thiz);
	int OnPressZ(int time_pressZ_mod15, int time_pressZ_mod_120)override;
	int EraseCard(int is_del)override;
	int OnGetOp()override;
	int OnEnterFrame2()override;
public:
	DWORD card_process_flag;//+0x54
};

int __fastcall M_MarisaOp::M_abMarisaOp_1C_shot_onTick_flag_2(int thiz)
{
	DWORD is_press_shift = VALUED(PPLAYER + 0x476CC);
	VALUED(PPLAYER + 0x476CC) = 0;


	ShotInfo* p_shotInfo = (ShotInfo*)((VALUED(VALUED(PPLAYER + 0x47940) + 4 * (VALUED(thiz + 0xAC) >> 8) + 0xE0))
		+ 92 * (unsigned __int8)(VALUED(thiz + 0xAC)));
	float angle = p_shotInfo->angle;
	VALUEF(thiz + 0x64) = angle;
	AttackObj* att = (AttackObj*)(VALUED(0x004CF410) + 0x204D8 + 0x9C * VALUED(thiz + 0xB0));
	att->angle = angle;
	att->damage = 40;//P O W E R

	int ret = ((DWORD(__fastcall*)(int))(0x45FC80))(thiz);
	VALUED(PPLAYER + 0x476CC) = is_press_shift;
	return ret;
}

int M_MarisaOp::OnPressZ(int time_pressZ_mod_15, int time_pressZ_mod_120)
{
	PlayerOp* op = (PlayerOp*)this->card_process_flag;
	int ShotGroupId = 11;
	ShotInfo* p_shot = *(ShotInfo**)(VALUED(PPLAYER + 0x47940) + 4 * ShotGroupId + 0xE0);

	//find a attackable obj
	DWORD enmBase = *(DWORD*)(0x004CF2D0);
	vector2f pPos = VALUEV(PPLAYER + 0x620, vector2f);
	vector2f aimPos = { cosf(p_shot->angle),sinf(p_shot->angle) };//aim to -1.57079f
	float maxCos = -1;
	int enmCount = 0;
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
					vector2f ePos = VALUEV(pEnm + 0x1270, vector2f);
					ePos = { ePos.x - pPos.x,ePos.y - pPos.y };
					float cos_EP = (ePos.x * aimPos.x + ePos.y * aimPos.y) / ((hypotf(ePos.x, ePos.y) * hypotf(aimPos.x, aimPos.y)) + 0.01);
					if (cos_EP > maxCos)
					{
						aimPos = ePos;
						maxCos = cos_EP;
					}
					enmCount++;
				}
			}
			iter = VALUED(iter + 0x4);
		}
	}
	if (enmCount == 0)
	{
		aimPos = { 0.0f,-1.0f };
	}
	vector2f aimPosNorm = { aimPos.x / hypotf(aimPos.x,aimPos.y),aimPos.y / hypotf(aimPos.x,aimPos.y) };
	vector2f aimPosNorm_ori = { cosf(p_shot->angle),sinf(p_shot->angle) };
	float a = 0.2f, b = 0.8f;
	vector2f aimPosNow = { aimPosNorm.x * a + aimPosNorm_ori.x * b,aimPosNorm.y * a + aimPosNorm_ori.y * b };
	p_shot->angle = atan2f(aimPosNow.y, aimPosNow.x);
	p_shot->func_tick = M_abMarisaOp_1C_shot_onTick_flag_2;
	((DWORD(__stdcall*)(int, int, int, int))(0x40A9C0))(this->card_process_flag, time_pressZ_mod_15, time_pressZ_mod_120, ShotGroupId);
	return 0;
}

int M_MarisaOp::EraseCard(int is_del)
{
	return ((int(__thiscall*)(AbCardBase*, char))(0x40AD40))(this, is_del);
}

int M_MarisaOp::OnGetOp()
{
	card_process_flag = ((DWORD(__thiscall*)(AbCardBase*, AbCardBase*, int, AbCardBase*, int, int))(0x40A790))
		(this, this, 16, this, 16, 6);
	return 0;
}

int M_MarisaOp::OnEnterFrame2()
{
	if (IS_ABLE_COLDDOWN_CARD)
	{
		EnemyInfo_init info = { 0 };
		info.drop = Drop::smallG;
		info.hp = 99999;
		info.pos = { 0.0f,460.0f,0 };
		info.loc_9982 = VALUED(0x004CCCE8);
		SummonEnm(*(DWORD*)(0x004CF2D0), 0, "DanmakuMarisaOp", &info, 0);
	}
	return 0;
}

class M_ReimuOp :AbCardBase
{
public:
	int OnEnterFrame2()override;
};

int M_ReimuOp::OnEnterFrame2()
{
	if (IS_ABLE_COLDDOWN_CARD)
	{
		EnemyInfo_init info = { 0 };
		info.drop = Drop::smallG;
		info.hp = 99999;
		info.pos = { 0.0f,0.0f,0 };
		info.loc_9982 = VALUED(0x004CCCE8);
		SummonEnm(*(DWORD*)(0x004CF2D0), 0, "DanmakuReimuOp", &info, 0);
	}
	return 0;
}

class M_Mokou :public AbCardBase
{
public:
	int Init2()override;
	int OnEnterFrame2()override;
};

int M_Mokou::Init2()
{
	if (VALUED(0x4CF2E4) && (VALUEV(this + 0x50,BYTE) & 2) != 0)
	{
		VALUED(0x4CCD54) = 7;
		for (UINT i = 0; i < 7; i++)
		{
			addLife(0x4CCCDC);
		}
		VALUEV(this + 0x50, BYTE) &= 0xFFFFFFFD;
	}
	return 0;
}

int M_Mokou::OnEnterFrame2()
{
	if (IS_ABLE_COLDDOWN_CARD)
	{
		EnemyInfo_init info = { 0 };
		info.drop = Drop::smallG;
		info.hp = 99999;
		info.pos = { 0.0f,0.0f,0 };
		info.loc_9982 = VALUED(0x004CCCE8);
		SummonEnm(*(DWORD*)(0x004CF2D0), 0, "DanmakuMokou", &info, 0);
	}
	return 0;
}

class M_SanaeOp2 :public AbCardBase
{
public:
	int OnEnterFrame2()override;
};

int M_SanaeOp2::OnEnterFrame2()
{
	if (IS_ABLE_COLDDOWN_CARD)
	{
		EnemyInfo_init info = { 0 };
		info.drop = Drop::smallG;
		info.hp = 99999;
		info.pos = { 0.0f,0.0f,0 };
		info.loc_9982 = VALUED(0x004CCCE8);
		SummonEnm(*(DWORD*)(0x004CF2D0), 0, "DanmakuSanaeOp2", &info, 0);
	}
	return 0;
}

class M_MainShotPu :public AbCardBase
{
public:
	int OnEnterFrame2()override;
	int OnShotOp(ShotAttObj* shotAttobj)override;//+28
};

int M_MainShotPu::OnShotOp(ShotAttObj* shotAttobj)
{
	int v1; // ebx
	int v2; // eax
	DWORD a1 = (DWORD)shotAttobj;
	v1 = *(DWORD*)(*(DWORD*)(PPLAYER + 0x47940) + 4 * (*(int*)(a1 + 0xAC) >> 8) + 0xE0)
		+ 92 * (unsigned __int8)*(DWORD*)(a1 + 0xAC);
	if (*(BYTE*)(v1 + 0x21) == 7)
	{
		v2 = (int)getAnmObjFromHandle(VALUED(0x51F65C), *(DWORD*)(a1 + 8));
		if (v2)
		{
			*(DWORD*)(v2 + 1332) |= 8u;
			*(float*)(v2 + 0x54) = 3.0f;
			*(float*)(v2 + 0x58) = 3.0f;
		}
		else
		{
			*(DWORD*)(a1 + 8) = 0;
		}
		*(DWORD*)(a1 + 0x9C) = (int)(float)((float)*(__int16*)(v1 + 2) * 1.8);
		*(float*)(a1 + 0xA0) = *(float*)(v1 + 0xC) * 3.0;
		*(float*)(a1 + 0xA4) = *(float*)(v1 + 0x10) * 3.0;
	}
	return 0;
}

int M_MainShotPu::OnEnterFrame2()
{
	if (IS_ABLE_COLDDOWN_CARD)
	{
		EnemyInfo_init info = { 0 };
		info.drop = Drop::smallG;
		info.hp = 99999;
		info.pos = { 0.0f,0.0f,0 };
		info.loc_9982 = VALUED(0x004CCCE8);
		SummonEnm(*(DWORD*)(0x004CF2D0), 0, "DanmakuMainShotPu", &info, 0);
	}
	return 0;
}

class M_Moon :public AbCardBase
{
public:
	int __fastcall OnActivate(int a2)override;
	int OnEnterFrame2()override;
public:
	DWORD flag;//+54
		//  vector3f moon_nowPos;//+58,5C,60
		//  vector3f moon_spawnPos;//+64,68,6C
		//  vector3f moon_pos2;//+70,74,78
		vector2f moonPos[3];//+58~+6C
		HANM moonANM[3];//+70,74,78
		//DWORD unk[9];//+7C
		vector2f moonVel[3];//+7C
		AttackObj *moonAttObj[3];
		//TimeObj moonTime;//+A0,A4,A8,AC
		float moonTime[3];
		DWORD occu;//+AC

	DWORD unk2;//+B0
	UINT32 time_float;//+B4
	DWORD unk3;//+B8
	AttackObj *attObj_orig;//+BC
	float y_offs;//+C0

};

int __fastcall M_Moon::OnActivate(int a2)
{
	if (!this->flag && this->timeCD.time_now <= 0)
	{
		auto ppl = PPLAYER2;
		vector2f ppos = { ppl->fPosition()->x,ppl->fPosition()->y };
		moonPos[0] = moonPos[1] = moonPos[2] = ppos;

		float angle = -PI/2.0f;
		const float originalSpeed = 1.0f;
		for (UINT i = 0; i < 3; i++)
		{
			this->flag = 1;
			AnmObj* pAnmObj = prob_new_anmObj();
			attach_anm(VALUED(VALUED(0x4CF298) + 0xC), pAnmObj, 45);
			*pAnmObj->layerFlag()= (*pAnmObj->layerFlag() & 0xFFDFFFFF)| 0x101000;
			*pAnmObj->layer() = 13;
			*(vector2f*)pAnmObj->pos() = moonPos[i];
			anm_ins_switch(pAnmObj);
			HANM x;
			moonANM[i]=*insertAnmObj(&x, pAnmObj);
			vector3f attPos = { ppos.x,ppos.y,0.0f };
			int index = summon_attack_circle(PPLAYER2, 10.0, 0.0, &attPos, 99999, 45);
			moonAttObj[i] = PPLAYER2->attObjArr(index);
			moonVel[i] = { originalSpeed*cosf(angle),originalSpeed*sinf(angle)};
			angle = angle + PI * 2.0f / 3.0f;
			moonTime[i] = -1;
		}
		timePassed.time_now = 0;
		timePassed.time_now_f = 0;
		timePassed.time_last = -1;
		PlaySEWithChannel(ppos.x, 53);

		auto cd =this->totCD * CARDCD_SCALE;
		this->timeCD.time_last = cd-1;
		this->timeCD.time_now = cd;
		this->timeCD.time_now_f = cd;
	}
	return 0;
}

int M_Moon::OnEnterFrame2()
{
	M_Card_global::is_use_moon = true;
	if (IS_ABLE_COLDDOWN_CARD && this->timeCD.time_now > 0)
	{
		timeCD.DecTime();
	}
	if (this->flag)
	{
		auto ppl = PPLAYER2;
		vector2f ppos = { ppl->fPosition()->x,ppl->fPosition()->y };
		timePassed.IncTime();

		UINT n = 0;
		for (UINT i = 0; i < 3; i++)
		{
			const float originalSpeed = 1.0f;
			const float maxSpeed =9.0f;
			auto pAnmObj = HANM_TO_ANMOBJ(moonANM[i]);
			if (!pAnmObj)
			{
				n++;
				continue;
			}
			if (moonAttObj[i])
			{
				float moonScale = pAnmObj->scaleRel()->x * 50.0f;
				moonAttObj[i]->att_radio = moonScale;
				vector3f mps = { moonPos[i].x,moonPos[i].y,0.0f };
				erase_bullet_round(moonScale, &mps, 0, 999999, 0);
				erase_laser_round(moonScale, &mps, 0, 0);
			}
			if (moonTime[i] == -1.0f)//shooting
			{
				float spd_sc = 1.0f;
				if (moonAttObj[i] && moonAttObj[i]->damage_count > 0)
				{
					spd_sc = 0.08f;
					moonAttObj[i]->damage_count = 0;
				}
				if (timePassed.time_now>20 && timePassed.time_now<70)
				{
					float dist = 600.0f-DIST_VEC2(ppos, moonPos[i]);
					float velacc_scale = 0.05f * 0.0078125f*max(1.0f,(timePassed.time_now_f-90.0f)/360.0f)*(dist/600.0f);
					moonVel[i].x += PPLAYER2->fVelocity_fixed()->x * velacc_scale;
					moonVel[i].y += PPLAYER2->fVelocity_fixed()->y * velacc_scale;
					float vel = max(0.01f, hypotf(moonVel[i].x, moonVel[i].y));
					float spdNow = (1.0f / vel) * min(maxSpeed, vel);
					moonVel[i].x *= spdNow;
					moonVel[i].y *= spdNow;

					vel = hypotf(moonVel[i].x, moonVel[i].y);
					if (vel < maxSpeed)
					{
						moonVel[i].x *= 1.02f;
						moonVel[i].y *= 1.02f;
					}
				}else{
					float vel = hypotf(moonVel[i].x, moonVel[i].y);
					if (vel < maxSpeed)
					{
						moonVel[i].x *= 1.06f;
						moonVel[i].y *= 1.06f;
					}
				}
				moonPos[i].x += spd_sc * moonVel[i].x*GAME_SPD;
				moonPos[i].y += spd_sc * moonVel[i].y*GAME_SPD;
				if (moonPos[i].x <= -180.0f || moonPos[i].x>=180.0f || moonPos[i].y<=0.0f || moonPos[i].y>=448.0f)
				{
					PlaySEWithChannel(moonPos[i].x, 51);
					shake_stage(1,20,5, 5, 0, 0x5B);
					moonVel[i] = { 0,0 };
					moonTime[i] = 0;
				}
			}else if(moonTime[i]>=0.0f){
				moonTime[i] += GAME_SPD;
				if (moonTime[i] >= 60.0f)
				{
					moonVel[i].x = ppos.x - moonPos[i].x;
					moonVel[i].y = ppos.y - moonPos[i].y;
					float moonVel_len=max(0.01f,hypotf(moonVel[i].x,moonVel[i].y));
					moonVel[i].x /= moonVel_len;
					moonVel[i].y /= moonVel_len;
					float dist = DIST_VEC2(ppos,moonPos[i]);
					float plSpeed = hypotf(PPLAYER2->fVelocity_fixed()->x, PPLAYER2->fVelocity_fixed()->y)/128.0f;
					float speed=max(8.0f,(moonTime[i]-60.0f)/15.0f);
					if (dist <= 150.0f)
					{
						if (dist <= 20.0f)
						{
							if (moonAttObj[i])
							{
								anm_ins5_switch(moonANM[i], 1);
								moonAttObj[i]->time.time_last = 0;
								moonAttObj[i]->time.time_now = 1;
								moonAttObj[i]->time.time_now_f = 1;
								moonAttObj[i] = nullptr;
							}
						}
						if (dist <= 50.0f)
						{
							PPLAYER2->iPosition_fixed()->x -= moonVel[i].x * GAME_SPD * 128.0f * 1.0f + PPLAYER2->fVelocity_fixed()->x * 0.8f;
							PPLAYER2->iPosition_fixed()->y -= moonVel[i].y * GAME_SPD * 128.0f * 1.0f + PPLAYER2->fVelocity_fixed()->y * 0.8f;//another force
						}
						speed = min(speed+plSpeed, dist/30.0f+plSpeed);
					}
					moonVel[i].x *= speed;
					moonVel[i].y *= speed;

					float spd_sc = 1.0f;
					if (moonAttObj[i] && moonAttObj[i]->damage_count > 0)
					{
						spd_sc = 0.2f;
						moonAttObj[i]->damage_count = 0;
					}
					moonPos[i].x += spd_sc * moonVel[i].x * GAME_SPD;
					moonPos[i].y += spd_sc * moonVel[i].y * GAME_SPD;
				}
			}
			if (pAnmObj)
			{
				pAnmObj->pos()->x = moonPos[i].x;
				pAnmObj->pos()->y = moonPos[i].y;
			}
			if (moonAttObj[i])
			{
				moonAttObj[i]->pos.x = moonPos[i].x;
				moonAttObj[i]->pos.y = moonPos[i].y;
			}
		}
		if (n == 3)
		{
			this->flag = 0;
		}
	}
	return 0;
}

class M_Autobomb :public AbCardBase
{
public:
	int OnDying(int a1)override;
	int OnEnterFrame()override;
public:
};

int M_Autobomb::OnDying(int a1)
{
	DWORD* v1; // edx
	DWORD* v2=(DWORD*)this; // ecx
	if (!a1)
	{
		v1 = (DWORD*)VALUED((VALUED(0x4CF298)+ 0x1C));
		if (!v1)
			goto LABEL_7;
		while (*(DWORD*)(*v1 + 4) != 35)
		{
			v1 = (DWORD*)v1[1];
			if (!v1)
				goto LABEL_7;
		}
		if (VALUED(0x4CCD34)< 200)
		{
		LABEL_7:
			bool (*prob_isable_bomb)()=nullptr;
			VALUED(&prob_isable_bomb) = 0x420420;
			if (VALUED(0x4CF2B8) && prob_isable_bomb())
			{
				bool is_ab = VALUED(0x004CCD58) >= 2;

				int (__thiscall * sub_40A2A0)(DWORD* ecx)=nullptr;
				VALUED(&sub_40A2A0) = 0x40A2A0;
				sub_40A2A0(v2);
				int(__thiscall * sub_45D590)(DWORD ecx) = nullptr;
				VALUED(&sub_45D590) = 0x45D590;
				sub_45D590(PPLAYER);
				if(is_ab)
					addBomb(0x4CCCDC, 0);
				return 1;
			}
		}
	}
	return 0;
}

int M_Autobomb::OnEnterFrame()
{
	M_Card_global::is_use_autobomb = true;
	int(__thiscall * original_func)(void* thiz) = nullptr;
	VALUED(&original_func) = 0x40A560;
	return original_func(this);
}

int __fastcall M_BulletShooting(int thiz, int a2, DWORD BulletShootInfo)
{
	vector2f normalDir = VALUEV(BulletShootInfo + 0x14, vector2f);
	vector2f normalSpd = VALUEV(BulletShootInfo + 0x1C, vector2f);
	if (M_Card_global::is_use_ufo)
	{
		VALUEF(BulletShootInfo + 0x14) += getRndFloat11_Rep() * 0.1f;
		VALUEF(BulletShootInfo + 0x18) += getRndFloat11_Rep() * 0.1f;
		VALUEF(BulletShootInfo + 0x1C) *= 1.0f + getRndFloat11_Rep() * 0.1f;
		VALUEF(BulletShootInfo + 0x20) *= 1.0f + getRndFloat11_Rep() * 0.1f;
	}
	if (M_Card_global::is_use_sakuyaOp)
	{
		VALUEF(BulletShootInfo + 0x1C) *= 0.9f;
		VALUEF(BulletShootInfo + 0x20) *= 0.9f;
	}
	if (M_Card_global::is_use_moon)
	{
		VALUEF(BulletShootInfo + 0x1C) *= 1.5f;
		VALUEF(BulletShootInfo + 0x20) *= 1.5f;
	}
	//VALUED(BulletShootInfo + 0) = 0;
	int(__fastcall * originalFunc)(int thiz, int a2, DWORD BulletShootInfo) = 0;
	VALUED(&originalFunc) = 0x427810;
	auto ret = originalFunc(thiz, a2, BulletShootInfo);
	VALUEV(BulletShootInfo + 0x14, vector2f) = normalDir;
	VALUEV(BulletShootInfo + 0x1C, vector2f) = normalSpd;
	return ret;
}

void InjectAbCard()
{
	HookCall((LPVOID)0x43401A, (LPVOID)M_BulletShooting);
	Hook((LPVOID)0x00442750, 6, Init_M_CardGlobal);
	{
		M_RiceBall rc;
		Address<DWORD>(0x004B4DE0).SetValue(*(DWORD*)((*(DWORD*)(&rc)) + 0x8));
		Address<DWORD>(0x004B4E04).SetValue(*(DWORD*)((*(DWORD*)(&rc)) + 0x2C));
		//drop
		Address<DWORD>(0x446EF7).SetValue(0x51EC8B55);
		Address<DWORD>(0x446EFB).SetValue(0xEB575653);
		Address<BYTE>(0x446EFF).SetValue(0x47);
		//set the original call
		Address<BYTE>(0x446F40).SetValue(0xB8);
		Address<DWORD>(0x446F41).SetValue((DWORD)M_RiceBall::M_DropItem);
		Address<WORD>(0x446F45).SetValue(0xE0FF);
		//end the DropItem call
	}
	{
		M_Null mn;
		Address<DWORD>(0x004B4C7C).SetValue(*(DWORD*)((*(DWORD*)(&mn)) + 0x4));
	}
	{
		M_BassDrum bd;
		Address<DWORD>(0x004B4FBC).SetValue(*(DWORD*)((*(DWORD*)(&bd)) + 0x2C));
	}
	{
		M_Kozuchi kz;
		Address<DWORD>(0x004B511C).SetValue(*(DWORD*)((*(DWORD*)(&kz)) + 0x2C));
	}
	{
		M_Narumi nm;
		Address<DWORD>(0x004B5DBC).SetValue(*(DWORD*)((*(DWORD*)(&nm)) + 0x14));
		Address<DWORD>(0x004B5DD4).SetValue(*(DWORD*)((*(DWORD*)(&nm)) + 0x2C));
	}
	{
		M_Yuyuko yyk;
		Address<DWORD>(0x004B5314).SetValue(*(DWORD*)((*(DWORD*)(&yyk)) + 0x14));
		Address<DWORD>(0x004B532C).SetValue(*(DWORD*)((*(DWORD*)(&yyk)) + 0x2C));
	}
	{
		M_MagicScroll mgs;
		Address<DWORD>(0x004B54D8).SetValue(*(DWORD*)((*(DWORD*)(&mgs)) + 0x20));
		Address<DWORD>(0x004B54E0).SetValue(*(DWORD*)((*(DWORD*)(&mgs)) + 0x28));
		Hook((LPVOID)0x004574D3, 7, mgs.OnReleaseBomb);
	}
	{
		M_NueOp nop;
		Address<DWORD>(0x004B5744).SetValue(*(DWORD*)((*(DWORD*)(&nop)) + 0x24));
	}
	{
		M_SakuyaOp sop;
		Address<DWORD>(0x004B5A0C).SetValue(*(DWORD*)((*(DWORD*)(&sop)) + 0x2C));
	}
	{
		M_MarisaOp mop;
		Address<DWORD>(0x004B5AA8).SetValue(*(DWORD*)((*(DWORD*)(&mop)) + 0x18));
		Address<DWORD>(0x004B5AAC).SetValue(*(DWORD*)((*(DWORD*)(&mop)) + 0x1C));
		Address<DWORD>(0x004B5ABC).SetValue(*(DWORD*)((*(DWORD*)(&mop)) + 0x2C));
		Address<DWORD>(0x004B5AE0).SetValue(*(DWORD*)((*(DWORD*)(&mop)) + 0x50));
	}
	{
		M_ReimuOp mop;
		Address<DWORD>(0x004B5B6C).SetValue(*(DWORD*)((*(DWORD*)(&mop)) + 0x2C));
	}
	{
		M_Mokou mk;
		Address<DWORD>(0x004B5E04).SetValue(*(DWORD*)((*(DWORD*)(&mk)) + 0x4));
		Address<DWORD>(0x004B5E2C).SetValue(*(DWORD*)((*(DWORD*)(&mk)) + 0x2C));
	}
	{
		M_SanaeOp2 sop;
		Address<DWORD>(0x004B5904).SetValue(*(DWORD*)((*(DWORD*)(&sop)) + 0x2C));
	}
	{
		M_MainShotPu msp;
		Address<DWORD>(0x004B55E8).SetValue(*(DWORD*)((*(DWORD*)(&msp)) + 0x28));
		Address<DWORD>(0x004B55EC).SetValue(*(DWORD*)((*(DWORD*)(&msp)) + 0x2C));
	}
	{
		M_Moon mm;
		Address<DWORD>(0x004B5200).SetValue(*(DWORD*)((*(DWORD*)(&mm)) + 0x8));
		Address<DWORD>(0x004B5224).SetValue(*(DWORD*)((*(DWORD*)(&mm)) + 0x2C));
	}
	{
		M_Autobomb ab;
		Address<DWORD>(0x004B5BFC).SetValue(*(DWORD*)((*(DWORD*)(&ab)) + 0xC));
		Address<DWORD>(0x004B5C14).SetValue(*(DWORD*)((*(DWORD*)(&ab)) + 0x24));
	}
}

