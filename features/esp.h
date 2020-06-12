#pragma once

#include "../sdk/sdk.h"
#include "../sdk/player.h"
#include <iostream>
#include <fstream>
#include <string>
#include "../render/menu.hpp"
#include "BulletPrediction.hpp"
#include <thread>
#include <DirectOverlay.h>

typedef void(__fastcall* serverrpc_func)(uintptr_t object, mono_string funcname);
serverrpc_func ServerRPC;

Vector2 MenuPos = { 200.f, 200.f };

JBMenu::JBMenu(void)
{
	this->Visible = true;
}


void JBMenu::Init_Menu(ID3D11DeviceContext* pContext, LPCWSTR Title, int x, int y)
{
	this->pContext = pContext;
	this->Is_Ready = true;
	this->sMenu.Title = Title;
	this->sMenu.x = x;
	this->sMenu.y = y;
}


void JBMenu::AddFolder(LPCWSTR Name, int* Pointer, int limit)
{
	sOptions[this->Items].Name = (LPCWSTR)Name;
	sOptions[this->Items].Function = Pointer;
	sOptions[this->Items].Type = T_FOLDER;
	this->Items++;
}

void JBMenu::AddOption(LPCWSTR Name, int* Pointer, int* Folder, int type, int lower_limit=0, int upper_limit = 2)
{
	if (*Folder == 0)
		return;
	sOptions[this->Items].Name = Name;
	sOptions[this->Items].Function = Pointer;
	sOptions[this->Items].Type = type;
	sOptions[this->Items].upper_limit = upper_limit - 1;
	sOptions[this->Items].lower_limit = lower_limit;
	this->Items++;
}

void JBMenu::Navigation()
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		this->Visible = !this->Visible;

	if (!this->Visible)
		return;

	int value = 0;

	if (GetAsyncKeyState(VK_DOWN))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		this->Cur_Pos++;
		if (sOptions[this->Cur_Pos].Name == 0)
			this->Cur_Pos--;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		this->Cur_Pos--;
		if (this->Cur_Pos == -1)
			this->Cur_Pos++;
	}

	else if (GetAsyncKeyState(VK_RIGHT))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (*sOptions[this->Cur_Pos].Function <= sOptions[this->Cur_Pos].upper_limit)
		{
			value++;
		}
	}

	else if ((GetAsyncKeyState(VK_LEFT)))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (*sOptions[this->Cur_Pos].Function > sOptions[this->Cur_Pos].lower_limit)
		{
			value--;
		}
	}


	if (value)
	{
		*sOptions[this->Cur_Pos].Function += value;
		if (sOptions[this->Cur_Pos].Type == T_FOLDER)
		{
			memset(&sOptions, 0, sizeof(sOptions));
			this->Items = 0;
		}
	}


}

void JBMenu::Draw_Menu()
{
	if (!this->Visible)
		return;

	//this->DrawText(this->sMenu.Title, 14, sMenu.x + 10, sMenu.y, this->Color_Font);
	for (int i = 0; i < this->Items; i++)
	{
		if (this->sOptions[i].Type == T_OPTION)
		{
			if (*this->sOptions[i].Function)
			{
				DrawString("On", 15, (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)));
				//i_renderer.helper->string(Vector2{ (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)) }, L"On");
				//Render::String(vec2_t{ (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)) }, L"On");
				//this->DrawText(L"On", 12, sMenu.x + 150, sMenu.y + LineH * (i + 2), this->Color_On);
			}
			else
			{
				DrawString("Off", 15, (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)));
				//i_renderer.helper->string(Vector2{ (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)) }, L"Off");
				//this->DrawText(L"Off", 12, sMenu.x + 150, sMenu.y + LineH * (i + 2), this->Color_Off);
			}
		}
		if (this->sOptions[i].Type == T_FOLDER)
		{
			if (*this->sOptions[i].Function)
			{
				DrawString("Opened", 15, (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)));
				//i_renderer.helper->string(Vector2{ (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)) }, L"Opened");
				//this->DrawText(L"Opend", 12, sMenu.x + 150, sMenu.y + LineH * (i + 2), this->Color_Folder);
			}
			else
			{
				DrawString("Closed", 15, (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)));
				//i_renderer.helper->string(Vector2{ (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)) }, L"Closed");
				//this->DrawText(L"Closed", 12, sMenu.x + 150, sMenu.y + LineH * (i + 2), this->Color_Folder);
			}
		}
		if (this->sOptions[i].Type == T_BOOL)
		{
			if (*this->sOptions[i].Function)
			{
				DrawString("True", 15, (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)), D2D1::ColorF::Green);
				//i_renderer.helper->string(Vector2{ (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)) }, L"True", D2D1::ColorF::Green);
				//this->DrawText(L"Opend", 12, sMenu.x + 150, sMenu.y + LineH * (i + 2), this->Color_Folder);
			}
			else
			{
				DrawString("False", 15, (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)), D2D1::ColorF::Red);
				//i_renderer.helper->string(Vector2{ (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)) }, L"False", D2D1::ColorF::Red);
				//this->DrawText(L"Closed", 12, sMenu.x + 150, sMenu.y + LineH * (i + 2), this->Color_Folder);
			}
		}
		if (this->sOptions[i].Type == T_INT)
		{
			auto sval = std::to_string((*this->sOptions[i].Function));
			std::wstring wide_string = std::wstring(sval.begin(), sval.end());
			const wchar_t* result = wide_string.c_str();
			//i_renderer.helper->string(Vector2{ (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)) }, result);
			DrawWString(result, 15, (float)(sMenu.x + 150), (float)(sMenu.y + LineH * (i + 2)));
		}
		D2D1::ColorF Color = D2D1::ColorF(250, 250, 250);
		if (this->Cur_Pos == i)
			Color = D2D1::ColorF(250, 0, 200);
		else if (this->sOptions[i].Type == T_FOLDER)
			Color = D2D1::ColorF(0, 250, 250);

		//this->DrawText(this->sOptions[i].Name, 12, sMenu.x + 5, sMenu.y + LineH * (i + 2), Color);
		if (this->sOptions[i].Type == T_FOLDER)
			DrawWString(this->sOptions[i].Name, 15, (float)(sMenu.x + 15), (float)(sMenu.y + LineH * (i + 2)), Color);
		else
			DrawWString(this->sOptions[i].Name, 15, (float)(sMenu.x + 30), (float)(sMenu.y + LineH * (i + 2)), Color);

	}
}

