#include "gui.h"
#include "pch.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

#define COL_HITBOX_BULLET_FILL 0x300000FF
#define COL_HITBOX_BULLET 0xFF0000FF

#define COL_HITBOX_ATTACK_FILL 0x10FF0000
#define COL_HITBOX_ATTACK 0xFFFF0000

#define COL_HITBOX_ENM_HIT 0xFF00FF00
#define COL_HITBOX_ENM_ATT 0xFFFF00FF

#define COL_HP 0xFF00CCFF

#define COL_DROP 0xFFCCCCCC
#define PI 3.141592653589793

extern HWND wind_hwnd;
extern IDirect3D9* g_d3d9;


bool g_isDrawAttackHitBox = true;
bool g_isDrawAnmObj = false;
bool g_isDrawEnemy = true;
bool g_isDrawBulletHitBox = true;
bool g_isDrawDrop = true;

bool g_isEnableCameraControl = true;
float g_cameraMoveSpeed=2.0f;


void EnemyGUIStageObjects::DrawSelectRect(ImDrawList* p,vector2f clientSz)
{
	p->AddCircle(GetClientFromStage(*posFin,clientSz), GetClientFromStage(10.0f, clientSz), 0xFF00FFCC);
	return;
}

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

float GetStageFromClient(float x, vector2f client_sz)
{
	return x / client_sz.x * 640.0f;//1280*960
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

void GUI_DrawAnmObj(vector2f clientSz)//not finished
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
			if(i==0)
				p->AddPolyline(pt, 4, 0xFF00FFFF, ImDrawFlags_Closed, 1.0f);
			else
				p->AddPolyline(pt, 4, 0xFF00FFCC, ImDrawFlags_Closed, 1.0f);
			iter = VALUED(iter + 4);
		}
	}
}

void DrawAttackHitbox(ImDrawList* p, AttackObj* att, vector2f clientSz,DWORD lineColor,DWORD fillColor)
{
	{
		float x = att->pos.x;
		float y = att->pos.y;
		ImVec2 cen = GetClientFromStage({ x,y }, clientSz);
		if (att->is_round_hitbox())//round
		{
			float r = att->att_radio;
			p->AddCircleFilled(cen, GetClientFromStage(r, clientSz), fillColor, 8);
			p->AddCircle(cen, GetClientFromStage(r, clientSz), lineColor, 8);
		}
		else {

			float w = att->att_box.x;
			float h = att->att_box.y;
			float a = att->angle;

			vector2f p1{ x - w / 2 ,y - h / 2 };
			vector2f p2{ x + w / 2 ,y - h / 2 };
			vector2f p3{ x + w / 2 ,y + h / 2 };
			vector2f p4{ x - w / 2 ,y + h / 2 };

			vector2f pm{ x,y };
			vector2f ang{ cosf(a),sinf(a) };
			p1 = RotateVec(p1, pm, ang);
			p2 = RotateVec(p2, pm, ang);
			p3 = RotateVec(p3, pm, ang);
			p4 = RotateVec(p4, pm, ang);
			p1 = GetClientFromStage(p1, clientSz);
			p2 = GetClientFromStage(p2, clientSz);
			p3 = GetClientFromStage(p3, clientSz);
			p4 = GetClientFromStage(p4, clientSz);
			p->AddQuad(p1, p2, p3, p4, lineColor);
			p->AddQuadFilled(p1, p2, p3, p4, fillColor);
		}
	}
}

