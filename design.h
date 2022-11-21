#pragma once
#include "vars.h"
#include "rage/invoker/natives.h"
#include "util/util.h"
#include "util/fiber.h"


VOID DrawSprite(PCHAR szDict, PCHAR szTexture, FLOAT fPosX, FLOAT fPosY, FLOAT fRot, RGBA g_Col, FLOAT fScaleX = 1.0f, FLOAT fScaleY = 1.0f, bool resu = true) {
	INT g_ScreenRes[2];
	native::get_screen_resolution(&g_ScreenRes[0], &g_ScreenRes[1]);
	math::vector3 g_TextureRes = native::get_texture_resolution(szDict, szTexture);
	native::request_streamed_texture_dict(szDict, FALSE);
	
	if (resu)
		native::draw_sprite(szDict, szTexture, fPosX, fPosY, (g_TextureRes.x / (FLOAT)g_ScreenRes[0]) * fScaleX, (g_TextureRes.y / (FLOAT)g_ScreenRes[1]) * fScaleY, fRot, g_Col.r, g_Col.g, g_Col.b, g_Col.a);
	else
		native::draw_sprite(szDict, szTexture, fPosX, fPosY, fScaleX, fScaleY, fRot, g_Col.r, g_Col.g, g_Col.b, g_Col.a);
}



void push_menu(menu_cxt_fn menu, std::string title, std::int32_t max_options = 10) {
	if (menu != nullptr) {
		std::locale loc;
		g_menu_cxt_to_push.m_current_option = 1;
		g_menu_cxt_to_push.m_max_options = max_options;
		g_menu_cxt_to_push.m_option_count = 0;
		std::stringstream ss;
		for (std::string::size_type i = 0; i < title.length(); ++i)
			ss << std::toupper(title[i], loc);
		g_menu_cxt_to_push.m_title = ss.str();
		g_menu_cxt_to_push.m_cxt_fn = menu;
		g_push_menu_cxt = true;
	}
}
void pop_menu() {
	if (g_menu_cxt_stack.size() > 1) g_menu_cxt_stack.pop();
	else g_menu_is_open = false;
}
void init_menu(menu_cxt_fn menu, std::string title) {
	push_menu(menu, title);
}

void draw_sprite(const char* dictionary, const char* texture, std::float_t x, std::float_t y, std::float_t rot, rgba col, std::float_t w = 1.0f, std::float_t h = 1.0f) {
	std::int32_t screen_res[2];
	native::get_active_screen_resolution(&screen_res[0], &screen_res[1]);
	std::float_t aspect = native::get_aspect_ratio(0);
	std::float_t factor = (native::to_float(screen_res[0]) / native::to_float(screen_res[1]));
	std::float_t scale = (factor / aspect);
	math::vector3 texture_res = native::get_texture_resolution(dictionary, texture);
	texture_res.x = (texture_res.x * (0.5f / scale));
	texture_res.y = (texture_res.y * (0.5f / scale));
	if (native::has_streamed_texture_dict_loaded(dictionary))
		native::draw_sprite(dictionary, texture, x, y, ((texture_res.x / 1280.0f) * scale) * w, ((texture_res.y / 720.0f) * scale) * h, rot, col.r, col.g, col.b, col.a);
	else
		native::request_streamed_texture_dict(dictionary, FALSE);
}

struct Notification {
	char m_lpText[200];
	int m_lClearAt;
	int m_lAlpha;
};

Notification g_lNotificationStore[10];

//call to display notify
void Notify(unsigned long a_lTicks, char* a_lpText, ...) {
	va_list l_lpList;
	for (int i = 9; i > 0; i--)
		memcpy(&g_lNotificationStore[i], &g_lNotificationStore[i - 1], sizeof(Notification));
	va_start(l_lpList, a_lpText);
	vsprintf(g_lNotificationStore[0].m_lpText, a_lpText, l_lpList);
	va_end(l_lpList);
	g_lNotificationStore[0].m_lAlpha = 0;
	g_lNotificationStore[0].m_lClearAt = GetTickCount() + a_lTicks;
}
void print_notification(std::int32_t ticks, std::string text) {

	Notify(ticks, (char*)text.c_str());

}
void draw_text(std::string text, std::float_t x, std::float_t y, std::int32_t font, std::float_t scale, rgba col, bool centered, bool right_justified, bool outlined) {
	native::set_text_font(font);
	native::set_text_scale(0.f, scale);
	native::set_text_colour(col.r, col.g, col.b, col.a);
	if (right_justified) native::set_text_wrap(0.0f, x);
	else native::set_text_wrap(0.0f, 1.0f);
	native::set_text_centre(centered);
	if (outlined) native::set_text_outline();
	native::set_text_justification(right_justified);
	native::begin_text_command_display_text("STRING");
	native::add_text_component_substring_player_name(text.c_str());
	native::end_text_command_display_text(x, y, 0);
}

VOID Draw_Text_Ninja(std::string szText, INT g_Font, FLOAT fPosX, FLOAT fPosY, FLOAT fScaleX, FLOAT fScaleY, RGBA g_Col, BOOL bCentered, BOOL bRightJustified, BOOL bOutlined) {
	native::set_text_font(g_Font);
	native::set_text_scale(0.f, fScaleY);
	native::set_text_colour(g_Col.r, g_Col.g, g_Col.b, g_Col.a);
	if (bRightJustified)
		native::set_text_wrap(0.0f, fPosX);
	else
		native::set_text_wrap(0.0f, 1.0f);
	native::set_text_centre(bCentered);
	if (bOutlined)
		native::set_text_justification(bRightJustified);
	native::begin_text_command_display_text("STRING");
	native::add_text_component_substring_player_name(szText.c_str());
	native::end_text_command_display_text(fPosX, fPosY, 0);
}

void draw_rect(std::float_t x, std::float_t y, std::float_t w, std::float_t h, rgba col) {
	native::DRAW_RECT(x, y, w, h, col.r, col.g, col.b, col.a);
}


