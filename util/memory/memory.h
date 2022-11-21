#pragma once
#include "stdafx.h"
#include <vector>

namespace memory {
	template<typename V>
	void write(uint64_t address, V value) {
		uint32_t old;
		VirtualProtect((void*)address, sizeof(value), PAGE_EXECUTE_READWRITE, (DWORD*)&old);
		memcpy((void*)address, &value, sizeof(value));
		VirtualProtect((void*)address, sizeof(value), old, (DWORD*)&old);
	}

	template<typename V>
	void write(uint64_t address, V value, uint32_t size) {
		uint32_t old;
		VirtualProtect((void*)address, size, PAGE_EXECUTE_READWRITE, (DWORD*)&old);
		memcpy((void*)address, value, size);
		VirtualProtect((void*)address, size, old, (DWORD*)&old);
	}

	void nop(uint64_t address, uint32_t size);
	void write_vector(uint64_t address, std::vector<uint8_t> data);
	uint64_t read_instruction(uint64_t address, int opcode_size = 3, int opcode_length = 7);
}


#pragma once

#include <Windows.h>
#include <vector>

namespace mem
{
	typedef struct _module_ctx
	{
		DWORD_PTR start;
		DWORD_PTR end;

	} module_ctx;

	typedef struct _memscan
	{

	public:

		LPVOID base;

		__forceinline _memscan* add(INT value)
		{
			if (base != NULL)
				base = (LPVOID)((DWORD_PTR)base + value);
			return this;
		}

		__forceinline _memscan* rip()
		{
			if (base != NULL)
			{
				DWORD offset = *reinterpret_cast<DWORD*>(base);
				base = (LPVOID)((DWORD_PTR)base + offset + sizeof(DWORD));
			}
			return this;
		}

		__forceinline LPVOID get_addr() { return base; }
		__forceinline LPVOID get_call()
		{
			if (base != NULL)
			{
				INT offset = *reinterpret_cast<INT*>((DWORD_PTR)base + 1);
				base = (LPVOID)((DWORD_PTR)base + 5 + offset);
			}
			return base;
		}

	} memscan;

	static __forceinline DWORD get_pattern_size(LPCSTR pattern, PDWORD markers)
	{
		DWORD length = 0;
		DWORD _markers = 0;

		for (DWORD i = 0; i < strlen(pattern); i++)
		{
			if ((BYTE)pattern[i] == 32) continue; //space
			else if ((BYTE)pattern[i] == 63) //question mark (?)
			{
				_markers++;
				continue;
			}
			length++;
		}

		if ((length % 2) != 0) return 0;

		*markers = _markers;
		return (length / 2);
	}

	static inline BOOL mem_compare(PBYTE pattern, PBYTE mem, DWORD searchmask, DWORD symbols)
	{
		if (pattern == NULL || mem == NULL) return FALSE;

		BOOL is_in_search = FALSE;
		DWORD counter = 0;

		for (DWORD i = 0; i < symbols; i++)
		{
			is_in_search = searchmask & (1 << i);
			if (is_in_search)
			{
				if (pattern[counter] != mem[i]) return FALSE;
				else
				{
					counter++;
					continue;
				}
			}
		}
		return TRUE;
	}

	static PBYTE alloc_strhex2array(LPCSTR pattern, PDWORD symbols, PDWORD searchmask)
	{
		if (pattern == NULL || symbols == NULL || searchmask == NULL) return NULL;

		PBYTE sig_bytes = NULL;
		DWORD markers = 0;
		DWORD sig_length = 0;

		DWORD _searchmask = 0;
		memset(&_searchmask, 0xff, sizeof(DWORD));

		if ((sig_length = get_pattern_size(pattern, &markers)) == 0) return NULL;
		if ((sig_bytes = (PBYTE)malloc(sig_length)) == NULL) return NULL;

		DWORD counter = 0, offset = 0;
		for (DWORD i = 0; i < strlen(pattern); i++)
		{
			if (counter == (sig_length + markers)) break;
			if ((BYTE)pattern[i] != 63 && (BYTE)pattern[i] != 32)
			{
				sscanf_s(&pattern[i], "%2hhx", &sig_bytes[counter]);
				i += 2;
				counter++;
				offset++;
			}

			if (pattern[i] == 63)
			{
				_searchmask &= ~(1 << offset);
				offset++;
			}
		}

		*searchmask = _searchmask;
		*symbols = sig_length + markers;
		return sig_bytes;
	}

