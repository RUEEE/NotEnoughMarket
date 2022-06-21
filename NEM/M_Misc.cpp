#include "pch.h"
#include "ThFunction.h"
#include "injection.h"
#include "M_Misc.h"
#include <set>
#include <algorithm>
#include <d3dx9.h>

bool __fastcall M_InitShop(Shop* sp,UINT edx,vector3f* pos)
{
    {
        DWORD dShop = (DWORD)sp;

        auto v3 = (int)InsertStageEff((LPVOID)0x417CC0);
        VALUED(v3 + 4) |= 2u;
        VALUED(v3 + 36) = dShop;

        int(__stdcall * sub_401180)(DWORD a1, int a2) = (int(__stdcall*)(DWORD a1, int a2))(0x401180);

        sub_401180(v3, 12);
        VALUED(dShop + 4) = v3;
        auto v4 = (int)InsertStageEff((LPVOID)0x418C30);
        VALUED(v4 + 4) |= 2u;
        VALUED(v4 + 36) = dShop;

        int(__stdcall * sub_401230)(DWORD a1, int a2) = (int(__stdcall*)(DWORD a1, int a2))(0x401230);
        sub_401230(v4, 81);

        auto v5 = VALUED(dShop + 4);
        VALUED(dShop + 8) = v4;
        VALUED(v5 + 4) |= 2u;
        VALUED(VALUED(dShop + 8) + 4) |= 2u;
        auto v6 = VALUED(dShop + 460);
        if ((v6 & 1) == 0)
        {
            VALUED(dShop + 0x1C8) = 0;
            VALUED(dShop + 460) = v6 | 1;
        }
        VALUED(dShop + 0x1C0) = 0;
        VALUED(dShop + 0x1C4) = 0;
        VALUED(dShop + 0x1BC) = -1;
        auto v7 = VALUED(dShop + 516);
        if ((v7 & 1) == 0)
        {
            VALUED(dShop + 512) = 0;
            VALUED(dShop + 516) = v7 | 1;
        }
        VALUED(dShop + 504) = 0;
        VALUED(dShop + 508) = 0;
        VALUED(dShop + 500) = -1;
        auto v8 = VALUED(dShop + 536);
        if ((v8 & 1) == 0)
        {
            VALUED(dShop + 532) = 0;
            VALUED(dShop + 536) = v8 | 1;
        }
        VALUED(dShop + 524) = 0;
        VALUED(dShop + 528) = 0;
        VALUED(dShop + 520) = -1;
        *(vector3f*)(dShop + 464) = *pos;
        VALUED(dShop + 20) = 2;
        
        VALUED(dShop + 0xDC) = 1;
    }
    //>>>>>>>>>>>>>>>>>>>>>>>>select card
    std::set<UINT> cardId_modified = { 53,49,48,42,33,26,20,12,10,8,15,7,25,44,23,54,30 };//,36
    CardInShopInfo* cardPool[100] = { 0 };
    UINT cardN = 0;
    for (auto i : cardId_modified)
    {
         if (!VALUED(VALUED(0x004CF298) + 0xC84 + i * 0x4))
         {
             cardPool[cardN]= GetpCardInfoFromId(i);
             cardN++;
         }
    }
    for (UINT i = 0; i < cardN; i++)
    {
        UINT num_to_swap = GetRndInt_Rep()%(i+1);
        std::swap(cardPool[i],cardPool[num_to_swap]);
    }
    if (cardN==0)
    {
        *sp->abCardInfo(*sp->num_abCard()) = GetpCardInfoFromId(0);
        *sp->num_abCard()+=1;
    }
    for (UINT i = 0; i < min(4, cardN); i++)
    {
        *sp->abCardInfo(*sp->num_abCard()) = cardPool[i];
        *sp->num_abCard() += 1;
    }
    //for (UINT i = 0; i < 56; i++)
    //{
    //    *sp->abCardInfo(i) = GetpCardInfoFromId(i);
    //    *sp->num_abCard()+=1;
    //}
    //>>>>>>>>>>>>>>>>>>>>>>>>select card
    *(sp->menu()->max_Selectable())= *sp->num_abCard();
    SetMenuSelection(sp->menu(), 0);
    return 0;
}

