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

void loader::load_directory(const std::wstring& dirPath, const node& cur_node) 
{
	for (const auto& dir_entry : fs::directory_iterator(dirPath)) 
	{
		bool is_file = dir_entry.is_regular_file();
		bool is_dir = dir_entry.is_directory();

		fs::path cur_path = dir_entry.path();
		std::wstring name = cur_path.filename();

		int64_t name_string_id;
		{
			const auto str_cache_it = m_string_cache.find(name);
			if (str_cache_it == m_string_cache.end()) 
			{
				name_string_id = strings::get_id(m_db, name);
				m_string_cache.insert({ name, name_string_id });
			}
			else
				name_string_id = str_cache_it->second;
		}

		node new_node;
		auto node_in_path = nodes::get_node_in_parent(m_db, cur_node.m_id, name_string_id);
		if (!node_in_path.has_value()) 
		{
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
			load_directory(cur_path, new_node);
	}
}
