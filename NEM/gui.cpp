#include "gui.h"
#include "pch.h"
#include "..\imgui\imgui.h"
#include "..\imgui\imgui_impl_dx9.h"
#include "..\imgui\imgui_impl_win32.h"

#include "Address.h"
#include "ThStructure.h"

#include <d3d9.h>
#include <d3dx9.h>


#define COL_HITBOX_BULLET_FILL 0x300000FF
#define COL_HITBOX_BULLET 0xFF0000FF

#define COL_HITBOX_ATTACK_FILL 0x10FF0000
#define COL_HITBOX_ATTACK 0xFFFF0000

#define COL_HITBOX_ENM_HIT 0xFF00FF00
#define COL_HITBOX_ENM_ATT 0xFFFF00FF


extern HWND wind_hwnd;

int(__stdcall* playSE)(int id, int a2)
	= (int(__stdcall*)(int, int))(0x476BE0);
int(__thiscall* addCard)(DWORD thiz, int ab_id, int a3)
	= (int(__thiscall*)(DWORD, int,int))(0x00411460);
void (__thiscall *clearCard)(DWORD thiz, int a2)
	= (void(__thiscall*)(DWORD, int))(0x407DA0);
int (__thiscall *update_shot)(DWORD thiz, int a2)
	= (int(__thiscall*)(DWORD, int))(0x45D5E0);
