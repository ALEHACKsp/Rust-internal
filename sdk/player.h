#pragma once

#include "../globals.h"
#include <codecvt>
#include <iostream>
#include <fstream>

namespace settings
{
	namespace player_esp
	{
		int folder;
		int enabled = true;
		int names_enabled = true;
		int distance_enabled = true;
		int health_enabled = true;
		int show_held_enabled = true;
		int show_hotbar_enabled = true;
	}

	namespace collectable_esp
	{
		int folder;
		int enabled = true;
		int hemp_enabled = true;
	}

	namespace ore_esp
	{
		int folder;
		int enabled = true;
		int stone_enabled = true;
		int metal_enabled = true;
		int sulfur_enabled = true;
	}

	namespace dropped_item_esp
	{
		int folder;
		int enabled = true;
	}

	namespace animal_esp
	{
		int folder;
		int enabled = true;
		int boars_enabled;
		int chickens_enabled;
		int bears_enabled;
		int wolves_enabled;
	}

	namespace stash_esp
	{
		int folder;
		int enabled = true;
	}

	namespace container_esp
	{
		int folder;
		int enabled = true;
		int crates_enabled = true;
		int barrels_enabled = true;
	}

	namespace vehicle_esp
	{
		int folder;
		int enabled = true;
		int boats_enabled = true;
		int copters_enabled = true;
		int horses_enabled = true;
	}

	namespace misc_esp
	{
		int folder;
		int enabled = true;
		int corpses_enabled = true;
		int supply_drops_enabled = true;
	}

	namespace aimbot
	{
		int folder;
		int enabled = true;
		int shoothead = false;
		int restore_position = false;
		int prediction = false;
		int silent_aim = false;
		int normalize = false;
		int fov = 50;
	}

	namespace misc
	{
		int folder;
		int spider_enabled = true;
		int always_day_enabled = true;
		int fat_bullet_enabled = true;
		int admin_flags_enabled = true;
		int fast_heal_enabled = true;
		int always_automatic_enabled = false;
		int anti_aim_enabled = true;
	}
}

void Log(std::string data)
{
	if (false)
	{
		std::ofstream outfile;

		outfile.open("C\\Aspect Rust\\Log.txt", std::ios_base::app);
		outfile << data;
		outfile << "\n";
		outfile.close();
	}
}

std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

std::wstring read_unicode(std::uintptr_t address, std::size_t size)
{
	const auto buffer = std::make_unique<wchar_t[]>(size);
	memcpy(buffer.get(), reinterpret_cast<const void*>(address), size * 2);
	return std::wstring(buffer.get());
}

std::string read_ascii(const std::uintptr_t address, std::size_t size)
{
	std::unique_ptr<char[]> buffer(new char[size]);
	//mex.Read(address, buffer.get(), size);
	safe_memcpy(buffer.get(), address, size);
	return std::string(buffer.get());
}

