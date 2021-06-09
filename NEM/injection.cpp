#include "pch.h"
#include "injection.h"
#include "..\imgui\imgui.h"
#include "..\imgui\imgui_impl_dx9.h"
#include "..\imgui\imgui_impl_win32.h"
#include "gui.h"

#include "M_AbilityCard.h"


#include <memory>
#include <d3d9.h>
#include <d3dx9.h>
void Assert(const WCHAR* s)
{
    MessageBox(NULL, s, L"warning", MB_OK);
}

void Hook(LPVOID addr_inject, size_t move_bytes, LPVOID callee)//inject a call( void call(void) ) in anywhere
{
    if (move_bytes < 5) { Assert(L"space limited"); return; }
    DWORD oldprotect;
    DWORD oldprotect2;
    VirtualProtect(addr_inject, move_bytes, PAGE_EXECUTE_READWRITE, &oldprotect);

    BYTE code[14] = { 0x60,0xB8,0xCC,0xCC,0xCC,0xCC,0xFF,0xD0,0x61,0xE9, 0xCC,0xCC,0xCC,0xCC };
    BYTE* alloc = (BYTE*)malloc(move_bytes + 14);
    if (!alloc) { Assert(L"failed to allocate memory"); return; }
    for (size_t i = 0; i < move_bytes; i++)
    {
        alloc[i] = *(BYTE*)((DWORD)addr_inject + i);
    }
    (*((LPVOID*)&(code[2]))) = callee;
    DWORD jmpTo = ((DWORD)addr_inject + move_bytes) - ((DWORD)alloc + move_bytes + 9) - 5;
    (*((LPVOID*)&(code[10]))) = (LPVOID)jmpTo;
    for (size_t i = 0; i < 14; i++)
    {
        alloc[i + move_bytes] = code[i];
    }
    ((BYTE*)addr_inject)[0] = 0xE9;
    (*(DWORD*)((DWORD)addr_inject + 1)) = (DWORD)alloc - (DWORD)addr_inject - 5;
    for (int i = 5; i < move_bytes; i++)
    {
        ((BYTE*)addr_inject)[i] = 0x90;
    }
    VirtualProtect(addr_inject, move_bytes, oldprotect, &oldprotect2);
    VirtualProtect(alloc, move_bytes + 14, PAGE_EXECUTE_READWRITE, &oldprotect2);
    return;
}

void HookCall()//the function must be same with the function injected
{

}


void** findImportAddress(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName)
{
    // 被hook的模块基址
    uintptr_t hookModuleBase = (uintptr_t)hookModule;
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hookModuleBase;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(hookModuleBase + dosHeader->e_lfanew);
    // 导入表
    PIMAGE_IMPORT_DESCRIPTOR importTable = (PIMAGE_IMPORT_DESCRIPTOR)(hookModuleBase
        + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);


    // 遍历导入的模块
    for (; importTable->Characteristics != 0; importTable++)
    {
        // 不是函数所在模块
        if (_stricmp((LPCSTR)(hookModuleBase + importTable->Name), moduleName) != 0)
            continue;


        PIMAGE_THUNK_DATA info = (PIMAGE_THUNK_DATA)(hookModuleBase + importTable->OriginalFirstThunk);
        void** iat = (void**)(hookModuleBase + importTable->FirstThunk);


        // 遍历导入的函数
        for (; info->u1.AddressOfData != 0; info++, iat++)
        {
            if ((info->u1.Ordinal & IMAGE_ORDINAL_FLAG) == 0) // 是用函数名导入的
            {
                PIMAGE_IMPORT_BY_NAME name = (PIMAGE_IMPORT_BY_NAME)(hookModuleBase + info->u1.AddressOfData);
                if (strcmp((LPCSTR)name->Name, functionName) == 0)
                    return iat;
            }
        }


        return NULL; // 没找到要hook的函数
    }


    return NULL; // 没找到要hook的模块
}


BOOL hookVTable(void* pInterface, int index, void* hookFunction, void** oldAddress)
{
    void** address = &(*(void***)pInterface)[index];
    if (address == NULL)
        return FALSE;

    // 保存原函数地址
    if (oldAddress != NULL)
        *oldAddress = *address;

    // 修改虚函数表中地址为hookFunction
    DWORD oldProtect, oldProtect2;
    VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
    *address = hookFunction;
    VirtualProtect(address, sizeof(DWORD), oldProtect, &oldProtect2);

    return TRUE;
}


BOOL unhookVTable(void* pInterface, int index, void* oldAddress)
{
    // 修改回原函数地址
    return hookVTable(pInterface, index, oldAddress, NULL);
}



BOOL hookIAT(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName, void* hookFunction, void** oldAddress)
{
    void** address = findImportAddress(hookModule, moduleName, functionName);
    if (address == NULL)
        return FALSE;


    // 保存原函数地址
    if (oldAddress != NULL)
        *oldAddress = *address;


    // 修改IAT中地址为hookFunction
    DWORD oldProtect, oldProtect2;
    VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
    *address = hookFunction;
    VirtualProtect(address, sizeof(DWORD), oldProtect, &oldProtect2);


    return TRUE;
}


