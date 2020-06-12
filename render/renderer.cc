#include "../commons.h"
#include "renderer.h"
#include "../globals.h"

#include "../features/esp.h"
#include <minhook/MinHook.h>

DX11PresentFn oPresent = nullptr;
DX11ResizeFn oResize = nullptr;
WNDPROC original_wndproc;
HWND game_window;

ID3D11Device* device = nullptr;
ID3D11DeviceContext* context = nullptr;
ID3D11RenderTargetView* rendertargetview = nullptr;

typedef void (__fastcall* sendrpc_func)(void*, void*);
sendrpc_func original_sendrpc;

typedef void (__fastcall* sendclienttick_func)(void*);
sendclienttick_func original_sendclienttick;

typedef void*(__fastcall* create_projectile_func)(void*, void*, Vector3, Vector3, Vector3);
create_projectile_func original_create_projectile;

typedef bool (__fastcall* dohit_func)(void*, void*, Vector3, Vector3);
dohit_func original_dohit;

typedef void*(__fastcall* buildattackmessage_func)(void*);
buildattackmessage_func buildattackmessage;

typedef bool(__fastcall* canattack_func)(void*);
canattack_func original_canattack;

void Normalize(float& Yaw, float& Pitch)
{
	if (Pitch < -89) Pitch = -89;
	else if (Pitch > 89) Pitch = 89;
	if (Yaw < -360) Yaw += 360;
	else if (Yaw > 360) Yaw -= 360;
}

void CalcAngle(Vector3& src, Vector3& dst, Vector3& angles)

{

	double delta[3] = { (src.x - dst.x), (src.y - dst.y), (src.z - dst.z) };

	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);

	angles.x = (float)(asinf(delta[2] / hyp) * 57.295779513082f);

	angles.y = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);

	angles.z = 0.0f;

	if (delta[0] >= 0.0) { angles.y += 180.0f; }

}

void __fastcall hook_server_rpc(void* self, void* func)
{
	//std::cout << "Func: " << std::hex << std::uppercase << &func << std::endl;
	auto namesize = safe_read(func + 0x10, INT32);
	//std::cout << "Func Name Size: " << namesize << std::endl;
	auto wfuncname = read_unicode((uintptr_t)func + 0x14, namesize);
	auto funcname = ws2s(wfuncname);
	//std::cout << "Func Name: " << funcname << std::endl;
	original_sendrpc(self, func);
}

void __fastcall hook_sendclienttick(void* self)
{
	if (settings::misc::anti_aim_enabled)
	{
		auto input = safe_read(self + 0x5E0, uintptr_t);
		if (!input)
		{
			std::cout << "Failed to get input!" << std::endl;
			original_sendclienttick(self);
		}

		auto state = safe_read(input + 0x20, uintptr_t);
		if (!state)
		{
			std::cout << "Failed to get state!" << std::endl;
			original_sendclienttick(self);
		}

		auto current = safe_read(state + 0x10, uintptr_t);
		if (!current)
		{
			std::cout << "Failed to get current!" << std::endl;
			original_sendclienttick(self);
		}

		safe_write(current + 0x18, Vector3(100, rand() % 999 + -999, rand() % 999 + -999), Vector3);
	}
	original_sendclienttick(self);
}

