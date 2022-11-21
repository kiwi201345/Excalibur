#pragma once
#include "util/memory/memory.h"
#include "util/hooking/hooking.h"
#include "util/log.h"
#include "util/caller.h"
#include "global/vars.h"
#include "rage/invoker/natives.h"
#include "util/util.h"
#include "util/threads.h"
#include <intrin.h>
#include <Psapi.h>







namespace memory
{
    std::uintptr_t* scan(std::string name, const char* signature) {
        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto* start = const_cast<char*>(pattern);
            auto* end = const_cast<char*>(pattern) + strlen(pattern);

            for (auto* current = start; current < end; ++current) {
                if (*current == '?') {
                    ++current;
                    if (*current == '?')
                        ++current;
                    bytes.push_back(-1);
                }
                else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto* const module = GetModuleHandle(nullptr);

        auto* const dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
        auto* const nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(module) + dos_header->e_lfanew);

        const auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
        auto pattern_bytes = pattern_to_byte(signature);
        auto* scan_bytes = reinterpret_cast<std::uint8_t*>(module);

        const auto s = pattern_bytes.size();
        auto* const d = pattern_bytes.data();

        for (auto i = 0ul; i < size_of_image - s; ++i) {
            auto found = true;
            for (auto j = 0ul; j < s; ++j) {
                if (scan_bytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if (found) {
                LOG_CUSTOM_SUCCESS("Signature Success!", "%s Found at address  %llx", name, reinterpret_cast<std::uintptr_t*>(&scan_bytes[i]));
                return reinterpret_cast<std::uintptr_t*>(&scan_bytes[i]);
            }
        }
        return nullptr;
    }

    std::uintptr_t* scan_ex(const wchar_t* process, std::string name, const char* signature) {
        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto* start = const_cast<char*>(pattern);
            auto* end = const_cast<char*>(pattern) + strlen(pattern);

            for (auto* current = start; current < end; ++current) {
                if (*current == '?') {
                    ++current;
                    if (*current == '?')
                        ++current;
                    bytes.push_back(-1);
                }
                else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto* const module = GetModuleHandle(process);

        auto* const dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
        auto* const nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(module) + dos_header->e_lfanew);

        const auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
        auto pattern_bytes = pattern_to_byte(signature);
        auto* scan_bytes = reinterpret_cast<std::uint8_t*>(module);

        const auto s = pattern_bytes.size();
        auto* const d = pattern_bytes.data();

        for (auto i = 0ul; i < size_of_image - s; ++i) {
            auto found = true;
            for (auto j = 0ul; j < s; ++j) {
                if (scan_bytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if (found) {
                LOG_CUSTOM_SUCCESS("Signature Success!", "%s Found", name);
                return reinterpret_cast<std::uintptr_t*>(&scan_bytes[i]);
            }
        }
        return nullptr;
    }

    std::uintptr_t dereference(const uintptr_t address, const unsigned int offset) {
        return address == 0 ? reinterpret_cast<std::uintptr_t>(nullptr) : address +
            static_cast<int>(*reinterpret_cast<int*>(address + offset) +
                offset + sizeof(int));
    }
}
namespace memory
{
	std::uintptr_t* scan(std::string name, const char* signature);
	std::uintptr_t* scan_ex(const wchar_t* process, std::string name, const char* signature);
	std::uintptr_t dereference(uintptr_t address, unsigned int offset);
}

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

std::uint64_t tmp;
const wchar_t* socialclub = convertCharArrayToLPCWSTR("socialclub.dll");
bool pattern_scan()
{
	bool successful = true;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan_ex(socialclub, "SCGIT", "48 8D 05 ? ? ? ? 48 03 F8 44 8B 47 14 48 8D 57 20 E8 ? ? ? ? 85"));
	global::vars::g_social_club_game_info_table = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan_ex(socialclub, "SCGGIT", "E8 ? ? ? ? 85 C0 78 27 3B 47 14 7D 1E 48 98 48 8B D6 48 69 C8"));
	global::vars::g_social_club_get_game_info_table = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GS", "8B 05 ? ? ? ? 85 C0 0F 84 ? ? ? ? 83 F8 06 74 08 83 C0 F8"));
	global::vars::g_game_state = (game_state*)memory::read_instruction(tmp, 2, 6);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NRT", "48 8D 0D ? ? ? ? 48 8B 14 FA E8"));
	global::vars::g_native_registration = (rage::invoker::native_registration**)memory::read_instruction(tmp);
	global::vars::g_translate_native = memory::read_instruction(tmp + 0xB, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GSME", "40 53 48 83 EC 20 48 8B C2 48 8B D9 33 D2 48 8B C8 E8 ? ? ? ? 33 D2 44 8B D0 F7 35 ? ? ? ? 44 8B C2 48 8B 15 ? ? ? ? 46 8B 0C 82 41"));
	global::vars::g_get_store_module_extension = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RSF", "89 54 24 10 55 53 56 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 83 79 ? ? 41 8B F8 48 8B F1 75 07 32 C0 E9 ? ? ? ? 48 8B 06"));
	global::vars::g_request_streaming_file = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("CGFXF", "40 53 48 83 EC 20 C7 41 ? ? ? ? ? 48 8D 05 ? ? ? ? 48 8B D9 48 89 01 C7 41 ? ? ? ? ? 48 8D 05 ? ? ? ? 48 89 01"));
	global::vars::g_construct_gfx_font = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("D", "48 83 EC 28 F0 FF 49 08 75 0F 48 85 C9 74 0A 48 8B 01 BA ? ? ? ? FF 10 48 83 C4 28 C3"));
	global::vars::g_destruct = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("CIDFF", "40 55 48 8B EC 48 81 EC ? ? ? ? 49 8B 00 F3 0F 10 02 F3 0F 10 4A ? 83 65 B8 00 83 65 C0 00 83 65 C4 00 F3"));
	global::vars::g_create_id_for_font = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("AFL", "40 53 48 83 EC 20 48 8B 41 40 4D 63 C8 4C 8B D9 46 0F B6 14 08 48 8B DA 41 81 E2 ? ? ? ? 41 8B C2 45 8B CA 49 F7 D9 4C 0B"));
	global::vars::g_add_font_lib = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("VERTEX", "E8 ? ? ? ? F3 0F 10 35 ? ? ? ? F3 0F 10 44 1D ? F3 0F 10 4C 1D ? 8B 47 28 41 0F 28 DB 41 0F 28"));
	global::vars::g_vertex_begin = memory::read_instruction(tmp, 1, 5);
	global::vars::g_vertex_add = memory::read_instruction(tmp + 0x4D, 1, 5); // good
	global::vars::g_vertex_end = memory::read_instruction(tmp + 0x5C, 1, 5); // good
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RST", "48 8B C4 48 89 58 10 55 56 57 41 54 41 56 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 C8 0F 29 78 B8 44 0F 29 40 ? 44 0F"));
	global::vars::g_render_script_texture = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("FT", "48 8B 05 ? ? ? ? 48 8B CB 48 69 C9 ? ? ? ? 40 38 AC 01 ? ? ? ? 74 0F E8 ? ? ? ? 0F 28 CE 8B CB E8 ? ? ? ? F3 0F 10 05 ? ? ? ? B2 01 E8"));
	global::vars::g_font_table = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("FMH", "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 0D ? ? ? ? 33 DB 48 85 C9 74 20 E8 ? ? ? ? 48 8B 0D"));
	global::vars::g_font_memory_helper = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GFIDT", "48 85 D2 74 10 49 83 C8 FF 49 FF C0 42 80 3C 02 ? 75 F6 EB 03 45 33 C0 E9"));
	global::vars::g_get_font_id_table = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("MH", "48 8B 0D ? ? ? ? 45 33 C0 48 8B 01 41 8D 50 20 FF 50 50 48 85 C0 74 0D 48 8B C8 E8"));
	global::vars::g_memory_heap = *(rage::types::memory_heap_pt**)(memory::read_instruction(tmp));
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SM", "48 8D 0D ? ? ? ? 03 D3 E8 ? ? ? ? 66 44 39 7D ? 74 09 48 8B 4D E8 E8"));
	global::vars::g_store_manager = (rage::types::store_manager*)(memory::read_instruction(tmp));
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("D3DDEV", "48 8D 05 ? ? ? ? 33 D2 48 89 44 24 ? 48 8D 05 ? ? ? ? 48 8B CB 48 89 44 24 ? 48 8D 05 ? ? ? ? 48 89 44 24"));
	global::vars::g_d3d11_device = *(uint64_t*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GRCTFDX", "48 8B 0D ? ? ? ? 45 33 C0 48 8B 01 33 D2 FF 90 ? ? ? ? 48 8B 0D ? ? ? ? 83 64 24"));
	global::vars::g_grc_texture_factory_dx11 = *(uint64_t*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("TLSA", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 63 FA 48 8B D9 E8 ? ? ? ? C7 43 ? ? ? ? ? 48 8D 05 ? ? ? ? 48 89 03"));
	global::vars::g_thread_alloc = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SD", "48 8D 05 ? ? ? ? 33 D2 48 89 44 24 ? 48 8D 05 ? ? ? ? 48 8B CB 48 89 44 24 ? 48 8D 05 ? ? ? ? 48 89 44 24"));
	global::vars::g_setup_dictionary = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SSI", "E8 ? ? ? ? 8B 00 48 8B 0D ? ? ? ? 48 63 D0 0F AF 05 ? ? ? ? 44 0F B6 04 0A 48 63 D0 41 FF C7 48 03 15"));
	global::vars::g_setup_store_item = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("IVPS", "45 33 D2 4C 8B D9 85 D2 78 3A 48 8B 41 40 4C 63 C2 46 0F B6 0C 00 8B 41 4C 41 81 E1 ? ? ? ? 45 8B C1 0F AF"));
	global::vars::g_is_valid_pool_slot = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("ADTP", "89 54 24 10 48 83 EC 28 48 8B 41 40 4C 63 CA 46 0F B6 14 08 8B 41 4C 41 81 E2 ? ? ? ? 45 8B CA 0F AF C2"));
	global::vars::g_add_dictionary_to_pool = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("AGRCTD", "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 0F B7 41 28 45 33 D2 8B DA 4C 8B C9 49 8B F8 41 8B CA 8B D0 41 8D 72 01"));
	global::vars::g_add_grc_texture_to_dictionary = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("WEATHER", "48 8D 0D ? ? ? ? E8 ? ? ? ? 39 05 ? ? ? ? 74 0B 39 05"));
	global::vars::g_get_weather_id = memory::read_instruction(tmp + 7, 1, 5);
	global::vars::g_weather_table = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan_ex(socialclub, "SCGIT", "48 8D 05 ? ? ? ? 48 03 F8 44 8B 47 14 48 8D 57 20 E8 ? ? ? ? 85"));
	global::vars::g_sc_game_info_table = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan_ex(socialclub, "SCGGIT", "E8 ? ? ? ? 85 C0 78 27 3B 47 14 7D 1E 48 98 48 8B D6 48 69 C8"));
	global::vars::g_sc_get_game_info_table = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GSS", "48 8D 0D ? ? ? ? E8 ? ? ? ? 83 38 FF 74 2D"));
	global::vars::g_game_streamed_scripts = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GSS_G", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B EA 48 8B F1 49 8B D0 B9 ? ? ? ? 49 8B F8 E8"));
	global::vars::g_get_streamed_script = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("CPF", "48 8B 05 ? ? ? ? 45 ? ? ? ? 48 8B"));
	global::vars::g_ped_factory = *(rage::types::ped_factory**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GCP", "4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 11"));
	global::vars::g_global_cache = (uint64_t**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("WCHL", "8B 05 ? ? ? ? 44 8B D3 8D 48 FF 85 C9 78 35 46"));
	global::vars::g_weapon_components = { memory::read_instruction(tmp, 2, 6), memory::read_instruction(tmp + 0x14) }; // good
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("WIHL", "44 0F B7 05 ? ? ? ? 44 8B DD 41 FF C8 78 28 48 8B 1D"));
	global::vars::g_weapon_info = { memory::read_instruction(tmp, 4, 8), memory::read_instruction(tmp + 0x10) }; // good
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GCA", "48 8B 05 ? ? ? ? 4A 8B 1C F0 48 85 DB 0F 84"));
	global::vars::g_game_camera_angles = (rage::network::game_camera_angles*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GTPC", "E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B D3 48 8B F8 E8 ? ? ? ? 48 85 FF 74 3B 48 8B 17 48 8B CF FF 52"));
	global::vars::g_get_third_person_camera = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("TPCP", "48 8B 05 ? ? ? ? 8B A8 ? ? ? ? E8 ? ? ? ? 48 8B F8"));
	global::vars::g_third_person_camera = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GEA", "E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 48 8B 0D ? ? ? ? 8B D7 E8 ? ? ? ? 48 8B F8 48 85 C0"));
	global::vars::g_get_entity_address = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GEH", "48 F7 F9 49 8B 48 08 48 63 D0 C1 E0 08 0F B6 1C 11 03 D8"));
	global::vars::g_get_entity_handle_from_address = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GCNGPFI", "48 83 EC 28 33 C0 38 05 ? ? ? ? 74 0A 83 F9 1F 77 05 E8 ? ? ? ? 48 83 C4 28"));
	global::vars::g_get_net_game_player_from_index = tmp;
	tmp = NULL;


	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GMI", "E8 ? ? ? ? 0F B7 45 58 66 89 45 50 8B 45 50 41 0B C7 41 23 C6 0F"));
	global::vars::g_get_model_info = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SVG", "8B 91 ? ? ? ? F3 0F 10 05 ? ? ? ? 8D 42 FD A9"));
	global::vars::g_set_vehicle_gravity = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GVPA", "4C 8B 0D ? ? ? ? 45 33 C0 4D 85 C9 75 03 33 C0 C3"));
	global::vars::g_get_vehicle_paint_array = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("DOI", "48 83 EC 28 45 8B C8 44 8B C2 8B D1 48 8D 0D ? ? ? ? E8 ? ? ? ? 8B 05"));
	global::vars::g_draw_origin_index = memory::read_instruction(tmp, 2, 6);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("ATCT", "E8 ? ? ? ? 0F B7 45 58 66 89 45 50 8B 45 50 41 0B C7 41 23 C6 0F"));
	global::vars::g_add_to_clock_time = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("DST", "48 8D 3D ? ? ? ? 80 3B 00 76 12 48 8B 0F 48 85 C9 74 0A 48 8B 01 FF 50 10 84 C0 75 3C"));
	global::vars::g_dispatch_service_table = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GHNGP", "E8 ? ? ? ? 33 C9 48 85 C0 74 2A 44 8D 41 01 66 89 4C 24 ? 48 89 4C 24 ? 4C 8D 4C 24 ? 48"));
	global::vars::g_get_host_net_game_player = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SISE", "48 8B 8B ? ? ? ? E8 ? ? ? ? 8B D7 B9 ? ? ? ? 4C 8B C0 E8"));
	global::vars::g_send_increment_stat_event = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("UNOO", "48 83 60 ? ? 48 8D 05 ? ? ? ? 48 89 03 EB 02 33 DB 48 85 DB 0F 84 ? ? ? ? 48"));
	global::vars::g_update_net_object_owner = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NOM", "48 8B 0D ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 4C 8B 13 4C 8D 05 ? ? ? ? 48"));
	global::vars::g_network_object_manager = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NPM", "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 0D ? ? ? ? E8"));
	global::vars::g_network_player_manager = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("HROFV", "45 33 C9 45 85 C0 74 56 41 FF C8 74 4C 41 FF C8 74 42 41 FF C8 74 38 41 FF C8 74 2E 41 FF"));
	global::vars::g_handle_rotation_values_from_order = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GNGPFH", "E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B F0 E8 ? ? ? ? 33 FF 48"));
	global::vars::g_get_net_game_player_from_handle = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GST", "48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 4C 8B C1 48 8D 54 24 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B D8 83"));
	global::vars::g_get_sprite_texture = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("STMNM", "48 89 9C 24 ? ? ? ? E8 ? ? ? ? 48 8D 8C 24 ? ? ? ? 48 8B D8 E8 ? ? ? ? 48 8D 94 24 ? ? ? ? 0F 10"));
	global::vars::g_send_text_message_net_msg = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("STL", "40 53 48 83 EC 30 80 3D ? ? ? ? ? 0F 84 ? ? ? ? 48 8D 1D ? ? ? ? 4C 8D 0D ? ? ? ? 4C 8D 05"));
	global::vars::g_set_traffic_lights = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("UTL", "48 8B C4 48 89 58 08 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 83 3D"));
	global::vars::g_update_traffic_lights = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("STLC", "48 8B C4 48 89 58 08 55 48 8D 68 B8 48 81 EC ? ? ? ? 0F 28 05 ? ? ? ? 0F 28 2A 0F 29 70 E8"));
	global::vars::g_set_traffic_light_colors = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SVLC", "48 89 5C 24 ? 57 48 83 EC 20 80 3D ? ? ? ? ? 0F 84 ? ? ? ? 48 8D 3D ? ? ? ? 48 8D 1D"));
	global::vars::g_set_vehicle_lights = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GSTFCT", "0F B7 CA 83 F9 07 7F 5E 74 54 85 C9 74 48 FF C9 74 3C FF C9 74 30"));
	global::vars::g_get_sync_tree_from_clone_type = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GNOFNI", "48 89 5C 24 ? 48 89 6C 24 ? 66 89 54 24 ? 56 57 41 56 48 83 EC 20 45"));
	global::vars::g_get_network_object_from_network_id = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RBFBB", "48 89 5C 24 ? 57 48 83 EC 20 83 64 24 ? ? 48 8B FA BB ? ? ? ? 48 8D 54 24 ? 44 8B C3 E8"));
	global::vars::g_read_bool_from_bit_buffer = tmp;
	global::vars::g_read_uint_from_bit_buffer = memory::read_instruction(tmp + 0x1F, 1, 5); // good
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RIFBB", "E8 ? ? ? ? 48 8B 4C 24 ? 48 8D 53 34 41 B8 ? ? ? ? E8 ? ? ? ? 40 84 FF 74 1E 8B 0E E8")); //good
	global::vars::g_read_int_from_bit_buffer = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RV3FBB", "48 8B C4 48 89 58 08 48 89 70 10 57 48 83 EC 60 48 8B F2 48 8B F9 48 8D 50 C8 48 8D 48 E8 41 8B D9 E8")); //good
	global::vars::g_read_vector3_from_bit_buffer = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RAFBB", "48 89 5C 24 ? 57 48 83 EC 30 41 8B F8 4C 8B D2 48 8B D9 45 85 C0 74 4F 8A 41 1C A8 02 75 48 A8")); //good
	global::vars::g_read_array_from_bit_buffer = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RSFBB", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B D9 33 C9 41 8B F8 8A 43 1C 48 8B F2 A8 02 75 45 A8 01 74 05")); //good
	global::vars::g_read_short_from_bit_buffer = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RULLFBB", "48 8B C4 48 89 58 08 56 57 41 56 48 83 EC 20 41 8B F8 33 DB 41 B8 ? ? ? ? 4C 8B F2 48 8B F1 89 58 18 41 3B F8 7E 38 48 8D 50 20 89 58 20 E8"));
	global::vars::g_read_ulonglong_from_bit_Buffer = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RNMHFBB", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 8A 42 1C 40 32 FF 48 8B DA 48 8B F1 A8 02 0F 85 ? ? ? ? A8 01 74 05 8B 4A 0C EB 03 8B 4A 14 8B 42 10 83"));
	global::vars::g_read_msg_header_from_bit_buffer = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("WUITBB", "48 89 5C 24 ? 44 8B DA 83 CB FF 4D 63 D1 49 C1 FA 03 41 83 E1 07 4C 03 D1 B9 ? ? ? ? 41 2B C8 D3 E3 41 D3 E3 41 8B C9 8B D3 41 8B C3 C1 EA"));
	global::vars::g_write_uint_to_bit_buffer = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RPFBB", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 01 48 8D 71 0C 48 8B FA 66 83 3E 08 48 8B D9 0F 93 44 24 ? FF 50 10 48")); //good
	global::vars::g_read_pickup_from_bit_buffer = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GCFP", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 48 8B D9 48 81 C1 ? ? ? ? 48 8B EA 33 FF E8 ? ? ? ? 48 3B E8 75 09 48 8D BB ? ? ? ? EB 3D 8B F7 39 BB")); //good
	global::vars::g_get_network_config_from_peer = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NER", "48 8B 0D ? ? ? ? 4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 48 8B 0D ? ? ? ? 4C 8D 0D"));
	global::vars::g_network_event_registration = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("WAYP", "48 8D 0D ? ? ? ? 48 98 4C 8B C6 41 8B 44 85 ? 0D ? ? ? ? 89 44 24 30 83 64 24 ? ? C6 44 24"));
	global::vars::g_waypoint_data = (rage::types::waypoint_data*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NSQ", "48 8B 0D ? ? ? ? 41 8A D9"));
	global::vars::g_net_shop_queue = *(rage::network::net_shop_queue**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("UUIV1", "48 83 EC 18 48 8B 0D ? ? ? ? 4C 8D 05 ? ? ? ? F3 0F 10 89 ? ? ? ? F3 0F 10 81 ? ? ? ? F3 0F 10 99 ? ? ? ? 0F 14 D9"));
	global::vars::g_update_ui_values = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("UUIV2", "48 8B C4 53 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 0F 29 70 E8 0F 29 78 D8 48 8B D9 44 0F 29 40 ? 44 0F 29 48 ? 44")); //good
	global::vars::g_update_ui_values_2 = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("UIVS", "48 8D 0D ? ? ? ? 0F 29 44 24 ? E8 ? ? ? ? 0F 28 05 ? ? ? ? 0F 59 44 24 ? F3 48 0F 2C C0 0F C6 C0 55")); //good
	global::vars::g_ui_visual_settings = (rage::types::ui_visual_settings*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SVNUI", "40 53 48 83 EC 30 F3 0F 10 15 ? ? ? ? 48 8D 15 ? ? ? ? 41 B1 01 48 8B D9 E8 ? ? ? ? 48 8D 15 ? ? ? ? F3 0F"));
	global::vars::g_set_vehicle_neon_ui = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("VFXL", "48 8D 05 ? ? ? ? 4C 8D 1C 7F 48 8B 4A 10 49 C1 E3 05 4C 03 D8 0F B6 85 ? ? ? ? 48 23 C8 8B 42 18 48 3B"));
	global::vars::g_vfx_liquid = (rage::types::vfx_liquid*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("HVBO", "40 53 48 83 EC 20 8A D9 8A D1 48 8D 0D ? ? ? ? 45 33 C0 E8 ? ? ? ? 88 1D ? ? ? ? 48 83 C4 20"));
	global::vars::g_set_seethrough = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NS_ABTQ", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 30 45 8B F9 41 8B E8 4C 8B F2 48 8B F1 33 DB E8 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 33 D2"));
	global::vars::g_add_basket_to_queue = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NS_AITB", "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B F2 48 8D 54 24")); //good
	global::vars::g_add_item_to_basket = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SSIR", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 30 48 83 3D ? ? ? ? ? 8B")); //good
	global::vars::g_send_session_info_request = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RSINFO", "48 8D 05 ? ? ? ? 4D 69 C0 ? ? ? ? 4C 03 C0 41 83 B8 ? ? ? ? ? 0F 86 ? ? ? ? 83 CD FF 48 8D 15"));
	global::vars::g_rs_info = (rage::network::rs_info*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GSIFGS", "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B F2 33 D2 49 8B F8 4C 8B F1 44 8D 42 7C 48 8D 4C 24 ? 41 8A E9 C6 44 24 ? ? E8"));
	global::vars::g_get_session_info_from_gs = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SNER", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 57 41 56 41 57 48 83 EC 60 4C 8B F9 48 8D 48 C8 49 8B F1 49"));
	global::vars::g_send_network_event_reply = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SNER", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 57 41 56 41 57 48 83 EC 60 4C 8B F9 48 8D 48 C8 49 8B F1 49"));
	global::vars::g_send_network_event_reply = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SNEA", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 8B 6C 24 60 8B 74 24 68 49 8B C1 4D 8B D0"));
	global::vars::g_send_network_event_ack = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SGCE", "4C 8B DC 49 89 5B 08 49 89 73 10 49 89 7B 18 41 56 48 83 EC 40 48 8B F1 48 8B C2 49 89 53 F0 48 8D 0D")); //good
	global::vars::g_setup_give_control_event = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SFR", "80 3D ? ? ? ? ? 75 03 32 C0 C3 4C 8B CA 8B 15 ? ? ? ? 4C 8B C1 48 8D 0D ? ? ? ? E9")); //good
	global::vars::g_send_friend_request = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GCPK", "E8 ? ? ? ? 89 03 85 C0 78 21 8B 13 48 8B 8F ? ? ? ? 4C 8D 84 24 ? ? ? ? 41 B9 ? ? ? ? 4C 89 6C 24 ? E8")); //good
	global::vars::g_get_client_player_key = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RWOM", "F3 0F 10 05 ? ? ? ? 0F 57 C9 0F 2F C1 76 1B 48 8B 05 ? ? ? ? 0F 2F 48 70 73 0E 48 8D 0D ? ? ? ? 33"));
	global::vars::g_render_water_on_map_intensity = memory::read_instruction(tmp, 4, 8);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("MFI", "0F 2F 35 ? ? ? ? 0F 86 ? ? ? ? 0F 28 05 ? ? ? ? 48 8D 4D B0 41 0F 28 D9 48 8B D3 0F 28 D7 0F 29 45 A0"));
	global::vars::g_map_fog_intensity = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("VFXW", "48 8D 05 ? ? ? ? 48 6B FF 45 F3 0F 59 0D ? ? ? ? F3 41 0F 59 9E ? ? ? ? F3 0F 10 BD ? ? ? ? 48 03 FE 48 69 FF ? ? ? ? F3"));
	global::vars::g_vfx_wheel = { (rage::types::vfx_wheel*)memory::read_instruction(tmp), *(uint8_t*)(tmp + 0xA) }; // good
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("FL", "48 03 0D ? ? ? ? E8 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 84 C0 74 10 48 8D 4C 24 ? 44 8B")); //good
	global::vars::g_friends = *(rage::network::friends**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SPE", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 81 EC ? ? ? ? 48 8B EA 44 8B F1 49 8B D9 41")); //good
	global::vars::g_send_presence_event = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("MSH", "48 89 5C 24 ? 57 48 83 EC 50 48 8B FA 48 8B D9 E8 ? ? ? ? 83 7B 10 08")); //good
	global::vars::g_migrate_script_host = tmp;
	tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("HPBR", "E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 83 FB 01 75 2D 8B 45 7F 48 8D 55 CF 48 8D 4D EF 44 8B CE"));
	//global::vars::g_is_reported_for_reason = memory::read_instruction(tmp, 1, 5);
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("CHATINF", "4C 8B 05 ? ? ? ? 4D 85 C0 74 E4 83 CA FF 48 8D 05 ? ? ? ? 4C 8D 4B 70 89 54 24 48 89 54 24 40 48 89 44 24")); //good
	//global::vars::g_chat_info = memory::read_instruction(tmp);
	//tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("TEXS", "48 8D 0D ? ? ? ? E8 ? ? ? ? 8B 45 EC 4C 8D 45 F0 48 8D 55 EC 48 8D 0D ? ? ? ? 89 45 F0 E8")); //good
	global::vars::g_texture_store = (rage::types::texture_store*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SMMR", "48 8B C4 48 89 58 18 44 89 48 20 89 50 10 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81")); //good
	global::vars::g_send_matchmaking_request = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RXMLS", "E8 ? ? ? ? 48 85 C0 74 52 83 78 40 00 74 06 48 8B 40 38 EB 02 33 C0"));
	global::vars::g_read_xml_string = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RXMLNS", "E8 ? ? ? ? 48 89 45 38 48 85 C0 74 16 48 8D 55 38 48 8D 8F ? ? ? ? 41 B8")); //good
	//global::vars::g_read_xml_node_string = memory::read_instruction(tmp, 1, 5);
	//tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RYTD", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 54 41 56 41 57 48 83 EC 50 48 8B EA 4C 8B FA 48 8B D9 4D 85 C9 B2")); //good
	global::vars::g_register_streaming_file = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("CGIP", "48 89 5C 24 ? 57 48 83 EC 20 48 8B DA 48 8B F9 E8 ? ? ? ? 48 8B 43 60 48 89 47 60 48 8B 43 68 48 89 47 68 0F 10 43")); //good
	global::vars::g_create_game_invite_presence = tmp;
	tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SGIP", "48 89 5C 24 ? 57 48 81 EC ? ? ? ? 48 8B FA 48 8B D9 E8 ? ? ? ? 8B 83 ? ? ? ? 4C 8D 4B 78 85 C0 74 2E C7 44 24")); //good
	//global::vars::g_send_game_invite_presence = tmp;gpt
	// 
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SGIPT", "48 8D 05 ? ? ? ? 48 8D 3D ? ? ? ? 48 8D 4C 24 ? 48 69 D2 ? ? ? ? 48 03 D0 48 89 7C 24")); //good
	//global::vars::g_send_game_invite_presence_table = memory::read_instruction(tmp);
	//tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SRE", "40 53 48 83 EC 20 0F B7 D9 48 8B 0D ? ? ? ? B2 01 E8 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B C8 33 C0 48 85 C9 74 08 0F B7 D3 E8 ? ? ? ? 48 8B 0D")); //good
	global::vars::g_send_ragdoll_event = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_1", "48 8B 07 48 8B CF FF 90 ? ? ? ? 84 C0 74 07 41 C7 06 ? ? ? ? 41 39 36 76 5E 4C 8B 17 45 33 C9 48")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 11 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_2", "33 F6 39 75 00 0F 86 ? ? ? ? 48 8B 03 45 33 C9 44 8B FE 45 8D 41 03 48 8B CB 4D 6B FF 64 4D 8D 34 3F 49")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 2 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_3", "48 8B 07 48 8B CF FF 90 ? ? ? ? 84 C0 74 07 41 C7 06 ? ? ? ? 44 38 23 74 14 48 8B 07 48 8B CF FF 90")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 10 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_4", "48 8B 06 4C 8D B5 ? ? ? ? 49 8B D6 45 33 C0 48 8B CE FF 50 38 48 8B 06 48 8B CE FF 90 ? ? ? ? 33")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 32 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_5", "45 8B F4 45 39 27 76 7C 48 8B 07 45 33 C9 48 8D 96 ? ? ? ? 41 8B DE 45 8D 41 20 48 8B CF 48 8D 14")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 11 });
	global::vars::g_return_address_node_iterations.push_back({ tmp + 0xBB, 3 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_6", "48 8B 06 48 8B CE FF 90 ? ? ? ? 33 DB 84 C0 74 06 C7 07 ? ? ? ? 39 1F 76 21 4C 8B 0E 48 8D 95 ? ? ? ? 45 33 C0")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 32 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_7", "E8 ? ? ? ? 48 8B 07 48 8D 9E ? ? ? ? 48 8B D3 45 33 C0 48 8B CF FF 50 38 45 33 FF 44 38 3B 75 1C 48 8B 07")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 30 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_8", "48 8B 06 45 33 C9 4D 8D AE ? ? ? ? 45 8D 41 08 49 8B D5 48 8B CE FF 50 68 33 ED 4D 8D A6 ? ? ? ? 41")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 3 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_9", "33 ED 41 39 2E 76 2D 48 8D 96 ? ? ? ? 45 33 C0 48 8B CF 48 8D 14 AA E8 ? ? ? ? C6 84 35 ? ? ? ? ? C6 84 35")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 105 });
	global::vars::g_return_address_node_iterations.push_back({ tmp + 0x4D, 65 }); // good
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_10", "B8 ? ? ? ? 41 39 06 41 0F 42 06 33 ED 41 89 06 85 C0 74 7A 44 8B FD 45 33 C0 48 8B CF")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 10 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_11", "44 8B E7 41 39 3F 76 66 48 8B 06 45 33 C9 41 8B FC 49 8D 96 ? ? ? ? 48 8D 1C 7F 45 8D 41 03")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 3 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_12", "45 8B F7 44 39 3B 76 24 4C 8B 17 45 33 C9 41 8B D6 48 83 C2 4A 45 8D 41 20 48 8B CF 48 8D 14 96 41 FF 52 68 41 FF C6 44 3B 33 72 DC 48 8B")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 11 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_13", "48 8B 07 48 8B CF FF 90 ? ? ? ? 84 C0 74 06 C7 03 ? ? ? ? 41 BF ? ? ? ? 44 38 65 00 75 33 45 38 26 75 2E 48 8B 07 48 8B CF")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 10 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NODES_14", "45 33 E4 41 8B EC 45 39 26 76 44 48 8B 07 44 8B FD 45 33 C0 49 8D 1C 37 48 8B CF 48 8D 93 ? ? ? ? FF 50 38 44 38 A3 ? ? ? ? 74 1A 48 8B 07")); //good
	global::vars::g_return_address_node_iterations.push_back({ tmp, 16 });
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GCHP", "48 8B 0D ? ? ? ? 4C 8D 44 24 ? 48 8D 95 ? ? ? ? 48 89 85 ? ? ? ? E8 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 8D")); //good
	global::vars::g_global_chat_ptr = *(uint64_t*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("APCM", "4D 85 C9 0F 84 ? ? ? ? 48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 48")); //good
	global::vars::g_add_chat_message = tmp;
	global::vars::g_hack_patches["CHAT"].m_address = tmp + 0x211;
	global::vars::g_hack_patches["CHAT"].m_patch = { 0xBF, 0x01, 0x00, 0x00, 0x00 };
	global::vars::g_hack_patches["CHAT"].read(5);
	tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("PRESFIX", "E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B 85 ? ? ? ? 8B 9D")); //good
	//global::vars::g_hack_patches["JOINP"].m_address = tmp;
	//global::vars::g_hack_patches["JOINP"].m_patch = { 0xB0, 0x01, 0x90, 0x90, 0x90 };
	//global::vars::g_hack_patches["JOINP"].read(5);  
	//tmp = NULL;


	////// ^^^ BORKED IN 1.61 BUT IN 1.54.....

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GCHATS", "48 8B 05 ? ? ? ? 44 38 60 14 75 06 44 39 60 04 74 10 33 C9 E8 ? ? ? ? 83 B8")); //good
	global::vars::g_chat_config = memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GPIFNM", "44 8B CA 4C 8B C1 0F B7 C2 85 D2 78 31 3B 81 ? ? ? ? 73 29 8A 89 ? ? ? ? 8B D0 48 D3 EA 49 8B 88")); //good
	global::vars::g_get_player_info_from_net_msg = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GDROPS", "48 8D 2D ? ? ? ? 0F 2F C6 0F 97 C0 88 44 24 40 84 C0 74 4A 8B 0D ? ? ? ? 83 CB FF 44")); //good
	global::vars::g_drop_shaders = memory::read_instruction(tmp);
	global::vars::g_drop_shader_count = memory::read_instruction(tmp + 0x15, 2, 6); // good
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("GNMNP", "E8 ? ? ? ? 4C 8B E8 48 85 C0 0F 84 ? ? ? ? 48 8B C8 E8 ? ? ? ? 84 C0 0F 84")); //good
	global::vars::g_get_net_msg_net_player = memory::read_instruction(tmp, 1, 5);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("3DPED", "4C 8B 15 ? ? ? ? F3 0F 11 45 ? F3 0F 10 05 ? ? ? ? BF ? ? ? ? BB ? ? ? ? 41 BE ? ? ? ? F3 0F 11")); //good
	global::vars::g_ui_3d_draw_manager = memory::read_instruction(tmp);
	global::vars::g_push_scene_preset_to_manager = memory::read_instruction(tmp + 0x5D, 1, 5);// good
	global::vars::g_add_element_to_scene = memory::read_instruction(tmp + 0x9F, 1, 5);// good
	global::vars::g_set_scene_element_lighting = memory::read_instruction(tmp + 0xE4, 1, 5);// good
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("INVALIDS", "E8 ? ? ? ? 41 B8 ? ? ? ? 48 8B D7 48 8B C8 E8 ? ? ? ? EB 07 48 8D 05 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 20 5F C3")); //good
	global::vars::g_invalid_string_address = memory::read_instruction(tmp + 0x18);// good
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("BB_RTOK", "48 89 5C 24 ? 55 56 57 48 83 EC 20 33 C0 48 8B EA 48 8B D9 8D 70 01 C6 44 24 ? ? 89 44 24 51 66 89 44 24 ? 88")); //good
	global::vars::g_read_token_from_bit_buffer = tmp;
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("BB_RST", "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC 20 48 8B F2 45 8B F0 32 DB 88 1A 48 8D 54 24 ? 41 B8 ? ? ? ? 48 8B")); //good
	global::vars::g_read_string_from_bit_buffer = tmp;
	tmp = NULL;




	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SPECP", "48 89 5C 24 ? 57 48 83 EC 20 8B D9 E8 ? ? ? ? 84 C0 75 6A 8B CB E8")); //good
	//patches patch;
	//patch.m_address = tmp + 0xC;
	//patch.read(5);
	//global::vars::g_patches.push_back(patch);
	//memory::write_vector(tmp + 0xC, { 0xB0, 0x00, 0x90, 0x90, 0x90 });
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("PMSP", "40 53 48 83 EC 20 E8 ? ? ? ? 48 8B D8 48 85 C0 74 12")); //good
	//patches patch2;
	//patch2.m_address = tmp;
	//patch2.read(2);
	//global::vars::g_patches.push_back(patch2);
	//memory::write_vector(tmp, { 0xEB, 0x02 });
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("MSP", "48 85 C0 0F 84 ? ? ? ? 8B 48 50")); //good
	//patches patch3;
	//patch3.m_address = tmp;
	//patch3.read(24);
	//global::vars::g_patches.push_back(patch3);
	//memory::nop(tmp, 24);
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RSTRB", "40 22 F0 44 3B 77 20")); //good
	//patches patch4;
	//patch4.m_address = tmp;
	//patch4.read(3);
	//global::vars::g_patches.push_back(patch4);
	//memory::nop(tmp, 3);
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RSTEB", "40 22 F8 3B 73 44")); //good
	//patches patch5;
	//patch5.m_address = tmp;
	//patch5.read(3);
	//global::vars::g_patches.push_back(patch5);
	//memory::nop(tmp, 3);
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NARRCF3", "48 8B 0D ? ? ? ? 48 85 C9 74 08 48 8B 01 B2 01 FF 50 18 0F B7 56 0A 41 B0 01 48 8B CD E8 ? ? ? ? 33 C9")); //good
	//patches patch7;
	//patch7.m_address = tmp;
	//patch7.read(3);
	//global::vars::g_patches.push_back(patch7);
	//memory::write_vector(tmp, { 0xE9, 0x7C, 0x01, 0x00, 0x00 }); // cherax crash fix
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("CKL", "E8 ? ? ? ? 48 8B 56 08 84 C0 74 59 48 FF C3 44 38 3C 1A 75 F7 48 8D 4D 00 4C 8B C3 E8 ? ? ? ? 48 8D 4D 48 E8")); //good
	//patches patch8;
	//patch8.m_address = memory::read_instruction(tmp, 1, 5);
	//patch8.read(3);
	//global::vars::g_patches.push_back(patch8);
	//memory::write_vector(memory::read_instruction(tmp, 1, 5), { 0xB0, 0x00, 0xC3 });
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("CKK", "E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 48 8B 56 08 4C 8B C3 49 FF C0 46 38 3C 02 75 F7 48 8D 4D 00 E8 ? ? ? ? 48 8D 4D 00 E8 ? ? ? ? 48 8B 56 08 84 C0 0F 84")); //good
	//patches patch82;
	//patch82.m_address = memory::read_instruction(tmp, 1, 5);
	//patch82.read(3);
	//global::vars::g_patches.push_back(patch82);
	//memory::write_vector(memory::read_instruction(tmp, 1, 5), { 0xB0, 0x00, 0xC3 });
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("PMDN", "40 53 48 83 EC 20 48 8B 02 4C 8B C2 48 8B D9 48 8B D1 49 8B C8 FF 50 58 B8")); //good
	//patches patch9;
	//patch9.m_address = tmp;
	//patch9.read(1);
	//global::vars::g_patches.push_back(patch9);
	//memory::write_vector(tmp, { 0xC3 });
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SPH", "4C 8B 81 ? ? ? ? 4D 85 C0 0F 84 ? ? ? ? 41 8B 80 ? ? ? ? F3")); //good
	//patches patch10;
	//patch10.m_address = tmp;
	//patch10.read(1);
	//global::vars::g_patches.push_back(patch10);
	//memory::write_vector(tmp, { 0xC3 });
	//global::vars::g_ped_orientation_writer = tmp;
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SEM", "40 53 48 83 EC 20 48 81 C1 ? ? ? ? 48 8B DA 48 8B 01 FF 90 ? ? ? ? 33 C9 48 85 C0 0F 84 ? ? ? ? 0F 28 48 60 0F")); //good
	//patches patch11;
	//patch11.m_address = tmp;
	//patch11.read(1);
	//global::vars::g_patches.push_back(patch11);
	//memory::write_vector(tmp, { 0xC3 });
	//global::vars::g_entity_orientation_writer = tmp;
	//tmp = NULL;

	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("TCRASH", "44 38 3D ? ? ? ? 74 0E B1 01 E8 ? ? ? ? 33 C9 E8")); //good
	//patches patch12;
	//patch12.m_address = tmp;
	//patch12.read(5);
	//global::vars::g_patches.push_back(patch12);
	//memory::nop(tmp, 5);
	//tmp = NULL;


	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("ARRC2", "E8 ? ? ? ? EB 61 E8 ? ? ? ? 48 8B 0D ? ? ? ? 8B 51 20 8B 41 10 C1 E2 02 C1 FA 02 2B C2 85 C0 7E 30 E8")); //good
	//patches patch13;
	//patch13.m_address = tmp;
	//patch13.read(5);
	//global::vars::g_patches.push_back(patch13);
	//memory::nop(tmp, 5);
	//tmp = NULL;


	//tmp = reinterpret_cast<decltype(tmp)>(memory::scan("UCRASH", "E8 ? ? ? ? 48 8B 03 8B 97 ? ? ? ? 48 8B CB FF 90 ? ? ? ? 0F B7 83 ? ? ? ? 48 8B CB")); //good
	//patches patch14;
	//patch14.m_address = tmp;
	//patch14.read(5);
	//global::vars::g_patches.push_back(patch14);
	//uint32_t jump_size = (uint32_t)((tmp + 0x1CF) - tmp - 5);
	//memory::write_vector(tmp, { 0xE9, (uint8_t)(jump_size & 0xFF), (uint8_t)(jump_size >> 8 & 0xFF), (uint8_t)(jump_size >> 16 & 0xFF), (uint8_t)(jump_size >> 24 & 0xFF) });
	//tmp = NULL;

	return successful;
}

