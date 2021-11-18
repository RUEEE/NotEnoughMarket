#include "pch.h"
#include "M_AbilityCard.h"
#include "THFunction.h"
#include "Address.h"
#include "injection.h"
#include "M_Misc.h"
#include <set>

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

		float angle_acc = 2*PI / 3.0f;
		angle_acc = PI / 6.0f;
		float angle = -PI/2.0f-angle_acc;

		float ppl_moveY = PPLAYER2->fVelocity_fixed()->y;
		float ppl_moveX = PPLAYER2->fVelocity_fixed()->x;
		if(ppl_moveY !=0 || ppl_moveX !=0)
			angle = atan2f(ppl_moveY, ppl_moveX) - angle_acc;
		const float originalSpeed = 1.0f;
		for (UINT i = 0; i < 3; i++)
		{
			this->flag = 1;
			AnmObj* pAnmObj = AllocateAnmObj();
			AttachAnm(VALUED(VALUED(0x4CF298) + 0xC), pAnmObj, 45);
			*pAnmObj->layerFlag()= (*pAnmObj->layerFlag() & 0xFFDFFFFF)| 0x101000;
			*pAnmObj->layer() = 13;
			*(vector2f*)pAnmObj->pos() = moonPos[i];
			AnmInsSwitch(pAnmObj);
			HANM x;
			moonANM[i]=*InsertAnmObj(&x, pAnmObj);
			vector3f attPos = { ppos.x,ppos.y,0.0f };
			int index = SummonAttackCircle(PPLAYER2, 10.0, 0.0, &attPos, 99999, 45);
			moonAttObj[i] = PPLAYER2->attObjArr(index);
			moonVel[i] = { originalSpeed*cosf(angle),originalSpeed*sinf(angle)};
			angle = angle + angle_acc;
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
			auto pAnmObj = GetAnmObjFromHandle_F(moonANM[i]);
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
				EraseBullet_round(moonScale, &mps, 0, 999999, 0);
				EraseLaser_round(moonScale, &mps, 0, 0);
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
					float velacc_scale = 0.025f * 0.0078125f*max(1.0f,(timePassed.time_now_f-90.0f)/360.0f)*(dist/600.0f);
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
					ShakeStage(1,20,5, 5, 0, 0x5B);
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
								AnmIns5_switch(moonANM[i], 1);
								moonAttObj[i]->time.time_last = 0;
								moonAttObj[i]->time.time_now = 1;
								moonAttObj[i]->time.time_now_f = 1;
								moonAttObj[i] = nullptr;
							}
						}
						if (dist <= 50.0f && dist>=20.0f)
						{
							PPLAYER2->iPosition_fixed()->y -= moonVel[i].y * GAME_SPD * 128.0f * (0.7f*dist / 50.0f) + PPLAYER2->fVelocity_fixed()->y * (0.2+0.8*dist/50.0f);//another force
							PPLAYER2->iPosition_fixed()->x -= moonVel[i].x * GAME_SPD * 128.0f * (0.7f*dist / 50.0f) + PPLAYER2->fVelocity_fixed()->x * (0.2+0.8*dist/50.0f);
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

int __fastcall M_AddCard(DWORD thiz,DWORD edx, int ab_id, UINT getType)
//getType==0:dropped card    ==1 : in menu      ==2:market 
//thiz=0x4CF298
{
	if (ab_id <= 56)
	{
		return AddCard(thiz,ab_id,getType);
	}
	DWORD* pCardCount_whenGet = (DWORD*)(thiz + 0x28);
	AbCardBase* ptCard=nullptr;

	//here init the ptCard
	ptCard = new M_Moon();
	if (!ptCard)
		return -1;


	ptCard->id = ab_id;
	ptCard->count = *pCardCount_whenGet;
	ptCard->cardTypeFlag = getType & 1 | ptCard->cardTypeFlag & 0xFFFFFFFE;// type

	if ((getType & 1) == 0 && ptCard->Init() || (getType & 2) != 0 && ptCard->Init2()){
		delete ptCard;
		return *pCardCount_whenGet;
	}
	if (ab_id == 999)
	{
		ptCard->totCD = 1000;
	}

	//*(DWORD*)(thiz + 4 * ab_id + 0xC84) = 1;   set the card is equipped
	int (__thiscall *sb_412E90_cardList_insert)(DWORD thiz, DWORD pCard)=nullptr;
	VALUED(&sb_412E90_cardList_insert) = 0x412E90;
	sb_412E90_cardList_insert(thiz + 0x18, (DWORD)&(ptCard->pThis));
	VALUEV(thiz + 0x38,AbCardBase*) = ptCard;

	ptCard->cardTypeFlag |= 8;

	if ((ptCard->cardTypeFlag & 8) != 0){
		++VALUED(thiz + 0x2C);//activate card
	}else if ((ptCard->cardTypeFlag & 0x40) != 0){
		++VALUED(thiz + 0x30);//op card
	}else{
		++VALUED(thiz + 0x34);//passive card
	}

	//ptCard->pCardInfo = GetpCardInfoFromId(ptCard->id);
	static CardInShopInfo info = {0};
	info.cardId = 999;
	info.cardType = 3;
	info.cardName = "PACHE";
	info.spriteId_memu = 10;
	info.spriteId_stage = 11;
	auto iif = &info;
	ptCard->pCardInfo = &info;


	if (getType != 1)
	{
		void(__thiscall * sb_408D00_update_cardExhibit_stRight)(DWORD thiz, int a2)=nullptr;
		VALUED(&sb_408D00_update_cardExhibit_stRight) = 0x00408D00;
		sb_408D00_update_cardExhibit_stRight(thiz, 0);
	}
	return ++*pCardCount_whenGet;
}


void InjectAbCard()
{
	//{
	//	M_Moon mm;
	//	Address<DWORD>(0x004B5200).SetValue(*(DWORD*)((*(DWORD*)(&mm)) + 0x8));
	//	Address<DWORD>(0x004B5224).SetValue(*(DWORD*)((*(DWORD*)(&mm)) + 0x2C));
	//}
}
