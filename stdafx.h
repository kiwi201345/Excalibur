#pragma once
#pragma warning(disable: 4996)
#pragma warning(disable: 4309)

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <cassert>
#include <bitset>
#include <array>
#include <stdlib.h>
#pragma comment(lib, "Winmm.lib")
#include "VMProtectSDK.h"
#pragma comment(lib, "VMProtectSDK64.lib") //was missing
#pragma comment(lib, "libMinHook.x64.lib") //was missing

#undef min
#undef max

#define VERSION 34
#define RELEASE_MODE
#define TRANSLATE(name) name.get().c_str()

template<typename T, int N>
constexpr int NUMOF(T(&)[N]) { return N; }

template<typename T>
inline bool is_valid_ptr(T ptr) {
	uint64_t address = (uint64_t)ptr;
	if (address < 0x5000) return false;
	if ((address & 0xFFFFFFFF) == 0xFFFFFFFF) return false;
	if ((address & 0xFFFFFFFF) <= 0xFF) return false;

	if (*(uint8_t*)((uint64_t)&address + 6) != 0 || *(uint8_t*)((uint64_t)&address + 7) != 0) return false;

	return true;
}

#define is_valid_vtable(address) \
	[=]() -> bool { \
		if (!is_valid_ptr<uint64_t>(address)) return false; \
		return address > global::vars::g_game_address.first && address < (global::vars::g_game_address.first + global::vars::g_game_address.second); \
	}()

constexpr char CharacterMap[] = {
	'_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_',
	'_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_',
	'_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_',
	'_', '_', '_', '_', '_', '_', '_', '.', '/', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'_', '_',  '_', '_', '_', '_',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
};

constexpr static __forceinline uint32_t JenkinsHash32(const char* String, uint32_t CurrentHashValue = NULL) {
	while (*String != NULL) {
		CurrentHashValue += CharacterMap[*(String++)];
		CurrentHashValue += (CurrentHashValue << 10);
		CurrentHashValue ^= (CurrentHashValue >> 6);
	}

	CurrentHashValue += (CurrentHashValue << 3);
	CurrentHashValue ^= (CurrentHashValue >> 11);
	CurrentHashValue += (CurrentHashValue << 15);

	return CurrentHashValue;
}

#define joaat(String) \
    []( ) -> auto { \
        constexpr auto HashValue = JenkinsHash32( String ); \
        \
        return HashValue; \
    }( )


#define GET_XOR_KEYUI8  ( ( CONST_HASH( __FILE__ __TIMESTAMP__ ) + __LINE__ ) % UINT8_MAX )
#define GET_XOR_KEYUI16 ( ( CONST_HASH( __FILE__ __TIMESTAMP__ ) + __LINE__ ) % UINT16_MAX )
#define GET_XOR_KEYUI32 ( ( CONST_HASH( __FILE__ __TIMESTAMP__ ) + __LINE__ ) % UINT32_MAX )


#define XOR( s ) ( s )