void Notify22(const char* text, ...)
{
	static int MapNotifyHandle;
	static char MapNotifyBuffer[255];
	va_list mapnotifyargs;
	va_start(mapnotifyargs, text);
	sprintf_s(MapNotifyBuffer, text, mapnotifyargs);
	va_end(mapnotifyargs);

	native::remove_notification(MapNotifyHandle);
	native::begin_text_command_thefeed_post("STRING");
	native::add_text_component_substring_player_name(MapNotifyBuffer);
	MapNotifyHandle = native::draw_notification(0, 1);
}


void DrawBar(bool bInterpolation, bool bHorizontal, float fPosX, float fPosY, float fWidth, float fHeight, float fValMax, float m_Val, RGBA color = RGBA(255, 255, 255, 255)) { // assuming 0 is the default, im not making a handler for this as i cba
	if (bHorizontal) {
		native::DRAW_RECT(fPosX - (fWidth / 2) + (((fWidth / fValMax) * m_Val) / 2), fPosY, ((fWidth / fValMax) * m_Val), fHeight, color.r, color.g, color.b, color.a);
	}
	else {
		native::DRAW_RECT(fPosX, fPosY + (fHeight / 2) - (((fHeight / fValMax) * m_Val) / 2), fWidth, ((fHeight / fValMax) * m_Val), color.r, color.g, color.b, color.a);
	}
}


float textWidth;
bool isVariant = false;
float rightsideTextx = 0.013f;
FLOAT GetTextWidth(std::string szText, INT g_Font, FLOAT fScaleX, FLOAT fScaleY) {
	native::begin_text_command_width("STRING");
	native::add_text_component_substring_player_name(szText.c_str());
	native::set_text_font(g_Font);
	native::set_text_scale(0.f, fScaleY);
	return native::end_text_command_get_width(TRUE);
}

FLOAT NotifyXCoord = 0.76f;
FLOAT NotifyYCoord = 0.928999f;
FLOAT NotifyXScale = 0.6;
FLOAT NotifyYScale = 0.028;

FLOAT NotifyXCoord1 = 0.451f;
FLOAT NotifyYCoord1 = 0.928999f;
FLOAT NotifyXScale1 = 0.012;
FLOAT NotifyYScale1 = 0.02;

FLOAT NotifyXCoord2 = 0.451f;
FLOAT NotifyYCoord2 = 0.928999f;
FLOAT NotifyXScale2 = 0.016;
FLOAT NotifyYScale2 = 0.028;
FLOAT NotifyTextX = 0.46f;
FLOAT NotifyTextY = 0.914999f;
void DrawSprite2(char* StreamedTexture, char* textureName, float x, float y, float width, float height, float rotation, int r, int g, int b, int a)
{
	native::request_streamed_texture_dict(StreamedTexture, false);
	native::has_streamed_texture_dict_loaded(StreamedTexture);
	native::draw_sprite(StreamedTexture, textureName, x, y, width, height, rotation, r, g, b, a);
}
void MenuNotificationHandler() {
	for (int i = 0; i < 10; i++) {
		if (GetTickCount() < (DWORD)g_lNotificationStore[i].m_lClearAt) {
			DrawBar(true, true, NotifyXCoord, NotifyYCoord + (i * 0.028f), NotifyXScale, NotifyYScale, .6, GetTextWidth(g_lNotificationStore[i].m_lpText, 0, 0.35f, 0.35f), RGBA(0, 0, 0, (g_lNotificationStore[i].m_lAlpha < (180 - (i * 25))) ? (g_lNotificationStore[i].m_lAlpha += 25) : (g_lNotificationStore[i].m_lAlpha > (180 - (i * 25))) ? (g_lNotificationStore[i].m_lAlpha -= 25) : g_lNotificationStore[i].m_lAlpha));
			native::DRAW_RECT(NotifyXCoord2, NotifyYCoord2 + (i * 0.028f), NotifyXScale2, NotifyYScale2, 0, 0, 0, (g_lNotificationStore[i].m_lAlpha < (180 - (i * 25))) ? (g_lNotificationStore[i].m_lAlpha += 25) : (g_lNotificationStore[i].m_lAlpha > (180 - (i * 25))) ? (g_lNotificationStore[i].m_lAlpha -= 25) : g_lNotificationStore[i].m_lAlpha);
			DrawSprite2("timerbars", "circle_checkpoints", NotifyXCoord1, NotifyYCoord1 + (i * 0.028f), NotifyXScale1, NotifyYScale1, .0f, 255, 255, 255, (g_lNotificationStore[i].m_lAlpha < (200 - (i * 25))) ? (g_lNotificationStore[i].m_lAlpha += 25) : (g_lNotificationStore[i].m_lAlpha > (200 - (i * 25))) ? (g_lNotificationStore[i].m_lAlpha -= 25) : g_lNotificationStore[i].m_lAlpha);
			Draw_Text_Ninja(g_lNotificationStore[i].m_lpText, 0, NotifyTextX, NotifyTextY + (i * 0.028f), 0.35f, 0.35f, RGBA(255, 255, 255, (g_lNotificationStore[i].m_lAlpha < (250 - (i * 25))) ? (g_lNotificationStore[i].m_lAlpha += 25) : (g_lNotificationStore[i].m_lAlpha > (250 - (i * 25))) ? (g_lNotificationStore[i].m_lAlpha -= 25) : g_lNotificationStore[i].m_lAlpha), false, false, false);
		}
		else {
			DrawBar(true, true, NotifyXCoord, NotifyYCoord + (i * 0.028f), NotifyXScale, NotifyYScale, .6, GetTextWidth(g_lNotificationStore[i].m_lpText, 0, 0.35f, 0.35f), RGBA(0, 0, 0, (g_lNotificationStore[i].m_lAlpha > 0) ? (g_lNotificationStore[i].m_lAlpha -= 25) : g_lNotificationStore[i].m_lAlpha));
			native::DRAW_RECT(NotifyXCoord2, NotifyYCoord2 + (i * 0.028f), NotifyXScale2, NotifyYScale2, 0, 0, 0, (g_lNotificationStore[i].m_lAlpha > 0) ? (g_lNotificationStore[i].m_lAlpha -= 25) : g_lNotificationStore[i].m_lAlpha);
			DrawSprite2("timerbars", "circle_checkpoints", NotifyXCoord1, NotifyYCoord1 + (i * 0.028f), NotifyXScale1, NotifyYScale1, .0f, 255, 255, 255, (g_lNotificationStore[i].m_lAlpha > 0) ? (g_lNotificationStore[i].m_lAlpha -= 25) : g_lNotificationStore[i].m_lAlpha);
			Draw_Text_Ninja(g_lNotificationStore[i].m_lpText, 0, NotifyTextX, NotifyTextY + (i * 0.028f), 0.35f, 0.35f, RGBA(255, 255, 255, (g_lNotificationStore[i].m_lAlpha > 0) ? (g_lNotificationStore[i].m_lAlpha -= 25) : g_lNotificationStore[i].m_lAlpha), false, false, false);
		}
	}
}

