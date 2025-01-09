#pragma once

#include "db.h"

namespace nldb 
{
	struct node {
		node(int64_t id, int64_t parent_id, int64_t name_string_id, int64_t type_string_id)
			: m_id(id)
			, m_parent_id(parent_id)
			, m_name_string_id(name_string_id)
			, m_type_string_id(type_string_id)
		{
		}

		const int64_t m_id;
		const int64_t m_parent_id;
		const int64_t m_name_string_id;
		const int64_t m_type_string_id;
	};

	class nodes {
	public:
		static void setup(db& db);
		static std::optional<node> get_node_in_parent(db& db, int64_t parentNodeId, int64_t nameStringId);
		static std::optional<node> get_parent_node(db& db, int64_t nodeId);
	};
}
