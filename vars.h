#pragma once
#include "stdafx.h"
#include <locale>
#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>
#include <queue>
#include <Wincrypt.h>
#include <winternl.h>
#include <stdint.h>
#include <cstdint>
#include <stdio.h>
#include <cmath>
#include <array>
#include <Psapi.h>
#include <vector>
#include <stack>
#include <unordered_map>
#include <iomanip>
#include <functional>
#include <shlobj_core.h>
#include <intrin.h>
#include <chrono>

class simple_timer {
private:
	std::uint64_t m_ready_at;
	bool m_tick;
public:
	void start(std::uint64_t ticks) {
		if (this->m_tick) {
			this->m_ready_at = GetTickCount64() + ticks;
			this->m_tick = false;
		}
	}
	bool is_ready() {
		return GetTickCount64() > this->m_ready_at;
	}
	void reset() {
		this->m_tick = true;
	}
};

struct RGBA {
	int r;
	int g;
	int b;
	int a;
	RGBA(int red, int green, int blue, int alpha) {
		this->r = red;
		this->g = green;
		this->b = blue;
		this->a = alpha;
	}
};
struct rgba {
	std::int32_t r;
	std::int32_t g;
	std::int32_t b;
	std::int32_t a;
	rgba(std::int32_t r, std::int32_t g, std::int32_t b, std::int32_t a)
		: r(r), g(g), b(b), a(a) {}
};

std::int32_t KeyboardMenuOpen = VK_F4;
std::int32_t KeyboardMenuSelect = VK_RETURN;
std::int32_t KeyboardMenuBack = VK_BACK;
std::int32_t KeyboardMenuNavUp = VK_UP;
std::int32_t KeyboardMenuNavDown = VK_DOWN;
std::int32_t KeyboardMenuNavLeft = VK_LEFT;
std::int32_t KeyboardMenuNavRight = VK_RIGHT;


namespace KEYBOARD {

	bool IS_KEYBOARD_INPUT_JUST_PRESSED(std::int32_t key) {
		return GetAsyncKeyState(key) & 0x0001;
	}

	bool IS_KEYBOARD_INPUT_PRESSED(std::int32_t key) {
		return GetAsyncKeyState(key) & 0x8000;
	}
}


RGBA NinjaColors[10] = {
	RGBA(255, 255, 255, 255),// NinjaColors[0]  // banner
	RGBA(255, 255, 255, 255),// ninja color
	RGBA(255, 255, 255, 255),// NinjaColors[2]  // option text
	RGBA(0, 0, 0, 255),// NinjaColors[3] // highlighted text
	RGBA(255, 255, 255, 255),// NinjaColors[4] // selector
	RGBA(255, 255, 255, 255),// NinjaColors[5] // header text
	RGBA(255, 255, 255, 255),// NinjaColors[6] // footer arrows
	RGBA(0, 0, 0, 180),// NinjaColors[7] // backgrouund color
	RGBA(0, 0, 0, 215),// NinjaColors[8] // footer color
	RGBA(0, 0, 0, 215),// NinjaColors[9] // header color
};


typedef void(*menu_cxt_fn)();
struct menu_cxt {
	std::int32_t m_current_option{};
	std::int32_t m_max_options{};
	std::int32_t m_option_count{};
	menu_cxt_fn m_cxt_fn{};
	std::string m_title{};
};
bool g_menu_is_open = false;
bool g_push_menu_cxt = true;
menu_cxt g_menu_cxt_to_push;
std::stack<menu_cxt> g_menu_cxt_stack;
#define MENU_TITLE g_menu_cxt_stack.top().m_title
#define MAX_OPTIONS g_menu_cxt_stack.top().m_max_options
#define OPTION_COUNT g_menu_cxt_stack.top().m_option_count
#define CURRENT_OPTION g_menu_cxt_stack.top().m_current_option
#define CONTEXT_STACK_VALID g_menu_cxt_stack.size() > NULL



simple_timer g_menu_scroll_timer[5];
simple_timer isp_timer[5];
bool initialized = false;
bool g_option_select_pressed = false;
bool g_option_up_pressed = false;
bool g_option_down_pressed = false;
bool g_option_left_pressed = false;
bool g_option_right_pressed = false;
bool g_option_alt_pressed = false;
std::string g_menu_banner_texture = "exploit";


float bannerscale[2] = { 0.197301f, 0.0971974f };

std::float_t rahx = 0.8700f;
std::float_t rahy = 0.0650f;
std::float_t g_menu_banner_pos_x = 0.8700f;
std::float_t g_menu_banner_pos_x_cache = 0.8700f;
std::float_t g_menu_banner_pos_y = 0.0650f;
std::float_t g_menu_banner_pos_y_cache = 0.0650f;

std::float_t g_menu_banner_size_x = 0.2000f;
std::float_t g_menu_banner_size_y = 0.0970f;
std::float_t g_menu_text_padding = 0.0035f;
std::float_t g_menu_text_offset = 0.0145f;

std::float_t g_menu_scroll_bar_pos = 0.0000f;
std::float_t g_menu_scroll_bar_dest = 0.0000f;
std::float_t g_menu_scroll_bar_inc = 0.0000f;
std::float_t g_menu_scroll_bar_smoothness = 5.0000f;

rgba* g_selected_menu_colour_to_edit;
rgba g_menu_header_colour(10, 20, 30, 255);
rgba g_menu_header_text_colour(66, 225, 221, 255);
rgba g_menu_background_colour(10, 20, 30, 170);
rgba g_menu_option_text_colour(255, 255, 255, 255);
rgba g_menu_selected_option_text_colour(66, 225, 221, 255);
rgba g_menu_scroll_bar_colour(10, 20, 30, 255);
rgba g_menu_footer_colour(10, 20, 30, 255);
rgba g_menu_footer_arrows_colour(66, 225, 221, 255);
rgba g_menu_footer_text_colour(66, 225, 221, 255);
rgba g_menu_variable_arrows_colour(66, 225, 221, 255);
rgba g_menu_submenu_accent_colour(66, 225, 221, 110);
rgba g_menu_toggle_on_colour(66, 225, 221, 255);
rgba g_menu_toggle_off_colour(210, 50, 90, 255);
rgba g_menu_break_option_colour(140, 140, 140, 255);







int lastSelectedOption;

FLOAT fMenuTextOffset = 0.01300000000000000000000000000000f; // 0.01300000000000000000000000000000f
FLOAT fMenuTextSize = 0.35000000000000000000000000000000f; // 0.35000000000000000000000000000000f
FLOAT fMenuIncrement = 0.03703703703703703703703703703704f; // 0.03703703703703703703703703703704f
FLOAT fBannerPosX = 0.138959f;	// 0.84895833333333333333333333333333f
FLOAT fBannerPosY = 0.08611111111111111111111111111111f;	// 0.08611111111111111111111111111111f
FLOAT fBannerSizeX = 0.22395833333333333333333333333333f;	// 0.22395833333333333333333333333333f
FLOAT fBannerSizeY = 0.09000000000000000000000000000000f;	// 0.09000000000000000000000000000000f

FLOAT finfobox = 0.568959f;	// 0.84895833333333333333333333333333f

FLOAT fMenuScale = 1.0f;
float fScrollPos = ((fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2))) + ((fMenuIncrement * fMenuScale) / 2)) + (1 * (fMenuIncrement * fMenuScale));
float fScrollRemain;
float fScrollDest = 0;
float fScrollInc = 0;