bool globals_in_game() {
	bool successful = true;



	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("POOL_1", "48 8B 05 ? ? ? ? 8B 78 10 85 FF")); //good
	global::vars::g_object_pool = *(guid_pool**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("POOL_2", "48 8B 05 ? ? ? ? 41 0F BF C8 0F BF 40 10")); //good
	global::vars::g_ped_pool = *(guid_pool**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("POOL_3", "48 8B 0D ? ? ? ? 45 33 C9 44 8B C5 BA ? ? ? ? E8 ? ? ? ? 48")); //good
	global::vars::g_vehicle_pool = *(vehicle_pool**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("POOL_4", "48 8B 05 ? ? ? ? 0F B7 48 10 66 03 CA 66 44 03 C1 41 80 F9 04")); //good
	global::vars::g_pickup_pool = *(guid_pool**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("EXPD", "48 8D ? ? ? ? ? C1 EF 12 8B D0 89 85 88 00 00 00 40 80 E7 01 E8")); //good
	global::vars::g_explosion_data = *(rage::types::explosion_data**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("EXPFX", "48 8D 0D ? ? ? ? 41 0F 28 D9 45 33 C0 48 8B D3 E8 ? ? ? ? 44")); //good
	global::vars::g_explosion_fx = *(rage::types::explosion_fx**)(memory::read_instruction(tmp) + 0x10);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("NBC", "48 8B 0D ? ? ? ? 33 D2 E8 ? ? ? ? 84 C0 75 ? 48 8B 05 ? ? ? ? BA")); //good
	global::vars::g_network_base_config = *(rage::network::network_base_config**)(memory::read_instruction(tmp));
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("OQ", "40 53 48 83 EC 20 33 DB 44 8D 43 01 41 3B C8 75 60")); //good
	global::vars::g_ocean_quads.m_ocean = (rage::types::ocean_quad_info*)memory::read_instruction(tmp);
	global::vars::g_ocean_quads.m_calming = (rage::types::ocean_quad_info*)memory::read_instruction(tmp + 0xC);
	global::vars::g_ocean_quads.m_wave = (rage::types::ocean_quad_info*)memory::read_instruction(tmp + 0x25);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("WT", "4C 8D 0D ? ? ? ? 48 0F 45 D1 48 8B 0D ? ? ? ? 48 83 64 24 ? ? 4C 8D 05 ? ? ? ? E8 ? ? ? ? 84 C0 0F 85")); //good
	global::vars::g_water_tune = (rage::types::water_tune*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("VFX-5", "48 8D 0D ? ? ? ? 44 8A CE 44 8A C5 8B D3 44 89 64 24 ? 89 7C 24 20 E8 ? ? ? ? 8D")); //good
	global::vars::g_ui_weather = (rage::types::ui_weather*)(memory::read_instruction(tmp) + 0x60);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("VFX-P", "48 8B 05 ? ? ? ? 83 20 00 48 8B 0D ? ? ? ? 48 83 C4 28 E9")); //good
	global::vars::g_ui_puddle = *(rage::types::ui_puddle**)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("RESOL", "44 8B 05 ? ? ? ? 48 8D 0D ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 05 ? ? ? ? 48 0F 45 C1 44 39 2D ? ? ? ? 48 8D 0D")); //good
	uint64_t info = memory::read_instruction(tmp);
	if (info) {
		RECT desktop;
		GetWindowRect(GetDesktopWindow(), &desktop);
		global::vars::g_resolution = { *(int*)(info), *(int*)(info + 4) };
		global::vars::g_desktop_resolution = { (int)desktop.right, (int)desktop.bottom };
		LOG_WARN("Game Resolution: %ix%i, Screen Resolution: %ix%i",
			global::vars::g_resolution.x, global::vars::g_resolution.y,
			global::vars::g_desktop_resolution.x, global::vars::g_desktop_resolution.y);

	}
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("BL", "4C 8D 05 ? ? ? ? 0F B7 C1")); //good
	global::vars::g_blip_list = (rage::types::blip_list*)memory::read_instruction(tmp);
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("SHP", "0F B7 15 ? ? ? ? 45 33 F6 8B E9 85 C9 B8 ? ? ? ? 45 8D 7E 01 41 8A F6 41 8B DE 0F 44 E8")); //good
	global::vars::g_script_handlers = { memory::read_instruction(tmp), *(uint64_t*)(memory::read_instruction(tmp) - 8) };
	tmp = NULL;

	tmp = reinterpret_cast<decltype(tmp)>(memory::scan("INVPLY", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 49 8B D9 41 8B F8 48 8B F2 48 8B E9 4D 85 C9 75 16 48 8D 15 ? ? ? ? 48")); //good
	global::vars::g_invite_player = tmp;
	tmp = NULL;


	return successful;
}