const wchar_t* GetWC(char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

inline float distance_cursor(Vector2 vec)
{
	POINT p;
	if (GetCursorPos(&p))
	{
		float ydist = (vec.y - p.y);
		float xdist = (vec.x - p.x);
		float ret = sqrt(pow(ydist, 2) + pow(xdist, 2));
		return ret;
	}
}

class mono_string
{
public:
	char pad_0[0x10];
	uint32_t size;
	wchar_t buffer[64];
};

enum BoneList : int
{
	l_hip = 1,
	l_knee,
	l_foot,
	l_toe,
	l_ankle_scale,
	pelvis,
	penis,
	GenitalCensor,
	GenitalCensor_LOD0,
	Inner_LOD0,
	GenitalCensor_LOD1,
	GenitalCensor_LOD2,
	r_hip,
	r_knee,
	r_foot,
	r_toe,
	r_ankle_scale,
	spine1,
	spine1_scale,
	spine2,
	spine3,
	spine4,
	l_clavicle,
	l_upperarm,
	l_forearm,
	l_hand,
	l_index1,
	l_index2,
	l_index3,
	l_little1,
	l_little2,
	l_little3,
	l_middle1,
	l_middle2,
	l_middle3,
	l_prop,
	l_ring1,
	l_ring2,
	l_ring3,
	l_thumb1,
	l_thumb2,
	l_thumb3,
	IKtarget_righthand_min,
	IKtarget_righthand_max,
	l_ulna,
	neck,
	head,
	jaw,
	eyeTranform,
	l_eye,
	l_Eyelid,
	r_eye,
	r_Eyelid,
	r_clavicle,
	r_upperarm,
	r_forearm,
	r_hand,
	r_index1,
	r_index2,
	r_index3,
	r_little1,
	r_little2,
	r_little3,
	r_middle1,
	r_middle2,
	r_middle3,
	r_prop,
	r_ring1,
	r_ring2,
	r_ring3,
	r_thumb1,
	r_thumb2,
	r_thumb3,
	IKtarget_lefthand_min,
	IKtarget_lefthand_max,
	r_ulna,
	l_breast,
	r_breast,
	BoobCensor,
	BreastCensor_LOD0,
	BreastCensor_LOD1,
	BreastCensor_LOD2,
	collision,
	displacement
};

class item_t
{
public:

	uintptr_t GetItemReference()
	{
		return safe_read((uintptr_t)this + 0x98, uintptr_t);
	}

	Vector3 GetTransformPosition(uintptr_t transform)
	{
		if (!transform)
			return Vector3{ 0.f, 0.f, 0.f };

		struct Matrix34 { BYTE vec0[16]; BYTE vec1[16]; BYTE vec2[16]; };
		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		int Index = safe_read(transform + 0x40, int); //mex.Read<int>(transform + 0x40);// *(PINT)(transform + 0x40);
		uintptr_t pTransformData = safe_read(transform + 0x38, uintptr_t); //mex.Read<uintptr_t>(transform + 0x38);
		uintptr_t transformData[2];
		safe_memcpy(&transformData, (PVOID)(pTransformData + 0x18), 16);

		size_t sizeMatriciesBuf = 48 * Index + 48;
		size_t sizeIndicesBuf = 4 * Index + 4;

		PVOID pMatriciesBuf = malloc(sizeMatriciesBuf);
		PVOID pIndicesBuf = malloc(sizeIndicesBuf);

		if (pMatriciesBuf && pIndicesBuf)
		{

			safe_memcpy(pMatriciesBuf, (PVOID)transformData[0], sizeMatriciesBuf);
			safe_memcpy(pIndicesBuf, (PVOID)transformData[1], sizeIndicesBuf);

			__m128 result = *(__m128*)((ULONGLONG)pMatriciesBuf + 0x30 * Index);
			int transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * Index);

			while (transformIndex >= 0)
			{
				Matrix34 matrix34 = *(Matrix34*)((ULONGLONG)pMatriciesBuf + 0x30 * transformIndex);
				__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));
				__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));
				__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));
				__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));
				__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));
				__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));
				__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

				result = _mm_add_ps(
					_mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
							tmp7)), *(__m128*)(&matrix34.vec0));
				try {
					transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * transformIndex);
				}
				catch (...)
				{
					// Do nothing
				}
			}

			return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
		}
	}

	int GetAmount()
	{
		return safe_read(this + 0x30, int);
	}

	int GetUID()
	{
		return safe_read((uintptr_t)this + 0x28, int);
	}

	std::wstring GetItemName()
	{
		if ((uintptr_t)this == NULL)
		{
			return L"No Item!";
		}
		auto info = safe_read((uintptr_t)this + 0x20, uintptr_t); // mex.Read<uintptr_t>((uintptr_t)Item + 0x20);
		if ((uintptr_t)info == NULL)
		{
			return L"No Item!";
		}
		auto name = safe_read(info + 0x20, uintptr_t);
		if ((uintptr_t)name == NULL)
		{
			return L"No Item!";
		}

		//auto namesize = safe_read(name + 0x10, INT32);

		return read_unicode(name + 0x14, 64);
	}

	bool IsItemGun()
	{
		auto ItemName = GetItemName();
		//std::cout << "Item Name: " << ws2s(ItemName) << std::endl;
		if (ItemName.find(L"rifle") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"pistol") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"bow") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"lmg") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"shotgun") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"smg") != std::string::npos)
		{
			return true;
		}
		else
			return false;
	}

	float GetBulletSpeed()
	{
		auto ItemName = GetItemName();
		//std::cout << "Item Name: " << ws2s(ItemName) << std::endl;
		if (ItemName.find(L"rifle") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"pistol") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"bow") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"lmg") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"shotgun") != std::string::npos)
		{
			return true;
		}
		if (ItemName.find(L"smg") != std::string::npos)
		{
			return true;
		}
		else
			return false;

	}

	void SetAutomatic()
	{
		if (!this)
			return;

		if (!IsItemGun())
			return;

		auto Held = safe_read(this + 0x98, uintptr_t); //mex.Read<uintptr_t>(Item + 0x98); //BaseProjectile
		if (!Held)
			return;

		safe_write(Held + 0x270, true, bool);
	}

	void SetFastReload()
	{
		if (!this)
			return;

		if (!IsItemGun())
			return;

		auto Held = safe_read(this + 0x98, uintptr_t); //mex.Read<uintptr_t>(Item + 0x98); //BaseProjectile
		if (!Held)
			return;

		//mex.Write<bool>(Held + 0x2A8, 1.f);
		safe_write(Held + 0x2A8, true, bool);
	}

	void SetFatBullet()
	{
		if (!this)
		{
			//std::cout << "!this" << std::endl;
			return;
		}

		if (this == NULL)
		{
			//std::cout << "Current Item == NULL" << std::endl;
			return;
		}

		if (!IsItemGun())
		{
			//std::cout << "!IsItemGun(CurrentHeldItem)" << std::endl;
			return;
		}

		auto Held = safe_read(this + 0x98, uintptr_t); // mex.Read<uintptr_t>(this->GetHeldItem() + 0x98); //BaseProjectile

		auto CreatedProjectiles = safe_read(Held + 0x338, uintptr_t); //mex.Read<uintptr_t>(Held + 0x338);

		//std::cout << "CreatedProjectiles: " << std::hex << std::uppercase << CreatedProjectiles << std::endl;

		auto CreatedProjectilesArray = safe_read(CreatedProjectiles + 0x10, uintptr_t); //mex.Read<uintptr_t>(CreatedProjectiles + 0x10);

		const auto size = safe_read(CreatedProjectiles + 0x18, int); //mex.Read<int>(CreatedProjectiles + 0x18);

		for (int i = 0u; i < size; i++)
		{
			uintptr_t Projectile = safe_read(CreatedProjectilesArray + (0x20 + (i * 0x8)), uintptr_t); //mex.Read<uintptr_t>(CreatedProjectilesArray + (0x20 + (i * 0x8)));

			std::cout << "Old Thicc: " << safe_read(Projectile + 0x2C, float) << std::endl;
			safe_write(Projectile + 0x2C, 1.5f, float); //mex.Write<float>(Projectile + 0x2C, 1.5f);
			std::cout << "New Thicc: " << safe_read(Projectile + 0x2C, float) << std::endl;
			//std::cout << "Projectile: " << std::hex << std::uppercase << Projectile << std::endl;
		}
	}

	Vector3 GetMuzzlePosition()
	{
		uintptr_t Transform = safe_read(this + 0x290, uintptr_t);
		return GetTransformPosition(Transform);
	}
};

