#pragma once
#include "handler.h"
bool g_self[99];
void self_menu()
{
	add_toggle_option(&g_self[1], "God Mode");
	add_toggle_option(&g_self[2], "Invisible");
	add_toggle_option(&g_self[3], "No Ragdoll");
	add_toggle_option(&g_self[4], "No Cops");
	add_toggle_option(&g_self[5], "Super Run");
	add_toggle_option(&g_self[6], "Super Swim");
	add_toggle_option(&g_self[7], "Super Jump");
	add_toggle_option(&g_self[8], "Explosive Punch");
	add_toggle_option(&g_self[9], "Infinite Ammo");
	add_toggle_option(&g_self[10], "Explosive Ammo");
	add_toggle_option(&g_self[11], "Fire Ammo");
	add_toggle_option(&g_self[12], "Slow-Motion");
	add_toggle_option(&g_self[13], "Mobile Radio");
}
void main_menu()
{
	add_submenu_option(self_menu, "SELF");
}