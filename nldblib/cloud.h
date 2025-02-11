#pragma once

#include "db.h"
#include "link.h"
#include "node.h"

namespace nldb
{
	struct cloudlink
	{
		link baseLink;
		node* fromNode;
		node* toNode;

		cloudlink(const link& otherLink = link(), node* otherFromNode = nullptr, node* otherToNode = nullptr)
			: baseLink(otherLink)
			, fromNode(otherFromNode)
			, toNode(otherToNode)
		{
		}
	};

	class cloud
	{
	public:
		cloud(db& db) : m_db(db) {}
		
		~cloud() 
		{
			clear();
		}

		void seed(int64_t nodeId);
		void expand(int generations);
		void clear();

		const std::vector<cloudlink>& links() const 
		{
			return m_links;
		}
	private:
		node* ensureNode(int64_t nodeId);

	private:
		db& m_db;
		std::unordered_map<int64_t, node*> m_nodeMap;
		std::vector<cloudlink> m_links;
	};
}