bool add_text_option(std::string text) {
	bool return_value = false;
	if (CONTEXT_STACK_VALID) {
		OPTION_COUNT++;
		if (CURRENT_OPTION == OPTION_COUNT) {
			if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS)
				Draw_Text_Ninja(text, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, FALSE, FALSE);
			else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION)
				Draw_Text_Ninja(text, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, FALSE, FALSE);
		}
		else {
			if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS)
				Draw_Text_Ninja(text, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, FALSE, FALSE);
			else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION)
				Draw_Text_Ninja(text, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, FALSE, FALSE);
		}
		if (CURRENT_OPTION == OPTION_COUNT) {

			if (return_value = g_option_select_pressed) {
				// do stuff
			}
		}
	}
	return return_value;
}
bool add_submenu_option(menu_cxt_fn menu_function, std::string text) {
	bool return_value = false;
	if (CONTEXT_STACK_VALID) {
		OPTION_COUNT++;
		if (CURRENT_OPTION == OPTION_COUNT) {
			if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS)
				Draw_Text_Ninja(text, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, FALSE, FALSE);
			else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION)
				Draw_Text_Ninja(text, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, FALSE, FALSE);
		}
		else {
			if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS)
				Draw_Text_Ninja(text, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, FALSE, FALSE);
			else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION)
				Draw_Text_Ninja(text, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, FALSE, FALSE);
		}
		if (CURRENT_OPTION == OPTION_COUNT) {

			if (return_value = g_option_select_pressed) {
				push_menu(menu_function, text);
			}
		}
	}
	return return_value;
}
void add_break_option(std::string text) {
	if (CONTEXT_STACK_VALID) {
		OPTION_COUNT++;
		OPTION_COUNT++;
		if (CURRENT_OPTION == OPTION_COUNT) {
			if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS)
				Draw_Text_Ninja(text, 1, fBannerPosX, (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (0.015f * fMenuScale), (0.42f * fMenuScale), (0.42f * fMenuScale), RGBA(100, 100, 100, 255), TRUE, FALSE, FALSE);
			else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION)
				Draw_Text_Ninja(text, 1, fBannerPosX, (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (0.015f * fMenuScale), (0.42f * fMenuScale), (0.42f * fMenuScale), RGBA(100, 100, 100, 255), TRUE, FALSE, FALSE);
		}
		else {
			if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS)
				Draw_Text_Ninja(text, 1, fBannerPosX, (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (0.015f * fMenuScale), (0.42f * fMenuScale), (0.42f * fMenuScale), RGBA(100, 100, 100, 255), TRUE, FALSE, FALSE);
			else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION)
				Draw_Text_Ninja(text, 1, fBannerPosX, (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (0.015f * fMenuScale), (0.42f * fMenuScale), (0.42f * fMenuScale), RGBA(100, 100, 100, 255), TRUE, FALSE, FALSE);
		}
		if (CURRENT_OPTION == OPTION_COUNT) {
			if (g_option_up_pressed)
				CURRENT_OPTION--;
			if (g_option_down_pressed)
				CURRENT_OPTION++;
		}
	}
}
FLOAT g_fToggleRotation = 0.0f;
VOID UpdateToggleRotation() {
	if (g_fToggleRotation > -360.0f)
		g_fToggleRotation -= 12.0f;
	else
		g_fToggleRotation = 0.0f;
}

void AddToggle(std::string option, bool boolean, const char* info = NULL)
{

	OPTION_COUNT++;
	if (CURRENT_OPTION == OPTION_COUNT) {
		if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS) {
			Draw_Text_Ninja(option, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, FALSE, FALSE);
			DrawSprite("timerbars", "circle_checkpoints", fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.009f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))), boolean ? g_fToggleRotation : 0.0f, boolean ? RGBA(255, 255, 255, 255) : RGBA(255, 255, 255, 100), fMenuScale * 0.4f, fMenuScale * 0.4f);
		}
		else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION) {
			Draw_Text_Ninja(option, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, FALSE, FALSE);
			DrawSprite("timerbars", "circle_checkpoints", fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.009f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))), boolean ? g_fToggleRotation : 0.0f, boolean ? RGBA(255, 255, 255, 255) : RGBA(255, 255, 255, 100), fMenuScale * 0.4f, fMenuScale * 0.4f);
		}
	}
	else {
		if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS) {
			Draw_Text_Ninja(option, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, FALSE, FALSE);
			DrawSprite("timerbars", "circle_checkpoints", fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.009f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))), boolean ? g_fToggleRotation : 0.0f, boolean ? RGBA(255, 255, 255, 255) : RGBA(255, 255, 255, 100), fMenuScale * 0.4f, fMenuScale * 0.4f);
		}
		else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION) {
			Draw_Text_Ninja(option, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, FALSE, FALSE);
			DrawSprite("timerbars", "circle_checkpoints", fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.009f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))), boolean ? g_fToggleRotation : 0.0f, boolean ? RGBA(255, 255, 255, 255) : RGBA(255, 255, 255, 100), fMenuScale * 0.4f, fMenuScale * 0.4f);
		}
	}
}


