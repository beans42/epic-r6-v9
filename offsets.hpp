#pragma once
//Game -> GameProfile ->
constexpr uintptr_t GAMEPROFILE_CHAIN1 = 0x78;
constexpr uintptr_t GAMEPROFILE_CHAIN2 = 0x0;
constexpr uintptr_t GAMEPROFILE_CHAIN3 = 0x130;

//Game -> GameProfile -> Camera ->
constexpr uintptr_t OFFSET_CAMERA_VIEWRIGHT = 0x1C0;
constexpr uintptr_t OFFSET_CAMERA_VIEWUP = 0x1D0;
constexpr uintptr_t OFFSET_CAMERA_VIEWFORWARD = 0x1E0;
constexpr uintptr_t OFFSET_CAMERA_VIEWTRANSLATION = 0x1F0;
constexpr uintptr_t OFFSET_CAMERA_VIEWFOVX = 0x380;
constexpr uintptr_t OFFSET_CAMERA_VIEWFOVY = 0x384;

//Game -> GameManager ->
constexpr uintptr_t OFFSET_GAMEMANAGER_ENTITYLIST = 0xC8;
constexpr uintptr_t OFFSET_GAMEMANAGER_ENTITY = 0x8; //Size

//Game -> GameManager -> EntityList -> INDEX -> Entity ->
constexpr uintptr_t OFFSET_ENTITY_ENTITYINFO = 0x28;
constexpr uintptr_t OFFSET_ENTITYINFO_MAINCOMPONENT = 0xD8; // EntityInfo ->
constexpr uintptr_t OFFSET_MAINCOMPONENT_CHILDCOMPONENT = 0x8; // MainComponent ->
constexpr uintptr_t OFFSET_CHILDCOMPONENT_HEALTH_INT = 0x168; // ChildComponent ->

//Game -> GameManager -> EntityList -> INDEX ->
constexpr uintptr_t OFFSET_ENTITY_REF = 0x20;
constexpr uintptr_t OFFSET_ENTITY_HEAD = 0x6A0;
constexpr uintptr_t OFFSET_ENTITY_FEET = 0x700;