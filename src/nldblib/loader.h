#pragma once

#include "db.h"
#include "node.h"

namespace nldb
{
	class loader {
	public:
		loader(db& db);
		void load_directory(const std::wstring& dirPath, const node& cur_node = node());

	private:
		db& m_db;
		std::unordered_map<std::wstring, int64_t> m_string_cache;
	};
}
