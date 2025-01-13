#pragma once

#include "node.h"
#include "db.h"

namespace nldb 
{
	class nodes {
	public:
		static void setup(db& db);

		static node create(db& db, int64_t parentNodeId, int64_t nameStringId, int64_t typeStringId);
		static void move(db& db, int64_t nodeId, int64_t newParentNodeId);

		static std::optional<node> get_node(db& db, int64_t nodeId);
		static std::optional<node> get_node_in_parent(db& db, int64_t parentNodeId, int64_t nameStringId);
		static std::optional<node> get_parent_node(db& db, int64_t nodeId);

		static std::vector<node> get_path_nodes(db& db, const std::wstring& path);
		static std::wstring get_path_str(db& db, const node& cur);
	};
}
