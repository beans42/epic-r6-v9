#include "includes.hpp"
#include "memory.hpp"
#include "game.hpp"
#include "renderer.hpp"
#include <vector>

//defining globals
const unsigned int g_screen_width = GetSystemMetrics(SM_CXSCREEN);
const unsigned int g_screen_height = GetSystemMetrics(SM_CYSCREEN);

HWND g_hwnd, g_game_hwnd;

DWORD g_process_id;
HANDLE g_process_handle;
MODULEENTRY32 g_module;

uintptr_t g_game_man_offset, g_prof_man_offset;
uintptr_t g_game_manager, g_entity_list, g_camera;
view_matrix_t g_vm;

IDirect3D9* g_d3;
IDirect3DDevice9* g_d3dev;
ID3DXFont* g_d3Font;
ID3DXLine* g_d3Line;

void render() {
	g_d3dev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	g_d3dev->BeginScene();


	std::vector<entity_t> ents;
	for (int i = 1; i < 32; i++) {
		entity_t buffer;
		buffer.m_ptr = RPM<uintptr_t>(g_entity_list + i * OFFSET_GAMEMANAGER_ENTITY); if (!buffer.m_ptr) continue;
		buffer.set_all();
		if (buffer.m_health < 1 || buffer.m_health > 100) continue;

		ents.push_back(buffer);
	}

	g_vm.update();

	for (entity_t ent : ents) {
		auto feet_position = g_vm.world_to_screen(ent.m_origin);  if (feet_position.z < 0.f) continue;
		auto head_position = g_vm.world_to_screen(ent.m_headpos); if (head_position.z < 0.f) continue;
		auto box_top = g_vm.world_to_screen(ent.m_top_origin);    if (box_top.z < 0.f)       continue;

		float box_height = feet_position.y - box_top.y;
		float box_width = box_height / 2.4;
		
		draw_outlined_rect(box_top.x - box_width / 2, box_top.y, box_width, box_height, epic_blue);
		draw_healthbars(box_top.x - box_width / 2 - 8, box_top.y, 2, box_height, ent.m_health, 100, epic_blue);
		draw_circle(head_position.x, head_position.y, box_height / 12.5f, 60, epic_blue);
	}

	g_d3dev->EndScene();
	g_d3dev->Present(NULL, NULL, NULL, NULL);
}

void pause(const char* msg) {
	std::cout << msg << "\nPress any key to continue . . ."; getchar();
	exit(EXIT_FAILURE);
}

int main() {
	SetConsoleTitleA("jizzware R6 v2");

	g_game_hwnd = FindWindowA(NULL, "Rainbow Six");

	if (g_game_hwnd) {
		std::cout << "[+] Attached to process" << std::endl;

		GetWindowThreadProcessId(g_game_hwnd, &g_process_id);
		g_process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_process_id);

		if (g_process_handle == INVALID_HANDLE_VALUE || g_process_handle == NULL) { pause("[+] Could not attach to process"); }

		g_module = get_module("RainbowSix.exe", g_process_id);

		setup_window();
		std::cout << "[+] Created overlay" << std::endl;

		g_game_man_offset = find_pattern(g_module, "\x48\x8B\x05\x00\x00\x00\x00\x8B\x8E", "xxx????xx") + 0x3;
		g_game_man_offset += (uint64_t)RPM<uint32_t>(g_game_man_offset) + 0x4 - (uintptr_t)g_module.modBaseAddr;
		printf("[+] Found GameManager @ 0x%X\n", g_game_man_offset);

		g_prof_man_offset = find_pattern(g_module, "\x48\x8B\x05\x00\x00\x00\x00\x33\xD2\x4C\x8B\x40\x78", "xxx????xxxxxx") + 0x3;
		g_prof_man_offset += (uint64_t)RPM<uint32_t>(g_prof_man_offset) + 0x4 - (uintptr_t)g_module.modBaseAddr;
		printf("[+] Found ProfileManager @ 0x%X\n", g_prof_man_offset);

		resolve_pointers();

		std::cout << "[+] Started reading thread, starting rendering" << std::endl;
		loop();
	} else { pause("[+] Game must be running to continue"); }
}