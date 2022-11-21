// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "siggy.h"
#include "global/vars.h"
#include "util/dirs.h"
#include "util/log.h"
#include "util/threads.h"
#include "util/va.h"
#include "util/wmi.h"
#include "util/util.h"
#include "util/fiber.h"
#include "util/fiber_pool.h"
#include <fstream>
#include <filesystem>
#include <timeapi.h>
#include "handler.h"
float g_delta = 0.f;
void stat_get_int(rage::invoker::native_context* context) {
	static bool call_once_loaded = false;


	static int frame_cache = 0;
	static uint32_t this_frame = 0;
	static uint32_t last_frame = 0;

	if (frame_cache < native::get_frame_count()) {
		frame_cache = native::get_frame_count();

		this_frame = timeGetTime();
		g_delta = (float)(this_frame - last_frame) / 1000;
		last_frame = this_frame;

		if (!call_once_loaded) {
			try {
				util::fiber::load();
				util::fiber::load_shv();
				util::fiber::pool::load();

				util::fiber::add("F_BASE", [] {
					menu_handler();
					});

				
				native::request_streamed_texture_dict("commonmenu", false);
				
				call_once_loaded = true;
			}
			catch (std::exception& exception) {
				LOG("Exception initializing menu: %s", exception.what());
			}
		}

		if (!global::vars::g_unloading && call_once_loaded) {
			util::fiber::update();
			util::fiber::update_shv();
		}
	}

	context->set_return(0, native::stat_get_int(context->get_argument<uint32_t>(0), context->get_argument<int*>(1), context->get_argument<int>(2)));
}
LONG WINAPI exception_filter(struct _EXCEPTION_POINTERS* ExceptionInfoPtr) {
#undef exception_code
#undef exception_info

	DWORD exception_code = ExceptionInfoPtr->ExceptionRecord->ExceptionCode;
	ULONG_PTR exception_info = ExceptionInfoPtr->ExceptionRecord->ExceptionInformation[0];
	ULONG_PTR exception_info1 = ExceptionInfoPtr->ExceptionRecord->ExceptionInformation[1];
	ULONG_PTR exception_info2 = ExceptionInfoPtr->ExceptionRecord->ExceptionInformation[2];

	uint64_t exception_address = (uint64_t)ExceptionInfoPtr->ExceptionRecord->ExceptionAddress;

	LOG_CUSTOM_ERROR("Crash", "Game crashed with code 0x%X", exception_code);
	LOG_CUSTOM_ERROR("Crash", "Crashed @ 0x%llx (game=%llx cheat=%llx)", exception_address, global::vars::g_game_address.first, global::vars::g_module_handle);
	// LOG_DEV("Game Script: %s", *(const char**)(global::vars::g_game_address.first + 0x2D59018));

	switch (exception_code) {
	case EXCEPTION_ACCESS_VIOLATION:
		LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_ACCESS_VIOLATION");
		if (exception_info == 0) {
			LOG_CUSTOM_ERROR("Crash", "Attempted to read from: 0x%llx", exception_info1);
		}
		else if (exception_info == 1) {
			LOG_CUSTOM_ERROR("Crash", "Attempted to write to: 0x%llx", exception_info1);
		}
		else if (exception_info == 8) {
			LOG_CUSTOM_ERROR("Crash", "Data Execution Prevention (DEP) at: 0x%llx", exception_info1);
		}
		else {
			LOG_CUSTOM_ERROR("Crash", "Unknown access violation at: 0x%llx", exception_info1);
		}
		break;

	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_ARRAY_BOUNDS_EXCEEDED"); break;
	case EXCEPTION_BREAKPOINT: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_BREAKPOINT"); break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_DATATYPE_MISALIGNMENT"); break;
	case EXCEPTION_FLT_DENORMAL_OPERAND: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_FLT_DENORMAL_OPERAND"); break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_FLT_DIVIDE_BY_ZERO"); break;
	case EXCEPTION_FLT_INEXACT_RESULT: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_FLT_INEXACT_RESULT"); break;
	case EXCEPTION_FLT_INVALID_OPERATION: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_FLT_INVALID_OPERATION"); break;
	case EXCEPTION_FLT_OVERFLOW: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_FLT_OVERFLOW"); break;
	case EXCEPTION_FLT_STACK_CHECK: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_FLT_STACK_CHECK"); break;
	case EXCEPTION_FLT_UNDERFLOW: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_FLT_UNDERFLOW"); break;
	case EXCEPTION_ILLEGAL_INSTRUCTION: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_ILLEGAL_INSTRUCTION"); break;
	case EXCEPTION_IN_PAGE_ERROR:
		LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_IN_PAGE_ERROR");
		if (exception_info == 0) {
			LOG_CUSTOM_ERROR("Crash", "Attempted to read from: 0x%llx", exception_info1);
		}
		else if (exception_info == 1) {
			LOG_CUSTOM_ERROR("Crash", "Attempted to write to: 0x%llx", exception_info1);
		}
		else if (exception_info == 8) {
			LOG_CUSTOM_ERROR("Crash", "Data Execution Prevention (DEP) at: 0x%llx", exception_info1);
		}
		else {
			LOG_CUSTOM_ERROR("Crash", "Unknown access violation at: 0x%llx", exception_info1);
		}

		LOG_CUSTOM_ERROR("Crash", "NTSTATUS: 0x%llx", exception_info2);
		break;

	case EXCEPTION_INT_DIVIDE_BY_ZERO: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_INT_DIVIDE_BY_ZERO"); break;
	case EXCEPTION_INT_OVERFLOW: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_INT_OVERFLOW"); break;
	case EXCEPTION_INVALID_DISPOSITION: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_INVALID_DISPOSITION"); break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_NONCONTINUABLE_EXCEPTION"); break;
	case EXCEPTION_PRIV_INSTRUCTION: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_PRIV_INSTRUCTION"); break;
	case EXCEPTION_SINGLE_STEP: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_SINGLE_STEP"); break;
	case EXCEPTION_STACK_OVERFLOW: LOG_CUSTOM_ERROR("Crash", "Cause: EXCEPTION_STACK_OVERFLOW"); break;
	case DBG_CONTROL_C: LOG_CUSTOM_ERROR("Crash", "Cause: DBG_CONTROL_C"); break;
	default: LOG_CUSTOM_ERROR("Crash", "Cause: 0x%08x", exception_code);
	}

	LOG_CUSTOM_ERROR("Crash", "Dumping ASM registers:");
	LOG_CUSTOM_ERROR("Crash", "RAX: 0x%llx || RSI: 0x%llx", ExceptionInfoPtr->ContextRecord->Rax, ExceptionInfoPtr->ContextRecord->Rsi);
	LOG_CUSTOM_ERROR("Crash", "RBX: 0x%llx || RDI: 0x%llx", ExceptionInfoPtr->ContextRecord->Rbx, ExceptionInfoPtr->ContextRecord->Rdi);
	LOG_CUSTOM_ERROR("Crash", "RCX: 0x%llx || RBP: 0x%llx", ExceptionInfoPtr->ContextRecord->Rcx, ExceptionInfoPtr->ContextRecord->Rbp);
	LOG_CUSTOM_ERROR("Crash", "RDX: 0x%llx || RSP: 0x%llx", ExceptionInfoPtr->ContextRecord->Rdx, ExceptionInfoPtr->ContextRecord->Rsp);
	LOG_CUSTOM_ERROR("Crash", "R8: 0x%llx || R9: 0x%llx", ExceptionInfoPtr->ContextRecord->R8, ExceptionInfoPtr->ContextRecord->R9);
	LOG_CUSTOM_ERROR("Crash", "R10: 0x%llx || R11: 0x%llx", ExceptionInfoPtr->ContextRecord->R10, ExceptionInfoPtr->ContextRecord->R11);
	LOG_CUSTOM_ERROR("Crash", "R12: 0x%llx || R13: 0x%llx", ExceptionInfoPtr->ContextRecord->R12, ExceptionInfoPtr->ContextRecord->R13);
	LOG_CUSTOM_ERROR("Crash", "R14: 0x%llx || R15: 0x%llx", ExceptionInfoPtr->ContextRecord->R14, ExceptionInfoPtr->ContextRecord->R15);
	LOG_CUSTOM_ERROR("Crash", util::get_stack_trace().c_str());

	return EXCEPTION_CONTINUE_SEARCH;
}