int sOption()
{
	if (g_option_select_pressed)
		return CURRENT_OPTION;
	else return 0;
}
bool AddToggle(std::string option, bool boolean, bool looped, const char* info = NULL)
{
	AddToggle(option, boolean, info);
	if (CURRENT_OPTION == OPTION_COUNT && g_option_select_pressed) return true;
	else return false;
}
std::string buffer;
void AddToggleVar(std::string option, bool* boolean, const char* info = 0)
{
	AddToggle(option, *boolean, info);
	if (sOption() == OPTION_COUNT) {
		*boolean = !*boolean;
	}
}
bool add_toggle_option(bool* boolean, std::string option, const char* info = NULL)
{
	bool return_value = false;
	AddToggleVar(option, boolean, info);
	if (CURRENT_OPTION == OPTION_COUNT && g_option_select_pressed) return true;
	else return false;
	if (g_option_select_pressed) {
		print_notification(7000, option + ":~s~ " + ((*boolean = !*boolean) ? "~g~Enabled~s~." : "~r~Disabled~s~."));
	}
}


void RightSidedModifier(std::string option, std::string RightSidestring, bool Var = FALSE, INT Font = 0) {

	OPTION_COUNT++;
	if (CURRENT_OPTION == OPTION_COUNT) {
		if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS) {
			Draw_Text_Ninja(option, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, FALSE, FALSE);
			if (Var) {
				Draw_Text_Ninja(RightSidestring, Font, fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (rightsideTextx * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, TRUE, FALSE);
				DrawSprite("commonmenu", "arrowright", fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.009f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))), 0.0f, RGBA(255, 255, 255, 255), fMenuScale * 0.4f, fMenuScale * 0.4f);
				DrawSprite("commonmenu", "arrowleft", fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.017f * fMenuScale) - GetTextWidth(RightSidestring, 0, (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale)), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))), 0.0f, RGBA(255, 255, 255, 255), fMenuScale * 0.4f, fMenuScale * 0.4f);

			}
			else
				Draw_Text_Ninja(RightSidestring, Font, fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, TRUE, FALSE);
		}
		else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION) {
			Draw_Text_Ninja(option, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, FALSE, FALSE);
			if (Var) {
				Draw_Text_Ninja(RightSidestring, Font, fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (rightsideTextx * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, TRUE, FALSE);
				DrawSprite("commonmenu", "arrowright", fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.009f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))), 0.0f, RGBA(255, 255, 255, 255), fMenuScale * 0.4f, fMenuScale * 0.4f);
				DrawSprite("commonmenu", "arrowleft", fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.017f * fMenuScale) - GetTextWidth(RightSidestring, 0, (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale)), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))), 0.0f, RGBA(255, 255, 255, 255), fMenuScale * 0.4f, fMenuScale * 0.4f);
			}
			else
				Draw_Text_Ninja(RightSidestring, Font, fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[3], FALSE, TRUE, FALSE);
		}
	}
	else {
		if (CURRENT_OPTION <= MAX_OPTIONS && OPTION_COUNT <= MAX_OPTIONS) {
			Draw_Text_Ninja(option, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, FALSE, FALSE);
			Draw_Text_Ninja(RightSidestring, Font, fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + (OPTION_COUNT * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, TRUE, FALSE);
		}
		else if ((OPTION_COUNT > (CURRENT_OPTION - MAX_OPTIONS)) && OPTION_COUNT <= CURRENT_OPTION) {
			Draw_Text_Ninja(option, 0, fBannerPosX - (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) + (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, FALSE, FALSE);
			Draw_Text_Ninja(RightSidestring, Font, fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.004f * fMenuScale), (((fBannerPosY + (((fBannerSizeY * fMenuScale) * fMenuScale) / 2)) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT - (CURRENT_OPTION - MAX_OPTIONS)) * (fMenuIncrement * fMenuScale))) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[2], FALSE, TRUE, FALSE);
		}
	}
}


template<typename A, typename B>
bool add_editor_option(bool exec_on_arrow_press, A to_display, B* value_ref, B min, B max, B accuracy, std::string text) {
	bool return_value = false;
	if (CONTEXT_STACK_VALID) {
		std::stringstream ss;
		ss << to_display;
		RightSidedModifier(text, ss.str(), false, 0);
		if (CURRENT_OPTION == OPTION_COUNT) {
			if (g_option_left_pressed) {
				*value_ref -= accuracy;
				if (*value_ref < min)
					*value_ref = max;
				exec_on_arrow_press ? return_value = true : return_value = false;
			}
			if (g_option_right_pressed) {
				*value_ref += accuracy;
				if (*value_ref > max)
					*value_ref = min;
				exec_on_arrow_press ? return_value = true : return_value = false;
			}
			if (g_option_select_pressed) {
				exec_on_arrow_press ? return_value = false : return_value = true;
			}
		}
	}
	return return_value;
}
template<typename A, typename B>
bool add_editor_toggle_option(bool* control, A to_display, B* value_ref, B min, B max, B accuracy, std::string text) {
	bool return_value = false;
	if (CONTEXT_STACK_VALID) {
		std::stringstream ss;
		ss << to_display;
		RightSidedModifier(text, ss.str(), true, 0);
		if (CURRENT_OPTION == OPTION_COUNT) {
			if (g_option_left_pressed) {
				*value_ref -= accuracy;
				if (*value_ref < min)
					*value_ref = max;
			}
			if (g_option_right_pressed) {
				*value_ref += accuracy;
				if (*value_ref > max)
					*value_ref = min;
			}
			if (return_value = g_option_select_pressed) {
				print_notification(7000, text + ":~s~ " + ((*control = !*control) ? "~g~On~s~." : "~r~Off~s~."));
			}
		}
	}
	return return_value;
}
bool add_string_array_option(bool exec_on_arrow_press, std::string* array, std::int32_t array_size, std::int32_t* value_ref, std::string text) {
	bool return_value = false;
	if (CONTEXT_STACK_VALID) {
		std::stringstream ss;
		ss << array[*value_ref] << " ~c~[" << *value_ref + 1 << "/" << array_size << "]";
		RightSidedModifier(text, ss.str(), false, 0);
		if (CURRENT_OPTION == OPTION_COUNT) {
			if (g_option_left_pressed) {
				if (-- * value_ref < 0) *value_ref = (array_size - 1);
				exec_on_arrow_press ? return_value = true : return_value = false;
			}
			if (g_option_right_pressed) {
				if (++ * value_ref > (array_size - 1)) *value_ref = 0;
				exec_on_arrow_press ? return_value = true : return_value = false;
			}
			if (g_option_select_pressed) {
				exec_on_arrow_press ? return_value = false : return_value = true;
			}
		}
	}
	return return_value;
}
bool add_string_array_toggle_option(bool* control, std::string* array, std::int32_t array_size, std::int32_t* value_ref, std::string text) {
	bool return_value = false;
	if (CONTEXT_STACK_VALID) {
		std::stringstream ss;
		ss << array[*value_ref] << " ~c~[" << *value_ref + 1 << "/" << array_size << "]";
		RightSidedModifier(text, ss.str(), true, 0);
		if (CURRENT_OPTION == OPTION_COUNT) {
			if (g_option_left_pressed) {
				if (-- * value_ref < 0) *value_ref = (array_size - 1);
			}
			if (g_option_right_pressed) {
				if (++ * value_ref > (array_size - 1)) *value_ref = 0;
			}
			if (return_value = g_option_select_pressed) {
				print_notification(7000, text + " (" + array[*value_ref] + "):~s~ " + ((*control = !*control) ? "~g~On~s~." : "~r~Off~s~."));
			}
		}
	}
	return return_value;
}


