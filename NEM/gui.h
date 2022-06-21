#pragma once
#include <d3d9.h>
#include "..\imgui\imgui.h"
#include "..\imgui\imgui_impl_dx9.h"
#include "..\imgui\imgui_impl_win32.h"

#include "Address.h"
#include "THFunction.h"
#include "M_Misc.h"
#include "M_AbilityCard.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <cmath>
#include <list>
#include <algorithm>

void SetGUI(IDirect3DDevice9* device);
vector2f GetStageFromClient(vector2f client, vector2f client_sz);
float GetStageFromClient(float x, vector2f client_sz);
vector2f GetClientFromStage(vector2f stage, vector2f client_sz);
float GetClientFromStage(float x, vector2f client_sz);

enum GUI_Tool
{
	TOOL_EMPTY = 0, TOOL_SELECT_MOVE, TOOL_ENEMYSPAWN, TOOL_DROPSPAWN
};

enum class SelectableGUIStageObejctsType
{
	ENEMY, BULLET, DROP
};


inline bool IsSelected(vector2f posFin, vector2f mouse_stagePos, float mouseHitbox = 10.0f) {
	if (hypotf(mouse_stagePos.x - posFin.x, mouse_stagePos.y - posFin.y) < mouseHitbox) { return true; }return false;
}

inline bool IsSelected(vector2f posFin, vector2f stagePos1, vector2f stagePos2,float rangeInc=0.0f)
{
	if (stagePos1.x > stagePos2.x)
		std::swap(stagePos1.x, stagePos2.x);
	if (stagePos1.y > stagePos2.y)
		std::swap(stagePos1.y, stagePos2.y);
	if (posFin.x > stagePos1.x- rangeInc && posFin.x < stagePos2.x+rangeInc && posFin.y > stagePos1.y- rangeInc && posFin.y < stagePos2.y+ rangeInc)
		return true;
	return false;
}

class SelectableGUIStageObjects
{
protected:
	vector2f* posFin;
	vector2f* posAbs;
	float mouseHitbox;
public:
	SelectableGUIStageObjects() :posFin(nullptr), posAbs(nullptr), mouseHitbox(10.0f) {}
	SelectableGUIStageObjects(vector2f* posf, vector2f* posa, float hb = 10.0f) :posFin(posf), posAbs(posa), mouseHitbox(hb) {}
	virtual LPVOID GetPointer() { return nullptr; }
	virtual SelectableGUIStageObejctsType GetType() = 0;
	virtual void Update() = 0;
	virtual void GetGUIWindow() = 0;
	virtual void DrawSelectRect(ImDrawList* p, vector2f clientSz) = 0;
	virtual vector2f* GetPosFin() { return posFin; }

	void DragDelta(vector2f delta_stage) { if (posAbs) { posAbs->x += delta_stage.x; posAbs->y += delta_stage.y; } }
};

class EnemyGUIStageObjects :public SelectableGUIStageObjects
{
protected:
	Enemy* pEnm;
public:
	virtual SelectableGUIStageObejctsType GetType()override { return SelectableGUIStageObejctsType::ENEMY; };
	EnemyGUIStageObjects(Enemy* penm) :pEnm(penm), SelectableGUIStageObjects(penm->positionFin(), penm->positionAbs()) {}
	virtual LPVOID GetPointer() override { return pEnm; }
	virtual void Update()override {}
	virtual void GetGUIWindow()override {}
	virtual void DrawSelectRect(ImDrawList* p, vector2f clientSz)override;
};
