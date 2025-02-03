#pragma once

#include "db.h"
#include "node.h"

namespace nldb
{
	class cmd
	{
	public:
		cmd(db& db) : m_db(db) {}

		void mount(const std::wstring& dirPath);

		std::wstring get_cur_path();
		node get_node_from_path(const std::wstring& path);

		void cd(const std::wstring& newPath);
		std::vector<std::wstring> dir();

		void mknode(const std::wstring& newNodeName);
		void copy(const std::wstring& newParent);
		void move(const std::wstring& newParent);
		void remove();
		void rename(const std::wstring& newName);

	private:
		db& m_db;
		node m_cur;
	};
}