	static memscan find(LPCSTR pattern, module_ctx* ctx)
	{
		PBYTE sig_bytes = NULL;
		DWORD symbols;
		DWORD searchmask;
		LPVOID ret = NULL;
		LPVOID curr_ptr = NULL;
		memscan scan = { NULL };

		if ((sig_bytes = alloc_strhex2array(pattern, &symbols, &searchmask)) == NULL) return scan;

		DWORD counter = 0;
		while (TRUE)
		{
			curr_ptr = (LPVOID)(ctx->start + (DWORD_PTR)counter);
			if ((DWORD_PTR)curr_ptr + symbols >= ctx->end) break;

			if (mem_compare(sig_bytes, (PBYTE)curr_ptr, searchmask, symbols))
			{
				ret = curr_ptr;
				break;
			}
			counter++;
		}

		free(sig_bytes);
		scan.base = ret;
		return scan;
	}
}



#pragma once
#include <stdio.h>
#include <Windows.h>

#define LOG_BUFFER_SIZE 128

typedef enum LOGTYPE
{
	BLACK = 0,
	DARKBLUE = FOREGROUND_BLUE,
	DARKGREEN = FOREGROUND_GREEN,
	DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
	DARKRED = FOREGROUND_RED,
	DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
	DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	GRAY = FOREGROUND_INTENSITY,
	BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
	MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
} LOGTYPE;

class cmd
{
private:

	static __forceinline VOID SetTextColor(DWORD color)
	{
		CONSOLE_SCREEN_BUFFER_INFO buffer;
		HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		GetConsoleScreenBufferInfo(outputHandle, &buffer);

		WORD attributes = buffer.wAttributes & ~FOREGROUND_RED & ~FOREGROUND_GREEN & ~FOREGROUND_BLUE & ~FOREGROUND_INTENSITY;
		attributes |= color;

		SetConsoleTextAttribute(outputHandle, attributes);
	}

public:

	static VOID write(LOGTYPE logtype, LPCSTR format, ...)
	{
		char buff[LOG_BUFFER_SIZE];

		va_list list;
		va_start(list, format);
		vsprintf_s(buff, format, list);
		va_end(list);

		SetTextColor(logtype);
		char buff2[LOG_BUFFER_SIZE];
		sprintf_s(buff2, "%s\n", buff);
		printf(buff2);
	}

	static VOID write_nr(LOGTYPE logtype, LPCSTR format, ...)
	{
		char buff[LOG_BUFFER_SIZE];

		va_list list;
		va_start(list, format);
		vsprintf_s(buff, format, list);
		va_end(list);

		SetTextColor(logtype);
		char buff2[LOG_BUFFER_SIZE];

		sprintf_s(buff2, "%s", buff);
		printf(buff2);
	}

	static VOID write_inline(LOGTYPE logtype, LPCSTR format, ...)
	{
		char buff[LOG_BUFFER_SIZE];

		va_list list;
		va_start(list, format);
		vsprintf_s(buff, format, list);
		va_end(list);

		SetTextColor(logtype);
		char buff2[LOG_BUFFER_SIZE];

		sprintf_s(buff2, "%s\n", buff);
		printf(buff2);
	}
};


class gta5
{
public:

	static inline LPVOID baseAddress;
	static inline size_t moduleSize;
};