void update_user_input() {
	if (native::is_input_disabled(2)) {
		native::hide_help_text_this_frame();
		native::hide_hud_component_this_frame(10);
		native::hide_hud_component_this_frame(6);
		native::hide_hud_component_this_frame(7);
		native::hide_hud_component_this_frame(9);
		native::hide_hud_component_this_frame(8);
		native::set_cinematic_button_active(1);
		native::set_input_exclusive(2, INPUT_CURSOR_SCROLL_UP);
		native::set_input_exclusive(2, INPUT_CURSOR_SCROLL_DOWN);
		native::disable_control_action(0, INPUT_HUD_SPECIAL, TRUE);
		native::disable_control_action(0, INPUT_LOOK_BEHIND, TRUE);
		native::disable_control_action(0, INPUT_NEXT_CAMERA, TRUE);
		native::disable_control_action(0, INPUT_VEH_SELECT_NEXT_WEAPON, TRUE);
		native::disable_control_action(0, INPUT_VEH_CIN_CAM, TRUE);
		native::disable_control_action(2, INPUT_FRONTEND_ACCEPT, TRUE);
		native::disable_control_action(2, INPUT_FRONTEND_CANCEL, TRUE);
		native::disable_control_action(2, INPUT_FRONTEND_LEFT, TRUE);
		native::disable_control_action(2, INPUT_FRONTEND_RIGHT, TRUE);
		native::disable_control_action(2, INPUT_FRONTEND_DOWN, TRUE);
		native::disable_control_action(2, INPUT_FRONTEND_UP, TRUE);
		native::disable_control_action(2, INPUT_FRONTEND_ACCEPT, TRUE);
		native::disable_control_action(0, INPUT_SELECT_CHARACTER_FRANKLIN, TRUE);
		native::disable_control_action(0, INPUT_SELECT_CHARACTER_MICHAEL, TRUE);
		native::disable_control_action(0, INPUT_SELECT_CHARACTER_TREVOR, TRUE);
		native::disable_control_action(0, INPUT_SELECT_CHARACTER_MULTIPLAYER, TRUE);
		native::disable_control_action(0, INPUT_CHARACTER_WHEEL, TRUE);
		native::disable_control_action(0, INPUT_PHONE, TRUE);
		native::disable_control_action(2, INPUT_CELLPHONE_CANCEL, TRUE);
		native::disable_control_action(2, INPUT_CELLPHONE_SELECT, TRUE);
		native::disable_control_action(2, INPUT_CELLPHONE_UP, TRUE);
		native::disable_control_action(2, INPUT_CELLPHONE_DOWN, TRUE);

		if (KEYBOARD::IS_KEYBOARD_INPUT_JUST_PRESSED(KeyboardMenuSelect)) // sel
			g_option_select_pressed = true;

		if (KEYBOARD::IS_KEYBOARD_INPUT_JUST_PRESSED(KeyboardMenuBack)) // back
			pop_menu();

		if (KEYBOARD::IS_KEYBOARD_INPUT_JUST_PRESSED(VK_F3)) // alt
			g_option_alt_pressed = true;

		if (KEYBOARD::IS_KEYBOARD_INPUT_JUST_PRESSED(KeyboardMenuNavLeft)) { // left
			g_option_left_pressed = true;
		}
		else if (KEYBOARD::IS_KEYBOARD_INPUT_PRESSED(KeyboardMenuNavLeft)) { // left
			g_menu_scroll_timer[3].start(500);
			if (g_menu_scroll_timer[3].is_ready()) {
				g_option_left_pressed = true;
			}
		}
		else { g_menu_scroll_timer[3].reset(); }

		if (KEYBOARD::IS_KEYBOARD_INPUT_JUST_PRESSED(KeyboardMenuNavRight)) { // right
			g_option_right_pressed = true;
		}
		else if (KEYBOARD::IS_KEYBOARD_INPUT_PRESSED(KeyboardMenuNavRight)) { // right
			g_menu_scroll_timer[4].start(500);
			if (g_menu_scroll_timer[4].is_ready()) {
				g_option_right_pressed = true;
			}
		}
		else { g_menu_scroll_timer[4].reset(); }

		if (KEYBOARD::IS_KEYBOARD_INPUT_JUST_PRESSED(KeyboardMenuNavDown) || native::is_disabled_control_just_released(0, INPUT_CURSOR_SCROLL_DOWN)) { // down
			initialized = false;
			g_option_down_pressed = true;
			CURRENT_OPTION++;
			if (CURRENT_OPTION > OPTION_COUNT)
				CURRENT_OPTION = 1;
		}
		else if (KEYBOARD::IS_KEYBOARD_INPUT_PRESSED(KeyboardMenuNavDown)) { // down
			g_menu_scroll_timer[1].start(500);
			if (g_menu_scroll_timer[1].is_ready()) {
				g_menu_scroll_timer[0].start(40);
				if (g_menu_scroll_timer[0].is_ready()) {
					g_option_down_pressed = true;
					CURRENT_OPTION++;
					lastSelectedOption = CURRENT_OPTION;
					if (CURRENT_OPTION > OPTION_COUNT)
						CURRENT_OPTION = 1;
					g_menu_scroll_timer[0].reset();
				}
			}
		}
		else { g_menu_scroll_timer[1].reset(); }

		if (KEYBOARD::IS_KEYBOARD_INPUT_JUST_PRESSED(KeyboardMenuNavUp) || native::is_disabled_control_just_pressed(0, INPUT_CURSOR_SCROLL_UP)) { // up
			g_option_up_pressed = true;
			CURRENT_OPTION--;
			lastSelectedOption = CURRENT_OPTION;
			if (CURRENT_OPTION < 1)
				CURRENT_OPTION = OPTION_COUNT;
		}
		if (KEYBOARD::IS_KEYBOARD_INPUT_PRESSED(KeyboardMenuNavUp)) { // up
			g_menu_scroll_timer[2].start(500);
			if (g_menu_scroll_timer[2].is_ready()) {
				g_menu_scroll_timer[0].start(40);
				if (g_menu_scroll_timer[0].is_ready()) {
					g_option_up_pressed = true;
					CURRENT_OPTION--;
					lastSelectedOption = CURRENT_OPTION;
					if (CURRENT_OPTION < 1)
						CURRENT_OPTION = OPTION_COUNT;
					g_menu_scroll_timer[0].reset();
				}
			}
		}
		else { g_menu_scroll_timer[2].reset(); }
	}
	else {
		native::hide_help_text_this_frame();
		native::hide_hud_component_this_frame(10);
		native::hide_hud_component_this_frame(6);
		native::hide_hud_component_this_frame(7);
		native::hide_hud_component_this_frame(9);
		native::hide_hud_component_this_frame(8);
		native::set_cinematic_button_active(1);
		native::set_input_exclusive(2, INPUT_FRONTEND_X);
		native::set_input_exclusive(2, INPUT_FRONTEND_ACCEPT);
		native::set_input_exclusive(2, INPUT_FRONTEND_CANCEL);
		native::set_input_exclusive(2, INPUT_FRONTEND_DOWN);
		native::set_input_exclusive(2, INPUT_FRONTEND_UP);
		native::set_input_exclusive(2, INPUT_FRONTEND_LEFT);
		native::set_input_exclusive(2, INPUT_FRONTEND_RIGHT);
		native::disable_control_action(0, INPUT_NEXT_CAMERA, TRUE);
		native::disable_control_action(0, INPUT_HUD_SPECIAL, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON_UNARMED, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON_MELEE, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON_HANDGUN, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON_SHOTGUN, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON_SMG, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON_AUTO_RIFLE, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON_SNIPER, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON_HEAVY, TRUE);
		native::disable_control_action(0, INPUT_SELECT_WEAPON_SPECIAL, TRUE);
		native::disable_control_action(0, INPUT_WEAPON_WHEEL_NEXT, TRUE);
		native::disable_control_action(0, INPUT_WEAPON_WHEEL_PREV, TRUE);
		native::disable_control_action(0, INPUT_WEAPON_SPECIAL, 1);
		native::disable_control_action(0, INPUT_WEAPON_SPECIAL_TWO, TRUE);
		native::disable_control_action(0, INPUT_DIVE, TRUE);
		native::disable_control_action(0, INPUT_MELEE_ATTACK_LIGHT, TRUE);
		native::disable_control_action(0, INPUT_MELEE_ATTACK_HEAVY, TRUE);
		native::disable_control_action(0, INPUT_MELEE_BLOCK, TRUE);
		native::disable_control_action(0, INPUT_ARREST, TRUE);
		native::disable_control_action(0, INPUT_VEH_HEADLIGHT, TRUE);
		native::disable_control_action(0, INPUT_VEH_RADIO_WHEEL, TRUE);
		native::disable_control_action(0, INPUT_CONTEXT, TRUE);
		native::disable_control_action(0, INPUT_RELOAD, TRUE);
		native::disable_control_action(0, INPUT_VEH_CIN_CAM, TRUE);
		native::disable_control_action(0, INPUT_JUMP, TRUE);
		native::disable_control_action(0, INPUT_VEH_SELECT_NEXT_WEAPON, TRUE);
		native::disable_control_action(0, INPUT_VEH_FLY_SELECT_NEXT_WEAPON, TRUE);
		native::disable_control_action(0, INPUT_SELECT_CHARACTER_FRANKLIN, TRUE);
		native::disable_control_action(0, INPUT_SELECT_CHARACTER_MICHAEL, TRUE);
		native::disable_control_action(0, INPUT_SELECT_CHARACTER_TREVOR, TRUE);
		native::disable_control_action(0, INPUT_SELECT_CHARACTER_MULTIPLAYER, TRUE);
		native::disable_control_action(0, INPUT_CHARACTER_WHEEL, TRUE);
		native::disable_control_action(2, INPUT_CELLPHONE_CANCEL, TRUE);
		native::disable_control_action(2, INPUT_CELLPHONE_SELECT, TRUE);
		native::disable_control_action(2, INPUT_CELLPHONE_UP, TRUE);
		native::disable_control_action(2, INPUT_CELLPHONE_DOWN, TRUE);
		native::disable_control_action(0, INPUT_DETONATE, 1);
		native::disable_control_action(0, INPUT_SPRINT, 1);
		native::disable_control_action(0, INPUT_VEH_DUCK, 1);
		native::disable_control_action(0, INPUT_VEH_HEADLIGHT, 1);
		native::disable_control_action(0, INPUT_VEH_PUSHBIKE_SPRINT, 1);
		native::disable_control_action(0, INPUT_VEH_PUSHBIKE_PEDAL, 1);

		if (native::is_control_just_pressed(0, INPUT_FRONTEND_ACCEPT))
			g_option_select_pressed = true;

		if (native::is_control_just_pressed(0, INPUT_FRONTEND_CANCEL))
			pop_menu();

		if (native::is_control_just_pressed(0, INPUT_FRONTEND_X))
			g_option_alt_pressed = true;

		if (native::is_control_just_pressed(0, INPUT_FRONTEND_LEFT)) {
			g_option_left_pressed = true;
		}
		else if (native::is_control_pressed(0, INPUT_FRONTEND_LEFT)) {
			g_menu_scroll_timer[3].start(500);
			if (g_menu_scroll_timer[3].is_ready()) {
				g_option_left_pressed = true;
			}
		}
		else { g_menu_scroll_timer[3].reset(); }

		if (native::is_control_just_pressed(0, INPUT_FRONTEND_RIGHT)) {
			g_option_right_pressed = true;
		}
		else if (native::is_control_pressed(0, INPUT_FRONTEND_RIGHT)) {
			g_menu_scroll_timer[4].start(500);
			if (g_menu_scroll_timer[4].is_ready()) {
				g_option_right_pressed = true;
			}
		}
		else { g_menu_scroll_timer[4].reset(); }

		if (native::is_control_just_pressed(0, INPUT_FRONTEND_DOWN)) {
			g_option_down_pressed = true;
			CURRENT_OPTION++;
			lastSelectedOption = CURRENT_OPTION;
			if (CURRENT_OPTION > OPTION_COUNT)
				CURRENT_OPTION = 1;
		}
		else if (native::is_control_pressed(0, INPUT_FRONTEND_DOWN)) {
			g_menu_scroll_timer[1].start(500);
			if (g_menu_scroll_timer[1].is_ready()) {
				g_menu_scroll_timer[0].start(40);
				if (g_menu_scroll_timer[0].is_ready()) {
					g_option_down_pressed = true;
					CURRENT_OPTION++;
					lastSelectedOption = CURRENT_OPTION;
					if (CURRENT_OPTION > OPTION_COUNT)
						CURRENT_OPTION = 1;
					g_menu_scroll_timer[0].reset();
				}
			}
		}
		else { g_menu_scroll_timer[1].reset(); }

		if (native::is_disabled_control_just_pressed(0, INPUT_FRONTEND_UP)) {
			g_option_up_pressed = true;
			lastSelectedOption = CURRENT_OPTION;
			CURRENT_OPTION--;
			if (CURRENT_OPTION < 1)
				CURRENT_OPTION = OPTION_COUNT;
		}
		else if (native::is_control_pressed(0, INPUT_FRONTEND_UP)) {
			g_menu_scroll_timer[2].start(500);
			if (g_menu_scroll_timer[2].is_ready()) {
				g_menu_scroll_timer[0].start(40);
				if (g_menu_scroll_timer[0].is_ready()) {
					g_option_up_pressed = true;
					lastSelectedOption = CURRENT_OPTION;
					CURRENT_OPTION--;
					if (CURRENT_OPTION < 1)
						CURRENT_OPTION = OPTION_COUNT;
					g_menu_scroll_timer[0].reset();
				}
			}
		}
		else { g_menu_scroll_timer[2].reset(); }
	}
}

