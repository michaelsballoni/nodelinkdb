#pragma once

#include "link.h"
#include "db.h"

namespace nldb
{
	class links {
	public:
		static link create(db& db, int64_t fromNodeId, int64_t toNodeId, int64_t typeStringId = 0, const std::optional<std::wstring>& payload = std::nullopt);
		static bool remove(db& db, int64_t fromNodeId, int64_t toNodeId, int64_t typeStringId = 0);

		static std::optional<link> get_link(db& db, int64_t linkId);

		static std::vector<link> get_out_links(db& db, int64_t fromNodeId);
		static std::vector<link> get_in_links(db& db, int64_t toNodeId);
	};
}
