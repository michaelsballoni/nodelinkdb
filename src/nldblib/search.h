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

		search_criteria
		(
			int64_t nameStringId = 0, 
			std::wstring valueString = L"", 
			bool useLike = false
		)
		: m_nameStringId(nameStringId)
		, m_valueString(valueString)
		, m_useLike(useLike)
		{
		}
	};

	struct search_query
	{
		std::vector<search_criteria> m_criteria;
		std::wstring m_orderBy;
		bool m_orderAscending;
		int64_t m_limit;
		bool m_includePayload;

		search_query
		(
			const std::vector<search_criteria>& criteria = {}, 
			const std::wstring& orderBy = L"", 
			bool orderAscending = true, 
			int limit = -1, 
			bool includePayload = false
		)
		: m_criteria(criteria)
		, m_orderBy(orderBy)
		, m_orderAscending(orderAscending)
		, m_limit(limit)
		, m_includePayload(includePayload)
		{
		}
	};

	class search
	{
	public:
		static std::vector<node> find_nodes(db& db, const search_query& query);
		static std::vector<link> find_links(db& db, const search_query& query);
	};
}
