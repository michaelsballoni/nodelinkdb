#pragma once

#include "db.h"
#include "node.h"

namespace nldb
{
	class cmd
	{
	public:
		cmd(db& db);

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

		void set_prop(const std::vector<std::wstring>& cmds);
		void set_payload(const std::wstring& payload);

		std::wstring tell();

		std::wstring search(const std::vector<std::wstring>& cmd);

		static std::vector<std::wstring> parse_cmds(const std::wstring& cmd);

	private:
		db& m_db;
		node m_cur;
		int64_t m_nodeItemTypeId;
	};
}