bool __fastcall M_InitSuperShop(Shop* sp, UINT edx, vector3f* pos)
{
    {
        DWORD dShop = (DWORD)sp;

        auto v3 = (int)InsertStageEff((LPVOID)0x417CC0);
        VALUED(v3 + 4) |= 2u;
        VALUED(v3 + 36) = dShop;

        int(__stdcall * sub_401180)(DWORD a1, int a2) = (int(__stdcall*)(DWORD a1, int a2))(0x401180);

        sub_401180(v3, 12);
        VALUED(dShop + 4) = v3;
        auto v4 = (int)InsertStageEff((LPVOID)0x418C30);
        VALUED(v4 + 4) |= 2u;
        VALUED(v4 + 36) = dShop;

        int(__stdcall * sub_401230)(DWORD a1, int a2) = (int(__stdcall*)(DWORD a1, int a2))(0x401230);
        sub_401230(v4, 81);

        auto v5 = VALUED(dShop + 4);
        VALUED(dShop + 8) = v4;
        VALUED(v5 + 4) |= 2u;
        VALUED(VALUED(dShop + 8) + 4) |= 2u;
        auto v6 = VALUED(dShop + 460);
        if ((v6 & 1) == 0)
        {
            VALUED(dShop + 0x1C8) = 0;
            VALUED(dShop + 460) = v6 | 1;
        }
        VALUED(dShop + 0x1C0) = 0;
        VALUED(dShop + 0x1C4) = 0;
        VALUED(dShop + 0x1BC) = -1;
        auto v7 = VALUED(dShop + 516);
        if ((v7 & 1) == 0)
        {
            VALUED(dShop + 512) = 0;
            VALUED(dShop + 516) = v7 | 1;
        }
        VALUED(dShop + 504) = 0;
        VALUED(dShop + 508) = 0;
        VALUED(dShop + 500) = -1;
        auto v8 = VALUED(dShop + 536);
        if ((v8 & 1) == 0)
        {
            VALUED(dShop + 532) = 0;
            VALUED(dShop + 536) = v8 | 1;
        }
        VALUED(dShop + 524) = 0;
        VALUED(dShop + 528) = 0;
        VALUED(dShop + 520) = -1;
        *(vector3f*)(dShop + 464) = *pos;
        VALUED(dShop + 20) = 2;

        VALUED(dShop + 0xDC) = 1;
    }
    //>>>>>>>>>>>>>>>>>>>>>>>>select card
    for (UINT i = 0; i < 56; i++)
    {
        *sp->abCardInfo(i) = GetpCardInfoFromId(i);
        *sp->num_abCard()+=1;
    }
    //>>>>>>>>>>>>>>>>>>>>>>>>select card
    *(sp->menu()->max_Selectable()) = *sp->num_abCard();
    SetMenuSelection(sp->menu(), 0);
    return 0;
}

CardInShopInfo* GetpCardInfoFromId(int id)
{
    for (UINT i = 0; i < 56; i++)
    {
        if (VALUED(i * 0x34 + 0x4C53C4) == id)
        {
            return (CardInShopInfo*)(i * 0x34 + 0x4C53C0);
        }
    }
    return (CardInShopInfo*)(VALUED(0x4C5F20));
}