float GetMovementOverTime(float startY, float endY, int timeTicks) {
	float difY = endY - startY;
	return (float)((endY - startY) / timeTicks);
}


void SmoothScrolling() {
	if (CURRENT_OPTION > MAX_OPTIONS && lastSelectedOption > MAX_OPTIONS && lastSelectedOption != 1) {
		fScrollPos = ((fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2))) + ((fMenuIncrement * fMenuScale) / 2)) + (MAX_OPTIONS * (fMenuIncrement * fMenuScale));
	}
	else {
		fScrollDest = ((fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2))) + ((fMenuIncrement * fMenuScale) / 2)) + ((lastSelectedOption <= MAX_OPTIONS && CURRENT_OPTION > MAX_OPTIONS) ? (MAX_OPTIONS * (fMenuIncrement * fMenuScale)) : lastSelectedOption == 1 && CURRENT_OPTION != 2 ? CURRENT_OPTION > MAX_OPTIONS ? (MAX_OPTIONS * (fMenuIncrement * fMenuScale)) : CURRENT_OPTION * (fMenuIncrement * fMenuScale) : CURRENT_OPTION * (fMenuIncrement * fMenuScale));
		fScrollInc = GetMovementOverTime(fScrollPos, fScrollDest, 2);
		if (fScrollInc != 0) {
			if (abs(fScrollPos - fScrollDest) <= fScrollInc + 0.00010f) {
				fScrollPos = fScrollDest;
				fScrollInc = 0;
			}
			else {
				fScrollPos += fScrollInc;
			}
		}
	}
}


