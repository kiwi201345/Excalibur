#pragma once
#include "stdafx.h"

namespace rage::invoker {
	struct native_registration {
		uint64_t m_next_registration1;
		uint64_t m_next_registration2;
		uint64_t m_handlers[7];
		uint32_t m_num_entries1;
		uint32_t m_num_entries2;
		uint64_t m_hashes;

		static uint64_t get_handler(uint64_t hash);
	};
}