bool JBMenu::IsReady()
{
	if (this->Items)
		return true;
	return false;
}

namespace features
{
	class _esp_t
	{
	public:
		bool LoggingEnabled = false;
		JBMenu menu;
	public:
		void Log(std::string data)
		{
			if (LoggingEnabled)
			{
				std::ofstream outfile;

				outfile.open("Log.txt", std::ios_base::app); // append instead of overwrite
				outfile << data;
				outfile << "\n";
				outfile.close();
			}
		}
		std::string get_class_name(std::uint64_t class_object)
		{
			const auto object_unk = safe_read(class_object, uintptr_t);//mex.Read<uintptr_t>(class_object);

			if (!object_unk)
				return {};

			auto cname = safe_read(object_unk + 0x10, uintptr_t); //mex.Read<uintptr_t>(object_unk + 0x10);
			return read_ascii(cname, 64);
		}

		uint64_t scan_for_klass(const char* name)
		{
			std::cout << "base" << std::endl;
			auto base = (uintptr_t)GetModuleHandleA("GameAssembly.dll"); //mex.GetModuleBase("GameAssembly.dll"); //mem::get_module_base(L"GameAssembly.dll");
			std::cout << "dos_header" << std::endl;
			auto dos_header = safe_read(base, IMAGE_DOS_HEADER); //mex.Read<IMAGE_DOS_HEADER>(base);
			std::cout << "data_header" << std::endl;
			auto data_header = safe_read(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (3 * 40), IMAGE_SECTION_HEADER); //mex.Read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (3 * 40));
			std::cout << "next_section" << std::endl;
			auto next_section = safe_read(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (4 * 40), IMAGE_SECTION_HEADER); //mex.Read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (4 * 40));
			std::cout << "data_size" << std::endl;
			auto data_size = next_section.VirtualAddress - data_header.VirtualAddress;

			if (strcmp((char*)data_header.Name, ".data")) {
				printf("[!] Section order changed\n");
			}
			std::cout << "Looping" << std::endl;
			for (uint64_t offset = data_size; offset > 0; offset -= 8) {
				char klass_name[0x100] = { 0 };
				std::cout << "klass" << std::endl;
				auto klass = safe_read(base + data_header.VirtualAddress + offset, uint64_t); //mex.Read<uint64_t>(base + data_header.VirtualAddress + offset);
				if (klass == 0 || klass == NULL)
				{
					std::cout << "!klass" << std::endl;
					continue;
				}
				std::cout << "name_pointer" << std::endl;
				auto name_pointer = safe_read(klass + 0x10, uint64_t); //mex.Read<uint64_t>(klass + 0x10);
				if (name_pointer == 0 || name_pointer == 0)
				{
					std::cout << "!name_pointer" << std::endl;
					continue;
				}
				//mex.Read(name_pointer, klass_name, sizeof(klass_name));
				std::cout << "safe_memcpy" << std::endl;
				safe_memcpy(klass_name, name_pointer, sizeof(klass_name));
				std::cout << "klass_name" << klass_name << std::endl;
				if (!strcmp(klass_name, name)) {
					//printf("[*] 0x%x -> %s\n", data_header.VirtualAddress + offset, name);
					return klass;
				}
			}

			printf("[!] Unable to find %s in scan\n", name);
			//exit(0);
		}

		void Normalize(float& Yaw, float& Pitch)
		{
			if (Pitch < -89) Pitch = -89;
			else if (Pitch > 89) Pitch = 89;
			if (Yaw < -360) Yaw += 360;
			else if (Yaw > 360) Yaw -= 360;
		}

		Vector3 get_obj_pos(std::uint64_t entity)
		{
			const auto player_visual = safe_read(entity + 0x8, uintptr_t); //mex.Read<uintptr_t>(entity + 0x8);

			if (!player_visual)
				return {};

			const auto visual_state = safe_read(player_visual + 0x38, uintptr_t); //mex.Read<uintptr_t>(player_visual + 0x38);

			if (!visual_state)
				return {};

			return safe_read(visual_state + 0x90, Vector3); //mex.Read<Vector3>(visual_state + 0x90);
		}

		Vector3 GetCurrentObjectPosition(std::uintptr_t entity)
		{
			const auto unk1 = safe_read(entity + 0x10, uintptr_t); //mex.Read<uintptr_t>(entity + 0x10);

			if (!unk1)
				return Vector3{ NULL, NULL, NULL };

			const auto unk2 = safe_read(unk1 + 0x30, uintptr_t); //mex.Read<uintptr_t>(unk1 + 0x30);

			if (!unk2)
				return Vector3{ NULL, NULL, NULL };



			const auto unk3 = safe_read(unk2 + 0x30, uintptr_t); //mex.Read<uintptr_t>(unk2 + 0x30);

			if (!unk3)
				return Vector3{ NULL, NULL, NULL };



			/* shouldn't be needed, but just in case */
			if (!entity)
				return Vector3{ NULL, NULL, NULL };

			Vector2 ScreenPos;
			return get_obj_pos(unk3);
		}