void M_OpenSuperShop()
{
    vector3f v = {448,32,0};
    DWORD shop = (DWORD)AllocMemory(0xE3C);
    VALUED(shop + 0x98) = 0;
    VALUED(shop + 0xC) = 0;
    VALUED(shop + 0xE0) = 0;
    VALUED(shop + 0xDC) = 1;
    VALUED(shop + 0x14) = 999;
    VALUED(shop + 0x170) = 0;
    VALUED(shop + 0xE4) = 0;
    VALUED(shop + 0x1B8) = 0;
    VALUED(shop + 0x1B4) = 1;
    VALUED(shop + 0xEC) = 999;
    VALUED(shop + 0x1CC) &= 0xFFFFFFFE;
    VALUED(shop + 0x204) &= 0xFFFFFFFE;
    VALUED(shop + 0x218) &= 0xFFFFFFFE;
    VALUED(shop + 0x228) = 0;
    memset((void*)(shop + 0x22C), 0, 0x400u);
    memset((void*)(shop + 0x62C), 0, 0x400u);
    InitMemory((void*)shop, 0, 0xE3Cu);
    VALUED(0x4CF2A4) = shop;
    VALUED(shop) |= 2u;
    if (PPLAYER2)
        HidePlayerOpANM(PPLAYER2);
    if (M_InitSuperShop((Shop*)shop,0, &v))
    {
        int* (__thiscall * sub_4176E0)(int thiz)=0;
        VALUED(&sub_4176E0) = 0x4176E0;
        sub_4176E0(shop);
    }
}

void InjectShop()
{
    //HookCall((LPVOID)0x00443C06, (LPVOID) M_InitShop);
}


