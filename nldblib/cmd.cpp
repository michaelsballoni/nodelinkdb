#include "pch.h"
#include "cmd.h"
#include "links.h"
#include "loader.h"
#include "nodes.h"
#include "props.h"
#include "search.h"
#include "strings.h"

using namespace nldb;

cmd::cmd(db& db) : m_db(db)
{
	m_nodeItemTypeId = strings::get_id(db, L"node");
}

void cmd::mount(const std::wstring& dirPath)
{
	loader loader(m_db);
	loader.load_directory(dirPath, m_cur);
}

std::wstring cmd::get_cur_path()
{
	return nodes::get_path_str(m_db, m_cur);
}

node cmd::get_node_from_path(const std::wstring& path)
{
	if (path.empty())
		throw nldberr("Specify a path");

	if (path == L"/")
		return node();

	std::wstring new_cur_path;
	if (path[0] != '/') // relative
	{
		new_cur_path = nodes::get_path_str(m_db, m_cur);
		if ((new_cur_path.empty() || new_cur_path.back() != '/') && path.find('/') == std::wstring::npos)
			new_cur_path += '/';
		new_cur_path += path;
	}
	else // absolute
		new_cur_path = path;

	auto nodes_opt = nodes::get_path_nodes(m_db, new_cur_path);
	if (!nodes_opt.has_value() || nodes_opt.value().empty())
		throw nldberr("Path does not resolve to node: " + toNarrowStr(new_cur_path));
	else
		return nodes_opt.value().back();
}

void cmd::cd(const std::wstring& newPath)
{
	m_cur = get_node_from_path(newPath);
}

std::vector<std::wstring> cmd::dir()
{
	std::vector<std::wstring> paths;
	for (auto child : nodes::get_children(m_db, m_cur.m_id))
		paths.emplace_back(nodes::get_path_str(m_db, child));
	std::sort(paths.begin(), paths.end());
	return paths;
}

void cmd::mknode(const std::wstring& newNodeName)
{
	nodes::create(m_db, m_cur.m_id, strings::get_id(m_db, newNodeName));
}

void cmd::copy(const std::wstring& newParentNode)
{
	nodes::copy(m_db, m_cur.m_id, get_node_from_path(newParentNode).m_id);
}

void cmd::move(const std::wstring& newParentNode)
{
	nodes::move(m_db, m_cur.m_id, get_node_from_path(newParentNode).m_id);
}

void cmd::remove()
{
	int64_t orig_id = m_cur.m_id;
	auto parent = nodes::get(m_db, m_cur.m_parentId);
	nodes::remove(m_db, orig_id);
	m_cur = parent;
}

void cmd::rename(const std::wstring& newName)
{
	int64_t new_name_string_id = strings::get_id(m_db, newName);
	nodes::rename(m_db, m_cur.m_id, new_name_string_id);
	m_cur.m_nameStringId = new_name_string_id;
}

void cmd::set_prop(const std::vector<std::wstring>& cmds)
{
	if (cmds.size() < 2)
		throw nldberr("Specify the name of the property to set");
	else if (cmds.size() > 3)
		throw nldberr("Specify the name and value of the property to set");
	else if (cmds.size() == 2)
		props::set(m_db, m_nodeItemTypeId, m_cur.m_id, strings::get_id(m_db, cmds[1]), -1);
	else
		props::set(m_db, m_nodeItemTypeId, m_cur.m_id, strings::get_id(m_db, cmds[1]), strings::get_id(m_db, cmds[2]));
}

void cmd::set_payload(const std::wstring& payload)
{
	nodes::set_payload(m_db, m_cur.m_id, payload);
}

std::wstring cmd::tell()
{
	std::wstringstream stream;

	stream << L"ID:      " << m_cur.m_id << L"\n";
	stream << L"Name:    " << strings::get_val(m_db, m_cur.m_nameStringId) << L"\n";
	stream << L"Parent:  " << nodes::get_path_str(m_db, m_cur) << L"\n";

	auto payload_opt = nodes::get(m_db, m_cur.m_id).m_payload;
	stream << L"Payload: " << payload_opt.value_or(L"(not loaded)") << L"\n";

	auto prop_string_ids = props::get(m_db, m_nodeItemTypeId, m_cur.m_id);
	if (!prop_string_ids.empty())
	{
		stream << L"Properties:\n" << props::summarize(m_db, prop_string_ids) << L"\n";
	}
	else
		stream << L"Properties: (none)" << L"\n";

	auto out_links = links::get_out_links(m_db, m_cur.m_id);
	if (!out_links.empty())
	{
		stream << L"Out Links:" << L"\n";
		for (const auto& out_link : out_links)
			stream << nodes::get_path_str(m_db, nodes::get(m_db, out_link.m_toNodeId)) << L"\n";
	}
	else
		stream << L"Out Links: (none)" << L"\n";

	auto in_links = links::get_in_links(m_db, m_cur.m_id);
	if (!in_links.empty())
	{
		stream << L"In Links:" << L"\n";
		for (const auto& in_link : in_links)
			stream << nodes::get_path_str(m_db, nodes::get(m_db, in_link.m_fromNodeId)) << L"\n";
	}
	else
		stream << L"In Links:  (none)" << L"\n";

	return stream.str();
}

std::wstring cmd::search(const std::vector<std::wstring>& cmd)
{
	if (cmd.size() < 3)
		throw nldberr("Pass in name / value pairs to search properties with");
	
	if (((int)cmd.size() - 1) % 2)
		throw nldberr("Pass in evenly matched name / value pairs to search with");

	search_query query;
	for (size_t s = 1; s + 1 < cmd.size(); s += 2)
	{
		query.m_criteria.push_back
		(
			search_criteria(strings::get_id(m_db, cmd[s]), cmd[s + 1])
		);
	}
	std::wstring output;
	for (const auto& node : search::find_nodes(m_db, query))
	{
		if (!output.empty())
			output += '\n';
		output += nodes::get_path_str(m_db, node);
	}
	return output;
}

void cmd::link(const std::wstring& toPath)
{
	auto to_node = get_node_from_path(toPath);
	links::create(m_db, m_cur.m_id, to_node.m_id);
}

void cmd::unlink(const std::wstring& toPath)
{
	auto to_node = get_node_from_path(toPath);
	links::remove(m_db, m_cur.m_id, to_node.m_id);
}

std::vector<std::wstring> cmd::parse_cmds(const std::wstring& cmd)
{
	std::vector<std::wstring> output;
	std::wstring collector;

	bool in_quote = false;

	for (size_t s = 0; s < cmd.length(); ++s)
	{
		wchar_t c = cmd[s];
		if (c == '\"')
		{
			if (!in_quote)
				collector = trim(collector);
			
			if (in_quote || !collector.empty())
			{
				output.emplace_back(collector);
				collector.clear();
			}

			in_quote = !in_quote;
			continue;
		}

		if (!in_quote && c == ' ')
		{
			collector = trim(collector);
			if (!collector.empty())
			{
				output.emplace_back(collector);
				collector.clear();
			}
			continue;
		}

		collector += c;
	}

	collector = trim(collector);
	if (!collector.empty())
	{
		output.emplace_back(collector);
		collector.clear();
	}

	return output;
}
