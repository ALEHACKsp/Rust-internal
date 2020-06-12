#pragma once

#include "math.h"
#include "XOR.h"
/*
#define E(Str) xorstr(str).crypt_get()

namespace Fn
{
	PVOID RayInit;
	PVOID MuzzlePos;
	PVOID GetEntName;
	PVOID Trampoline;
	PVOID World2Screen;
	PVOID UpdateAbsOrigin;
	PVOID UpdateAbsVelocity;
}

//FACE Base Utils
namespace FUtils
{
	//StrLen, ToLowerChar, RIP2VA
	template <typename StrType> __forceinline int StrLen(StrType Str) {
		if (!Str) return 0;
		StrType Str2 = Str;
		while (*Str2) *Str2++;
		return (int)(Str2 - Str);
	}
#define ConstStrLen(Str) ((sizeof(Str) - sizeof(Str[0])) / sizeof(Str[0]))
#define ToLower(Char) ((Char >= 'A' && Char <= 'Z') ? (Char + 32) : Char)
#define RVA(Instr, InstrSize) (((DWORD64)Instr) + InstrSize + *(LONG*)(((DWORD64)Instr) + (InstrSize - sizeof(LONG))))

	//String to UInt
	template <typename StrType>
	__forceinline UINT64 Str2UInt(StrType Str)
	{
		if (!Str) return 0; ULONG64 UInt = 0;
		for (int i = 0; i < 10; ++i, ++Str) {
			wchar_t WChar = *Str;
			if ((WChar >= '0') && (WChar <= '9')) {
				UInt *= 10; UInt += (WChar - '0');
			}
			else break;
		} return UInt;
	}

	//StrCompare (with StrInStrI(Two = false))
	template <typename StrType, typename StrType2>
	__forceinline bool StrCmp(StrType Str, StrType2 InStr, bool Two) {
		if (!Str || !InStr) return false;
		wchar_t c1, c2; do {
			c1 = *Str++; c2 = *InStr++;
			c1 = ToLower(c1); c2 = ToLower(c2);
			if (!c1 && (Two ? !c2 : 1)) return true;
		} while (c1 == c2); return false;
	}

	//StrCpy (copy str with null terminator)
	template <typename StrType, typename StrType2>
	__forceinline void StrCpy(StrType Src, StrType2 Dst, wchar_t TNull = 0) {
		while (true) { wchar_t WChar = *Dst = *Src++; if (WChar == TNull) { *Dst = 0; break; } Dst++; }
	}

	//GetModuleBase (no relink)
	template <typename StrType>
	void* GetModBase(StrType ModuleName) {
		PPEB_LDR_DATA Ldr = ((PTEB)__readgsqword(FIELD_OFFSET(NT_TIB, Self)))->ProcessEnvironmentBlock->Ldr; void* ModBase = nullptr;
		for (PLIST_ENTRY CurEnt = Ldr->InMemoryOrderModuleList.Flink; CurEnt != &Ldr->InMemoryOrderModuleList; CurEnt = CurEnt->Flink) {
			PLDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(CurEnt, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
			if (!ModuleName || StrCmp(ModuleName, pEntry->BaseDllName.Buffer, false)) return pEntry->DllBase;
		} return nullptr;
	}
#define GetModuleBase FUtils::GetModBase

	//CRC16 StrHash
	template <typename StrType> __declspec(noinline) constexpr unsigned short HashStr(StrType Data, int Len) {
		unsigned short CRC = 0xFFFF; while (Len--) {
			auto CurChar = *Data++; if (!CurChar) break;
			CRC ^= ToLower(CurChar) << 8; for (int i = 0; i < 8; i++)
				CRC = CRC & 0x8000 ? (CRC << 1) ^ 0x8408 : CRC << 1;
		} return CRC;
	}
#define ConstHashStr(Str) [](){ constexpr unsigned short CRC = FUtils::HashStr(Str, ConstStrLen(Str)); return CRC; }()

	//EncryptDecryptPointer
	template <typename PtrType>
	__forceinline PVOID EPtr(PtrType Ptr) {
		typedef union { struct { USHORT Key1; USHORT Key2; USHORT Key3; USHORT Key4; }; ULONG64 Key; } CryptData;
		CryptData Key{ ConstHashStr(__TIME__), ConstHashStr(__DATE__), ConstHashStr(__TIMESTAMP__), ConstHashStr(__TIMESTAMP__) };
		return (PVOID)((DWORD64)Ptr ^ Key.Key);
	}
#define EPtr(Ptr) FUtils::EPtr(Ptr)

	//RAddress Call Spoofer
	extern "C" void* SpoofRetAsm();
	template<typename Ret = void, typename First = void*, typename Second = void*, typename Third = void*, typename Fourth = void*, typename... Stack>
	__forceinline Ret SpoofCall(void* Func, First a1 = {}, Second a2 = {}, Third a3 = {}, Fourth a4 = {}, Stack... Vars)
	{
		struct ShellData { PVOID Trampoline; PVOID Func; PVOID Stub; };
		ShellData ShellData = {
			EPtr(Fn::Trampoline),
			Func
		};
		typedef Ret(__fastcall* ShllFn)(...);
		return ((ShllFn)SpoofRetAsm)(a1, a2, a3, a4, &ShellData, nullptr, Vars...);
	}

	//CallFunc In VFTable
	template<typename Ret = void, typename... Args>
	Ret __forceinline VCall(PVOID Class, int Index, Args... args) {
		return SpoofCall<Ret>(*(PVOID*)(*(DWORD64*)Class + (Index * 8)), args...);
	}

	//GetProcAddress
	__forceinline void* LoadLib(const wchar_t* ModName);
	void* GetExport(void* ModBase, const char* Name, PIMAGE_EXPORT_DIRECTORY ExportDirectory = nullptr)
	{
		//get export directory
		if (!ExportDirectory) {
			PIMAGE_NT_HEADERS DllNtHeader = (PIMAGE_NT_HEADERS)((DWORD64)ModBase + ((PIMAGE_DOS_HEADER)ModBase)->e_lfanew);
			ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD64)ModBase + DllNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		}

		//by ordinal
		USHORT Ordinal = 0xFFFF;
		if ((ULONG64)Name <= 0xFFFF) {
			if ((ULONG64)Name < ExportDirectory->Base) return nullptr;
			Ordinal = (USHORT)((ULONG64)Name - ExportDirectory->Base);
		}

		//name
		else {
			for (ULONG i = 0; i < ExportDirectory->NumberOfNames; ++i) {
				if (StrCmp(Name, (const char*)((DWORD64)ModBase + ((ULONG*)((DWORD64)ModBase + ExportDirectory->AddressOfNames))[i]), true)) {
					Ordinal = ((USHORT*)((DWORD64)ModBase + ExportDirectory->AddressOfNameOrdinals))[i]; break;
				}
			}
		}

		//check ordinal & get func address
		if (Ordinal >= ExportDirectory->NumberOfFunctions) return nullptr;
		ULONG64 FuncAddr = ((DWORD64)ModBase + ((ULONG*)((DWORD64)ModBase + ExportDirectory->AddressOfFunctions))[Ordinal]);

		//forward sign check
		int isForward = false, OrdinalOffset = 0, NameOffset = 0; for (int i = 0; i < 256; ++i) {
			char Char = ((char*)FuncAddr)[i]; if (Char == '#') { OrdinalOffset = i + 1; continue; }
			if (Char == 0) { isForward = (NameOffset && (i >= 5)); break; } if (Char == '.') { if (i < 2) break; NameOffset = i; continue; }
			if (!(((Char >= 'A') && (Char <= 'Z')) || ((Char >= 'a') && (Char <= 'z')) || ((Char >= '0') && (Char <= '9')) || (Char == '-'))) break;
		}

		//resolved export
		if (!isForward)
			return (PVOID)FuncAddr;

		//get forward mod base
		wchar_t ModNameCut[128];
		StrCpy((const char*)FuncAddr, ModNameCut, '.');
		ModBase = GetModBase(ModNameCut);
		if (!ModBase) ModBase = LoadLib(ModNameCut);
		if (!ModBase) return nullptr;

		//get forward func address
		if (!OrdinalOffset) return GetExport(ModBase, &((const char*)FuncAddr)[NameOffset + 1]);
		return GetExport(ModBase, (const char*)(Str2UInt(&((const char*)FuncAddr)[OrdinalOffset])));
	}

	//LoadLibrary
	__forceinline void* LoadLib(const wchar_t* ModName) {
		PVOID ModBase; int ModNameSize = (USHORT)StrLen(ModName) << 1;
		PVOID LdrLoadDllFn = GetExport(GetModBase(xorstr("ntdll").crypt_get()), xorstr("LdrLoadDll").crypt_get());
		UNICODE_STRING Mod = { (USHORT)ModNameSize, (USHORT)(ModNameSize + 2), (wchar_t*)ModName };
		typedef void(__fastcall* LoadDllFn)(void*, void*, PUNICODE_STRING, void**);
		SpoofCall(LdrLoadDllFn, nullptr, nullptr, &Mod, &ModBase); return ModBase;
	}

	//FindPattern
#define INRANGE(x, a, b) (x >= a && x <= b) 
#define getBits(x) (INRANGE((x&(~0x20)),'A','F')?((x&(~0x20))-'A'+0xA):(INRANGE(x,'0','9')?x-'0':0))
#define getByte(a, b) (getBits(a) << 4 | getBits(b))
	DWORD64 FindPattern_Wrapper(const char* Pattern, const char* ModName = nullptr)
	{
		DWORD64 Start = (DWORD64)GetModBase(ModName); if (!Start) return 0; PBYTE FirstMatch = nullptr; const char* CurPatt = Pattern;
		PBYTE End = (PBYTE)(Start + ((PIMAGE_NT_HEADERS)(Start + ((PIMAGE_DOS_HEADER)Start)->e_lfanew))->OptionalHeader.SizeOfImage);

		for (PBYTE pCur = (PBYTE)Start; pCur < End; pCur++)
		{
			char Byte = CurPatt[0]; if (!Byte) return (DWORD64)FirstMatch;
			if (Byte == '\?' || *(BYTE*)pCur == getByte(Byte, CurPatt[1])) {
				if (!FirstMatch) FirstMatch = pCur;
				if (!CurPatt[2]) return (DWORD64)FirstMatch;
				((Byte == '\?') ? (CurPatt += 2) : (CurPatt += 3));
			}

			else {
				CurPatt = Pattern;
				FirstMatch = nullptr;
			}
		}

		return 0;
	}
#define FindPattern FUtils::FindPattern_Wrapper

	//Resolve & Call Import Function
#define FC(Mod, Name, ...) [&](){ static PVOID FAddr = nullptr; \
		if (!FAddr) FAddr = EPtr(FUtils::GetExport(FUtils::GetModBase(xorstr_(#Mod)), xorstr_(#Name))); \
		return FUtils::SpoofCall<decltype(Name(__VA_ARGS__))>(EPtr(FAddr), __VA_ARGS__); \
	}()

	/*Allocate Memory
	__forceinline void* Alloc(SIZE_T Size) {
		PVOID AllocBase = nullptr;
		NTSTATUS NtAllocateVirtualMemory(HANDLE, PVOID*, ULONG_PTR, PSIZE_T, ULONG, ULONG);
		FC(ntdll, NtAllocateVirtualMemory, (HANDLE)-1, &AllocBase, 0, &Size, MEM_COMMIT, PAGE_READWRITE);
		return AllocBase;
	}
	#define Alloc(Size) FUtils::Alloc(Size)

	//Free Memory
	__forceinline void Free(void* Base) {
		SIZE_T ZeroSize = 0;
		NTSTATUS NtFreeVirtualMemory(HANDLE, PVOID*, PSIZE_T, ULONG);
		FC(ntdll, NtFreeVirtualMemory, (HANDLE)-1, &Base, &ZeroSize, MEM_RELEASE);
	}
	#define Free(Ptr) FUtils::Free(Ptr)
}

//FACE VMT Hook v4
int VMT_FilledRecords = 0;
#pragma comment(linker,"/SECTION:.hkdata,RW")
#pragma bss_seg(push, ".hkdata")
PVOID VMTHookData[10000];
#pragma bss_seg(pop)
class VMT_Hook
{
private:
	PVOID* TblEnc = nullptr; DWORD Size = 0;
	bool InExecMemory(PVOID Addr, DWORD64 ModBase, PIMAGE_NT_HEADERS NtHeader) {
		PIMAGE_SECTION_HEADER Sect = IMAGE_FIRST_SECTION(NtHeader);
		for (DWORD Cnt = 0; Cnt < NtHeader->FileHeader.NumberOfSections; Cnt++, Sect++) {
			if ((Sect->Characteristics & IMAGE_SCN_MEM_EXECUTE) &&
				((DWORD64)Addr >= (ModBase + Sect->VirtualAddress)) &&
				((DWORD64)Addr < (ModBase + Sect->VirtualAddress + Sect->Misc.VirtualSize)))
				return true;
		} return false;
	}

public:
	template <typename T>
	__forceinline VMT_Hook(const char* Module, T Class) {
		DWORD64 ModBase = (DWORD64)GetModuleBase(Module); Size = 0; PVOID* VFTable = *(PVOID**)Class;
		PIMAGE_NT_HEADERS NtHeader = (PIMAGE_NT_HEADERS)(ModBase + ((PIMAGE_DOS_HEADER)ModBase)->e_lfanew);
		while (InExecMemory(VFTable[Size], ModBase, NtHeader)) Size++;
		PVOID* VFT_New = (PVOID*)&VMTHookData[VMT_FilledRecords];
		__movsq((PDWORD64)VFT_New, (const PDWORD64)VFTable, Size);
		*(PVOID**)Class = VFT_New; TblEnc = (PVOID*)EPtr(VFT_New);
		VMT_FilledRecords += Size;
	}

	template <typename T>
	__forceinline void SetupHook(T* Orginal, PVOID Hook, int Index) {
		PVOID* Tbl = (PVOID*)EPtr(TblEnc);
		*Orginal = (T)Tbl[Index];
		Tbl[Index] = Hook;
	}
};



//FACE DBG Helper v2
#define DEBUG
#ifdef DEBUG
template<typename... Args>
void Print(const char* Format, Args... args) {
	static bool Once = false;
	if (!Once) { FC(kernel32, AllocConsole); Once = true; }
	char Buff[128]; FC(ntdll, sprintf, Buff, Format, args...);
	HANDLE Out = FC(kernel32, GetStdHandle, STD_OUTPUT_HANDLE);
	FC(kernel32, WriteConsoleA, Out, Buff, FUtils::StrLen(Buff), NULL, NULL);
}
#define dp(a) Print(E("DEC (%d): %d\n"), __LINE__, (a))
#define hp(a) Print(E("HEX (%d): %p\n"), __LINE__, (a))
#define sp(a) Print(E("STR (%d): %s\n"), __LINE__, (a))
#define wsp(a) Print(E("WSTR (%d): %ws\n"), __LINE__, (a))
#define fp(a) Print(E("FLOAT (%d): %d.%d\n"), __LINE__, (int)(a), (int)(((a) - (int)(a)) * 1000000));
#define v2p(a) { fp(a.x); fp(a.y); }
#define v3p(a) { fp(a.x); fp(a.y); fp(a.z); }
#endif
*/