void __fastcall M_OnPlayerPositionSet__autoAimMod(int ecx_vx,int edx_vy)
{
    return;
    AnmObj* pStageAnm=VALUEV(0x004CCFB4,AnmObj*);
    *pStageAnm->renderFlag() |= 0x04000000;//enable 2d rotation
    *pStageAnm->renderFlag() &= ~0x00400000;//disable left base
    *pStageAnm->renderFlag() &= ~0x01000000;//disable top base
    vector3f posA=*pStageAnm->posAbs();
    vector3f posB = { 0,0,0 };
    float st_ratio = posA.x / 32.0f;
    posB.x= st_ratio *192.0f;
    posB.y= st_ratio *224.0f;
    *pStageAnm->pos() = posB;
    //calculate the pos by middle point
    //[004CF410]+20574
    
    static UINT id_enm_Focused=1;
    static vector2f focusedPos = { 0,0 };
    vector2f pos_to_focus = { 0,0 };
    bool can_find_enm=false;
    {
        DWORD enmBase = *(DWORD*)(0x004CF2D0);
        if (enmBase)
        {
            DWORD iter = VALUED(enmBase + 0x18C);
            while (iter)
            {
                Enemy* pEnm = VALUEV(iter, Enemy*);
                {
                    if (*pEnm->id() == id_enm_Focused && ((*pEnm->flag() & 0xC000021) == 0))
                    {
                        pos_to_focus.x = pEnm->positionFin()->x;
                        pos_to_focus.y = pEnm->positionFin()->y;
                        can_find_enm = true;
                    }
                }
                iter = VALUED(iter + 0x4);
            }
        }
    }
    //find the enm to focus
    //shift+D to change the focused enm
    if (((VALUED(0x004CA434) & 0x800) && (VALUED(0x004CA428) & 0x8))|| can_find_enm==false)
    {
        bool flag = false;
        bool is_find = false;
        if (can_find_enm == false)
            flag = true;//find the 1st enm which can attack
        UINT i = 0;
        do
        {
            DWORD enmBase = *(DWORD*)(0x004CF2D0);
            if (enmBase)
            {
                DWORD iter = VALUED(enmBase + 0x18C);
                while (iter)
                {
                    Enemy* pEnm = VALUEV(iter, Enemy*);
                    {
                        if (flag == true && ((*pEnm->flag() & 0xC000021) == 0))
                        {
                            is_find = true;
                            id_enm_Focused = *pEnm->id(); break;
                        }
                        if (*pEnm->id() == id_enm_Focused && ((*pEnm->flag() & 0xC000021) == 0)) {
                            flag = true;
                        }
                    }
                    iter = VALUED(iter + 0x4);
                }
            }
            i++;
            if (is_find == false)
                flag = true;
        } while (is_find == false && (i!=2));
    }

    float speed_focus = 0.02f;
    focusedPos.x = focusedPos.x * (1-speed_focus) + pos_to_focus.x * speed_focus;
    focusedPos.y = focusedPos.y * (1-speed_focus) + pos_to_focus.y * speed_focus;

    auto ppl=PPLAYER2;
    vector2f cur_pos_ppl = { ppl->fPosition()->x,ppl->fPosition()->y };
    static vector2f pos_ppl = { 0,400 };

    float speed_focus3 = 0.06f;
    pos_ppl.x = pos_ppl.x * (1 - speed_focus3) + cur_pos_ppl.x * speed_focus3;
    pos_ppl.y = pos_ppl.y * (1 - speed_focus3) + cur_pos_ppl.y * speed_focus3;


    float angle = -(PI/2.0f+atan2f(focusedPos.y - pos_ppl.y, focusedPos.x - pos_ppl.x));
    pStageAnm->rotation()->z = angle;

    float speed_focus2 = 0.03f;
    static vector2f midPos = { 0,0 };
    vector2f midPos_to = { 0.5f * (pos_ppl.x + focusedPos.x)-0.0f,0.5f * (pos_ppl.y + focusedPos.y)-224.0f };
    midPos.x = midPos.x * (1 - speed_focus2) + midPos_to.x * speed_focus2;
    midPos.y = midPos.y * (1 - speed_focus2) + midPos_to.y * speed_focus2;

    midPos.y = midPos.y + 224.0f;
    float scale = 200.0f / DIST_VEC2(pos_ppl,midPos );
    scale = 2 * scale / (scale + 1.0f);
    pStageAnm->scaleRel()->x = scale;
    pStageAnm->scaleRel()->y = scale;
    midPos.y = midPos.y - 224.0f;

    vector2f midPosp = { midPos.x * cosf(angle) - midPos.y * sinf(angle),midPos.x * sinf(angle) + midPos.y * cosf(angle) };
    midPosp.x *= scale*st_ratio;
    midPosp.y *= scale*st_ratio;
    pStageAnm->pos2()->x = -midPosp.x;
    pStageAnm->pos2()->y = -midPosp.y;

    ppl->iPosition_fixed()->x -= ecx_vx;
    ppl->iPosition_fixed()->y -= edx_vy;//disable original move
    vector2i mv = { ecx_vx * cosf(-angle) - edx_vy * sinf(-angle),ecx_vx * sinf(-angle) + edx_vy * cosf(-angle) };
    ppl->iPosition_fixed()->x += mv.x/scale;
    ppl->iPosition_fixed()->y += mv.y / scale;
    *ppl->hitbox_focused() = 3.0f / scale;
    *ppl->hitbox_highspeed() = 3.0f / scale;
    VALUEF(0x004CCBF0) = 1.5 / (scale+0.5);//game speed

    DWORD psht=VALUED((DWORD)ppl+ 0x47940);
    for (UINT i = 0; i < 10; i++)
    {
        DWORD pshotGroup=VALUED(psht + 0xE0 + i * 4);
        while (VALUEV(pshotGroup,INT8) > 0)
        {
            if (VALUEF(pshotGroup + 0x1C) == 0)
            {
                VALUEF(pshotGroup + 0x1C) = VALUEF(pshotGroup + 0x14);
            }else{
                VALUEF(pshotGroup + 0x14) = VALUEF(pshotGroup + 0x1C)-angle;
            }
            pshotGroup += 0x5C;
        }
    }
    
}

vector2f GetUVStageBackGround(vector2f iuv)
{
    float scale = ANM_TOWIND_SCALE;
    vector2f iuv2 = { iuv.x*0.6f+0.2f,iuv.y*0.933333f+0.033333f };
    return { 0.5f + (iuv2.x-0.5f) / scale, 0.5f + (iuv2.y-0.5f) / scale };
}