void DrawEnemyHitbox(ImDrawList* p,Enemy* pEnm, vector2f clientSz, DWORD hurtboxColor, DWORD killboxColor,bool is_enable_hp=true)
{
	DWORD flag = *pEnm->flag();
	vector2f hit = VALUEV(pEnm->hitbox_hurt(), vector2f);
	vector2f att = VALUEV(pEnm->hitbox_kill(), vector2f);
	vector2f pos = VALUEV(pEnm->positionFin(), vector2f);
	float angle = *pEnm->hitboxAngle();
	if (flag & 0x1000)
	{
		vector2f p1{ pos.x - att.y * 0.5f,pos.y - att.x * 0.5f };
		vector2f p2{ pos.x + att.y * 0.5f,pos.y - att.x * 0.5f };
		vector2f p3{ pos.x + att.y * 0.5f,pos.y + att.x * 0.5f };
		vector2f p4{ pos.x - att.y * 0.5f,pos.y + att.x * 0.5f };
		vector2f p_rot{ pos.x - att.y * 0.5f,pos.y };
		vector2f A{ cosf(angle),sinf(angle) };
		p1 = RotateVec(p1, p_rot, A);
		p2 = RotateVec(p2, p_rot, A);
		p3 = RotateVec(p3, p_rot, A);
		p4 = RotateVec(p4, p_rot, A);
		p1 = GetClientFromStage(p1, clientSz);
		p2 = GetClientFromStage(p2, clientSz);
		p3 = GetClientFromStage(p3, clientSz);
		p4 = GetClientFromStage(p4, clientSz);
		p->AddQuad(p1, p2, p3, p4, hurtboxColor);
		p1 = { pos.x - hit.x * 0.5f,pos.y - hit.y * 0.5f };
		p2 = { pos.x + hit.x * 0.5f,pos.y - hit.y * 0.5f };
		p3 = { pos.x + hit.x * 0.5f,pos.y + hit.y * 0.5f };
		p4 = { pos.x - hit.x * 0.5f,pos.y + hit.y * 0.5f };
		p_rot = { pos.x,pos.y };
		A = { cosf(angle),sinf(angle) };
		p1 = RotateVec(p1, p_rot, A);
		p2 = RotateVec(p2, p_rot, A);
		p3 = RotateVec(p3, p_rot, A);
		p4 = RotateVec(p4, p_rot, A);
		p1 = GetClientFromStage(p1, clientSz);
		p2 = GetClientFromStage(p2, clientSz);
		p3 = GetClientFromStage(p3, clientSz);
		p4 = GetClientFromStage(p4, clientSz);
		p->AddQuad(p1, p2, p3, p4, killboxColor);
	}
	else
	{
		vector2f posS = GetClientFromStage(pos, clientSz);
		p->AddCircle(posS, GetClientFromStage(att.x * 0.5f, clientSz), hurtboxColor, 10);
		p->AddCircle(posS, GetClientFromStage(hit.x * 0.5f, clientSz), killboxColor, 10);
	}

	if (is_enable_hp)
	{
		int tot_hp = *pEnm->totHp();
		int hp = *pEnm->nowHp();
		vector2f ptxt = { pos.x,pos.y - att.y * 0.9f };
		ptxt = GetClientFromStage(ptxt, clientSz);
		char hpt[20];
		sprintf_s(hpt, 20, "%4d/%4d", hp, tot_hp);
		ImFont font(*ImGui::GetFont());
		p->AddText(&font, 25, ptxt, COL_HP, hpt, NULL, 0.0f);
	}
}

void DrawBulletHitbox(ImDrawList* p, Bullet* pbt, vector2f clientSz, DWORD hitboxLineColor, DWORD hitboxFillColor, DWORD velocityColor, bool is_enable_velocity=true)
{
	float len = 20.0f;
	vector2f vec{ pbt->position()->x,pbt->position()->y };
	vector2f vec2(pbt->position()->x + pbt->velocity()->x * len,
		pbt->position()->y + pbt->velocity()->y * len);
	float hit = pbt->hitbox()->x;
	hit = GetClientFromStage(hit, clientSz);
	vec = GetClientFromStage(vec, clientSz);
	vec2 = GetClientFromStage(vec2, clientSz);
	p->AddCircle(vec, hit, hitboxLineColor, 8, 1.0f);
	p->AddCircleFilled(vec, hit, hitboxFillColor, 8);
	p->AddLine(vec, vec2, velocityColor, 1.0f);
}

void GUI_DrawAttackHitBox(vector2f clientSz) {
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
				DrawAttackHitbox(p, att, clientSz, COL_HITBOX_ATTACK, COL_HITBOX_ATTACK_FILL);
			iter = iter + 0x9C;
		}
	}
}

void GUI_DrawEnemyHitbox(vector2f clientSz)
{
	auto p = ImGui::GetOverlayDrawList();
	//[004CF410]+20574
	DWORD enmBase = *(DWORD*)(0x004CF2D0);
	For_AllEnemies([=](Enemy* pEnm, int id) {
		{
			DrawEnemyHitbox(p, pEnm, clientSz, COL_HITBOX_ENM_HIT, COL_HITBOX_ENM_ATT, true);
		}
	});
}

