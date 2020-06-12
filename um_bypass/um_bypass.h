#pragma once

#include "../detours.h"
#include "../src/defines.h"

#pragma comment(lib,"ntdll")
#ifdef _WIN64
#pragma comment(lib,"detours64")
#else
#pragma comment(lib,"detours")
#endif

class EAC
{
public:
	static EAC& GetInstance() {
		static EAC Instance;
		return Instance;
	}
	bool Init();
	bool Uninit();

private:
	EAC(EAC const&) = delete;
	EAC(EAC&&) = delete;
	EAC& operator=(EAC const&) = delete;
	EAC& operator=(EAC&&) = delete;

	static bool Bypass_DeviceIoControl(bool detourStatus);
protected:
	EAC()
	{
		hEvent = CreateEvent(0, 0, 0, 0);
	}
	~EAC()
	{
		CloseHandle(hEvent);
	}
	static HANDLE hEvent;
};