#include "pch.h"
#include "paths.h"
#include "strings.h"
#include "core.h"

using namespace nldb;

std::vector<node> paths::get_nodes(db& db, const std::wstring& path) {
	std::vector<node> output;

	std::vector<std::wstring> splits;
	std::wstring builder;
	for (wchar_t c : path) {
		if (c == '/') {
			splits.push_back(builder);
			builder.clear();
		}
		else
			builder += c;
	}
	if (!builder.empty())
		splits.push_back(builder);

	if (splits.empty())
		return output;
	else
		output.reserve(splits.size());

	int64_t cur_node_id = 0;
	for (size_t p = 0; p < splits.size(); ++p) {
		const std::wstring& part = splits[p];
		
		int64_t cur_name_string_id = strings::get_id(db, part);

		auto node_opt = nodes::get_node_in_parent(db, cur_node_id, cur_name_string_id);
		if (!node_opt.has_value())
			throw nldberr("Node not found: " + toNarrowStr(part));

		output.emplace_back(node_opt.value());

		cur_node_id = node_opt.value().m_id;
	}

	return output;
}

std::wstring paths::get_path(db& db, const node& cur) {
	std::vector<int64_t> path_str_ids;

	std::unordered_set<int64_t> seen_node_ids;
	seen_node_ids.insert(cur.m_id);

	node cur_node = cur;
	do
	{
		path_str_ids.push_back(cur_node.m_name_string_id);
		
		auto new_node_opt = nodes::get_parent_node(db, cur_node.m_id);
		if (!new_node_opt.has_value())
			break;
		cur_node = new_node_opt.value();

		if (seen_node_ids.find(cur_node.m_id) != seen_node_ids.end())
			break;
		else
			seen_node_ids.insert(cur.m_id);
	} while (cur_node.m_parent_id > 0);

	std::reverse(path_str_ids.begin(), path_str_ids.end());

	auto strs_map = strings::get_vals(db, path_str_ids);

	std::wstring path_str;
	for (int64_t path_str_id : path_str_ids) {
		path_str += L'/';
		path_str += strs_map[path_str_id];
	}
	return path_str;
}
