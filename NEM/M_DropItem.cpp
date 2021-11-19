#include "pch.h"
#include "M_DropItem.h"
#include "Address.h"
#include "injection.h"
#include "THFunction.h"
#include <map>



//thiz=VALUED(0x004CF2EC)

int(__thiscall* original_DropItem)(DWORD thiz, DWORD type, vector3f* pos, int a4, float dir, float spd, int delay, int a8, int a9)
	= (int(__thiscall*)(DWORD thiz, DWORD type, vector3f * pos, int a4, float dir, float spd, int delay, int a8, int a9))(0x446EF7);

std::map<int, std::pair<int,int>> drop_type_script;//pair<script,show_script>
std::map<int, int> drop_type_cardId;
int __fastcall M_DropItem(int thiz,int edx, 
int type, vector3f* pos, int a4, float dir, float spd, int delay, int a8, int a9)
{
	
	if (type < 20 && type>0)
	{
		return original_DropItem(thiz,type,pos,a4,dir,spd,delay,a8,a9);
	}
	DWORD pDrop2 = VALUED(thiz + 0xE6BAF8);
	if (pDrop2 && (drop_type_script.count(type)!=0) || (drop_type_cardId.count(type)!=0))
	{
		VALUED((pDrop2 + 0xC74)) = 1;//set the move type

		VALUEV((pDrop2 + 0xC2C), vector3f) = *pos;
		if (VALUEF(pDrop2 + 0xC2C) > 192.0f)
			VALUEF(pDrop2 + 0xC2C) = 192.0f;
		else if((VALUEF(pDrop2 + 0xC2C) < -192.0f))
			VALUEF(pDrop2 + 0xC2C) = -192.0f;
		//set the pos
		VALUEF(pDrop2 + 0xC38) = spd * cos(dir);
		VALUEF(pDrop2 + 0xC3C) = spd * sin(dir);
		VALUEF(pDrop2 + 0xC40) = 0;
		//set the speed
		int(__thiscall * setTime)(int, int) = nullptr;
		*(DWORD*)&setTime = 0x405D10;
		setTime(pDrop2 + 0xC4C, 0);
		//set time
		VALUED(pDrop2 + 0xC44) = 0;
		VALUED(pDrop2 + 0xC80) = 0;
		VALUED(pDrop2 + 0xC84) = delay;
		VALUED(pDrop2 + 0xC90) = -1;
		if (delay <= 0)
		{
			int(__thiscall * drop_effect_and_SE)(int) = nullptr;
			*(DWORD*)&drop_effect_and_SE = 0x4472B0;
			VALUED(pDrop2 + 0xC78) = 16;//use the life peice card effect
			drop_effect_and_SE(pDrop2);
		}
		VALUED(pDrop2 + 0xC78) = type;
		if ((drop_type_cardId.count(type) != 0))
		{
			int scriptId = drop_type_cardId[type];
			VALUED(pDrop2 + 0x544) &= 0xFFFFFFFE;
			VALUED(pDrop2 + 0xB50) &= 0xFFFFFFFE;
			VALUED(pDrop2 + 0x3C) = -1;
			VALUED(pDrop2 + 0x648) = -1;
			int* (__stdcall * create_dropItemCard_ANM)(int*, int) = nullptr;
			*(DWORD*)&create_dropItemCard_ANM = 0x409310;
			int tmp;
			VALUED(pDrop2 + 0xC28) = *create_dropItemCard_ANM(&tmp, scriptId);

			int(__thiscall * set_anmPos_from_handle)(int, vector3f*) = nullptr;
			*(DWORD*)&set_anmPos_from_handle = 0x488F70;
			set_anmPos_from_handle(pDrop2 + 0xC28, (vector3f*)(pDrop2 + 0xC2C));

			int(__thiscall * set_anmColor_from_handle)(int, DWORD) = nullptr;
			*(DWORD*)&set_anmColor_from_handle = 0x4892F0;
			set_anmColor_from_handle(pDrop2 + 0xC28, 0xFFFFFFFF);
		}
		else
		{
			int(__thiscall * attach_anmScript_play)(int, int, int)=nullptr;
			VALUED(&attach_anmScript_play) = 0x41F920;
			attach_anmScript_play(VALUED(VALUED(0x004CF2BC)+0x7A41EC)
				, pDrop2 + 0x10,drop_type_script[type].first);
			attach_anmScript_play(VALUED(VALUED(0x004CF2BC) + 0x7A41EC),
				pDrop2 + 0x61C,drop_type_script[type].second);
			VALUED(pDrop2 + 0xC28) = 0;
			VALUED(pDrop2 + 0x534) = -1;
		}
		VALUED(pDrop2 + 0xC8C) = 0;
		void(__thiscall * DroplistChangeIter)(int) = nullptr;
		VALUED(&DroplistChangeIter) = 0x404F20;
		DroplistChangeIter(pDrop2);
		return pDrop2;
	}
	return 0;
}

void __stdcall M_ItemSwitchDefault(int type)
{
	if (type <= 0)//ability card drops
	{
		if (!IsCardEquipped(-type))
		{
			AddCard(VALUED(0x004CF298), -type, 2); 
			UpdateShot_F();
		}
	}
	else
	{
		switch (type)
		{
		case 21:
			VALUED(PPLAYER + 0x476AC) = 4;
			//VALUED(PPLAYER + 0x62C) = (int)(getRndFloat11_Rep() * 192 * 128);
			//VALUED(PPLAYER + 0x630) = (int)(getRndFloat01_Rep() * 448 * 128);
			break;
		default:
			break;
		}
	}

}

void InjectDropItem()
{
	//set the global vars
	for (int i = 0; i < 56; i++)
	{
		drop_type_cardId[-i] = i;
	}
	//drop_type_script[21] = std::make_pair(0x70,0x86);//bullet.anm
	drop_type_script[21] = std::make_pair(0x74,0x86);//bullet.anm
	//..................
	Address<DWORD>(0x446EF7).SetValue(0x51EC8B55);
	Address<DWORD>(0x446EFB).SetValue(0xEB575653);
	Address<BYTE>(0x446EFF).SetValue(0x47);
	//set the original call
	Address<BYTE>(0x446F40).SetValue(0xB8);
	Address<DWORD>(0x446F41).SetValue((DWORD)M_DropItem);
	Address<WORD>(0x446F45).SetValue(0xE0FF);
	//end the DropItem call
	Address<DWORD>(0x44626D).SetValue(0xFFFFF424);
	//switch
	Address<DWORD>(0x445695).SetValue(0x00B85260);
	Address<DWORD>(0x445698).SetValue((DWORD)M_ItemSwitchDefault);
	Address<DWORD>(0x44569C).SetValue(0x06EBD0FF);
	Address<WORD>(0x4456A6).SetValue(0xE961);
	Address<DWORD>(0x4456A8).SetValue(0x00000ED8);
	//call
}