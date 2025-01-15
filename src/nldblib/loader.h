#pragma once

#include "db.h"
#include "node.h"

namespace nldb
{
	class loader {
	public:
		loader(db& db);
		void load_directory(const std::wstring& dirPath);
		void load_directory2(const std::wstring& dirPath, const node& cur_node = node());

	private:
		db& m_db;

		int64_t m_file_type_string_id;
		int64_t m_dir_type_string_id;

		std::unordered_map<std::wstring, int64_t> m_string_cache;
		std::unordered_map<std::string, node> m_parent_node_cache;
	};
}
