#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <dwmapi.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <D3dtypes.h>

#include "offsets.hpp"

//globals
extern const unsigned int g_screen_width, g_screen_height;

extern HWND g_hwnd, g_game_hwnd;

extern DWORD g_process_id;
extern HANDLE g_process_handle;
extern MODULEENTRY32 g_module;

extern uintptr_t g_game_man_offset, g_prof_man_offset;
extern uintptr_t g_game_manager, g_entity_list, g_camera;

extern IDirect3D9* g_d3;
extern IDirect3DDevice9* g_d3dev;
extern ID3DXFont* g_d3Font;
extern ID3DXLine* g_d3Line;