static BYTE spoofer_cleanup[] =
{
	0x48, 0x83, 0xEC, 0x10, 0x4C,
	0x8B, 0x5F, 0x08, 0x48, 0x8B,
	0x7F, 0x10, 0x41, 0xFF, 0xE3
};
/*
*
sub rsp, 0x8
mov r11, [rdi+0x8]
mov rdi, [rdi+0x10]
jmp r11
*/

/*static BYTE spoofer_cleanup[] =
{
	 0x4C, 0x8B, 0x5F, 0x08,
	 0x48, 0x8B, 0x7F, 0x10,
	 0x48, 0x83, 0xEC, 0x10,
	 0x41, 0xFF, 0xE3
};*/

/*
*
mov r11, [rdi+0x8]
mov rdi, [rdi+0x10]
sub rsp, 0x8
jmp r11

*/

static BYTE spoofer_payload[] =
{
	0x41, 0x5A, 0x48, 0x83, 0xC4, 0x08, 0x48,
	0x89, 0xE0, 0x48, 0x83, 0xC0, 0x18, 0x48,
	0x8B, 0x00, 0x4C, 0x8B, 0x18, 0x4C, 0x89,
	0x1C, 0x24, 0x48, 0x89, 0x78, 0x10, 0x48,
	0x89, 0xC7, 0x4C, 0x8B, 0x58, 0x08, 0x4C,
	0x89, 0x50, 0x08, 0x49, 0xBA, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x4C,
	0x89, 0x10, 0x41, 0xFF, 0xE3
};


typedef struct _x64_fastcall_spoof_ctx
{
	LPVOID payload;
	LPVOID gadget;

} x64_fastcall_spoof_ctx;

class x64
{
private:

	template<typename Ret, typename...Args>
	static inline auto call_payload(x64_fastcall_spoof_ctx* call_ctx, Args...args) -> Ret
	{
		auto pf = (Ret(*)(Args...))((LPVOID)call_ctx->payload);
		return pf(args...);
	}

	//5 or more args
	template<size_t Argc, typename>
	struct args_remapper
	{
		template<typename Ret, class First, class Second, class Third, class Fourth, typename...Pack>
		static auto do_call(x64_fastcall_spoof_ctx* call_ctx, LPVOID p_params, First first, Second second, Third third, Fourth fourth, Pack...pack) -> Ret
		{
			return call_payload<Ret, First, Second, Third, Fourth, void*, void*, Pack...>(call_ctx, first, second, third, fourth, p_params, nullptr, pack...);
		}
	};

	//only for 4 args or less
	template<size_t Argc>
	struct args_remapper<Argc, std::enable_if_t<Argc <= 4>>
	{
		template<typename Ret, class First = LPVOID, class Second = LPVOID, class Third = LPVOID, class Fourth = LPVOID>
		static auto do_call(x64_fastcall_spoof_ctx* call_ctx, LPVOID p_params, First first = First{},
			Second second = Second{}, Third third = Third{}, Fourth fourth = Fourth{}) -> Ret
		{
			return call_payload<Ret, First, Second, Third, Fourth, LPVOID, void*>(call_ctx, first, second, third, fourth, p_params, nullptr);
		}
	};

public:

	template<typename Ret, typename...Args>
	static inline auto spoof_call(Ret(*fn)(Args...), x64_fastcall_spoof_ctx* call_ctx, Args...args) -> Ret
	{
		struct p_params
		{
			LPVOID gadget;
			LPVOID fn;
			PULONG64 rbp;
		};

		p_params params;
		params.gadget = call_ctx->gadget;
		params.fn = static_cast<LPVOID>(fn);

		using remapper = args_remapper<sizeof...(Args), void>;
		return remapper::template do_call<Ret, Args...>(call_ctx, (LPVOID)&params, args...);
	}

public:

	static inline x64_fastcall_spoof_ctx fastcall_ctx;
	static inline LPVOID spoofer_cave = NULL;
	static inline size_t spoofer_size = NULL;
};

