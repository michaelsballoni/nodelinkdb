#pragma once

#include "db.h"

namespace nldb
{
	class props
	{
	public:
		static void setup(db& db);
		static void set(db& db, int64_t item_type_string_id, int64_t item_id, int64_t name_string_id, int64_t value_string_id); // use < 0 to delete
		static std::unordered_map<int64_t, int64_t> get(db& db, int64_t item_type_string_id, int64_t item_id);

		static std::map<std::wstring, std::wstring> fill(db& db, const std::unordered_map<int64_t, int64_t>& map);
		static std::wstring summarize(db& db, const std::unordered_map<int64_t, int64_t>& map);
	};
}