void __fastcall M_OnPlayerPositionSet(int ecx_vx, int edx_vy)
{
   // return;
    AnmObj* pStageAnm = STAGE_FINAL_ANMOBJ;
    Custom_vertex1* vertices = nullptr;
    const UINT num_vertex = 7*8*6;
    if (*pStageAnm->p_verticeArr() == nullptr)
    {
        vertices = new Custom_vertex1[num_vertex];
        *pStageAnm->p_verticeArr() = vertices;
        *pStageAnm->anm_globalVar_10000_10003(0) = num_vertex/3;
        *pStageAnm->anm_globalVar_10000_10003(1) = 999999;
        *pStageAnm->verticeNum() = num_vertex;
        *pStageAnm->renderFlag() |= 28 << 26;
    }
    vertices = (Custom_vertex1*) *pStageAnm->p_verticeArr();
    {
        float scale = ANM_TOWIND_SCALE;
        float offy = VALUEV(0x004CD2BC, vector2i).y;
        float offx = VALUEV(0x004CD2BC, vector2i).x;
        static float tx = 0.0f;
        tx += 0.05f;
        float t = cosf(tx)*0.4f + 0.6f;
        float a = cosf(tx)*0.5f;
        vector2f Angle = { 1,0 };
        UINT k = 0;
        for (UINT i = 0; i <= 6; i++)
        {
            for (UINT j = 0; j <= 7; j++)
            {
                float px_a = i * 64*scale+offx;
                float py_a = j * 64*scale+offy;
                float dx = 64 * scale*t;
                float dy = 64 * scale*t;

                float px_uv = i * 64/384.0f;
                float py_uv = j * 64/448.0f;
                float dx_uv = 64 / 384.0f;
                float dy_uv = 64 / 448.0f;
                vertices[k].pos = ToVec3f(RotateVec({ px_a,py_a},Angle));
                vertices[k].rhw = 1.0f;vertices[k].spec=0xFFFFFFFF;
                vertices[k].uv = GetUVStageBackGround({ px_uv,py_uv }); k++;
                vertices[k].pos = ToVec3f(RotateVec({ px_a + dx,py_a }, Angle));
                vertices[k].rhw = 1.0f; vertices[k].spec = 0xFFFFFFFF;
                vertices[k].uv = GetUVStageBackGround({ px_uv+dx_uv,py_uv }); k++;
                vertices[k].pos = ToVec3f(RotateVec({ px_a ,py_a + dy }, Angle));
                vertices[k].rhw = 1.0f; vertices[k].spec = 0xFFFFFFFF;
                vertices[k].uv = GetUVStageBackGround({ px_uv,py_uv+dy_uv }); k++;

                vertices[k].pos = ToVec3f(RotateVec({ px_a + dx,py_a }, Angle));
                vertices[k].rhw = 1.0f; vertices[k].spec = 0xFFFFFFFF;
                vertices[k].uv = GetUVStageBackGround({ px_uv + dx_uv,py_uv }); k++;
                vertices[k].pos = ToVec3f(RotateVec({ px_a ,py_a + dy }, Angle));
                vertices[k].rhw = 1.0f; vertices[k].spec = 0xFFFFFFFF;
                vertices[k].uv = GetUVStageBackGround({ px_uv,py_uv + dy_uv }); k++;
                vertices[k].pos = ToVec3f(RotateVec({ px_a + dx  ,py_a + dy }, Angle));
                vertices[k].rhw = 1.0f; vertices[k].spec = 0xFFFFFFFF;
                vertices[k].uv = GetUVStageBackGround({ px_uv + dx_uv,py_uv + dy_uv }); k++;

            }
        }


        //for (UINT i = 0; i < num_vertex;)
        //{
        //    float xPos_st = GetRndFloat01_Rep();
        //    float yPos_st = GetRndFloat01_Rep();
        //
        //    float shake1 = GetRndFloat11_Rep() * 0.01f;
        //    float shake2 = GetRndFloat11_Rep() * 0.01f;
        //
        //    float xPos = xPos_st * 384.0f * scale+ offx;
        //    float yPos = yPos_st * 448.0f * scale+ offy;
        //
        //    float deltaX = 40.0f;
        //
        //    vertices[i].pos = { xPos,yPos- deltaX*scale*1.7f,0};
        //    vertices[i].rhw = 1.0f;
        //    vertices[i].spec=0xFFFFFFFF;
        //    vertices[i].uv = GetUVStageBackGround({ xPos_st+ shake1,yPos_st- deltaX/448.0f*1.7f +shake2});
        //    i++;
        //    vertices[i].pos = { xPos+ deltaX * scale,yPos,0 };
        //    vertices[i].rhw = 1.0f;
        //    vertices[i].spec = 0xFFFFFFFF;
        //    vertices[i].uv = GetUVStageBackGround({ xPos_st + deltaX / 384.0f +shake1,yPos_st + shake2 });
        //    i++; 
        //    vertices[i].pos = { xPos- deltaX * scale,yPos,0 };
        //    vertices[i].rhw = 1.0f;
        //    vertices[i].spec = 0xFFFFFFFF;
        //    vertices[i].uv = GetUVStageBackGround({ xPos_st - deltaX / 384.0f + shake1,yPos_st + shake2 });
        //    i++;
        //}

    }
}