class item_container_t
{
public:
	item_t* GetItem(int id)
	{
		auto item_list = safe_read((uintptr_t)this + 0x38, uintptr_t); //mex.Read<uintptr_t>((uintptr_t)ItemContainer + 0x38);

		if (!item_list)
		{
			//std::cout << "!item_list" << std::endl;
			return (item_t*)NULL;
		}

		auto Items = safe_read(item_list + 0x10, uintptr_t); //mex.Read<uintptr_t>((uintptr_t)item_list + 0x10);

		if (!Items)
		{
			//std::cout << "!Items" << std::endl;
			return (item_t*)NULL;
		}

		auto TheItem = safe_read(Items + 0x20 + (id * 0x8), uintptr_t); //mex.Read<uintptr_t>(Items + 0x20 + (id * 0x8));
		if (!TheItem)
		{
			//std::cout << "!TheItem" << std::endl;
			return (item_t*)NULL;
		}

		return (item_t*)TheItem;
	}
};

class player_inventory_t
{
public:
	item_container_t* GetBelt()
	{
		auto belt = safe_read((uintptr_t)this + 0x28, uintptr_t);
		return (item_container_t*)(belt);
	}
};

class input_t
{
public:
	Vector2 GetRecoilAngles()
	{
		return safe_read(this + 0x64, Vector2);
	}