void GUI_DrawBulletHitBox(vector2f clientSz)
{
	auto dl = ImGui::GetOverlayDrawList();
	For_AllBullets([=](Bullet* pbt, int id)
		{
			DrawBulletHitbox(dl,pbt,clientSz,COL_HITBOX_BULLET, COL_HITBOX_BULLET_FILL, COL_HITBOX_BULLET,true);
		});
}

void GUI_DrawDrop(vector2f clientSz)
{
	auto dl = ImGui::GetOverlayDrawList();
	int sum = 0;
	DWORD pdrop = VALUED(0x004CF2EC);
	if (pdrop)
	{
		pdrop += 0x14;
		for (int i = 0; i < 4696; i++)
		{
			if (VALUED(pdrop + 0xC74))
			{
				vector2f vec{ VALUEF(pdrop + 0xC2C),VALUEF(pdrop + 0xC30) };
				vec = GetClientFromStage(vec, clientSz);
				dl->AddCircle(vec, 20.0f, COL_DROP, 6, 3.0f);
			}
			pdrop += 0xC94;
		}
	}
	return;
}

void GUI_EnemyWind()
{
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{

	}
}

void GUI_Always(vector2f clientSz)
{

	if (g_isDrawAttackHitBox)
		GUI_DrawAttackHitBox(clientSz);
	if (g_isDrawAnmObj)
		GUI_DrawAnmObj(clientSz);
	if (g_isDrawEnemy)
		GUI_DrawEnemyHitbox(clientSz);
	if (g_isDrawDrop)
		GUI_DrawDrop(clientSz);
	if (g_isDrawBulletHitBox)
		GUI_DrawBulletHitBox(clientSz);
	//camera control
	{
		if (g_isEnableCameraControl)
		{
			vector3f* face = (vector3f*)(0x4CCDF0 + 0x688 + 0x24);
			static vector2f mousePos_ori = { 0,0 };
			auto mousePos = ImGui::GetMousePos();

			vector3f* pos = (vector3f*)(0x004CCDF0 + 0x688 + 0x0);
			vector3f* up = (vector3f*)(0x4CCDF0 + 0x688 + 0x18);
			D3DXVECTOR3 dpos = { pos->x,pos->y,pos->z };
			D3DXVECTOR3 dup = { up->x,up->y,up->z };
			D3DXVECTOR3 dface = { face->x,face->y,face->z };
			D3DXVECTOR3 dres = dpos;
			D3DXVECTOR3 dright;
			D3DXVECTOR3 dfront;
			D3DXVec3Cross(&dright, &dup, &dface);
			D3DXVec3Normalize(&dright, &dright);
			D3DXVec3Cross(&dfront, &dup, &dright);

			if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
			{
				if (mousePos_ori.x == 0)
				{
					mousePos_ori = ImGui::GetMousePos();
				}
				vector2f del{ mousePos.x - mousePos_ori.x,mousePos.y - mousePos_ori.y };
				mousePos_ori = mousePos;
				D3DXMATRIX mx, my;
				//D3DXMatrixRotationZ(&mx, -del.x * 0.005);
				//D3DXMatrixRotationX(&my, (lookAt->y > 0 ? 1 : -1) * del.y * 0.005);
				D3DXMatrixRotationAxis(&mx, &dup, del.x * 0.005);
				D3DXMatrixRotationAxis(&my, &dright, del.y * 0.005);
				D3DXVec3Transform((D3DXVECTOR4*)face, (D3DXVECTOR3*)face, &mx);
				D3DXVec3Transform((D3DXVECTOR4*)face, (D3DXVECTOR3*)face, &my);
				ImGui::ResetMouseDragDelta();
			}
			else {
				mousePos_ori.x = 0;
			}


			D3DXVec3Scale(&dup, &dup, g_cameraMoveSpeed);
			D3DXVec3Scale(&dfront, &dfront, g_cameraMoveSpeed);
			D3DXVec3Scale(&dright, &dright, g_cameraMoveSpeed);
			if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Space)))
				D3DXVec3Add(&dres, &dpos, &dup);
			if (ImGui::IsKeyDown(0x10))//shift
				D3DXVec3Subtract(&dres, &dpos, &dup);
			if (ImGui::IsKeyDown('A'))
				D3DXVec3Subtract(&dres, &dpos, &dright);
			if (ImGui::IsKeyDown('D'))
				D3DXVec3Add(&dres, &dpos, &dright);
			if (ImGui::IsKeyDown('W'))
				D3DXVec3Subtract(&dres, &dpos, &dfront);
			if (ImGui::IsKeyDown('S'))
				D3DXVec3Add(&dres, &dpos, &dfront);
			*pos = { dres.x,dres.y,dres.z };
		}
	}
	
	
}

