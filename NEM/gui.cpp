#include "gui.h"
#include "pch.h"
#include "..\imgui\imgui.h"
#include "..\imgui\imgui_impl_dx9.h"
#include "..\imgui\imgui_impl_win32.h"

#include "Address.h"
#include "ThStructure.h"

#include <d3d9.h>


#define COL_HITBOX_BULLET_FILL 0x300000FF
#define COL_HITBOX_BULLET 0xFF0000FF

#define COL_HITBOX_ATTACK_FILL 0x10FF0000
#define COL_HITBOX_ATTACK 0xFFFF0000
extern HWND wind_hwnd;

int(__stdcall* playSE)(int id, int a2)
	= (int(__stdcall*)(int, int))(0x476BE0);

vector2f GetStageFromClient(vector2f client,vector2f client_sz)
{
	float x, y;
	x = client.x / client_sz.x;
	y = client.y/ client_sz.y;
	x = (x - 0.05) /0.6;
	y = (y - 0.03333) * 1.07142857;
	x = (x - 0.5) * 384.0;
	y = y * 448.0;
	return vector2f{ x,y };
}

vector2f GetClientFromStage(vector2f stage, vector2f client_sz)
{
	float x = stage.x;
	float y = stage.y;

	y=y / 448.0;
	x = (x / 384.0) + 0.5;
	y = y / 1.07142857 + 0.03333;
	x = x * 0.6 + 0.05;

	return vector2f{ x* client_sz.x,y* client_sz.y };
}

float GetClientFromStage(float x, vector2f client_sz)
{
	return x*client_sz.x/640.0f;//1280*960
}

void DrawAnmObj(vector2f clientSz)//not finished
{
	auto p = ImGui::GetOverlayDrawList();
	DWORD iter = VALUED(0x0051F65C)+0x6F0;
	iter = VALUED(iter);
	while (iter)
	{
		DWORD pobj=VALUED(iter);
		//
		//	float x = VALUEF(pobj + 0x5F0)+VALUEF(pobj+0x4E4)+VALUEF(pobj+0x30);
		//	float y = VALUEF(pobj + 0x5F4)+VALUEF(pobj+0x4E8)+VALUEF(pobj+0x34);
		//	//ImVec2 cen{ x,y };
		//	ImVec2 cen = GetClientFromStage({ x,y }, clientSz);
		//	if (x == 0.0f && y == 0.0f)
		//	{
		//		//goto a;
		//	}
		//	p->AddCircle(cen, 20.0f, 0xFF0000FF, 5);
		float x1 = VALUEF(pobj + 0x4F0);
		float y1 = VALUEF(pobj + 0x4F4);
		
		float x2 = VALUEF(pobj + 0x4FC);
		float y2 = VALUEF(pobj + 0x500);
		
		float x3 = VALUEF(pobj + 0x508);
		float y3 = VALUEF(pobj + 0x50C);
		
		float x4 = VALUEF(pobj + 0x514);
		float y4 = VALUEF(pobj + 0x518);
		ImVec2 pt[4] = { {x1,y1},{x2,y2},{x4,y4},{x3,y3} };
		int flag =VALUED(pobj + 0x538);
		if ((!(flag & (1 << 21))) && (flag & (1 << 20)))
		{
			for (int i = 0; i < 4; i++)
			{
				pt[i].x -= VALUED(0x56ACBC);
				pt[i].y -= VALUED(0x56ACC0);

				pt[i].x *= VALUEF(0x0056ACA0);
				pt[i].y *= VALUEF(0x0056ACA0);

				pt[i].x += 64+384;
				pt[i].y += 32;//!!!!
			}
		}
		//ImVec2 cen = GetClientFromStage({ x,y }, clientSz);
		p->AddPolyline(pt, 4, 0xFF00FFFF, ImDrawFlags_Closed, 1.0f);

		iter = VALUED(iter + 4);
	}
}

void DrawAttackHitBox(vector2f clientSz) {
		auto p = ImGui::GetOverlayDrawList();
		//[004CF410]+20574
		DWORD ppl = *(DWORD*)(0x4CF410);
		if (ppl)
		{
			DWORD iter = ppl + 0x20574;
			for (int i = 0; i < 1024; i++)
			{
				int flag = *(DWORD*)(iter + 0x0);
				if (flag & 1)
				{
					float x = *(float*)(iter + 0x1C);
					float y = *(float*)(iter + 0x20);
					ImVec2 cen = GetClientFromStage({ x,y }, clientSz);
					if (flag & 2)//round
					{
						float r = *(float*)(iter + 0x4);
						p->AddCircleFilled(cen, GetClientFromStage(r, clientSz), COL_HITBOX_ATTACK_FILL,8);
						p->AddCircle(cen, GetClientFromStage(r, clientSz), COL_HITBOX_ATTACK,8);
					}else{
						float w = *(float*)(iter + 0x14);
						float h = *(float*)(iter + 0x18);
						float a = *(float*)(iter + 0xC);
						float wx = w * cosf(-a),
							wy = w * sinf(-a),
							hx = h * cosf(-a),
							hy = h * sinf(-a);
						ImVec2 v[4];
						v[0] = { x + wx /2 +hy/2, y - wy/2+ hx/2};
						v[1] = { x - wx / 2+hy/2, y + wy/2+ hx/2};
						v[3] = { v[0].x - hy, v[0].y - hx};
						v[2] = { v[1].x - hy,v[1].y - hx};
						v[0] = GetClientFromStage(v[0], clientSz);
						v[1] = GetClientFromStage(v[1], clientSz);
						v[2] = GetClientFromStage(v[2], clientSz);
						v[3] = GetClientFromStage(v[3], clientSz);
						p->AddPolyline(v, 4, COL_HITBOX_ATTACK, ImDrawFlags_::ImDrawFlags_Closed, 1.0);
						p->AddConvexPolyFilled(v, 4, COL_HITBOX_ATTACK_FILL);
					}
				}
				iter = iter + 0x9C;
			}
		}
}

