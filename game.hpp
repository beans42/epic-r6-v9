#pragma once
#include "includes.hpp"
#include "memory.hpp"

void resolve_pointers() {
	g_game_manager = RPM<uintptr_t>((uintptr_t)g_module.modBaseAddr + g_game_man_offset);
	g_entity_list = RPM<uintptr_t>(g_game_manager + OFFSET_GAMEMANAGER_ENTITYLIST);

	g_camera = RPM<uintptr_t>((uintptr_t)g_module.modBaseAddr + g_prof_man_offset);
	g_camera = RPM<uintptr_t>(g_camera + GAMEPROFILE_CHAIN1);
	g_camera = RPM<uintptr_t>(g_camera + GAMEPROFILE_CHAIN2);
	g_camera = RPM<uintptr_t>(g_camera + GAMEPROFILE_CHAIN3);
}

struct entity_t {
	uintptr_t m_ptr = 0x0;
	int m_health = 0;
	D3DXVECTOR3 m_origin = { 0.f, 0.f, 0.f }, m_top_origin = { 0.f, 0.f, 0.f }, m_headpos = { 0.f, 0.f, 0.f };

	void set_health() {
		auto buffer = RPM<uintptr_t>(m_ptr + OFFSET_ENTITY_ENTITYINFO);
		buffer = RPM<uintptr_t>(buffer + OFFSET_ENTITYINFO_MAINCOMPONENT);
		buffer = RPM<uintptr_t>(buffer + OFFSET_MAINCOMPONENT_CHILDCOMPONENT);
		m_health = RPM<int>(buffer + OFFSET_CHILDCOMPONENT_HEALTH_INT);
	}

	void set_origin() {
		struct bone_t {
			uint8_t padding[OFFSET_ENTITY_HEAD];
			D3DXVECTOR3 head; //+ 0x6A0
			uint8_t filler[OFFSET_ENTITY_FEET - OFFSET_ENTITY_HEAD - sizeof(D3DXVECTOR3)];
			D3DXVECTOR3 feet; //+ 0x700
		};

		auto buffer = RPM<uintptr_t>(m_ptr + OFFSET_ENTITY_REF);
		auto bones = RPM<bone_t>(buffer);
		m_origin = bones.feet;
		m_headpos = bones.head;
		m_top_origin = bones.head + D3DXVECTOR3(0.f, 0.f, 0.2f);
	}

	void set_all() {
		set_health();
		set_origin();
	}
};

struct view_matrix_t {
	uint8_t padding[OFFSET_CAMERA_VIEWRIGHT];
	D3DXVECTOR3 ViewRight; //+ 0x1C0
	uint8_t filler0[OFFSET_CAMERA_VIEWUP - OFFSET_CAMERA_VIEWRIGHT - sizeof(D3DXVECTOR3)];
	D3DXVECTOR3 ViewUp; //+ 0x1D0
	uint8_t filler1[OFFSET_CAMERA_VIEWFORWARD - OFFSET_CAMERA_VIEWUP - sizeof(D3DXVECTOR3)];
	D3DXVECTOR3 ViewForward; //+ 0x1E0
	uint8_t filler2[OFFSET_CAMERA_VIEWTRANSLATION - OFFSET_CAMERA_VIEWFORWARD - sizeof(D3DXVECTOR3)];
	D3DXVECTOR3 ViewTranslation; //+ 0x1F0
	uint8_t filler3[OFFSET_CAMERA_VIEWFOVX - OFFSET_CAMERA_VIEWTRANSLATION - sizeof(D3DXVECTOR3)];
	D3DXVECTOR2 fov; //+ 0x380

	void update() {
		auto buffer     = RPM<view_matrix_t>(g_camera);
		ViewRight       = buffer.ViewRight;
		ViewUp          = buffer.ViewUp;
		ViewForward     = buffer.ViewForward;
		ViewForward    *= -1;
		ViewTranslation = buffer.ViewTranslation;
		fov             = buffer.fov;
	}

	D3DXVECTOR3 world_to_screen(D3DXVECTOR3 position) {
		D3DXVECTOR3 temp = position - ViewTranslation;

		fov.x = std::abs(fov.x);
		fov.y = std::abs(fov.y);

		float x = temp.x * ViewRight.x + temp.y * ViewRight.y + temp.z * ViewRight.z;
		float y = temp.x * ViewUp.x + temp.y * ViewUp.y + temp.z * ViewUp.z;
		float z = temp.x * ViewForward.x + temp.y * ViewForward.y + temp.z * ViewForward.z;

		return D3DXVECTOR3{
			(g_screen_width / 2) * (1 + x / fov.x / z),
			(g_screen_height / 2) * (1 - y / fov.y / z),
			z
		};
	}
};

extern view_matrix_t g_vm;