void __fastcall M_OnPlayerPositionSet__hideBullets(int ecx_vx, int edx_vy)
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
            //DWORD pbt = VALUED(iter);
            float threat = 0.0f;
            Bullet* pbt = VALUEV(iter,Bullet*);
            Player* ppl = PPLAYER2;
            vector2f xBt = *(vector2f*)pbt->position();
            vector2f xPl = *(vector2f*)ppl->fPosition();
            vector2f vPl = { ecx_vx / 128.0f,edx_vy / 128.0f };
           
            vector2f vBt = *(vector2f*)pbt->velocity();
            //xB+vB*t=xP+vP*(cos a + isin a)*t
            //xP-xB=(vB-vP*(cos a + isin a))t
            float spdPl = 0.0f;
            float hiSpd = *ppl->iVelocity_fixed_hiSpd() / 128.0f;
            float loSpd= *ppl->iVelocity_fixed_loSpd() / 128.0f;
            if (hypotf(vPl.x, vPl.y) <hiSpd - 0.1f)
            {
                spdPl = loSpd*1.5f;
            }else{
                spdPl = hiSpd;
            }
            vector2f delX = { xPl.x - xBt.x,xPl.y - xBt.y };
            float lenDelX = max(0.01f,hypotf(delX.x, delX.y));
            float lenVBt=hypotf(vBt.x,vBt.y);

            float proj = (delX.x * vBt.x + delX.y * vBt.y) / lenDelX;
            float vert=sqrtf(max(0.0f,lenVBt* lenVBt-proj*proj));
            float proj2s = spdPl * spdPl - vert * vert;
            float proj2s_hiSpd = hiSpd * hiSpd - vert * vert;
            if (proj2s_hiSpd < 0)
                goto COLOR_BULLET;
            if (proj2s < 0)
            {
                threat = 0.1f;
                goto COLOR_BULLET;
            }
            float proj2 = sqrtf(proj2s);
            float t1 = lenDelX/(proj - proj2);
            float t2 = lenDelX / (proj + proj2);
            if (t2 > 90)
                goto COLOR_BULLET;
            threat = 1.0f;
            float c = (vBt.x*delX.x+vBt.y*delX.y)/(lenVBt*lenDelX);
            c = max(c, 0);

            if (c >= 0.7f)
                threat = 1.0f;
            else
                threat = (c+2.0f)*(c+2.0f)/9.0f;

            if (c >= 0.8f)
                goto COLOR_BULLET;

            float th2 = 1.0f;
            if(lenDelX>100.0f)
                th2 = expf(-powf(lenDelX / 200.0f,5.0f));//e^(-((x/200)^5.0))
            threat *= th2;
COLOR_BULLET:
            *(pbt->pAnm()->color1())&=0x00FFFFFF;
            *(pbt->pAnm()->color1()) |= ((BYTE)(threat * 255))<<24;
            iter = VALUED(iter + 0x4);
        }
    }
}

void InjectPlayerPositionTest()
{
    //Hook((LPVOID)0x0045B6FF, 6, (LPVOID)M_OnPlayerPositionSet__autoAimMod);
    //Hook((LPVOID)0x0045B6FF, 6, (LPVOID)M_OnPlayerPositionSet__hideBullets);
}