void GUI_globalVarSetter()
{
	ImGui::PushID("GUI_globalVarSetter");
	if (VALUED(0x4CF2D0))
	{
		DWORD eb = VALUED(0x4CF2D0);
		static float spd = 1.0f;
		ImGui::DragFloat("drag speed", &spd);
		ImGui::DragInt("-9923", (int*)eb+0x18,spd);
		ImGui::DragInt("-9924", (int*)eb+0x14, spd);
		ImGui::DragInt("-9925", (int*)eb+0x10, spd);
		ImGui::DragInt("-9926", (int*)eb+0xC, spd);
		ImGui::Separator();
		ImGui::DragFloat("-9915", (float*)eb + 0x1C,spd);
		ImGui::DragFloat("-9916", (float*)eb + 0x20,spd);
		ImGui::DragFloat("-9917", (float*)eb + 0x24,spd);
		ImGui::DragFloat("-9918", (float*)eb + 0x28,spd);
		ImGui::DragFloat("-9919", (float*)eb + 0x2C,spd);
		ImGui::DragFloat("-9920", (float*)eb + 0x30,spd);
		ImGui::DragFloat("-9921", (float*)eb + 0x34,spd);
		ImGui::DragFloat("-9922", (float*)eb + 0x38,spd);
		//bool autoInc;
		//ImGui::Checkbox("autoInc", &autoInc);
		//if (autoInc)
		//{
		//	static float inc[4] = { 0,0,0,0 };
		//	static float inc2[4] = { 0,0,0,0 };
		//	ImGui::DragFloat4("acc:-9918~-9915", inc, scale);
		//	ImGui::DragFloat4("acc:-9922~-9919", inc2, scale);
		//	for (DWORD x = 0x2C, i = 0; x <= 0x38; x += 4, i++)
		//		VALUEF(eb + x) += inc[i];
		//	for (DWORD x = 0x1C, i = 0; x <= 0x28; x += 4, i++)
		//		VALUEF(eb + x) += inc2[i];
		//}
	}
	ImGui::PopID();
}

