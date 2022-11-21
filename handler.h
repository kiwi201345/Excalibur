#pragma once
#include "design.h"
#include "rage/engine.h"
#include "main_menu.h"

bool g_was_texture_file_load_attempted = false;
bool g_was_texture_file_loaded = false;

void menu_handler()
{

		
		render_menu();
		MenuNotificationHandler();
}

extern void main_menu();