void DrawBulletHitBox(vector2f clientSz)
{
	auto dl = ImGui::GetOverlayDrawList();
	DWORD pbtx = VALUED(0x004CF2BC);
	if (pbtx)
	{
		DWORD iter = VALUED(pbtx + 0xC0);
		while (iter)
		{
			DWORD pbt = VALUED(iter);
			float len = 20.0f;
			vector2f vec{ VALUEF(pbt + 0x638),VALUEF(pbt + 0x63C) };
			vector2f vec2(VALUEF(pbt + 0x638) + VALUEF(pbt + 0x644)* len,
				VALUEF(pbt + 0x63C) + VALUEF(pbt + 0x648)* len);

			float hit = VALUEF(pbt + 0x658);
			hit = GetClientFromStage(hit, clientSz);
			vec = GetClientFromStage(vec, clientSz);
			vec2 = GetClientFromStage(vec2, clientSz);
			dl->AddCircle(vec, hit, COL_HITBOX_BULLET, 8, 1.0f);
			dl->AddCircleFilled(vec, hit, COL_HITBOX_BULLET_FILL, 8);

			dl->AddLine(vec, vec2, COL_HITBOX_BULLET,1.0f);

			iter = VALUED(iter + 0x4);
		}
	}
}

void SetGUI(IDirect3DDevice9* device)
{
	return;
	//ImGui::ShowDemoWindow();
	auto mousePos = ImGui::GetMousePos();
	vector2f clientSz= ImGui::GetMainViewport()->Size;

	vector2f smouse;
	smouse = GetStageFromClient(vector2f{(float)mousePos.x,(float)mousePos.y }, clientSz);
	ImGui::Begin("test");
	static char subName[100] = { 0 };
	if(subName[0]=='\0')
		wsprintfA(subName, "Lily");
	ImGui::InputText("name", subName, 100);
	ImGui::Text("mousePos: %f,%f", smouse.x, smouse.y);

	static int life=20;
	ImGui::InputInt("life", &life, 1);
	static int drop = 4;
	ImGui::InputInt("drop", &drop, 1);
	static int i1[4] = { 1,2,3,4 };
	ImGui::InputInt4("iiii", i1);
	static float f1[4] = { 1,2,3,4 };
	ImGui::InputFloat4("ffff", f1);

	static int SE = 1;
	ImGui::InputInt("SE", &SE, 1);
	if (ImGui::Button("play SE"))
	{
		playSE(SE, 0);
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && fabsf(smouse.x)<180.0)
	{
		void (_fastcall *summon)(int thiz,int edx,char* name,EnemyInfo_init* info,int a)=nullptr;
		*(DWORD*)(&summon) = (0x0042D7D0);
		EnemyInfo_init info = { 0 };
		info.drop = (Drop)drop;
		info.hp = life;
		info.pos.x = smouse.x;
		info.pos.y = smouse.y;
		if(*(DWORD*)(0x004CF2D0))
			summon(*(DWORD*)(0x004CF2D0), 0, subName, &info, 0);
		//void (_fastcall *drop)(int thiz,int zero,
		//	int type, vector3f * a3
		//	, int a4, float dir, float spd, int a7, int a8, int a9) =nullptr;
		//*(DWORD*)(&drop) = (0x00446F40);
		//vector3f v{ msx,msy,0 };
		//float d = 0;
		//for (int i = 0; i < 50; i++)
		//{
		//	d += 6.28 / 50;
		//	drop(*(DWORD*)(0x4CF2EC), 0, 7, &v, 20,d, f1[1], i1[0], i1[1], i1[2]);
		//}
		//int (*c)();
		//*(DWORD*)&c = 0x40D2E0;
		//c();
	}
	static bool isDrawAttackHitBox = true;
	static bool isDrawAnmObj = false;
	static bool isDrawBulletHitBox = true;
	ImGui::Checkbox("drawAttackHitBox", &isDrawAttackHitBox);
	ImGui::Checkbox("drawAnmObj", &isDrawAnmObj);
	ImGui::Checkbox("drawBulletHitBox", &isDrawBulletHitBox);
	if(isDrawAttackHitBox)
		DrawAttackHitBox(clientSz);
	if (isDrawAnmObj)
		DrawAnmObj(clientSz);
	if (isDrawBulletHitBox)
		DrawBulletHitBox(clientSz);
	//
	ImGui::End();
}