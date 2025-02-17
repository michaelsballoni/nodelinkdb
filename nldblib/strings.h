#pragma once

#include "db.h"

namespace nldb
{
	class strings
	{
	public:
		static int64_t get_id(db& db, const std::wstring& str);
		static std::wstring get_val(db& db, int64_t id);
		static std::unordered_map<int64_t, std::wstring> get_vals(db& db, const std::vector<int64_t>& ids);
		static void flush_caches();
	};
}
