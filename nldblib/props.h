#pragma once

#include "db.h"

namespace nldb
{
	class props
	{
	public:
		static void set(db& db, int64_t itemTypeStringId, int64_t itemId, int64_t nameStringId, int64_t valueStringId); // use < 0 to delete
		static std::unordered_map<int64_t, int64_t> get(db& db, int64_t itemTypeStringId, int64_t itemId);

		static std::map<std::wstring, std::wstring> fill(db& db, const std::unordered_map<int64_t, int64_t>& map);
		static std::wstring summarize(db& db, const std::unordered_map<int64_t, int64_t>& map);
	};
}