bool __fastcall hook_DoHit(void* self, void* test, Vector3 point, Vector3 normal)
{
	safe_write(self + 0xFC, 999.f, float);

	auto attack = buildattackmessage(test);
	if (!attack)
	{
		std::cout << "!attack" << std::endl;
		original_dohit(self, test, point, normal);
	}

	auto hitMaterialId = safe_read(attack + 0x68, int);
	if (!hitMaterialId)
	{
		std::cout << "!hitMaterialId" << std::endl;
		original_dohit(self, test, point, normal);
	}

	std::cout << "hitMaterialId: " << hitMaterialId << std::endl;

	if (hitMaterialId != 1395914656)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool __fastcall hook_CanAttack(void* self)
{
	return true;
}


void* __fastcall hook_create_projectile(void* ecx, void* prefab_path, Vector3 pos, Vector3 forward, Vector3 velocity)
{
	//std::cout << "Position: " << pos.x << " | " << pos.y << " | " << pos.z << std::endl;
	//std::cout << std::endl;
	//std::cout << "Forward: " << forward.x << " | " << forward.y << " | " << forward.z << std::endl;

	if (settings::aimbot::silent_aim)
	{
		Vector3 Closest = Players::closestPlayer->GetBonePosition(jaw);
		Vector3 MyJaw = Players::LocalPlayer::LocalPlayer->GetBonePosition(jaw);
		auto distance = Math::Calc3D_Dist(pos, Closest);
		//std::cout << "Original Velocity: " << velocity.x << " | " << velocity.y << " | " << velocity.z << std::endl;
		//Vector3(Math::Calc3D_Dist(pos, Closest)).normalize()* velocity.Length();
		//auto calcang = Math::CalcAngle(Players::LocalPlayer::LocalPlayer->GetBonePosition(jaw), Closest);
		//Normalize(calcang.y, calcang.x);
		Vector3 AimAngle;
		CalcAngle(MyJaw, Closest, AimAngle);
		velocity = AimAngle * 375.f;
		//std::cout <<  "New Velocity: " << velocity.x << " | " << velocity.y << " | " << velocity.z << std::endl;
	}
	void* projectile;
	projectile = original_create_projectile(ecx, prefab_path, pos, forward, velocity);
	if (settings::misc::fat_bullet_enabled)
	{
		safe_write(projectile + 0x2C, 0.5f, float);
	}
	return projectile; //original_create_projectile(ecx, prefab_path, pos, forward, velocity);
}

HRESULT __stdcall hk_present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!device)
	{
		pSwapChain->GetDevice(__uuidof(device), (void**)&device);
		device->GetImmediateContext(&context);

		i_renderer.helper = std::make_unique<r_helper>();
	}

	if (i_renderer.helper->new_frame(pSwapChain))
	{
		//std::cout << "Drawing Aspect String!" << std::endl;
		auto elementalstring = xorstr("Elemental");
		auto elementalwstring = s2ws(elementalstring.crypt_get());
		i_renderer.helper->string({ 10.f, 10.f }, elementalwstring.c_str(), D2D1::ColorF::Red);

		//std::cout << "Getting Canvas Size!" << std::endl;
		// crosshair
		auto size = i_renderer.helper->canvas_size();
		scr_size = size;

		auto center = Vector2{ size.x / 2, size.y / 2 };
		//std::cout << "Drawing Cross Hair!" << std::endl;
		i_renderer.helper->line(Vector2{ center.x, center.y - 6 }, Vector2{ center.x , center.y + 6 }, D2D1::ColorF::Black, 3.5f);
		i_renderer.helper->line(Vector2{ center.x - 6, center.y }, Vector2{ center.x + 6, center.y }, D2D1::ColorF::Black, 3.5f);
		i_renderer.helper->line(Vector2{ center.x, center.y - 5 }, Vector2{ center.x , center.y + 5 }, D2D1::ColorF::Red);
		i_renderer.helper->line(Vector2{ center.x - 5, center.y }, Vector2{ center.x + 5, center.y }, D2D1::ColorF::Red);
		//std::cout << "Drawing ESP!" << std::endl;
		//features::esp.tick(i_renderer.helper.get());
		//std::cout << "Drew ESP!" << std::endl;
	}
	i_renderer.helper->end_frame();

	

	return oPresent(pSwapChain, SyncInterval, Flags);
}

inline LRESULT WINAPI hk_WndProc(HWND wnd, UINT msg, WPARAM wpar, LPARAM lpar)
{
	return CallWindowProcW(original_wndproc, wnd, msg, wpar, lpar);
}