		void do_menu()
		{
			//Player ESP
			menu.AddFolder(L"Player ESP", &settings::player_esp::folder, 1);
			menu.AddOption(L"Enabled", &settings::player_esp::enabled, &settings::player_esp::folder, T_BOOL);
			menu.AddOption(L"Show Names", &settings::player_esp::names_enabled, &settings::player_esp::folder, T_BOOL);
			menu.AddOption(L"Show Distance", &settings::player_esp::distance_enabled, &settings::player_esp::folder, T_BOOL);
			menu.AddOption(L"Show Health", &settings::player_esp::health_enabled, &settings::player_esp::folder, T_BOOL);
			menu.AddOption(L"Show Held", &settings::player_esp::show_held_enabled, &settings::player_esp::folder, T_BOOL);
			menu.AddOption(L"Show Hot Bar", &settings::player_esp::show_hotbar_enabled, &settings::player_esp::folder, T_BOOL);

			//Ore ESP
			menu.AddFolder(L"Ore ESP", &settings::ore_esp::folder, 1);
			menu.AddOption(L"Enabled", &settings::ore_esp::enabled, &settings::ore_esp::folder, T_BOOL);
			menu.AddOption(L"Show Stone Ore", &settings::ore_esp::stone_enabled, &settings::ore_esp::folder, T_BOOL);
			menu.AddOption(L"Show Metal Ore", &settings::ore_esp::metal_enabled, &settings::ore_esp::folder, T_BOOL);
			menu.AddOption(L"Show Sulfur Ore", &settings::ore_esp::sulfur_enabled, &settings::ore_esp::folder, T_BOOL);

			menu.AddFolder(L"Collectible ESP", &settings::collectable_esp::folder, 1);
			menu.AddOption(L"Enabled", &settings::collectable_esp::enabled, &settings::collectable_esp::folder, T_BOOL);

			menu.AddFolder(L"Dropped ESP", &settings::dropped_item_esp::folder, 1);
			menu.AddOption(L"Enabled", &settings::dropped_item_esp::enabled, &settings::dropped_item_esp::folder, T_BOOL);

			menu.AddFolder(L"Stash ESP", &settings::stash_esp::folder, 1);
			menu.AddOption(L"Enabled", &settings::stash_esp::enabled, &settings::stash_esp::folder, T_BOOL);

			menu.AddFolder(L"Container ESP", &settings::container_esp::folder, 1);
			menu.AddOption(L"Enabled", &settings::container_esp::enabled, &settings::container_esp::folder, T_BOOL);
			menu.AddOption(L"Show Barrels", &settings::container_esp::barrels_enabled, &settings::container_esp::folder, T_BOOL);
			menu.AddOption(L"Show Crates", &settings::container_esp::crates_enabled, &settings::container_esp::folder, T_BOOL);

			menu.AddFolder(L"Vehicle ESP", &settings::vehicle_esp::folder, 1);
			menu.AddOption(L"Enabled", &settings::vehicle_esp::enabled, &settings::vehicle_esp::folder, T_BOOL);
			menu.AddOption(L"Show Boats", &settings::vehicle_esp::boats_enabled, &settings::vehicle_esp::folder, T_BOOL);
			menu.AddOption(L"Show Copters", &settings::vehicle_esp::copters_enabled, &settings::vehicle_esp::folder, T_BOOL);
			menu.AddOption(L"Show Horses", &settings::vehicle_esp::horses_enabled, &settings::vehicle_esp::folder, T_BOOL);

			menu.AddFolder(L"Aimbot", &settings::aimbot::folder, 1);
			menu.AddOption(L"Enabled", &settings::aimbot::enabled, &settings::aimbot::folder, T_BOOL);
			menu.AddOption(L"FOV", &settings::aimbot::fov, &settings::aimbot::folder, T_INT, 10, 200);

			menu.AddFolder(L"Misc", &settings::misc::folder, 1);
			menu.AddOption(L"Spider/No Fall", &settings::misc::spider_enabled, &settings::misc::folder, T_BOOL);
			menu.AddOption(L"Always Day", &settings::misc::always_day_enabled, &settings::misc::folder, T_BOOL);
			menu.AddOption(L"Fat Bullet", &settings::misc::fat_bullet_enabled, &settings::misc::folder, T_BOOL);
			menu.AddOption(L"Admin Flags", &settings::misc::admin_flags_enabled, &settings::misc::folder, T_BOOL);
			menu.AddOption(L"Anti Aim", &settings::misc::anti_aim_enabled, &settings::misc::folder, T_BOOL);
			menu.AddOption(L"Fast Heal", &settings::misc::fast_heal_enabled, &settings::misc::folder, T_BOOL);
		}
	} inline esp;
}

void DrawLoopOld(int Width, int Height)
{
	std::cout << "Width: " << Width << std::endl;
	std::cout << "Height: " << Height << std::endl << std::endl;
	DrawString("Test", 15, 10, 10);
}

