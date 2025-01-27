#include "pch.h"
#include "cmd.h"
#include "loader.h"
#include "nodes.h"
#include "strings.h"

using namespace nldb;

void cmd::load(const std::wstring& dirPath)
{
	loader loader(m_db);
	loader.load_directory(dirPath, m_cur);
}

std::wstring cmd::get_cur_path()
{
	return nodes::get_path_str(m_db, m_cur);
}

void cmd::cd(const std::wstring& newPath)
{
	if (newPath.empty())
		throw nldberr("Specify a path to change the current directory to");

	if (newPath == L"/")
	{
		m_cur = node();
		return;
	}

	std::wstring new_cur_path;
	if (newPath[0] != '/') // absolute
	{
		new_cur_path = newPath;
	}
	else // relative
	{
		new_cur_path = nodes::get_path_str(m_db, m_cur);
		if (new_cur_path.back() != '/' && newPath.find('/') == std::wstring::npos)
			new_cur_path += '/';
		new_cur_path += newPath;
	}

	auto nodes_opt = nodes::get_path_nodes(m_db, new_cur_path);
	if (!nodes_opt.has_value())
		throw nldberr("Path does not resolve to node: " + toNarrowStr(new_cur_path));
	else
		m_cur = nodes_opt.value().back();
}

std::vector<std::wstring> cmd::dir()
{
	std::vector<std::wstring> paths;
	for (auto child : nodes::get_children(m_db, m_cur.m_id))
		paths.emplace_back(nodes::get_path_str(m_db, child));
	std::sort(paths.begin(), paths.end());
	return paths;
}

void cmd::remove()
{
	int64_t orig_id = m_cur.m_id;
	auto parent = nodes::get(m_db, m_cur.m_parentId);
	if (!parent.has_value())
		throw nldberr("Current node has no parent");
	nodes::remove(m_db, orig_id);
	m_cur = parent.value();
}

void cmd::rename(const std::wstring& newName)
{
	nodes::rename(m_db, m_cur.m_id, strings::get_id(m_db, newName));
}
