#pragma once

#include "nodes.h"

namespace nldb
{
	struct path_component {
		int64_t name_string_id;
		int64_t node_id;
	};

	class paths
	{
	public:
		static std::vector<node> get_nodes(db& db, const std::wstring& path);
		static std::wstring get_path(db& db, const node& cur);

		static void flush_caches();
	};
}
