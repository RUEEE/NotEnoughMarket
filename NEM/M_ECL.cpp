#include "pch.h"
#include "M_ANM.h"
#include "Address.h"
#include "THFunction.h"
#include <d3d9.h>


enum class EPP_Code
{
	DropCard=2000,
	PlayerSetPower=2500,
	PlayerSetPos=2501,

	AnmAllocVerticeBuffer=3000,
	AnmSetVertice=3001,
	AnmInitVerticeRectStrip_Enm=3002,
	AnmSetVerticeRectStrip_EnmMove =3003,
	AnmInitVertice_TriList_Enm=3004,
	AnmExplodeVertice_TriList_Enm=3005,
};

int __fastcall InsSwitchL_EPP(Enemy* pEnm,DWORD InsCode)
{
	ECL_ThreadObj* pThread = VALUEV(VALUED((DWORD)pEnm + 0x67B4)+0xC,ECL_ThreadObj*);
	switch ((EPP_Code)InsCode)
	{
	case EPP_Code::DropCard:
	{
		DropItem_F((Drop)ECL_GetArgI_Th(pThread, 0), (vector3f*)pEnm->positionFin(), 0, -PI / 2.0f, 1.0f, 0, 0, 0);
	}return 0;
	case EPP_Code::PlayerSetPower:
	{
		int power = ECL_GetArgI_Th(pThread, 0);
		if (power >= 400)power = 400; if (power < 0)power = 0;
		VALUED(0x004CCD38) = power;
		UpdateShot_F();
		
	}return 0;
	case EPP_Code::PlayerSetPos:
	{
		*PPLAYER2->iPosition_fixed() = { (int)(128 * ECL_GetArgF_Th(pThread, 0)) ,  (int)(128 * ECL_GetArgF_Th(pThread, 1)) };
	}return 0;
	case EPP_Code::AnmAllocVerticeBuffer:
	{
		AnmObj* pAnmObj = GetAnmObjFromHandle_F(VALUED((DWORD)pEnm+0x1350+4*ECL_GetArgI_Th(pThread, 0)));
		if (pAnmObj && ! (*pAnmObj->p_verticeArr()))
		{
			UINT n = ECL_GetArgI_Th(pThread, 1);
			UINT n_vertice = 0;
			switch (ECL_GetArgI_Th(pThread, 2))
			{
			case 0:
				*pAnmObj->p_verticeArr()=new Custom_vertex1[n_vertice=(n*2+2)];
				*pAnmObj->anm_globalVar_10000_10003(0) = n + 1;
				*pAnmObj->renderFlag() |= 12 << 26;;
				break;
			case 1:
				*pAnmObj->p_verticeArr() = new Custom_vertex1[n_vertice=(n*3)];
				*pAnmObj->anm_globalVar_10000_10003(0) = n;
				*pAnmObj->renderFlag() |= 28 << 26;;
				break;
			default:
				break;
			}
			*pAnmObj->anm_globalVar_10000_10003(1) = n_vertice;
			Custom_vertex1* buf = (Custom_vertex1*)(*pAnmObj->p_verticeArr());
			for (UINT i = 0; i < n_vertice; i++)
			{
				buf[i].pos = { 0,0,0 };
				buf[i].rhw = 1;
				buf[i].spec = 0xFFFFFFFF;
				buf[i].uv = { 0,0 };
			}
		}
	}return 0;
	case EPP_Code::AnmSetVertice:
	{
		AnmObj* pAnmObj = GetAnmObjFromHandle_F(VALUED((DWORD)pEnm + 0x1350 + 4 * ECL_GetArgI_Th(pThread, 0)));
		UINT n = ECL_GetArgI_Th(pThread, 1);
		if (pAnmObj && (*pAnmObj->p_verticeArr()) && n < *pAnmObj->anm_globalVar_10000_10003(1))
		{
			Custom_vertex1* buf = (Custom_vertex1*)(*pAnmObj->p_verticeArr());
			vector2i offs = VALUEV(0x0056ACBC,vector2i);
			buf[n].pos = {ECL_GetArgF_Th(pThread,2)+offs.x,ECL_GetArgF_Th(pThread,3)+offs.y,0.0f};
			buf[n].rhw = 1;
			*(int*)(&buf[n].spec) = ECL_GetArgI_Th(pThread, 4);
			buf[n].uv = { ECL_GetArgF_Th(pThread,5) ,ECL_GetArgF_Th(pThread,6) };
		}
		
	}return 0;
	case EPP_Code::AnmInitVerticeRectStrip_Enm:
	{
		//3002 SetAnmVerticeRectStrip_EnmMove(int s, float width)
		AnmObj* pAnmObj = GetAnmObjFromHandle_F(VALUED((DWORD)pEnm + 0x1350 + 4 * ECL_GetArgI_Th(pThread, 0)));
		if (pAnmObj && (*pAnmObj->p_verticeArr()))
		{
			*pAnmObj->anm_globalVar_10004_10007(1) = 0;
			UINT v_num = *pAnmObj->anm_globalVar_10000_10003(1);
			Custom_vertex1* buf = (Custom_vertex1*)(*pAnmObj->p_verticeArr());
			vector2i offsi = VALUEV(0x0056ACBC, vector2i);
			vector3f vpos = { pEnm->positionFin()->x + (float)offsi.x,pEnm->positionFin()->y + (float)offsi.y,0.0f };
			for (UINT i = 0; i < v_num; i++)
			{
				buf[i].pos = vpos;
				buf[i].rhw = 1.0f;
				buf[i].spec = 0xFFFFFFFF;
				buf[i].uv = { 0,0 };
			}
		}
	}return 0;
	case EPP_Code::AnmSetVerticeRectStrip_EnmMove:
	{
		//3002 SetAnmVerticeRectStrip_EnmMove(int s, float width)
		AnmObj* pAnmObj = GetAnmObjFromHandle_F(VALUED((DWORD)pEnm + 0x1350 + 4 * ECL_GetArgI_Th(pThread, 0)));
		if (pAnmObj && (*pAnmObj->p_verticeArr()))
		{
			UINT v_num = *pAnmObj->anm_globalVar_10000_10003(1);
			Custom_vertex1* buf = (Custom_vertex1*)(*pAnmObj->p_verticeArr());
			vector2i offsi = VALUEV(0x0056ACBC, vector2i); vector2f offs = { (float)offsi.x,(float)offsi.y };

			float move_tan = ECL_GetArgF_Th(pThread, 2);
			if (move_tan == -999999.0f)
				move_tan = ECL_GetGlobalArgF(pEnm, -9958);
			move_tan += PI / 2.0f;

			float width = ECL_GetArgF_Th(pThread, 1);
			float length = *pAnmObj->anm_globalVar_10004_10007(1);
			vector2f ofs_width = { width * cosf(move_tan) * 0.5f,width * sinf(move_tan) * 0.5f };

			Custom_vertex1 v0 = buf[0];
			Custom_vertex1 v1 = buf[1];
			vector3f b0 = buf[v_num-4].pos; vector3f b1 = buf[v_num-3].pos;
			vector3f b2 = buf[v_num-2].pos; vector3f b3 = buf[v_num-1].pos;
			//length -= hypotf(0.5f * (b0.x + b1.x - b2.x - b3.x), 0.5f * (b0.y + b1.y - b2.y - b3.y));
			buf[0].pos = { pEnm->positionFin()->x + offs.x + ofs_width.x,pEnm->positionFin()->y + offs.y + ofs_width.y,0.0f };
			buf[1].pos = { pEnm->positionFin()->x + offs.x - ofs_width.x,pEnm->positionFin()->y + offs.y - ofs_width.y,0.0f };
			buf[0].uv = { 0.0f,0.0f };
			buf[1].uv = { 0.0f,1.0f };
			b0 = buf[0].pos; b1 = buf[1].pos;
			b2 = buf[2].pos; b3 = buf[3].pos;
			//length += hypotf(0.5f * (b0.x + b1.x - b2.x - b3.x), 0.5f * (b0.y + b1.y - b2.y - b3.y));
			//lenth(t)~=length(t-1)
			if (length <= 0)
				length = 0.5f;
			length = 1.0f / length;
			float l2 = 0.0f;
			//calculate the length of the curved line
			for (UINT i = 2; i < v_num; i += 2)
			{
				Custom_vertex1 v2 = buf[i];
				Custom_vertex1 v3 = buf[i + 1];
				buf[i] = v0;
				buf[i + 1] = v1;
				v0 = v2; v1 = v3;//swap the vertice
				b0 = buf[i - 2].pos; b1 = buf[i - 1].pos;
				b2 = buf[i].pos; b3 = buf[i + 1].pos;
				l2 += hypotf(0.5f * (b0.x + b1.x - b2.x - b3.x), 0.5f * (b0.y + b1.y - b2.y - b3.y));
				float uvx=l2*length;
				buf[i].uv = { uvx,0 };
				buf[i + 1].uv = { uvx,1 };
			}
			*pAnmObj->anm_globalVar_10004_10007(1) = l2;
		}
	}return 0;
	case EPP_Code::AnmInitVertice_TriList_Enm:
	{
		//3004 InitAnmVertice_TriList_Enm(int s, int n, int m, float w, float h)
		AnmObj* pAnmObj = GetAnmObjFromHandle_F(VALUED((DWORD)pEnm + 0x1350 + 4 * ECL_GetArgI_Th(pThread, 0)));
		if (pAnmObj && (*pAnmObj->p_verticeArr()))
		{
			Custom_vertex1* buf = (Custom_vertex1*)(*pAnmObj->p_verticeArr());
			UINT v_num = *pAnmObj->anm_globalVar_10000_10003(1);
			UINT p_num = *pAnmObj->anm_globalVar_10000_10003(0);
			UINT n = ECL_GetArgI_Th(pThread, 1);
			UINT m = ECL_GetArgI_Th(pThread, 2);
			if (p_num >= n * m * 2)
			{
				float w = ECL_GetArgF_Th(pThread, 3);
				float h = ECL_GetArgF_Th(pThread, 4);
				float u_inc = 1.0f / (float)(n);
				float v_inc = 1.0f / (float)(m);
				float x_inc = w / (float)(n);
				float y_inc = h / (float)(m);
				vector2i offsi = VALUEV(0x0056ACBC, vector2i); vector2f offs = { (float)offsi.x,(float)offsi.y };
				vector2f posLT = { pEnm->positionFin()->x + offs.x - w / 2.0f,pEnm->positionFin()->y +offs.y-h/2.0f};
				vector2f uv = { 0.0f,0.0f };
				vector3f pos = { posLT.x,posLT.y,0.0f };
				for (UINT i = 0; i < n; i++)
				{
					uv.x = 0.0f;
					pos.x = posLT.x;
					for (UINT j = 0; j < m; j++)
					{
						UINT index = (i*m+j)*2*3;
						buf[index].pos = pos;buf[index].rhw = 1.0f;buf[index].spec = 0xFFFFFFFF;buf[index].uv = uv;
						index++;
						buf[index].pos = { pos.x + x_inc,pos.y,0.0f }; buf[index].rhw = 1.0f; buf[index].spec = 0xFFFFFFFF; buf[index].uv = { uv.x + u_inc ,uv.y};
						index++;
						buf[index].pos = { pos.x,pos.y+y_inc,0.0f }; buf[index].rhw = 1.0f; buf[index].spec = 0xFFFFFFFF; buf[index].uv ={ uv.x ,uv.y+v_inc };
						index++;
						buf[index].pos = { pos.x + x_inc,pos.y,0.0f }; buf[index].rhw = 1.0f; buf[index].spec = 0xFFFFFFFF; buf[index].uv = { uv.x + u_inc ,uv.y };
						index++;
						buf[index].pos = { pos.x,pos.y + y_inc,0.0f }; buf[index].rhw = 1.0f; buf[index].spec = 0xFFFFFFFF; buf[index].uv = { uv.x ,uv.y + v_inc };
						index++;
						buf[index].pos = { pos.x + x_inc,pos.y + y_inc,0.0f }; buf[index].rhw = 1.0f; buf[index].spec = 0xFFFFFFFF; buf[index].uv = { uv.x + u_inc ,uv.y + v_inc };
						pos.x += x_inc;
						uv.x += u_inc;
					}
					pos.y += y_inc;
					uv.y += u_inc;
				}
			}
		}
	}return 0;
	case EPP_Code::AnmExplodeVertice_TriList_Enm:
	{
		//3005 ExplodeAnmVertice_TriList_Enm(int s,int rnd_seed,float omega,float rnd_omega,float speed,float rnd_speed)
		AnmObj* pAnmObj = GetAnmObjFromHandle_F(VALUED((DWORD)pEnm + 0x1350 + 4 * ECL_GetArgI_Th(pThread, 0)));
		if (pAnmObj && (*pAnmObj->p_verticeArr()))
		{
			int seed[2] = {ECL_GetArgI_Th(pThread,1),0};
			float omega = ECL_GetArgF_Th(pThread, 2);
			float rOmega = ECL_GetArgF_Th(pThread, 3);
			float speed = ECL_GetArgF_Th(pThread, 4);
			float rSpeed = ECL_GetArgF_Th(pThread, 5);
			Custom_vertex1* buf = (Custom_vertex1*)(*pAnmObj->p_verticeArr());
			UINT v_num = *pAnmObj->anm_globalVar_10000_10003(1);
			UINT p_num = *pAnmObj->anm_globalVar_10000_10003(0);
			vector2i offsi = VALUEV(0x0056ACBC, vector2i);
			vector2f enmPosOffsed = { pEnm->positionFin()->x + (float)offsi.x ,pEnm->positionFin()->y + (float)offsi.y };
			for (UINT i = 0; i < v_num;i+=3)
			{
				Custom_vertex1& v1=buf[i];
				Custom_vertex1& v2=buf[i+1];
				Custom_vertex1& v3=buf[i+2];
				vector2f midPoint = { (v1.pos.x + v2.pos.x + v3.pos.x) * 0.33333333f,(v1.pos.y + v2.pos.y + v3.pos.y) * 0.33333333f };
				float angle = omega + rOmega*GetRndFloat11_Seed(seed);
				float speedFin = speed + rSpeed * GetRndFloat11_Seed(seed);
				vector2f vAngle = { cosf(angle),sinf(angle) };
				vector2f rv = {};
				rv = RotateVec({ v1.pos.x,v1.pos.y }, midPoint, vAngle); v1.pos.x = rv.x; v1.pos.y = rv.y;
				rv = RotateVec({ v2.pos.x,v2.pos.y }, midPoint, vAngle); v2.pos.x = rv.x; v2.pos.y = rv.y;
				rv = RotateVec({ v3.pos.x,v3.pos.y }, midPoint, vAngle); v3.pos.x = rv.x; v3.pos.y = rv.y;
				vector2f dif = { midPoint.x - enmPosOffsed.x,midPoint.y - enmPosOffsed.y };
				float dis = hypotf(dif.x, dif.y);
				dif.x *= speedFin/dis; dif.y *= speedFin/dis;
				v1.pos.x += dif.x;v1.pos.y += dif.y;
				v2.pos.x += dif.x;v2.pos.y += dif.y;
				v3.pos.x += dif.x;v3.pos.y += dif.y;
			}
		}
	}return 0;
	default:
		return 0;
	}
	return 0;
}

int __declspec(naked) InsSwitchL_InjectCall()
{
	__asm
	{
		mov ecx,edi//edi=this=pEnm+122C
		sub ecx,0x122C
		//mov edx,edx//edx=inscode
		mov eax,InsSwitchL_EPP
		call eax

		mov ecx, [ebp - 0x0C]
		mov fs : [00000000] , ecx
		pop ecx
		pop edi
		pop esi
		mov ecx, [ebp - 0x14]
		xor ecx, ebp
		mov eax, 0x0048DC60
		call eax
		mov esp, ebp
		pop ebp
		mov esp, ebx
		pop ebx
		ret

	}
}

void InjectECL()
{
	Address<DWORD>(0x00430DC9).SetValue((DWORD)InsSwitchL_InjectCall-0x00430DCD);
}
