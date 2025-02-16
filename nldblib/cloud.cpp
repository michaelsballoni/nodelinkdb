#include "pch.h"
#include "cloud.h"
#include "links.h"
#include "nodes.h"

using namespace nldb;

void cloud::seed(int64_t nodeId)
{
	clear();

	for (const auto& link : links::get_in_links(m_db, nodeId))
		m_links.emplace_back(link);

	for (const auto& link : links::get_out_links(m_db, nodeId))
		m_links.emplace_back(link);
}

std::vector<link> cloud::expand(int generations)
{
	std::vector<link> output;

	// track nodes in the cloud
	std::unordered_set<int64_t> cloud_node_ids;
	cloud_node_ids.reserve(m_links.size() * 2U);
	for (const link& link : m_links)
	{
		cloud_node_ids.insert(link.fromNodeId);
		cloud_node_ids.insert(link.toNodeId);
	}

	// a string of all node IDs in the cloud
	std::wstring cloud_node_ids_str;

	for (int g = 1; g <= generations; ++g)
	{
		// populate the string of existing node IDs
		cloud_node_ids_str.clear();
		for (int64_t id : cloud_node_ids)
		{
			if (!cloud_node_ids_str.empty())
				cloud_node_ids_str += ',';
			cloud_node_ids_str += std::to_wstring(id);
		}

		// get new links to grow the cloud...
		
		// from the cloud and not to the cloud
		size_t init_count = m_links.size();
		{
			std::wstring sql =
				L"SELECT id, from_node_id, to_node_id, type_string_id FROM links "
				L"WHERE from_node_id IN (" + cloud_node_ids_str + L") "
				L"AND to_node_id NOT IN (" + cloud_node_ids_str + L")";
			auto reader = m_db.execReader(sql, {});
			while (reader->read())
			{
				link cur_link(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));

				m_links.emplace_back(cur_link);
				output.emplace_back(cur_link);

				cloud_node_ids.insert(cur_link.fromNodeId);
				cloud_node_ids.insert(cur_link.toNodeId);
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

				m_links.emplace_back(cur_link);
				output.emplace_back(cur_link);

				cloud_node_ids.insert(cur_link.fromNodeId);
				cloud_node_ids.insert(cur_link.toNodeId);
			}
		}
		size_t after_count = m_links.size();

		// bail if nothing changed
		if (after_count == init_count)
			break;
	}

	return output;
}

void cloud::clear()
{
	m_links.clear();
}