	Vector2 GetViewAngles()
	{
		return safe_read(this + 0x3C, Vector2);
	}

	void SetViewAngles(Vector2 angle)
	{
		safe_write(this + 0x3C, angle, Vector2);
	}
};

class team_t
{
public:
	long teamID()
	{
		return safe_read(this + 0x18, long);
	}
};

class player_t
{
public:

	Vector3 GetTransformPosition(uintptr_t transform)
	{
		if (!transform) return Vector3{ 0.f, 0.f, 0.f };

		struct Matrix34 { BYTE vec0[16]; BYTE vec1[16]; BYTE vec2[16]; };
		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		int Index = safe_read(transform + 0x40, int); //mex.Read<int>(transform + 0x40);// *(PINT)(transform + 0x40);
		uintptr_t pTransformData = safe_read(transform + 0x38, uintptr_t); //mex.Read<uintptr_t>(transform + 0x38);
		uintptr_t transformData[2];
		//mex.Read((pTransformData + 0x18), &transformData, 16);
		safe_memcpy(&transformData, (PVOID)(pTransformData + 0x18), 16);
		//mex.Read(&transformData, (PVOID)(pTransformData + 0x18), 16);
		//safe_memcpy(&transformData, (PVOID)(pTransformData + 0x18), 16);

		size_t sizeMatriciesBuf = 48 * Index + 48;
		size_t sizeIndicesBuf = 4 * Index + 4;

		PVOID pMatriciesBuf = malloc(sizeMatriciesBuf);
		PVOID pIndicesBuf = malloc(sizeIndicesBuf);

		if (pMatriciesBuf && pIndicesBuf)
		{
			// Read Matricies array into the buffer
			//mex.Read(transformData[0], pMatriciesBuf, sizeMatriciesBuf);
			//impl::memory->read(transformData[0], pMatriciesBuf, sizeMatriciesBuf);
			// Read Indices array into the buffer
			//mex.Read(transformData[1], pIndicesBuf, sizeIndicesBuf);

			safe_memcpy(pMatriciesBuf, (PVOID)transformData[0], sizeMatriciesBuf);
			safe_memcpy(pIndicesBuf, (PVOID)transformData[1], sizeIndicesBuf);

			__m128 result = *(__m128*)((ULONGLONG)pMatriciesBuf + 0x30 * Index);
			int transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * Index);

			while (transformIndex >= 0)
			{
				Matrix34 matrix34 = *(Matrix34*)((ULONGLONG)pMatriciesBuf + 0x30 * transformIndex);
				__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));
				__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));
				__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));
				__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));
				__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));
				__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));
				__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

				result = _mm_add_ps(
					_mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
							tmp7)), *(__m128*)(&matrix34.vec0));
				try {
					transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * transformIndex);
				}
				catch (...)
				{
					// Do nothing
				}
			}

			return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
		}
	}

	Vector3 GetBonePosition(int Bone)
	{
		uintptr_t player_model = safe_read((uintptr_t)this + 0x118, uintptr_t); //mex.Read<uintptr_t>(Player + 0x118);
		if (!player_model)
			return Vector3{ 0, 0, 0 };
		uintptr_t BoneTransforms = safe_read(player_model + 0x48, uintptr_t); //mex.Read<uintptr_t>(player_model + 0x48);
		if (!BoneTransforms)
			return Vector3{ 0, 0, 0 };
		uintptr_t entity_bone = safe_read(BoneTransforms + (0x20 + (Bone * 0x8)), uintptr_t); //mex.Read<uintptr_t>(BoneTransforms + (0x20 + (Bone * 0x8)));
		if (!entity_bone)
			return Vector3{ 0, 0, 0 };
		auto transform = safe_read(entity_bone + 0x10, uintptr_t);
		if (!transform)
			return Vector3{ 0, 0, 0 };
		return GetTransformPosition(transform); //mex.Read<uintptr_t>(entity_bone + 0x10));
	}

	std::wstring GetName()
	{
		auto nameptr = safe_read((uintptr_t)this + 0x620, uintptr_t);//mex.Read<uintptr_t>(Player + 0x620);
		//auto namesize = safe_read(nameptr + 0x10, INT32);

		return read_unicode(nameptr + 0x14, 64);
	}

	int GetActiveWeaponID()
	{
		return safe_read((uintptr_t)this + 0x530, int);
	}

	float GetHealth()
	{
		return safe_read((uintptr_t)this + 0x1F4, float);
	}

	uintptr_t GetAddress()
	{
		return (uintptr_t)this;
	}

	bool valid()
	{
		return 0;
	}

	bool dead()
	{
		if (!this)
			return true;
		return safe_read(this + 0x1e4, bool);
	}

	bool IsSleeping()
	{
		auto flags = safe_read(this + 0x5B8, int);
		return flags & 16;
	}

	void SetAdminFlag()
	{
		auto flags = safe_read(this + 0x5B8, uintptr_t);
		flags |= 4;
		safe_write(this + 0x5B8, flags, uintptr_t);
	}

	void DoSpider()
	{
		auto BaseMovement = safe_read(this + 0x5E8, uintptr_t);
		if (!BaseMovement)
			return;

		safe_write(BaseMovement + 0xAC, 0.f, float);
		safe_write(BaseMovement + 0xB0, 0.f, float);
	}

	player_inventory_t* GetInventory()
	{
		auto inventory = safe_read((uintptr_t)this + 0x5C8, uintptr_t);
		return (player_inventory_t*)(inventory);
	}

	item_t* GetHeldItem()
	{
		//std::cout << "Getting Active Weapon ID!" << std::endl;
		auto ActiveWeaponID = GetActiveWeaponID();
		//std::cout << "LP Active Weapon ID: " << ActiveWeaponID << std::endl;

		if (!ActiveWeaponID)
		{
			//std::cout << "!ActiveWeaponID: " << ActiveWeaponID << std::endl;
			return (item_t*)NULL;
		}

		int CurrentWeaponID;
		for (int i = 0; i < 6; i++)
		{
			auto Inventory = GetInventory();
			auto Belt = Inventory->GetBelt();
			auto Item = Belt->GetItem(i);
			if (!Item)
				continue;
			CurrentWeaponID = Item->GetUID();
			//std::cout << "Current Weapon ID: " << CurrentWeaponID << std::endl;
			if (ActiveWeaponID == CurrentWeaponID)
			{
				//std::cout << "Return Item!" << std::endl;
				return Item;
			}
		}
		//std::cout << "Failed to find held item." << std::endl;
		return (item_t*)NULL;
	}

	input_t* GetInput()
	{
		auto Input = safe_read(this + 0x5E0, uintptr_t);
		return (input_t*)(Input);
	}

	Vector3 GetVelocity()
	{
		uintptr_t model = safe_read(this + 0x490, uintptr_t);
		return safe_read(model + 0x1D4, Vector3);
	}

	team_t* GetTeam()
	{
		auto Team = safe_read(this + 0x500, uintptr_t);
		return (team_t*)(Team);
	}
};

class player_base_t
{
public:
	player_t* player;
	Matrix4x4 viewmatrix;

	bool w2s(const Vector3& pos, Vector2& sc)
	{
		Vector3 TransVec = Vector3(viewmatrix._14, viewmatrix._24, viewmatrix._34);
		Vector3 RightVec = Vector3(viewmatrix._11, viewmatrix._21, viewmatrix._31);
		Vector3 UpVec = Vector3(viewmatrix._12, viewmatrix._22, viewmatrix._32);
		float w = Math::Dot(TransVec, pos) + viewmatrix._44;
		if (w < 0.098f)
			return false;
		float y = Math::Dot(UpVec, pos) + viewmatrix._42;
		float x = Math::Dot(RightVec, pos) + viewmatrix._41;
		sc = Vector2((scr_size.x / 2) * (1.f + x / w), (scr_size.y / 2) * (1.f - y / w));
		return true;
	}
} inline player_base;


namespace Players
{
	player_t* closestPlayer;

	namespace LocalPlayer
	{
		player_t* LocalPlayer;
		Vector3 LocalPosition;
	}
}