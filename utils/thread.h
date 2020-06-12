#pragma once

typedef struct
{
	DWORD64 dwEP;
	void* pParam;
}_CFUNC, * _PCFUNC;

typedef uintptr_t(*_FUNC)(void* p);

void __stdcall func_thread(_PCFUNC cf) {
	if (cf != 0 && cf->dwEP != 0)
		reinterpret_cast<_FUNC>(cf->dwEP)(cf->pParam);
}

DWORD get_size(DWORD64 base)
{
	IMAGE_DOS_HEADER dos_header = { 0 };
	IMAGE_NT_HEADERS nt_headers = { 0 };
	if (!base)
		return -1;
	dos_header = *(IMAGE_DOS_HEADER*)base;
	nt_headers = *(IMAGE_NT_HEADERS*)(base + dos_header.e_lfanew);
	return nt_headers.OptionalHeader.SizeOfImage;
}

namespace thread {
	__forceinline HANDLE
		create_thread(LPTHREAD_START_ROUTINE lpstart, void* param, LPDWORD thread_id)
	{
		HINSTANCE nt = GetModuleHandleA("ntdll.dll");
		if (nt)
		{
			uintptr_t isz = get_size((uintptr_t)nt);
			byte* mem_blob = (byte*)nt + isz - 0x400;
			if (mem_blob)
			{
				DWORD prc;
				VirtualProtect(mem_blob, 0x100, 0x40, &prc);
				_CFUNC* cf = (_CFUNC*)VirtualAlloc(0, 0x100, 0x3000 /* COMMIT | RESERVE*/, 0x40);
				cf->dwEP = (DWORD64)lpstart;
				cf->pParam = param;
				memcpy((LPVOID)mem_blob, (LPVOID)func_thread, 0x100);
				return CreateRemoteThread(GetCurrentProcess(), 0, 0, (LPTHREAD_START_ROUTINE)mem_blob, cf, 0, thread_id);
			}
		}
	}
}