void CheckKeybinds()
{
	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
	{
		settings::aimbot::shoothead = !settings::aimbot::shoothead;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void DrawLoop(int width, int height)
{
	scr_size = Vector2{ (float)width, (float)height };

	if (features::esp.menu.IsReady() == false)
	{
		//menu.Init_Menu(i_renderer., L"Aspect", 0, 0);
		features::esp.do_menu();
	}
	if (features::esp.menu.Visible)
	{
		features::esp.menu.Draw_Menu();
	}

	features::esp.menu.Navigation();
	CheckKeybinds();
	//auto screensize = i_renderer.helper->canvas_size();
	//scr_size = screensize;
	if (settings::aimbot::shoothead)
	{
		DrawString("Head", 15, 0, scr_size.y / 2, D2D1::ColorF::Red);
	}
	else
	{
		DrawString("Body", 15, 0, scr_size.y / 2, D2D1::ColorF::Red);
	}
	DrawCircle(scr_size.x / 2, scr_size.y / 2, settings::aimbot::fov, 1, D2D1::ColorF::Red, false);

	float FOV = 300, curFOV;
	Players::closestPlayer = NULL;
	//std::cout << "gom_addr!" << std::endl;
	static auto gom_addr = safe_read((uintptr_t)GetModuleHandle("UnityPlayer.dll") + 0x17A6AD8, uint64_t);
	if (!gom_addr || gom_addr == NULL)
	{
		std::cout << "!gom_addr" << std::endl;
		return;
	}
	//std::cout << "taggedObjects!" << std::endl;
	auto taggedObjects = safe_read(gom_addr + 0x8, uintptr_t);
	if (!taggedObjects || taggedObjects == NULL)
	{
		std::cout << "!taggedObjects" << std::endl;
		return;
	}
	//std::cout << "gameObject!" << std::endl;
	auto gameObject = safe_read(taggedObjects + 0x10, uintptr_t);
	if (!gameObject || gameObject == NULL)
	{
		std::cout << "!gameObject" << std::endl;
		return;
	}
	//std::cout << "objClass!" << std::endl;
	auto objClass = safe_read(gameObject + 0x30, uintptr_t);
	if (!objClass || objClass == NULL)
	{
		std::cout << "!objClass" << std::endl;
		return;
	}
	//std::cout << "ent!" << std::endl;
	auto ent = safe_read(objClass + 0x18, uintptr_t);
	if (!ent || ent == NULL)
	{
		std::cout << "!ent" << std::endl;
		return;
	}
	//std::cout << "viewmatrix!" << std::endl;
	player_base.viewmatrix = safe_read(ent + 0xDC, Matrix4x4);
	if (settings::misc::always_day_enabled)
	{
		for (taggedObjects; taggedObjects != safe_read(gom_addr, uintptr_t); taggedObjects = safe_read(taggedObjects + 0x8, uintptr_t))
		{
			gameObject = safe_read(taggedObjects + 0x10, uintptr_t);

			auto tag = safe_read(gameObject + 0x54, USHORT);

			if (tag == 20011)
			{
				//std::cout << "Found TOD_Sky!" << std::endl;

				objClass = safe_read(gameObject + 0x30, uintptr_t);
				if (!objClass || objClass == NULL)
				{
					std::cout << "!objClass" << std::endl;
					return;
				}

				ent = safe_read(objClass + 0x18, uintptr_t);
				if (!ent || ent == NULL)
				{
					std::cout << "!ent" << std::endl;
					return;
				}

				auto unk1 = safe_read(ent + 0x28, uintptr_t);
				//auto instance_list = safe_read(unk1 + 0x0, uintptr_t);

				//auto instances = safe_read(instance_list + 0x10, uintptr_t);
				//std::cout << "instances: " << std::hex << std::uppercase << instances << std::endl;
				//auto tSky = safe_read(instances + 0x20, uintptr_t);
				//std::cout << "tSky: " << std::hex << std::uppercase << tSky << std::endl;
				auto TOD_CycleParameters = safe_read(unk1 + 0x38, uintptr_t);
				//std::cout << "TOD_CycleParameters: " << std::hex << std::uppercase << TOD_CycleParameters << std::endl;
				//mex.Write<float>(TOD_CycleParameters + 0x10, 12.f);
				safe_write(TOD_CycleParameters + 0x10, 12.f, float);
			}
		}
	}

	//std::cout << "base_networkable!" << std::endl;
	//https://aspectnetwork.net/images/VgO4lB7hYFAvGuzn.png
	auto base_networkable = safe_read(GetModuleHandleA("GameAssembly.dll") + 0x285aff0, uint64_t);
	if (!base_networkable)
	{
		std::cout << "!base_networkable" << std::endl;
		return;
	}
	//std::cout << "unk1!" << std::endl;
	auto unk1 = safe_read(base_networkable + 0xb8, uint64_t);
	if (!unk1)
	{
		std::cout << "!unk1" << std::endl;
		return;
	}
	//std::cout << "entities!" << std::endl;
	auto entities = safe_read(unk1, uint64_t);
	if (!entities)
	{
		std::cout << "!entities" << std::endl;
		return;
	}
	//std::cout << "realm!" << std::endl;
	auto realm = safe_read(entities + 0x10, uint64_t);
	if (!realm)
	{
		std::cout << "!realm" << std::endl;
		return;
	}
	//std::cout << "buffer!" << std::endl;
	auto buffer = safe_read(realm + 0x28, uint64_t);
	if (!buffer)
	{
		std::cout << "!buffer" << std::endl;
		return;
	}
	//std::cout << "list!" << std::endl;
	auto list = safe_read(buffer + 0x18, uint64_t);
	if (!list)
	{
		std::cout << "!list" << std::endl;
		return;
	}
	//std::cout << "size!" << std::endl;
	auto size = safe_read(buffer + 0x10, uint64_t);
	for (auto i = 0; i < size; i++)
	{
		//std::cout << "current!" << std::endl;
		//Log("current");
		auto current = safe_read(list + (0x20 + (i * 8)), uint64_t);
		if (!current)
		{
			//std::cout << "!current" << std::endl;
			continue;
		}

		//std::cout << "base!" << std::endl;
		//Log("base");
		auto base = safe_read(current + 0x10, uint64_t);
		if (!base)
		{
			std::cout << "!base" << std::endl;
			continue;
		}

		//std::cout << "object!" << std::endl;
		//Log("object");
		auto object = safe_read(base + 0x30, uint64_t);

		//std::cout << "tag!" << std::endl;
		//Log("tag");
		auto tag = safe_read(object + 0x54, WORD);

		//std::cout << "localElement!" << std::endl;
		//Log("localElement");
		auto localElement = safe_read(list + 0x20, uint64_t); //mex.Read<DWORD64>(list + 0x20);
		//std::cout << "localBO!" << std::endl;
		//Log("localBO");
		auto localBO = safe_read(localElement + 0x10, uint64_t); //mex.Read<DWORD64>(localElement + 0x10);
		//std::cout << "localPlayer!" << std::endl;
		//Log("localPlayer");
		auto localPlayer = safe_read(localBO + 0x30, uint64_t); //mex.Read<DWORD64>(localBO + 0x30);
		//std::cout << "localOC!" << std::endl;
		//Log("localOC");
		auto localOC = safe_read(localPlayer + 0x30, uint64_t); //mex.Read<DWORD64>(localPlayer + 0x30);
		//std::cout << "localT!" << std::endl;
		//Log("localT");
		auto localT = safe_read(localOC + 0x8, uint64_t); //mex.Read<DWORD64>(localOC + 0x8);
		//std::cout << "localVS!" << std::endl;
		//Log("localVS");
		auto localVS = safe_read(localT + 0x38, uint64_t); //mex.Read<DWORD64>(localT + 0x38);
		//std::cout << "LocalPosition!" << std::endl;
		//Log("LocalPosition");
		Players::LocalPlayer::LocalPosition = safe_read(localVS + 0x90, Vector3); //mex.Read<Vector3>(localVS + 0x90);
		//std::cout << "ObjectClass!" << std::endl;
		//Log("ObjectClass");
		std::string ObjectClass = features::esp.get_class_name(current);
		//std::cout << "Object Class: " << ObjectClass << std::endl;
		//Log("Checking Tag");
		if (tag == 6 || tag == 5 || tag == 20011)
		{
			//std::cout << "obj_class!" << std::endl;
			features::esp.Log("obj_class");
			auto obj_class = safe_read(object + 0x30, uint64_t);
			if (!obj_class)
			{
				std::cout << "!obj_class" << std::endl;
				continue;
			}
			//std::cout << "entity!" << std::endl;
			features::esp.Log("entity");
			auto entity = safe_read(obj_class + 0x18, uint64_t);
			if (!entity)
			{
				std::cout << "!entity" << std::endl;
				continue;
			}

			//viewmatrix = safe_read(entity + 0xdc, Matrix4x4);
			if (tag == 6 && settings::player_esp::enabled)
			{
				//std::cout << "className!" << std::endl;
				features::esp.Log("className");
				char className[64];
				auto name_pointer = safe_read(object + 0x60, uint64_t);
				safe_memcpy(className, name_pointer, sizeof(className));
				//std::cout << "Class Name: " << className << std::endl;
				if (className == NULL)
				{
					std::cout << "!className == NULL" << std::endl;
					continue;
				}

				if (className == "")
				{
					std::cout << "className == \"\"" << std::endl;
					continue;
				}

				if (!className)
				{
					std::cout << "!className" << std::endl;
					continue;
				}
				//std::cout << "player!" << std::endl;
				features::esp.Log("player");
				auto player = (player_t*)safe_read(entity + 0x28, uint64_t);
				if (!player)
				{
					std::cout << "!player" << std::endl;
					continue;
				}

				if (strcmp(className, "LocalPlayer") != 0)
				{
					if (player->IsSleeping())
					{
						continue;
					}
					player_base.player = player;
					Vector3 pos = player->GetBonePosition(spine4);
					if (pos.x == 0 && pos.y == 0 && pos.z == 0)
						continue;

					auto Distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, pos);
					if (Distance > 1000)
						continue;

					Vector2 ScreenPos;
					if (player_base.w2s(pos, ScreenPos))
					{

						auto player_health = player->GetHealth();
						float healthf = nearbyint(player_health);
						int health = (int)(healthf);
						if (health < 1)
							continue;

						//helper->circle(ScreenPos, D2D1::ColorF::Red, 5.f);
						curFOV = distance_cursor(ScreenPos);
						if (300 > curFOV && curFOV < FOV && (Players::LocalPlayer::LocalPlayer->GetTeam()->teamID() == 0 || player->GetTeam()->teamID() != Players::LocalPlayer::LocalPlayer->GetTeam()->teamID()))
						{
							FOV = curFOV;
							Players::closestPlayer = player;
						}
						char healthbuffer[0x20]{};
						char distancebuffer[0x20]{};
						sprintf(healthbuffer, "[%d HP]", health);
						sprintf(distancebuffer, "[%dm]", (int)Distance);

						D2D1::ColorF color = D2D1::ColorF::Red;

						if (Players::LocalPlayer::LocalPlayer->GetTeam()->teamID() != 0 && player->GetTeam()->teamID() == Players::LocalPlayer::LocalPlayer->GetTeam()->teamID())
						{
							color = D2D1::ColorF::LawnGreen;
						}

						if (settings::player_esp::names_enabled)
						{
							DrawWString(player->GetName().c_str(), 15, ScreenPos.x, ScreenPos.y, color);
							ScreenPos.y += 15;
						}
						if (settings::player_esp::health_enabled)
						{
							DrawWString(GetWC(healthbuffer), 15, ScreenPos.x, ScreenPos.y, color);
							ScreenPos.y += 15;
						}
						if (settings::player_esp::distance_enabled)
						{
							DrawWString(GetWC(distancebuffer), 15, ScreenPos.x, ScreenPos.y, color);
						}
						
						if (settings::player_esp::show_held_enabled)
						{
							ScreenPos.y += 15;
							//features::esp.Log("Show Held Enabled!");
							DrawWString(player->GetHeldItem()->GetItemName().c_str(), 15, ScreenPos.x, ScreenPos.y, color);
						}
						if (curFOV <= 30 && Players::closestPlayer == player && settings::player_esp::show_hotbar_enabled)
						{
							for (int i = 0; i < 6; i++)
							{
								auto Inventory = player->GetInventory();
								if (!Inventory)
									continue;
								auto Belt = Inventory->GetBelt();
								if (!Belt)
									continue;
								auto Item = Belt->GetItem(i);
								if (!Item)
									continue;
								if (player->GetHeldItem()->GetUID() == Item->GetUID())
									continue;
								auto itemName = Item->GetItemName();
								if (itemName == L"No Item!")
									continue;
								ScreenPos.y += 15;
								char amountbuffer[0x20]{};
								//wsprintf(amountbuffer, "%S [%d]", itemName, Item->GetAmount());
								DrawWString(itemName.c_str(), 15, ScreenPos.x, ScreenPos.y, color);
								//helper->string_center(ScreenPos, GetWC(amountbuffer), D2D1::ColorF::Red);
							}
						}
						
					}
				}
				else
				{
					Players::LocalPlayer::LocalPlayer = player;
					//std::cout << "LocalPlayer: 0x" << std::hex << std::uppercase << LocalPlayer << std::endl;
					if (!Players::LocalPlayer::LocalPlayer->IsSleeping())
					{
						features::esp.Log("DoSpider");
						if (settings::misc::spider_enabled)
							Players::LocalPlayer::LocalPlayer->DoSpider();

						features::esp.Log("SetAdminFlag");
						if (settings::misc::admin_flags_enabled)
							Players::LocalPlayer::LocalPlayer->SetAdminFlag();

						features::esp.Log("FastHeal");
						if ((GetKeyState(VK_NUMPAD5) & 0x8000))
						{
							if (player->GetHealth() < 100)
							{
								auto itemname = Players::LocalPlayer::LocalPlayer->GetHeldItem()->GetItemName();
								if (itemname.find(L"bandage") != std::string::npos || itemname.find(L"syringe") != std::string::npos)
								{
									mono_string mymonostring;
									mymonostring.size = 7;
									//mymonostring.buffer = L'UseSelf';
									std::wcsncpy(mymonostring.buffer, L"UseSelf", mymonostring.size);
									//std::cout << "Getting ServerRPC Func!" << std::endl;
									ServerRPC = (serverrpc_func)(reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll")) + 0x32EEA0);
									//std::cout << "Got ServerRPC Func!" << std::endl;
									ServerRPC((uintptr_t)Players::LocalPlayer::LocalPlayer->GetHeldItem()->GetItemReference(), mymonostring);
									//std::cout << "Called ServerRPC!" << std::endl;
									std::this_thread::sleep_for(std::chrono::milliseconds(10));
								}
							}
						}
					}
				}
			}
		}

		if (ObjectClass.find("Collectible") != std::string::npos && settings::collectable_esp::enabled)
		{
			features::esp.Log("Collectable ESP");
			auto unk1 = safe_read(current + 0x10, uintptr_t); //mex.Read<uintptr_t>(ore.Ore + 0x10);

			if (!unk1)
				continue;

			//std::cout << "unk2!" << std::endl;
			features::esp.Log("unk2");
			auto unk2 = safe_read(unk1 + 0x30, uintptr_t); //mex.Read<uintptr_t>(unk1 + 0x30);

			if (!unk2)
				continue;

			//std::cout << "unk3!" << std::endl;
			features::esp.Log("unk3");
			auto unk3 = safe_read(unk2 + 0x30, uintptr_t); //mex.Read<uintptr_t>(unk2 + 0x30);

			if (!unk3)
				continue;

			//std::cout << "CollectablePos!" << std::endl;
			features::esp.Log("CollectablePos");
			auto CollectablePos = features::esp.get_obj_pos(unk3);
			auto Distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, CollectablePos);
			if (Distance > 300)
				continue;

			const auto oreName = safe_read(unk2 + 0x60, uintptr_t); //mex.Read<uintptr_t>(unk2 + 0x60);
			std::string name = read_ascii(oreName, 64);

			if (name.find("hemp") != std::string::npos)
			{
				Vector2 ScreenPos;
				if (player_base.w2s(CollectablePos, ScreenPos))
				{
					char distancebuffer[0x20]{};
					sprintf(distancebuffer, "[%dm]", (int)Distance);
					DrawString("Hemp", 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::LightGreen);
					ScreenPos.y += 15;
					DrawString(distancebuffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::LightGreen);
				}
			}
		}

		if (ObjectClass.find("OreRe") != std::string::npos && settings::ore_esp::enabled)
		{
			//std::cout << "Doing Ore ESP!" << std::endl;
			//std::cout << "unk1!" << std::endl;
			features::esp.Log("Doing Ore ESP!");
			auto unk1 = safe_read(current + 0x10, uintptr_t); //mex.Read<uintptr_t>(ore.Ore + 0x10);

			if (!unk1)
				continue;

			//std::cout << "unk2!" << std::endl;
			features::esp.Log("unk2");
			auto unk2 = safe_read(unk1 + 0x30, uintptr_t); //mex.Read<uintptr_t>(unk1 + 0x30);

			if (!unk2)
				continue;

			//std::cout << "unk3!" << std::endl;
			features::esp.Log("unk3");
			auto unk3 = safe_read(unk2 + 0x30, uintptr_t); //mex.Read<uintptr_t>(unk2 + 0x30);

			if (!unk3)
				continue;

			//std::cout << "OrePos!" << std::endl;
			features::esp.Log("OrePos");
			auto OrePos = features::esp.get_obj_pos(unk3);

			auto Distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, OrePos);
			if (Distance > 300)
				continue;

			//std::cout << "oreName!" << std::endl;
			features::esp.Log("oreName");
			const auto oreName = safe_read(unk2 + 0x60, uintptr_t); //mex.Read<uintptr_t>(unk2 + 0x60);
			std::string name = read_ascii(oreName, 64);
			//std::cout << "Ore Name: " << name << std::endl;
			if (name.find("stone-ore") != std::string::npos && settings::ore_esp::stone_enabled)
			{
				Vector2 ScreenPos;
				if (player_base.w2s(OrePos, ScreenPos))
				{
					char distancebuffer[0x20]{};
					sprintf(distancebuffer, "[%dm]", (int)Distance);
					DrawString("Stone Ore", 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aqua);
					ScreenPos.y += 15;
					DrawString(distancebuffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aqua);
				}
			}
			else if (name.find("metal-ore") != std::string::npos && settings::ore_esp::metal_enabled)
			{
				Vector2 ScreenPos;
				if (player_base.w2s(OrePos, ScreenPos))
				{
					char distancebuffer[0x20]{};
					sprintf(distancebuffer, "[%dm]", (int)Distance);
					DrawString("Metal Ore", 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aqua);
					ScreenPos.y += 15;
					DrawString(distancebuffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aqua);
				}
			}
			else if (name.find("sulfur-ore") != std::string::npos && settings::ore_esp::sulfur_enabled)
			{
				Vector2 ScreenPos;
				if (player_base.w2s(OrePos, ScreenPos))
				{
					char distancebuffer[0x20]{};
					sprintf(distancebuffer, "[%dm]", (int)Distance);
					DrawString("Sulfur Ore", 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aqua);
					ScreenPos.y += 15;
					DrawString(distancebuffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aqua);
				}
			}
		}

		else if (ObjectClass.find("DroppedItem") != std::string::npos && settings::dropped_item_esp::enabled)
		{
			auto CurrentItem = (item_t*)safe_read(current + 0x150, uintptr_t);
			if (!CurrentItem)
				continue;

			auto obj_class = safe_read(object + 0x30, uint64_t);
			Vector3 ItemPos = features::esp.get_obj_pos(obj_class);
			auto Distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, ItemPos);
			if (Distance > 100)
				continue;

			Vector2 ScreenPos;
			if (player_base.w2s(ItemPos, ScreenPos))
			{
				//std::cout << "Getting Amount!" << std::endl;
				auto amount = CurrentItem->GetAmount(); //safe_read(CurrentItem + 0x30, int);
				//std::cout << "Amount: " << amount << std::endl;
				char distancebuffer[0x20]{};
				sprintf(distancebuffer, "[%dm]", (int)Distance);
				char amountbuffer[0x20]{};
				sprintf(amountbuffer, "[%d]", (int)amount);
				//std::cout << "Getting Item Name!" << std::endl;
				std::wstring itemname = CurrentItem->GetItemName();
				//std::cout << "Drawing!" << std::endl;
				DrawWString(itemname.c_str(), 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Purple);
				ScreenPos.y += 15;
				DrawString(distancebuffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Purple);
				ScreenPos.y += 15;
				DrawString(amountbuffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Purple);
			}
		}

		else if (ObjectClass.find("StashContainer") != std::string::npos && settings::stash_esp::enabled)
		{
			auto StashPosition = features::esp.GetCurrentObjectPosition(current);
			auto Distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, StashPosition);
			if (Distance < 300)
			{
				Vector2 ScreenPos;
				if (player_base.w2s(StashPosition, ScreenPos))
				{
					char distancebuffer[0x20]{};
					sprintf(distancebuffer, "[%dm]", (int)Distance);
					DrawString("Stash", 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Purple);
					ScreenPos.y += 15;
					DrawString(distancebuffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Purple);
				}
			}
		}

		else if (ObjectClass.find("LootContainer") != std::string::npos && settings::container_esp::enabled)
		{
			auto StashPosition = features::esp.GetCurrentObjectPosition(current);
			auto distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, StashPosition);
			if (distance < 300)
			{
				Vector2 ScreenPos;
				if (player_base.w2s(StashPosition, ScreenPos))
				{
					auto unk1 = safe_read(current + 0x10, uintptr_t);

					if (!unk1)
						continue;

					auto unk2 = safe_read(unk1 + 0x30, uintptr_t);
					auto oreName = safe_read(unk2 + 0x60, uintptr_t);
					std::string name = read_ascii(oreName, 64);

					if (name.find("barrel") != std::string::npos && settings::container_esp::barrels_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "Barrel\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::CornflowerBlue);
					}
					else if (name.find("crate_normal.pre") != std::string::npos && settings::container_esp::crates_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "Military Crate\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aquamarine);
					}
					else if (name.find("crate_normal_2.pre") != std::string::npos && settings::container_esp::crates_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "Normal Crate\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aquamarine);
					}
					else if (name.find("crate_tool") != std::string::npos && settings::container_esp::crates_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "Tool Crate\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aquamarine);
					}
					else if (name.find("food") != std::string::npos && settings::container_esp::crates_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "Food Crate\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Aquamarine);
					}
				}
			}
		}

		else if (ObjectClass == "PlayerCorpse" && settings::misc_esp::corpses_enabled)
		{
			auto SupplyDropPos = features::esp.GetCurrentObjectPosition(current);
			auto distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, SupplyDropPos);
			Vector2 ScreenPos;
			if (distance < 300)
			{
				if (player_base.w2s(SupplyDropPos, ScreenPos))
				{
					char buffer[0x100]{};
					sprintf(buffer, "Corpse\n[%dm]", (int)distance);
					DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::HotPink);
				}
			}
		}

		else if (ObjectClass == "SupplyDrop" && settings::misc_esp::supply_drops_enabled)
		{
			auto SupplyDropPos = features::esp.GetCurrentObjectPosition(current);
			auto distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, SupplyDropPos);
			Vector2 ScreenPos;
			if (player_base.w2s(SupplyDropPos, ScreenPos))
			{
				char buffer[0x100]{};
				sprintf(buffer, "Supply Drop\n[%dm]", (int)distance);
				DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::HotPink);
			}
		}

		else if ((ObjectClass == "Boar" ||
			ObjectClass == "Wolf" ||
			ObjectClass == "Chicken" ||
			ObjectClass == "Bear") && settings::animal_esp::enabled)
		{
			auto AnimalPos = features::esp.GetCurrentObjectPosition(current);
			auto distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, AnimalPos);
			if (distance < 300)
			{
				Vector2 ScreenPos;
				if (player_base.w2s(AnimalPos, ScreenPos))
				{
					if (ObjectClass == "Boar")
					{
						char buffer[0x100]{};
						sprintf(buffer, "Boar\n[%dm]", (int)distance);
						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::DarkSalmon);
					}
					else if (ObjectClass == "Wolf")
					{
						char buffer[0x100]{};
						sprintf(buffer, "Wolf\n[%dm]", (int)distance);
						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::DarkSalmon);
					}
					else if (ObjectClass == "Chicken")
					{
						char buffer[0x100]{};
						sprintf(buffer, "Chicken\n[%dm]", (int)distance);
						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::DarkSalmon);
					}
					else if (ObjectClass == "Bear")
					{
						char buffer[0x100]{};
						sprintf(buffer, "Bear\n[%dm]", (int)distance);
						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::DarkSalmon);
					}
				}
			}
		}

		else if ((ObjectClass == "MiniCopter" ||
				ObjectClass == "ScrapTransportHelicopter" ||
				ObjectClass == "RidableHorse" ||
				ObjectClass == "RHIB" ||
				ObjectClass == "MotorRowboat") && settings::vehicle_esp::enabled)
		{
			auto VehiclePos = features::esp.GetCurrentObjectPosition(current);
			auto distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, VehiclePos);
			if (distance < 300)
			{
				Vector2 ScreenPos;
				if (player_base.w2s(VehiclePos, ScreenPos))
				{
					auto unk1 = safe_read(current + 0x10, uintptr_t);

					if (!unk1)
						continue;

					auto unk2 = safe_read(unk1 + 0x30, uintptr_t);
					if (!unk2)
						continue;

					auto oreName = safe_read(unk2 + 0x60, uintptr_t);
					std::string name = read_ascii(oreName, 64);
					if (name.find("rowboat") != std::string::npos && settings::vehicle_esp::boats_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "Row Boat\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Coral);
					}
					else if (name.find("minicopter") != std::string::npos && settings::vehicle_esp::copters_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "MiniCopter\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Coral);
					}
					else if (name.find("rhib") != std::string::npos && settings::vehicle_esp::boats_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "Motor Boat\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Coral);
					}
					else if (name.find("horse") != std::string::npos && settings::vehicle_esp::horses_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "Horse\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Coral);
					}
					else if (name.find("scraptrans") != std::string::npos && settings::vehicle_esp::copters_enabled)
					{
						char buffer[0x100]{};
						sprintf(buffer, "Scrap Heli\n[%dm]", (int)distance);
						auto text = s2ws(buffer);

						DrawString(buffer, 15, ScreenPos.x, ScreenPos.y, D2D1::ColorF::Coral);
					}
				}
			}
		}
	}
	try
	{
		if (true)
		{
			if (Players::closestPlayer != NULL && Players::LocalPlayer::LocalPlayer != NULL)
			{
				if ((GetKeyState(0x56) & 0x8000))
				{
					if (settings::aimbot::enabled)
					{
						Vector2 ScreenPos;

						Vector3 Pos;
						if (settings::aimbot::shoothead)
						{
							Pos = Players::closestPlayer->GetBonePosition(jaw); //GetBonePosition(closestPlayer, neck);
						}
						else
						{
							Pos = Players::closestPlayer->GetBonePosition(spine1);
						}
						auto distance = Math::Calc3D_Dist(Players::LocalPlayer::LocalPosition, Pos);
						if (distance < 300)
						{
							if (player_base.w2s(Pos, ScreenPos))
							{
								auto fov = distance_cursor(ScreenPos);
								if (fov < settings::aimbot::fov)
								{
									if (settings::aimbot::prediction)
									{
										auto lp = Players::LocalPlayer::LocalPlayer;
										PredictCtx Ctx;
										Ctx.StartPos = lp->GetHeldItem()->GetMuzzlePosition();
										Ctx.TargetPos = Pos;
										Ctx.BulletSpeed = lp->GetHeldItem()->GetBulletSpeed();
										Ctx.BulletGravity = (1 * 750.f);
										Ctx.TargetVel = Players::closestPlayer->GetVelocity();
									}
									else
									{
										auto lp = Players::LocalPlayer::LocalPlayer;
										auto helditem = lp->GetHeldItem();
										//std::cout << "Current Held Item" << std::hex << std::uppercase << (uintptr_t)helditem << std::endl;
										//printf("Current Item: %X\n", (uintptr_t)lp->GetHeldItem());
										//helper->circle(ScreenPos, D2D1::ColorF::Red, 25);
										auto OrigVA = Players::LocalPlayer::LocalPlayer->GetInput()->GetViewAngles();
										Vector3 LocalPos = Players::LocalPlayer::LocalPlayer->GetBonePosition(neck); // GetBonePosition(LocalPlayer, neck);
										//Vector3 EnemyPos = Prediction(LocalPos, closestPlayer, neck);
										auto RecAng = Players::LocalPlayer::LocalPlayer->GetInput()->GetRecoilAngles(); //GetRA(LocalPlayer);
										Vector2 Offset = Math::CalcAngle(LocalPos, Pos) - Players::LocalPlayer::LocalPlayer->GetInput()->GetViewAngles();
										//printf("Offset VA: %f | %f\n", Offset.x, Offset.y);
										features::esp.Normalize(Offset.y, Offset.x);
										Vector2 AngleToAim = Players::LocalPlayer::LocalPlayer->GetInput()->GetViewAngles() + Offset;
										AngleToAim = AngleToAim - RecAng;
										features::esp.Normalize(AngleToAim.y, AngleToAim.x);
										Players::LocalPlayer::LocalPlayer->GetInput()->SetViewAngles(AngleToAim);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	catch (...)
	{
		std::cout << "Aimbot Error!" << std::endl;
	}
}