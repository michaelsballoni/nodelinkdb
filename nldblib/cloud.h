#pragma once

#include "db.h"
#include "link.h"

namespace nldb
{
	class cloud
	{
	public:
		cloud(db& db, int64_t seedNodeId)
		: m_db(db)
		, m_seedNodeId(seedNodeId)
		, m_tableName(L"cloudlinks_" + std::to_wstring(m_seedNodeId))
		{}

		void init();
		int64_t seed();
		int64_t expand();

		int max_generation() const;
		std::vector<link> links(int minGeneration) const;

	private:
		db& m_db;
		int64_t m_seedNodeId;
		std::wstring m_tableName;
	};
}
