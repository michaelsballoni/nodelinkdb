#pragma once

#include "link.h"
#include "db.h"

namespace nldb
{
	class links {
	public:
		static void setup(db& db);

		static link create(db& db, int64_t fromNodeId, int64_t toNodeId, int64_t typeStringId);

		static std::optional<link> get_link(db& db, int64_t linkId);

		static std::vector<link> get_out_links(db& db, int64_t fromNodeId);
		static std::vector<link> get_in_links(db& db, int64_t toNodeId);
	};
}
