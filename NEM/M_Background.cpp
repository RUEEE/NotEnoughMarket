#include "pch.h"
#include "M_Background.h"
#include "Address.h"
#include <d3dx9.h>
#include <d3d9.h>
#include "injection.h"
extern IDirect3DDevice9* g_device;
int __fastcall M_RenderBG(DWORD thiz)
{
	static int(__thiscall * RenderBG)(int thiz)
		= (int (__thiscall *)(int))(0x41C290);
	return RenderBG(thiz);
	D3DXVECTOR3* nowPos = (D3DXVECTOR3*)(0x004CCDF0 + 0x688 + 0x0);
	D3DXVECTOR3* up_vec = (D3DXVECTOR3*)(0x004CCDF0 + 0x688 + 0x18);
	D3DXVECTOR3* lookAt = (D3DXVECTOR3*)(0x004CCDF0 + 0x688 + 0x24);
	float *yFov = (float*)(0x004CCDF0 + 0x688 + 0x54);
	D3DXMATRIX * perspectiveFov = (D3DXMATRIX*)(0x004CCDF0 + 0x688 + 0xA0);
	D3DXVECTOR3* offsNowPos = (D3DXVECTOR3*)(0x004CCDF0 + 0x688 + 0x3C);
	D3DXMATRIX* LookAt = (D3DXMATRIX*)(0x004CCDF0 + 0x688 + 0x60);
	int* RenderTarget_left = (int*)(0x004CD558);
	int* RenderTarget_top = (int*)(0x004CD55C);
	int* RenderTarget_width = (int*)(0x004CD560);
	int* RenderTarget_height = (int*)(0x004CD564);

	D3DRECT clearRC{ *RenderTarget_left,*RenderTarget_top,
		*RenderTarget_left+ *RenderTarget_width,
		*RenderTarget_top+ *RenderTarget_height };
	
	static bool is_read_file = false;
	static ID3DXMesh* g_mesh;
	static D3DMATERIAL9* g_mesh_materials;
	static IDirect3DTexture9** g_mesh_textures;
	static DWORD g_num_materials;
	static bool is_suc = false;
	if (!is_read_file)
	{
		ID3DXBuffer* material_buffer;;
		if (FAILED(D3DXLoadMeshFromXA("E:\\test\\tiger.x",
			D3DXMESH_MANAGED,
			g_device, NULL, &material_buffer, NULL,
			&g_num_materials, &g_mesh)))
		{
			int x = GetLastError();
			return 1;
		}
		is_read_file = true;
		is_suc = true;
		D3DXMATERIAL* xmaterials = (D3DXMATERIAL*)material_buffer->GetBufferPointer();
		g_mesh_materials = new D3DMATERIAL9[g_num_materials];
		g_mesh_textures = new IDirect3DTexture9 * [g_num_materials];

		for (DWORD i = 0; i < g_num_materials; i++)
		{
			g_mesh_materials[i] = xmaterials[i].MatD3D;

			// set ambient reflected coefficient, because .x file do not set it.
			g_mesh_materials[i].Ambient = g_mesh_materials[i].Diffuse;

			g_mesh_textures[i] = NULL;

			if (xmaterials[i].pTextureFilename != NULL && strlen(xmaterials[i].pTextureFilename) > 0)
			{
				// Create the texture
				if (FAILED(D3DXCreateTextureFromFileA(g_device,
					xmaterials[i].pTextureFilename,
					&g_mesh_textures[i])))
				{
					// If texture is not in current folder, try parent folder
					const CHAR* strPrefix = "E:\\test\\";
					CHAR strTexture[MAX_PATH];
					strcpy_s(strTexture, MAX_PATH, strPrefix);
					strcat_s(strTexture, MAX_PATH, xmaterials[i].pTextureFilename);
					// If texture is not in current folder, try parent folder
					if (FAILED(D3DXCreateTextureFromFileA(g_device,
						strTexture,
						&g_mesh_textures[i])))
					{
						MessageBox(NULL, L"Could not find texture map", L"Meshes.exe", MB_OK);
					}
				}
			}
		}
		material_buffer->Release();
	}
	if (is_suc)
	{

		D3DXMATRIX mat_world;
		static int time = 0;
		// Turn on ambient lighting 
		D3DLIGHT9 light = {};
		light.Type = D3DLIGHT_POINT;
		light.Diffuse=D3DXCOLOR(1,1,1,1);
		light.Specular=D3DXCOLOR(1,1,1,1);
		light.Ambient=D3DXCOLOR(1,1,1,1);
		light.Position=D3DXVECTOR3(0,1,0);
		light.Range=9999;
		light.Attenuation0=0;
		light.Attenuation1=1;
		light.Attenuation2=0;

		g_device->SetRenderState(D3DRS_LIGHTING,TRUE);
		g_device->SetLight(0, &light);
		g_device->LightEnable(0,TRUE);

		g_device->SetRenderState(D3DRS_AMBIENT, 0x00000000);
		g_device->SetRenderState(D3DRS_ZWRITEENABLE, 1);
		g_device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		g_device->SetRenderState(D3DRS_FOGENABLE,FALSE);
		g_device->SetRenderState(D3DRS_FOGENABLE,FALSE);
		g_device->SetRenderState(D3DRS_TEXTUREFACTOR,0x00000000);
		//g_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		//g_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

		//g_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
		g_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		//g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		g_device->Clear(0, NULL,  D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);

		D3DXMatrixRotationY(&mat_world, time++ / 1000.0f);
		time += 5;
		g_device->SetTransform(D3DTS_WORLD, &mat_world);
		D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);
		D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
		D3DXMATRIXA16 matView;
		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
		g_device->SetTransform(D3DTS_VIEW, &matView);
		D3DXMATRIXA16 matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
		g_device->SetTransform(D3DTS_PROJECTION, &matProj);

		for (DWORD i = 0; i < g_num_materials; i++)
		{
			g_device->SetMaterial(&g_mesh_materials[i]);
			g_device->SetTexture(0, g_mesh_textures[i]);

			g_mesh->DrawSubset(i);
		}
	}
	return 1;
}

void Inject_BG()
{
	//Address<DWORD>(0x41BAB6).SetValue((DWORD)M_RenderBG);
}