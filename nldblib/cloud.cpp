#include "pch.h"
#include "cloud.h"
#include "links.h"
#include "nodes.h"

using namespace nldb;

void cloud::seed(int64_t nodeId)
{
	clear();

	for (const auto& link : links::get_in_links(m_db, nodeId))
		m_links.emplace_back
		(
			link,
			ensureNode(link.fromNodeId),
			ensureNode(link.toNodeId)
		);

	for (const auto& link : links::get_out_links(m_db, nodeId))
		m_links.emplace_back
		(
			link,
			ensureNode(link.fromNodeId),
			ensureNode(link.toNodeId)
		);
}

void cloud::expand(int generations)
{
	for (int g = 1; g <= generations; ++g)
	{
		// get all nodes involved in our links
		std::unordered_set<int64_t> cloud_node_ids;
		cloud_node_ids.reserve(m_links.size() * 2U);
		for (const cloudlink& link : m_links)
		{
			cloud_node_ids.insert(link.fromNode->id);
			cloud_node_ids.insert(link.toNode->id);
		}

		std::wstring cloud_node_ids_str;
		for (int64_t id : cloud_node_ids)
		{
			if (!cloud_node_ids_str.empty())
				cloud_node_ids_str += ',';
			cloud_node_ids_str += std::to_wstring(id);
		}

		// get new links from the cloud...
		
		// from the cloud and not to the cloud
		size_t init_count = m_links.size();
		std::vector<link> output;
		{
			std::wstring sql =
				L"SELECT id, from_node_id, to_node_id, type_string_id FROM links "
				L"WHERE from_node_id IN (" + cloud_node_ids_str + L") "
				L"AND to_node_id NOT IN (" + cloud_node_ids_str + L")";
			auto reader = m_db.execReader(sql, {});
			while (reader->read())
			{
				link cur_link(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
				m_links.emplace_back
				(
					cur_link,
					ensureNode(cur_link.fromNodeId),
					ensureNode(cur_link.toNodeId)
				);
			}
		}

		// to the cloud and not from the cloud
		{
			std::wstring sql =
				L"SELECT id, from_node_id, to_node_id, type_string_id FROM links "
				L"WHERE to_node_id IN (" + cloud_node_ids_str + L") "
				L"AND from_node_id NOT IN (" + cloud_node_ids_str + L")";
			auto reader = m_db.execReader(sql, {});
			while (reader->read())
			{
				link cur_link(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
				m_links.emplace_back
				(
					cur_link,
					ensureNode(cur_link.fromNodeId),
					ensureNode(cur_link.toNodeId)
				);
			}
		}
		size_t after_count = m_links.size();

		// bail if nothing changed
		if (after_count == init_count)
			break;
	}
}

void cloud::clear()
{
	m_links.clear();

	for (auto it : m_nodeMap)
		delete it.second;
	m_nodeMap.clear();
}

node* cloud::ensureNode(int64_t nodeId)
{
	auto it = m_nodeMap.find(nodeId);
	if (it != m_nodeMap.end())
		return it->second;

	node* new_node = new node(nodes::get(m_db, nodeId));
	m_nodeMap.insert({ nodeId, new_node });
	return new_node;
}