void GUI_Background()
{
	ImGui::PushID("GUI_Background");
	static bool is_disable_cameraMove = false;
	if (ImGui::Checkbox("disableMoveCamera", &is_disable_cameraMove))
	{
		if (is_disable_cameraMove)
		{
			g_isEnableCameraControl = true;
			Address<WORD>(0x41C245).SetValue(0x9090);
			Address<WORD>(0x41C306).SetValue(0x9090);
			Address<WORD>(0x41C77A).SetValue(0x9090);
			//stop camera
			Address<WORD>(0x41D132).SetValue(0x9090);
			Address<BYTE>(0x41D14A).SetValue(0x0);
			//no fog
			vector3f* up = (vector3f*)(0x4CCDF0 + 0x688 + 0x18);
			up->y = 0;
			up->z = -1.0f;
			//set the up vec
		}else{
			g_isEnableCameraControl = false;
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

	static float speed_move = 10.0f;
	ImGui::DragFloat("cam mov speed", &g_cameraMoveSpeed);
	ImGui::DragFloat("FOV", (float*)(0x4CCDF0 + 0x688 + 0x54), 0.01);
	vector3f* pos = (vector3f*)(0x004CCDF0 + 0x688 + 0x0);
	ImGui::DragFloat3("posA", (float*)pos);
	ImGui::DragFloat3("posR", (float*)(0x004CCDF0 + 0x688 + 0x3C));
	ImGui::DragFloat3("upVec", (float*)(0x4CCDF0 + 0x688 + 0x18));
	ImGui::DragFloat3("face", (float*)(0x004CCDF0 + 0x688 + 0x24));
	ImGui::PopID();
}

void GUI_Stage(vector2f clientSz)
{
	ImGui::PushID("GUI_Stage");
	if (ImGui::Button("cd and money"))
	{
		DWORD pAb = VALUED(0x004CF298);
		if (pAb)
		{
			for (DWORD iter = VALUED(pAb + 0x1C); iter != 0; iter = VALUED(iter + 4))
			{
				DWORD ab = VALUED(iter);
				VALUED(ab + 0x34) = 0;
				VALUED(ab + 0x38) = 0;
				VALUED(ab + 0x3C) = 0;
				VALUED(ab + 0x48) = 0;
			}
		}
		VALUED(0x004CCD34) = 99999999;
	}
	//invincible
	static bool is_invincible = false;
	if (ImGui::Checkbox("is_invincible", &is_invincible))
	{
		if (is_invincible)
		{
			DWORD op;
			VirtualProtect((LPVOID)0x45D4E4, 10, PAGE_READWRITE, &op);
			for (int i = 0; i < 10; i++)
				VALUEV(0x45D4E4 + i, BYTE) = 0x90;
			VirtualProtect((LPVOID)0x45D4E4, 10, op, &op);
		}
		else {
			DWORD op;
			VirtualProtect((LPVOID)0x45D4E4, 10, PAGE_READWRITE, &op);
			BYTE normal[10] = { 0xC7, 0x87, 0xAC, 0x76, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00 };
			for (int i = 0; i < 10; i++)
				VALUEV(0x45D4E4 + i, BYTE) = normal[i];
			VirtualProtect((LPVOID)0x45D4E4, 10, op, &op);
		}
	}
	//end
	if (ImGui::Button("open shop"))
	{
		VALUED(0x004CCCC8) = 0;//make practice mode also have shop
		VALUED(VALUED(0x004CF2E4) + 0xB0) |= 0x20000;
	}
	if (ImGui::Button("open super shop"))
	{
		VALUED(0x004CCCC8) = 0;//make practice mode also have shop
		M_OpenSuperShop();
	}
	if (ImGui::Button("close shop"))
	{
		VALUED(VALUED(0x004CF2A4) + 0xE38) = 5;
	}
	static int id_card = 1;
	ImGui::InputInt("cardId", &id_card, 1); ImGui::SameLine();
	if (ImGui::Button("getCard"))
	{
		//addCard(VALUED(0x004CF298), id_card, 2);
		M_AddCard(VALUED(0x004CF298), 0, id_card, 2);
	}
	if (ImGui::Button("clearCard"))
		ClearCard(VALUED(0x004CF298), 0);

	static int power = 300;
	ImGui::DragInt("power", &power, 2.0f, 0, 400);ImGui::SameLine();
	if (ImGui::Button("setPower"))
	{
		VALUED(0x004CCD38) = power;
		UpdateShot(PPLAYER + 0x620, 0);
	}
	if (ImGui::CollapsingHeader("hitbox visibility"))
	{

		ImGui::Checkbox("Attack", &g_isDrawAttackHitBox);
		ImGui::Checkbox("AnmObj", &g_isDrawAnmObj);
		ImGui::Checkbox("Bullet", &g_isDrawBulletHitBox);
		ImGui::Checkbox("Enemy", &g_isDrawEnemy);
		ImGui::Checkbox("Drop", &g_isDrawDrop);
		int bt = 0;
		
	}
	
	ImGui::Separator();
	static int SE = 1;
	ImGui::InputInt("SE", &SE, 1); ImGui::SameLine();
	if (ImGui::Button("play SE"))
		PlaySE(SE, 0);

	ImGui::PopID();
}

GUI_Tool GUI_ToolBar()
{
	ImGui::PushID("GUI_ToolBar");
	static int ret=0;
	ImGui::RadioButton("noTool", &ret, TOOL_EMPTY); ImGui::SameLine();
	ImGui::RadioButton("select", &ret, TOOL_SELECT_MOVE); ImGui::SameLine();
	ImGui::RadioButton("spawnEnemy", &ret, TOOL_ENEMYSPAWN); ImGui::SameLine();
	ImGui::RadioButton("spawnDrop", &ret, TOOL_DROPSPAWN); ImGui::SameLine();
	ImGui::PopID();
	return static_cast<GUI_Tool>(ret);
}

void GUI_Tool_SELECT_MOVE()
{
	auto mousePos = ImGui::GetMousePos();
	vector2f clientSz = ImGui::GetMainViewport()->Size;
	vector2f smouse;
	smouse = GetStageFromClient(vector2f{ (float)mousePos.x,(float)mousePos.y }, clientSz);
}

void GUI_Tool_EnemySpawn(bool isGuiOpen)
{
	auto mousePos = ImGui::GetMousePos();
	vector2f clientSz = ImGui::GetMainViewport()->Size;
	vector2f smouse;
	smouse = GetStageFromClient(vector2f{ (float)mousePos.x,(float)mousePos.y }, clientSz);

	static char subName[100] = { 0 };
	static EnemyInfo_init info = { {0,0,0},0,(Drop)0,9999,0,0,0,0,0,0,0,0,0,0};
	if (subName[0] == '\0')
		wsprintfA(subName, "Lily");
	//gui part
	if (isGuiOpen){
		ImGui::InputText("subName", subName, 100);
		ImGui::InputInt("drop", (int*)&(info.drop));
		ImGui::InputInt("life", &(info.hp));
		ImGui::InputInt("var[-9982]", &(info.loc_9982));
		ImGui::InputInt("var[-9983]", &(info.loc_9983));
		ImGui::InputInt("var[-9984]", &(info.loc_9984));
		ImGui::InputInt("var[-9985]", &(info.loc_9985));
		ImGui::InputFloat("var[-9978]", &(info.loc_9978));
		ImGui::InputFloat("var[-9979]", &(info.loc_9979));
		ImGui::InputFloat("var[-9980]", &(info.loc_9980));
		ImGui::InputFloat("var[-9981]", &(info.loc_9981));
	}
	
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && fabsf(smouse.x) < 192.0)
	{
		info.pos.x = smouse.x;
		info.pos.y = smouse.y;
		if (*(DWORD*)(0x004CF2D0))
			SummonEnm(*(DWORD*)(0x004CF2D0), 0, subName, &info, 0);
	}
}

void GUI_Tool_DropSpawn(bool isGuiOpen)
{
	auto mousePos = ImGui::GetMousePos();
	vector2f clientSz = ImGui::GetMainViewport()->Size;
	vector2f smouse;
	smouse = GetStageFromClient(vector2f{ (float)mousePos.x,(float)mousePos.y }, clientSz);
	static bool faster = true;
	static int type = (int)Drop::lifePeice;
	static float dir= -PI/2.0f;
	static float speed= 1.0f;
	static int delay= 0;
	static float unkFloat= 0.0f;
	static int unkint1 = 0;
	static int unkint2 = 0;
	//gui part
	if (isGuiOpen) {
		ImGui::Checkbox("use MouseDown",&faster);
		ImGui::Separator();
		ImGui::InputInt("dropType", &type);
		ImGui::InputInt("delay", &delay);
		ImGui::DragFloat("direction", &dir);
		ImGui::DragFloat("speed", &speed);
		ImGui::Separator();
		ImGui::DragFloat("unkFloat", &unkFloat);
		ImGui::DragInt("unkInt1", &unkint1);
		ImGui::DragInt("unkInt2", &unkint2);
	}
	if((faster && (ImGui::GetMouseDragDelta().x != 0 || ImGui::IsMouseClicked(ImGuiMouseButton_Left)) && fabsf(smouse.x) < 192.0)
		|| (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && fabsf(smouse.x) < 192.0))
	{
		vector3f mpos{ smouse.x,smouse.y,0 };
		DropItem_F((Drop)type, &mpos, unkFloat, dir, speed, delay,unkint1,unkint2);
	}
}

void GUI_TOOL_SelectMove(bool isGuiOpen)
{
	auto p = ImGui::GetOverlayDrawList();
	auto mousePos = ImGui::GetMousePos();
	vector2f clientSz = ImGui::GetMainViewport()->Size;
	vector2f smouse;
	smouse = GetStageFromClient(vector2f{ (float)mousePos.x,(float)mousePos.y }, clientSz);

	static std::unordered_map<LPVOID,std::shared_ptr<SelectableGUIStageObjects>> selectedObj;
	std::unordered_set<Bullet*> selectableBt;
	std::unordered_set<Enemy*> selectableEnm;
	static bool btAbleS=false;
	static bool enmAbleS=true;
	static bool dropAbleS=false;

	//update
	{
		For_AllBullets([&selectableBt](Bullet* bt, int) {
			selectableBt.insert(bt); });
		For_AllEnemies([&selectableEnm](Enemy* enm, int) {
			selectableEnm.insert(enm); });
		for (auto it = selectedObj.begin(); it != selectedObj.end();)
		{
			bool isFind = false;
			switch ((it->second)->GetType())
			{
			case SelectableGUIStageObejctsType::ENEMY:
				isFind = selectableEnm.count((Enemy*)(it->second)->GetPointer());
				break;
			case SelectableGUIStageObejctsType::BULLET:
				isFind = selectableBt.count((Bullet*)(it->second)->GetPointer());
				break;
			default:
				break;
			}
			if (!isFind)
				it = selectedObj.erase(it);
			else
				it++;
		}
		for (auto& i : selectedObj)
		{
			i.second->Update();
			i.second->DrawSelectRect(p, clientSz);
		}
	}
	if (isGuiOpen)
	{
		ImGui::Checkbox("selectBullet", &btAbleS);
		ImGui::Checkbox("selectEnemy", &enmAbleS);
		ImGui::Checkbox("selectDrop", &dropAbleS);
	}

	if (ImGui::IsKeyDown('D') && ImGui::IsKeyDown(0x11))//ctrl+D
		selectedObj.clear();

	static vector2f s_mouseDownPos={0,0};
	static bool isTrySelect=false;
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		if (s_mouseDownPos.x==-999999)
			s_mouseDownPos = smouse;
		isTrySelect = true;
		for (auto& i : selectedObj)
		{
			if (IsSelected(*i.second->GetPosFin(), smouse, 40.0f))
			{
				isTrySelect = false;
				break;
			}
		}
		if (!isTrySelect && !ImGui::IsKeyDown(0x10) && !ImGui::IsKeyDown(0x12) )
		{
			auto delta = ImGui::GetMouseDragDelta();
			for (auto& i : selectedObj)
			{
				i.second->DragDelta({ GetStageFromClient(delta.x, clientSz),GetStageFromClient(delta.y, clientSz) });
			}
			ImGui::ResetMouseDragDelta();
		}else{
			if (!ImGui::IsKeyDown(0x10) && !ImGui::IsKeyDown(0x12))//shift,alt
				selectedObj.clear();
			p->AddRectFilled(GetClientFromStage(s_mouseDownPos, clientSz), mousePos, 0x8800CCCC);
		}
	}
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		if (isTrySelect)
		{
			float rangeInc = 2.0f;
			if (fabsf(smouse.x - s_mouseDownPos.x) < 2.0f && fabsf(smouse.y - s_mouseDownPos.y) < 2.0f)
				rangeInc = 10.0f;
			if (enmAbleS)
			{
				for (auto& i : selectableEnm)
				{
					if (IsSelected(*(vector2f*)i->positionFin(), s_mouseDownPos, smouse, rangeInc))
					{
						if (ImGui::IsKeyDown(0x12))//alt
							selectedObj.erase(i);
						else
							selectedObj[(LPVOID)i] = std::make_shared<EnemyGUIStageObjects>(i);
					}
				}
			}
			if (btAbleS)
			{

			}
		}
		s_mouseDownPos = { -999999,-999999};
	}
	

}

