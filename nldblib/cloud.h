#pragma once

#include "db.h"
#include "link.h"

namespace nldb
{
	class cloud
	{
	public:
		cloud(db& db) : m_db(db) {}

		void seed(int64_t nodeId);
		std::vector<link> expand(int generations);
		void clear();

		const std::vector<link>& links() const 
		{
			return m_links;
		}

	private:
		db& m_db;
		std::vector<link> m_links;
	};
}
