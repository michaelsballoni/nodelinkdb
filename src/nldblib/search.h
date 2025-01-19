#pragma once

#include "db.h"
#include "link.h"
#include "node.h"

namespace nldb
{
	struct search_criteria
	{
		int64_t m_nameStringId;
		std::wstring m_valueString;
		bool m_useLike;

		search_criteria(int64_t nameStringId = 0, std::wstring valueString = L"", bool useLike = false)
			: m_nameStringId(nameStringId)
			, m_valueString(valueString)
			, m_useLike(useLike)
		{
		}
	};

	class search
	{
	public:
		static std::vector<node> find_nodes(db& db, const std::vector<search_criteria>& criteria, const std::wstring& orderBy, bool orderAscending, int limit);
		static std::vector<link> find_links(db& db, const std::vector<search_criteria>& criteria, const std::wstring& orderBy, bool orderAscending, int limit);
	};
}