void CreateConsole()
{
	if (!AllocConsole()) {
		// Add some error handling here.
		// You can call GetLastError() to get more info about the error.
		return;
	}

	// std::cout, std::clog, std::cerr, std::cin
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	// std::wcout, std::wclog, std::wcerr, std::wcin
	HANDLE hConOut = CreateFile(("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFile(("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
}

void renderer_t::init()
{
	DirectOverlaySetOption(D2DOV_DRAW_FPS | D2DOV_FONT_ARIAL | D2DOV_REQUIRE_FOREGROUND);
	DirectOverlaySetup(DrawLoop);
	CreateConsole();
	if (MH_Initialize() != MH_OK)
	{
		std::cout << "Minhook failed to initialize." << std::endl;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//std::cout << "Getting Game Window!" << std::endl;
	game_window = FindWindowW(L"UnityWndClass", nullptr);
	
	std::cout << "Hooking Create Projectile!" << std::endl;
	auto offset = reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll")) + 0x3E4920; //Create Projectile RVA inside of BaseProjectile.

	if (MH_CreateHook(reinterpret_cast<void*>(offset), &hook_create_projectile, reinterpret_cast<LPVOID*>(&original_create_projectile)) != MH_OK)
	{
		std::cout << "Failed to hook create projectile!" << std::endl;
	}

	if (MH_EnableHook(reinterpret_cast<void*>(offset)) != MH_OK)
	{
		std::cout << "Failed to enable create projectile hook!" << std::endl;
	}

	std::cout << "Hooked Create Projectile!" << std::endl;

	std::cout << "Hooking SendClientTick!" << std::endl;
	offset = reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll")) + 0x2E5700; //SendClientTick RVA inside of BasePlayer.

	if (MH_CreateHook(reinterpret_cast<void*>(offset), &hook_sendclienttick, reinterpret_cast<LPVOID*>(&original_sendclienttick)) != MH_OK)
	{
		std::cout << "Failed to hook SendClientTick!" << std::endl;
	}

	if (MH_EnableHook(reinterpret_cast<void*>(offset)) != MH_OK)
	{
		std::cout << "Failed to enable SendClientTick hook!" << std::endl;
	}

	std::cout << "Hooked SendClientTick!" << std::endl;

	/*
	std::cout << "Setting BuildAttackMesssage" << std::endl;

	buildattackmessage = (buildattackmessage_func)(reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll")) + 0x4C1E60); //BuildAttackMessage RVA inside of HitTest.

	std::cout << "Set BuildAttackMessage" << std::endl;

	
	std::cout << "Hooking DoHit!" << std::endl;
	offset = reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll")) + 0x4A9D50; //DoHit RVA inside of Projectile.

	if (MH_CreateHook(reinterpret_cast<void*>(offset), &hook_DoHit, reinterpret_cast<LPVOID*>(&original_dohit)) != MH_OK)
	{
		std::cout << "Failed to hook DoHit!" << std::endl;
	}

	if (MH_EnableHook(reinterpret_cast<void*>(offset)) != MH_OK)
	{
		std::cout << "Failed to enable DoHit hook!" << std::endl;
	}

	std::cout << "Hooked DoHit!" << std::endl;
	*/

	std::cout << "Hooking CanAttack!" << std::endl;
	offset = reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll")) + 0x2D0220; //CanAttack RVA inside of BaseProjectile.

	if (MH_CreateHook(reinterpret_cast<void*>(offset), &hook_CanAttack, reinterpret_cast<LPVOID*>(&original_canattack)) != MH_OK)
	{
		std::cout << "Failed to hook CanAttack!" << std::endl;
	}

	if (MH_EnableHook(reinterpret_cast<void*>(offset)) != MH_OK)
	{
		std::cout << "Failed to enable CanAttack hook!" << std::endl;
	}

	std::cout << "Hooked CanAttack!" << std::endl;

	std::cout << "Hooking ServerRPC!" << std::endl;
	offset = reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll")) + 0x32EEA0; //CanAttack RVA inside of BaseProjectile.

	if (MH_CreateHook(reinterpret_cast<void*>(offset), &hook_server_rpc, reinterpret_cast<LPVOID*>(&original_sendrpc)) != MH_OK)
	{
		std::cout << "Failed to hook ServerRPC!" << std::endl;
	}

	if (MH_EnableHook(reinterpret_cast<void*>(offset)) != MH_OK)
	{
		std::cout << "Failed to enable ServerRPC hook!" << std::endl;
	}

	std::cout << "Hooked ServerRPC!" << std::endl;
}

//0x32EEA0