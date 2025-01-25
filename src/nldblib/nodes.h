#pragma once

#include "node.h"
#include "db.h"

namespace nldb 
{
	class nodes {
	public:
		static node create(db& db, int64_t parentNodeId, int64_t nameStringId, int64_t typeStringId = 0, const std::wstring& payload = L"");
		static void move(db& db, int64_t nodeId, int64_t newParentNodeId);
		static void remove(db& db, int64_t nodeId);

		static std::optional<node> get_node(db& db, int64_t nodeId);
		static std::optional<node> get_node_in_parent(db& db, int64_t parentNodeId, int64_t nameStringId);
		static std::optional<node> get_parent_node(db& db, int64_t nodeId);
		static std::vector<node> get_node_parents(db& db, int64_t nodeId);
		static std::vector<node> get_children(db& db, int64_t nodeId);

		static std::vector<node> get_path(db& db, const node& cur);
		static std::wstring get_path_str(db& db, const node& cur);
		static std::vector<node> get_path_nodes(db& db, const std::wstring& path);
	};
}
