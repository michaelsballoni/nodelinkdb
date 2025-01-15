#include "pch.h"
#include "loader.h"
#include "strings.h"
#include "nodes.h"

namespace fs = std::filesystem;

using namespace nldb;

loader::loader(db& db)
	: m_db(db)
{
	m_file_type_string_id = strings::get_id(db, L"file");
	m_dir_type_string_id = strings::get_id(db, L"dir");
}

void loader::load_directory(const std::wstring& dirPath) {
	std::vector<std::wstring> path_parts;

	for (const auto& dir_entry : fs::recursive_directory_iterator(dirPath)) {
#if _DEBUG
		printf("1: %S\n", dir_entry.path().wstring().c_str());
#endif
		bool is_file = dir_entry.is_regular_file();

		path_parts.clear();
		for (const std::wstring& path_part : dir_entry.path().relative_path())
			path_parts.push_back(path_part);

		node cur_node;
		for (size_t p = 0; p < path_parts.size(); ++p) {
			const std::wstring& path_part = path_parts[p];

			int64_t path_part_string_id;
			{
				const auto str_cache_it = m_string_cache.find(path_part);
				if (str_cache_it == m_string_cache.end()) {
					path_part_string_id = strings::get_id(m_db, path_part);
					m_string_cache.insert({ path_part, path_part_string_id });
				}
				else
					path_part_string_id = str_cache_it->second;
			}

			char parent_cache_key[100];
			sprintf(parent_cache_key, "%I64d_%I64d", cur_node.m_id, path_part_string_id);

			const auto parent_node_it = m_parent_node_cache.find(parent_cache_key);
			if (parent_node_it == m_parent_node_cache.end()) {
				auto node_in_path = nodes::get_node_in_parent(m_db, cur_node.m_id, path_part_string_id);
				if (!node_in_path.has_value())
					cur_node = nodes::create(m_db, cur_node.m_id, path_part_string_id, p == path_parts.size() - 1 && is_file ? m_file_type_string_id : m_dir_type_string_id);
				else
					cur_node = node_in_path.value();
				m_parent_node_cache.insert({ parent_cache_key, cur_node });
			}
			else
				cur_node = parent_node_it->second;
		}
	}
}

void loader::load_directory2(const std::wstring& dirPath, const node& cur_node) {
	for (const auto& dir_entry : fs::directory_iterator(dirPath)) {
#if _DEBUG
		printf("2: %S\n", dir_entry.path().wstring().c_str());
#endif
		bool is_file = dir_entry.is_regular_file();
		bool is_dir = dir_entry.is_directory();

		fs::path cur_path = dir_entry.path();
		std::wstring name = cur_path.filename();

		int64_t name_string_id;
		{
			const auto str_cache_it = m_string_cache.find(name);
			if (str_cache_it == m_string_cache.end()) {
				name_string_id = strings::get_id(m_db, name);
				m_string_cache.insert({ name, name_string_id });
			}
			else
				name_string_id = str_cache_it->second;
		}

		node new_node;
		auto node_in_path = nodes::get_node_in_parent(m_db, cur_node.m_id, name_string_id);
		if (!node_in_path.has_value()) {
			new_node =
				nodes::
				create
				(
					m_db, 
					cur_node.m_id, 
					name_string_id, 
					is_file ? m_file_type_string_id : m_dir_type_string_id
				);
		}
		else
			new_node = node_in_path.value();

		if (is_dir)
			load_directory2(cur_path.append(name), new_node);
	}
}