void GUI_AttrTool(GUI_Tool tool,bool isGuiOpen)
{
	ImGui::PushID("GUI_AttrTool");
	switch (tool)
	{
	case GUI_Tool::TOOL_ENEMYSPAWN:
		GUI_Tool_EnemySpawn(isGuiOpen);
		break;
	case GUI_Tool::TOOL_DROPSPAWN:
		GUI_Tool_DropSpawn(isGuiOpen);
		break;
	case GUI_Tool::TOOL_SELECT_MOVE:
		GUI_TOOL_SelectMove(isGuiOpen);
		break;
	default:
		break;
	}
	ImGui::PopID();
}

void SetGUI(IDirect3DDevice9* device)
{
	auto mousePos = ImGui::GetMousePos();
	vector2f clientSz= ImGui::GetMainViewport()->Size;
	vector2f smouse;
	smouse = GetStageFromClient(vector2f{(float)mousePos.x,(float)mousePos.y }, clientSz);
	
	GUI_Tool tool=GUI_Tool::TOOL_EMPTY;
	ImGui::Begin("toolbar");
	tool=GUI_ToolBar();
	ImGui::End();

	GUI_Always(clientSz);

	ImGui::Begin("test");
	{
		ImGui::Text("mousePos: %f,%f", smouse.x, smouse.y);
		ImGui::Separator();
		if (ImGui::BeginTabBar("mainbar"))
		{
			auto isToolGUIOpen = ImGui::BeginTabItem("ToolAttribute");
			GUI_AttrTool(tool,isToolGUIOpen);
			if(isToolGUIOpen)
				ImGui::EndTabItem();

			if (ImGui::BeginTabItem("Stage"))
			{
				GUI_Stage(clientSz);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("eclVar"))
			{
				GUI_globalVarSetter();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("background"))
			{
				GUI_Background();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
	
	



	{
		static int id_tex[2] = { 9,0 };
		IDirect3DBaseTexture9* tex = nullptr;
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
	
	//3D test
	
	//print src test
	//if (ImGui::Button("prtsc"))
	//{
	//	
	//	//device->CreateOffscreenPlainSurface
	//	if (FAILED(D3DXSaveSurfaceToFile(L"E:\\touhou\\TH18-HLD\\src.png", _D3DXIMAGE_FILEFORMAT::D3DXIFF_PNG,
	//		(LPDIRECT3DSURFACE9)(VALUED(0x004CCFA0)), NULL, NULL)))
	//	{
	//		int i = 0;
	//	}
	//}
	//stage texture test
	//{
	//	static bool is_render_stage = false;
	//	ImGui::Checkbox("render stage", &is_render_stage);
	//	if (is_render_stage)
	//	{
	//		if (VALUED(0x04CCF9C))
	//		{
	//			IDirect3DCubeTexture9* cb = 0;
	//			//[[[004CCDF0 + 0x82C]+ 0x124] + 0x48]
	//			*(DWORD*)&cb = VALUED(VALUED(VALUED(0x004CCDF0 + 0x82C) + 0x124) + 0x30);
	//			int x = 200, y = 100;
	//			Custom_vertex1 v[5] =
	//			{
	//				{{x,y + 0,0},1,0xFFFFFFFF,{0,0}},
	//				{{x + 384,y + 0,0},1,0xFFFFFFFF,{1,0}},
	//				{{x + 0,y + 448,0},1,0xFFFFFFFF,{0,1}},
	//				{{x + 384,y + 448,0},1,0xFFFFFFFF,{1,1}}
	//			};
	//			device->SetTexture(0, cb);
	//			device->SetFVF(FVF_VERTEX_1);
	//			device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Custom_vertex1));
	//		}
	//	}
	//}
	
	//texture test2
	//{
	//	DWORD iter = VALUED(0x51F65C)+0x2C;
	//	static int id_tex[2] = { 9,0 };
	//	IDirect3DCubeTexture9* tex = nullptr;
	//	static bool is_draw2 = false;
	//	ImGui::Checkbox("texture2", &is_draw2);
	//	ImGui::InputInt2("id_tex2", id_tex);
	//	if (is_draw2)
	//	{
	//		int x = 0, y = 0, n = 0,m=0;
	//		while (VALUED(iter -0xC)>=1)
	//		{
	//			m++;
	//			if (m >= 999)
	//				break;
	//			int id_anmFile = VALUED(iter - 0xC);
	//			int id2 = VALUED(iter - 8);
	//			if (VALUED(VALUED(0x0051F65C) + 4 * id_anmFile + 0x312072C))
	//			{
	//				*(DWORD*)(&tex) = VALUED(VALUED(VALUED(VALUED(0x0051F65C) + 4 * id_anmFile + 0x312072C) + 0x124)
	//					+ 0x18 * id2);
	//				device->SetTexture(0, tex);
	//				Custom_vertex1 v[5] =
	//				{
	//					{{x + 0,y + 0,0},1,0xFFFFFFFF,{0,0}},
	//					{{x + 20,y + 0,0},1,0xFFFFFFFF,{1,0}},
	//					{{x + 0,y + 20,0},1,0xFFFFFFFF,{0,1}},
	//					{{x + 20,y + 20,0},1,0xFFFFFFFF,{1,1}}
	//				};
	//				device->SetFVF(FVF_VERTEX_1);
	//				device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(Custom_vertex1));
	//				x += 25; n++;
	//				if (n % 10 == 9)
	//					y += 25;
	//			}
	//			iter += 0x28;
	//		}
	//	}
	//}
	ImGui::End();
}