void unload() {
	if (!global::vars::g_unloading) {
		global::vars::g_unloading = true;

		util::threads::add_job([](void*) {

			for (patches& patch : global::vars::g_patches) {
				memory::write_vector(patch.m_address, patch.m_bytes);
			}

			for (auto& hack_patch : global::vars::g_hack_patches) {
				memory::write_vector(hack_patch.second.m_address, hack_patch.second.m_bytes);
			}

			hooking::cleanup();
			util::threads::cleanup();
			util::fiber::cleanup();
			util::va::cleanup();
			util::log::cleanup();

			PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
			FreeConsole();


			});
	}
}

void entry(void* handle) {
	SetUnhandledExceptionFilter(exception_filter);
	srand(GetTickCount());

	MODULEINFO module_info;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(0), &module_info, sizeof(module_info));

	global::vars::g_steam = (uint64_t)GetModuleHandleA(XOR("steam_api64.dll")) > 0;
	global::vars::g_game_address = { (uint64_t)module_info.lpBaseOfDll, module_info.SizeOfImage };


	if (!util::dirs::load()) {
		unload();
		return;
	}

	util::log::load();

	LOG_DEV("Base: %llx", handle);
	LOG(XOR("Welcome to Ozark! Version V%i"), VERSION);


	if (!(global::vars::g_window = FindWindowA(XOR("grcWindow"), NULL))) {
		int timeout = 0;
		while (!global::vars::g_window && !global::vars::g_unloading) {
			if (timeout >= 20) {
				LOG_ERROR(XOR("Failed to find window!"));
				unload();
				return;
			}

			global::vars::g_window = FindWindowA(XOR("grcWindow"), NULL);

			timeout++;
			Sleep(1000);
		}
	}



	if (!pattern_scan()) {
		unload();
		return;
	}

	while (*global::vars::g_game_state != GameStatePlaying) Sleep(500);

	hooking::script(XOR("SGI"), XOR("main_persistent"), 0x767FBC2AC802EF3D, stat_get_int);
	init_menu(main_menu, "HOME");
	util::threads::add_thread(XOR("T_SH"), [](void*) {
		Sleep(1500);

		});

	util::threads::add_thread(XOR("T_SC"), [](void*) {
		});

}
#pragma comment(lib, "Winmm.lib")

BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        global::vars::g_module_handle = handle;

        if (handle) {
            MODULEINFO module_info;
            GetModuleInformation(GetCurrentProcess(), handle, &module_info, sizeof(module_info));
            global::vars::g_cheat_address = { (uint64_t)module_info.lpBaseOfDll, module_info.SizeOfImage };
        }

        util::threads::add_job(entry, handle);
        break;

    case DLL_PROCESS_DETACH:
        global::vars::g_unloading = true;
        break;
    }

    return TRUE;
}