//update_shot(PPLAYER + 0x620, 0);

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
	DWORD iter0 = VALUED(0x0051F65C) + 0x6F0;
	for(int i=0;i<2;i++){
		DWORD iter = iter0;
		iter0 += 8;


		auto p = ImGui::GetOverlayDrawList();
		iter = VALUED(iter);
		while (iter)
		{
			DWORD pobj = VALUED(iter);
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
			int flag = VALUED(pobj + 0x538);
			if ((!(flag & (1 << 21))) && (flag & (1 << 20)))
			{
				for (int i = 0; i < 4; i++)
				{
					pt[i].x -= VALUED(0x56ACBC);
					pt[i].y -= VALUED(0x56ACC0);

					pt[i].x *= VALUEF(0x0056ACA0);
					pt[i].y *= VALUEF(0x0056ACA0);

					pt[i].x += 64 + 384;
					pt[i].y += 32;//!!!!
				}
			}
			//ImVec2 cen = GetClientFromStage({ x,y }, clientSz);
			if(i==0)
				p->AddPolyline(pt, 4, 0xFF00FFFF, ImDrawFlags_Closed, 1.0f);
			else
				p->AddPolyline(pt, 4, 0xFF00FFCC, ImDrawFlags_Closed, 1.0f);
			iter = VALUED(iter + 4);
		}
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
				AttackObj* att = (AttackObj*)iter;
				if (att->is_used())
				{
					float x = *(float*)(iter + 0x1C);
					float y = *(float*)(iter + 0x20);
					ImVec2 cen = GetClientFromStage({ x,y }, clientSz);
					if (att->is_round_hitbox())//round
					{
						float r = att->att_radio.x;
						p->AddCircleFilled(cen, GetClientFromStage(r, clientSz), COL_HITBOX_ATTACK_FILL,8);
						p->AddCircle(cen, GetClientFromStage(r, clientSz), COL_HITBOX_ATTACK,8);
					}else{
						float w = att->att_box.x;
						float h = att->att_box.y;
						float a = att->angle;
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

void DrawEnemyHitbox(vector2f clientSz)
{
	auto p = ImGui::GetOverlayDrawList();
	//[004CF410]+20574
	DWORD penm = *(DWORD*)(0x004CF2D0);
	if (penm)
	{
		DWORD iter= VALUED(penm + 0x18C);
		while (iter)
		{
			DWORD pEnm = VALUED(iter);
			{
				DWORD flag = VALUED(pEnm + 0x635C);
				vector2f hit = VALUEV(pEnm + 0x1340, vector2f);
				vector2f att = VALUEV(pEnm + 0x1340, vector2f);
				vector2f pos = VALUEV(pEnm + 0x1270, vector2f);

				vector2f p1 = { pos.x - att.x * 0.5f,pos.y - att.y * 0.5f };
				vector2f p2 = { pos.x + att.x * 0.5f,pos.y + att.y * 0.5f };
				p1 = GetClientFromStage(p1, clientSz);
				p2 = GetClientFromStage(p2, clientSz);
				p->AddRect(p1,p2,COL_HITBOX_ENM_ATT);
				p1 = { pos.x - hit.x * 0.5f,pos.y - hit.y * 0.5f };
				p2 = { pos.x + hit.x * 0.5f,pos.y + hit.y * 0.5f };
				p1 = GetClientFromStage(p1, clientSz);
				p2 = GetClientFromStage(p2, clientSz);
				p->AddRect(p1, p2, COL_HITBOX_ENM_HIT);
			}
			iter = VALUED(iter + 0x4);
		}
	}
}

int DrawBulletHitBox(vector2f clientSz)
{
	auto dl = ImGui::GetOverlayDrawList();
	int sum = 0;
	DWORD pbtx = VALUED(0x004CF2BC);
	if (pbtx)
	{
		DWORD iter = VALUED(pbtx + 0xC0);
		while (iter)
		{
			sum++;
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
	return sum;
}

void SetGUI(IDirect3DDevice9* device)
{
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

	static int id_card = 1;
	ImGui::InputInt("cardId", &id_card, 1);
	if (ImGui::Button("getCard"))
		addCard(VALUED(0x004CF298), id_card, 2);
	if (ImGui::Button("clearCard"))
		clearCard(VALUED(0x004CF298),0);
	//the cards to test
	if (ImGui::Button("UFO"))
		addCard(VALUED(0x004CF298), 20, 2);
	if (ImGui::Button("door"))
		addCard(VALUED(0x004CF298), 19, 2);
	if (ImGui::Button("update shoot"))
		update_shot(PPLAYER+0x620, 0);

	static bool keyDown_spawn = true;
	ImGui::Checkbox("keyDown_spawn", &keyDown_spawn);
	if (keyDown_spawn && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && fabsf(smouse.x)<180.0)
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
	}
	static bool isDrawAttackHitBox = true;
	static bool isDrawAnmObj = false;
	static bool isDrawBulletHitBox = true;
	static bool isDrawEnemy = true;
	ImGui::Checkbox("drawAttackHitBox", &isDrawAttackHitBox);
	ImGui::Checkbox("drawAnmObj", &isDrawAnmObj);
	ImGui::Checkbox("drawBulletHitBox", &isDrawBulletHitBox);
	ImGui::Checkbox("drawEnemy", &isDrawEnemy);
	int bt = 0;
	if(isDrawAttackHitBox)
		DrawAttackHitBox(clientSz);
	if (isDrawAnmObj)
		DrawAnmObj(clientSz);
	if (isDrawBulletHitBox)
	{
		bt = DrawBulletHitBox(clientSz);
		ImGui::Text("bullet count:%d", bt);
	}
	if (isDrawEnemy)
	{
		DrawEnemyHitbox(clientSz);
	}


	//tryFire
	/*void(__stdcall * sb_40A9C0_AbOp_TryFire)
		(DWORD thiz, int time_m15, int time_tot, int slot);
	*(DWORD*)&sb_40A9C0_AbOp_TryFire = 0x40A9C0;
	static int tstsht[2] = {0,17};
	static int time=1;
	ImGui::DragInt2("Op,slot", tstsht);
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && fabsf(smouse.x) < 180.0)
	{
		time++;
		sb_40A9C0_AbOp_TryFire(PPLAYER+0xA30+tstsht[0]*0xF0,
			time % 15, time, tstsht[1]);
	}*/
	//setPower
	static int power = 300;
	ImGui::DragInt("power", &power, 2.0f, 0, 400);
	if (ImGui::Button("setPower"))
	{
		VALUED(0x004CCD38) = power;
		update_shot(PPLAYER + 0x620, 0);
	}
	//texture test
	/*{
		static int id_tex[2] = { 9,0 };
		IDirect3DBaseTexture9* tex;
		static bool is_draw = false;
		ImGui::Checkbox("shit", &is_draw);
		ImGui::InputInt2("id_tex", id_tex);
		if (is_draw)
		{
			if (VALUED(VALUED(0x0051F65C) + 4 * id_tex[0] + 0x312072C))
			{
				*(DWORD*)(&tex) = VALUED(VALUED(VALUED(VALUED(0x0051F65C) + 4 * id_tex[0] + 0x312072C) + 0x124)
					+ 0x18 * (BYTE)id_tex[1]);
				device->SetTexture(0, tex);
				Custom_vertex1 v[5] =
				{
					{{0,0,0},1,0xFFFFFFFF,{0,0}},
					{{300,0,0},1,0xFFFFFFFF,{1,0}},
					{{0,300,0},1,0xFFFFFFFF,{0,1}},
					{{300,300,0},1,0xFFFFFFFF,{1,1}}
				};
				device->SetFVF(FVF_VERTEX_1);
				device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Custom_vertex1));
			}
		}
	}
	*/
	
	//3D test
	{
		static bool is_disable_cameraMove = false;
		if (ImGui::Checkbox("disableMoveCamera", &is_disable_cameraMove))
		{
			if (is_disable_cameraMove)
			{
				Address<short>(0x41C245).SetValue(0x9090);
				Address<short>(0x41C306).SetValue(0x9090);
				Address<short>(0x41C77A).SetValue(0x9090);
				//stop camera
				Address<short>(0x41D132).SetValue(0x9090);
				Address<BYTE>(0x41D14A).SetValue(0x0);
				//no fog
				vector3f* up = (vector3f*)(0x4CCDF0 + 0x688 + 0x18);
				up->y = 0;
				up->z = -1.0f;
				//set the fucking up
			}
			else
			{
				Address<short>(0x41C245).SetValue(0xA5F3);
				Address<short>(0x41C306).SetValue(0xA5F3);
				Address<short>(0x41C77A).SetValue(0xA5F3);

				Address<short>(0x41D132).SetValue(0x2774);
				Address<BYTE>(0x41D14A).SetValue(0x1);

				vector3f* up = (vector3f*)(0x4CCDF0 + 0x688 + 0x18);
				up->y = 1;
				up->z = 0.0f;
			}
		}
		vector3f* lookAt = (vector3f*)(0x4CCDF0 + 0x688 + 0x24);
		static vector2f mousePos_ori = { 0,0 };
		if (is_disable_cameraMove)
		{
			if (ImGui::IsKeyDown('E'))
			{
				if (mousePos_ori.x == 0)
				{
					mousePos_ori = ImGui::GetMousePos();
				}
				vector2f del{ mousePos.x - mousePos_ori.x,mousePos.y - mousePos_ori.y };
				mousePos_ori = mousePos;
				D3DXMATRIX mx, my;
				D3DXMatrixRotationZ(&mx, -del.x * 0.005);
				D3DXMatrixRotationX(&my, (lookAt->y > 0 ? 1 : -1) * del.y * 0.005);
				D3DXVec3Transform((D3DXVECTOR4*)lookAt, (D3DXVECTOR3*)lookAt, &mx);
				D3DXVec3Transform((D3DXVECTOR4*)lookAt, (D3DXVECTOR3*)lookAt, &my);
				ImGui::ResetMouseDragDelta();
			}
			else {
				mousePos_ori.x = 0;
			}
			vector3f* pos = (vector3f*)(0x004CCDF0 + 0x688 + 0x0);
			static float speed_move = 10.0f;
			ImGui::DragFloat("cam mov speed", &speed_move);
			ImGui::DragFloat("FOV", (float*)(0x4CCDF0 + 0x688 + 0x54),0.01);
			ImGui::Text("cameraPos(%.2f,%.2f,%.2f)", pos->x, pos->y, pos->z);
			if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Space)))
				pos->z -= speed_move;
			if (ImGui::IsKeyDown(0x10))//shift
				pos->z += speed_move;
			if (ImGui::IsKeyDown('A'))
				pos->x -= speed_move * lookAt->y, pos->y += speed_move * lookAt->x;
			if (ImGui::IsKeyDown('D'))
				pos->x += speed_move * lookAt->y, pos->y -= speed_move * lookAt->x;
			if (ImGui::IsKeyDown('W'))
				pos->x += speed_move * lookAt->x, pos->y += speed_move * lookAt->y;
			if (ImGui::IsKeyDown('S'))
				pos->x -= speed_move * lookAt->x, pos->y -= speed_move * lookAt->y;
		}
	}
	
	ImGui::End();
}