void render_menu() {
	if (CONTEXT_STACK_VALID) {
		if (native::is_input_disabled(2)) {
			if (KEYBOARD::IS_KEYBOARD_INPUT_JUST_PRESSED(KeyboardMenuOpen))
				g_menu_is_open = !g_menu_is_open;
		}
		else if ((native::is_control_pressed(0, INPUT_FRONTEND_LEFT) && native::is_disabled_control_just_pressed(0, INPUT_FRONTEND_X)) || (native::is_control_just_pressed(0, INPUT_FRONTEND_LEFT) && native::is_control_pressed(0, INPUT_FRONTEND_X)))
			g_menu_is_open = !g_menu_is_open;

		if (g_menu_is_open) {
			g_menu_banner_pos_x_cache = g_menu_banner_pos_x;
			g_menu_banner_pos_y_cache = g_menu_banner_pos_y;

			update_user_input();
			OPTION_COUNT = NULL;

			if (g_menu_cxt_stack.top().m_cxt_fn != nullptr)
				g_menu_cxt_stack.top().m_cxt_fn();

			native::set2_d_layer(2);
			SmoothScrolling();

			std::stringstream ss;
			ss << CURRENT_OPTION << " / " << OPTION_COUNT;

			native::DRAW_RECT(fBannerPosX, fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2) + ((fMenuIncrement * fMenuScale) / 2)), ((fBannerSizeX * fMenuScale) * fMenuScale), (fMenuIncrement * fMenuScale), NinjaColors[9].r, NinjaColors[9].g, NinjaColors[9].b, NinjaColors[9].a); // mini banner
			DrawSprite("commonmenu", "gradient_bgd", fBannerPosX, fBannerPosY, 0.0f, NinjaColors[0], ((fBannerSizeX * fMenuScale) * fMenuScale), ((fBannerSizeY * fMenuScale) * fMenuScale), false);
		//	DrawSprite("exploit", "title", fBannerPosX, fBannerPosY, 0.0f, NinjaColors[0], ((fBannerSizeX * fMenuScale) * fMenuScale), ((fBannerSizeY * fMenuScale) * fMenuScale), false);


			char szTemp[16];
			sprintf_s(szTemp, "%d / %d", CURRENT_OPTION, OPTION_COUNT);
			Draw_Text_Ninja(szTemp, 0, fBannerPosX + (((fBannerSizeX * fMenuScale) * fMenuScale) / 2) - (0.004f * fMenuScale), fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2) + ((fMenuIncrement * fMenuScale) / 2)) - (fMenuTextOffset * fMenuScale), (fMenuTextSize * fMenuScale), (fMenuTextSize * fMenuScale), NinjaColors[5], 0, 1, 0);


			if (OPTION_COUNT > MAX_OPTIONS) {
				native::DRAW_RECT(fBannerPosX, (fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2 + ((MAX_OPTIONS * (fMenuIncrement * fMenuScale)) / 2))) + (fMenuIncrement * fMenuScale)), ((fBannerSizeX * fMenuScale) * fMenuScale), (fMenuIncrement * fMenuScale) * MAX_OPTIONS, NinjaColors[7].r, NinjaColors[7].g, NinjaColors[7].b, NinjaColors[7].a); //Background
				if (CURRENT_OPTION > MAX_OPTIONS)
					native::DRAW_RECT(fBannerPosX, fScrollPos, ((fBannerSizeX * fMenuScale) * fMenuScale), (fMenuIncrement * fMenuScale), NinjaColors[4].r, NinjaColors[4].g, NinjaColors[4].b, NinjaColors[4].a); //Scroller
				else
					native::DRAW_RECT(fBannerPosX, fScrollPos, ((fBannerSizeX * fMenuScale) * fMenuScale), (fMenuIncrement * fMenuScale), NinjaColors[4].r, NinjaColors[4].g, NinjaColors[4].b, NinjaColors[4].a); //Scroller
				native::DRAW_RECT(fBannerPosX, ((fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2))) + ((fMenuIncrement * fMenuScale) / 2)) + ((MAX_OPTIONS + 1) * (fMenuIncrement * fMenuScale)), ((fBannerSizeX * fMenuScale) * fMenuScale), (fMenuIncrement * fMenuScale), NinjaColors[8].r, NinjaColors[8].g, NinjaColors[8].b, NinjaColors[8].a);
				DrawSprite("commonmenu", "shop_arrows_upanddown", fBannerPosX, ((fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2))) + ((fMenuIncrement * fMenuScale) / 2)) + ((MAX_OPTIONS + 1) * (fMenuIncrement * fMenuScale)), 0.0f, NinjaColors[6], fMenuScale, fMenuScale);
			}
			else {
				native::DRAW_RECT(fBannerPosX, (fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2 + ((OPTION_COUNT * (fMenuIncrement * fMenuScale)) / 2))) + (fMenuIncrement * fMenuScale)), ((fBannerSizeX * fMenuScale) * fMenuScale), (fMenuIncrement * fMenuScale) * OPTION_COUNT, NinjaColors[7].r, NinjaColors[7].g, NinjaColors[7].b, NinjaColors[7].a); //Background
				native::DRAW_RECT(fBannerPosX, fScrollPos, ((fBannerSizeX * fMenuScale) * fMenuScale), (fMenuIncrement * fMenuScale), NinjaColors[4].r, NinjaColors[4].g, NinjaColors[4].b, NinjaColors[4].a); //Scroller
				native::DRAW_RECT(fBannerPosX, ((fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2))) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT + 1) * (fMenuIncrement * fMenuScale)), ((fBannerSizeX * fMenuScale) * fMenuScale), (fMenuIncrement * fMenuScale), NinjaColors[8].r, NinjaColors[8].g, NinjaColors[8].b, NinjaColors[8].a);
				DrawSprite("commonmenu", "shop_arrows_upanddown", fBannerPosX, ((fBannerPosY + ((((fBannerSizeY * fMenuScale) * fMenuScale) / 2))) + ((fMenuIncrement * fMenuScale) / 2)) + ((OPTION_COUNT + 1) * (fMenuIncrement * fMenuScale)), 0.0f, NinjaColors[6], fMenuScale, fMenuScale);
			}
		}
		g_option_select_pressed = false;
		g_option_up_pressed = false;
		g_option_down_pressed = false;
		g_option_left_pressed = false;
		g_option_right_pressed = false;
		g_option_alt_pressed = false;
	}
	if (g_push_menu_cxt) {
		g_menu_cxt_stack.push(g_menu_cxt_to_push);
		g_push_menu_cxt = false;
	}
}