BOOL unhookIAT(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName)
{
    // 取原函数地址
    void* oldAddress = GetProcAddress(GetModuleHandleA(moduleName), functionName);
    if (oldAddress == NULL)
        return FALSE;

    // 修改回原函数地址
    return hookIAT(hookModule, moduleName, functionName, oldAddress, NULL);
}
typedef IDirect3D9* (WINAPI* Direct3DCreate9Type)(UINT SDKVersion);
typedef HRESULT(STDMETHODCALLTYPE* CreateDeviceType)(IDirect3D9* thiz, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);
typedef HRESULT(STDMETHODCALLTYPE* PresentType)(IDirect3DDevice9* thiz, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
Direct3DCreate9Type RealDirect3DCreate9 = NULL;
CreateDeviceType RealCreateDevice = NULL;
PresentType RealPresent = NULL;

IDirect3D9* g_d3d9 = NULL;
IDirect3DDevice9* g_device = NULL;
bool is_hooked = false;

HWND wind_hwnd;
WNDPROC oldWndProc;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT STDMETHODCALLTYPE MyPresent(IDirect3DDevice9* thiz, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
    if (is_hooked)
    {
        static bool x = false;
        g_device->BeginScene();
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        SetGUI(thiz);
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        g_device->EndScene();
    }
    //hook=======================
    unhookVTable(g_device, 17, RealPresent);
    HRESULT res=g_device->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);
    hookVTable(g_device, 17, MyPresent, (void**)&RealPresent); // EndScene是IDirect3DDevice9第43个函数
   //present is no.18
    return res;
}

HRESULT STDMETHODCALLTYPE MyCreateDevice(IDirect3D9* thiz, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    unhookVTable(g_d3d9, 16, RealCreateDevice);
    HRESULT res = RealCreateDevice(thiz, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
    g_device = *ppReturnedDeviceInterface;
    hookVTable(g_device, 17, MyPresent, (void**)&RealPresent); // EndScene是IDirect3DDevice9第43个函数
    //hook===============================
    ImGui_ImplDX9_Init(g_device);
    is_hooked = true;
    {
        auto fonts = ImGui::GetIO().Fonts;
        fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 13.0f, NULL, fonts->GetGlyphRangesChineseSimplifiedCommon());
    }
    return res;
}

IDirect3D9* WINAPI MyDirect3DCreate9(UINT SDKVersion)
{
    //hook
    unhookIAT(GetModuleHandle(NULL), "d3d9.dll", "Direct3DCreate9");
    g_d3d9 = RealDirect3DCreate9(SDKVersion);
    hookVTable(g_d3d9, 16, MyCreateDevice, (void**)&RealCreateDevice); // CreateDevice是IDirect3D9第17个函数
    return g_d3d9;
}

void HookD3D()
{
    hookIAT(GetModuleHandle(NULL), "d3d9.dll", "Direct3DCreate9", MyDirect3DCreate9, (void**)&RealDirect3DCreate9);
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD dwCurProcessId = *((DWORD*)lParam);
    DWORD dwProcessId = 0;

    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
    {
        *((HWND*)lParam) = hwnd;
        return FALSE;
    }
    return TRUE;
}
HWND GetMainWindow()
{
    DWORD dwCurrentProcessId = GetCurrentProcessId();
    if (!EnumWindows(EnumWindowsProc, (LPARAM)&dwCurrentProcessId))
    {
        return (HWND)dwCurrentProcessId;
    }
    return NULL;
}


LRESULT CALLBACK MyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        unHook();
        CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
        exit(666);
        break;
    default:
        break;
    }
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;
    return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
}

HWND (WINAPI* RealCreateWindowExA)(DWORD, LPCSTR , LPCSTR , DWORD , int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
HWND MyCreateWindowExA(DWORD dwExStyle
    ,LPCSTR lpClassName
    , LPCSTR  lpWindowName
    , DWORD dwStyle
    , int X,int Y,int nWidth,int nHeight
    , HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{
    
    HWND hw= RealCreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);
    unhookIAT(GetModuleHandle(NULL), "user32.dll", "CreateWindowExA");
    ImGui_ImplWin32_Init(hw);
    wind_hwnd = hw;
    oldWndProc = (WNDPROC)SetWindowLongA(wind_hwnd, GWL_WNDPROC, (LONG)MyWndProc);
    return hw;
}

BOOL (*WINAPI RealTextOutA)(HDC hdc, int nXStart, int nYStart, LPCSTR lpString, int cbString);

BOOL WINAPI MyTextOutA(HDC hdc, int nXStart, int nYStart, LPCSTR lpString, int cbString)
{
    RECT rc;
    
    //auto hFont = CreateFontA(15, 15, 0, 0, FW_THIN, true, false, false,
   //     CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
   //     CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
   //     FF_MODERN, "宋体");
   
    //SelectObject(hdc, hFont);
   // DeleteObject(hFont);
    rc.left = nXStart; rc.top = nYStart;
    rc.right = nXStart + 1000;
    rc.bottom = rc.top + 1000;

    auto hBrush = CreateSolidBrush(RGB(255, 255, 0));
    //FillRect(hdc, &rc, hBrush);
    return DrawTextA(hdc, lpString, cbString, &rc, DT_LEFT | DT_TOP);
}


void HookWindow()
{
    /*oldWndProc = (WNDPROC)0x00472280;
    DWORD op;
    VirtualProtect((LPVOID)0x00473902, 4, PAGE_EXECUTE_READWRITE, &op);
    *(DWORD*)(0x00473902) = (DWORD)MyWndProc;
    VirtualProtect((LPVOID)0x00473902, 4, op, &op);*/
    hookIAT(GetModuleHandle(NULL), "user32.dll", "CreateWindowExA", MyCreateWindowExA, (void**)&RealCreateWindowExA);
    hookIAT(GetModuleHandle(NULL), "gdi32.dll", "TextOutA", MyTextOutA, (void**)&RealTextOutA);
}

void unHook()
{
    if (is_hooked)
    {
        is_hooked = false;
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        unhookVTable(g_device, 17, RealPresent);
        SetWindowLongA(wind_hwnd, GWL_WNDPROC, (LONG)oldWndProc);
    }
}

void InjectAll()
{
    InjectAbCard();
}