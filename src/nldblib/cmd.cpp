#include "pch.h"
#include "cmd.h"
#include "loader.h"
#include "nodes.h"
#include "strings.h"

using namespace nldb;

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
	nodes::rename(m_db, m_cur.m_id, strings::get_id(m_db, newName));
}

std::vector<std::wstring> cmd::parseCommands(const std::wstring& cmd)
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

		if (c == '\\')
		{
			if (s + 1 >= cmd.length())
				throw nldberr("\\ at end of string");
			wchar_t replacement = 0;
			switch (cmd[s + 1]) {
			case 't': replacement = '\t'; break;
			case 'n': replacement = '\n'; break;
			case '\'': replacement = '\''; break;
			case '\"': replacement = '\"'; break;
			case '\\': replacement = '\\'; break;
			default: throw nldberr("Invalid string after \\: " + toNarrowStr((std::wstring(cmd[s + 1], 1))));
			}
			collector += replacement;
			++s;
		}
		else
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
