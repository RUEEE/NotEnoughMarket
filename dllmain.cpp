// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "injection.h"
#include "Address.h"
#include "..\imgui\imgui.h"

extern "C" __declspec(dllexport)void func()
{

}

void test()
{
    Address<int> pos(0x004CF410);
    pos = pos + 0x62C;
    pos.SetValue(99999);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ImGui::CreateContext();
        ImGui::StyleColorsLight();
        HookWindow();
        HookCreateMutex();
        HookD3D();
        //Hook((LPVOID)0x0045B717, 0x6, test);
        InjectAll();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        unHook();
        break;
    }
    return TRUE;
}

