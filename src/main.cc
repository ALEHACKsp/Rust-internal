#include "commons.h"
#include "../um_bypass/um_bypass.h"
#include "utils/thread.h"
#include "render/renderer.h"
#include <thread>
bool RendererStarted = false;

HMODULE dllinstance;

unsigned long __stdcall main(LPVOID)
{
	// renderer
	std::cout << "Loaded!" << std::endl;
	
	while (!GetAsyncKeyState(VK_HOME))
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	if (!RendererStarted)
	{
		std::cout << "Starting!" << std::endl;
		i_renderer.init();
		RendererStarted = true;
		std::cout << "Started!" << std::endl;
	}

	while (!GetAsyncKeyState(VK_END)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	FreeConsole();
	FreeLibraryAndExitThread(dllinstance, 0);
}


__declspec(dllexport) int UNKNOWN(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	VM_START
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		dllinstance = hDll;
		// bypass eac (um)
		EAC::GetInstance().Init();
		Beep(523, 500);
		// create our console
		//AllocConsole();
		//FILE* pFILE = 0;
		//freopen_s(&pFILE, "CONOUT$", "w", stdout);
		DWORD id;
		thread::create_thread(main, NULL, &id);
	}
	VM_END
		return true;
}
