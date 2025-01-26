#include "pch.h"
#include "nodes.h"
#include "core.h"
#include "strings.h"

using namespace nldb;

static std::wstring ids_to_sql_in(const std::vector<int64_t>& ids)
{
	if (ids.empty())
		throw nldberr("ids_to_sql_in: called with no IDs");

	std::wstring output;
	for (auto id : ids)
	{
		if (!output.empty())
			output += ',';
		output += std::to_wstring(id);
	}
	return output;
}

// only IDs found between separators are returned
// nothing in, nothing out
static std::vector<int64_t> str_to_ids(const std::wstring& str, wchar_t separator) 
{
	std::vector<int64_t> ids;
	std::wstring collector;
	for (auto c : str)
	{
		if (c == separator)
		{
			if (!collector.empty())
			{
				ids.push_back(_wtoi64(collector.c_str()));
				collector.clear();
			}
		}
		else
			collector.push_back(c);
	}
	if (!collector.empty())
		ids.push_back(_wtoi64(collector.c_str()));
	return ids;
}

static std::wstring ids_to_parents_str(const std::vector<int64_t>& ids)
{
	if (ids.empty() || (ids.size() == 1 && ids[0] == 0))
		return std::wstring();

	std::wstring output = L"/";
	for (auto id : ids)
	{
		if (id != 0)
		{
			output += std::to_wstring(id);
			output += '/';
		}
	}
	return output;
}

static std::vector<int64_t> get_parents_node_ids(db& db, int64_t nodeId)
{
	if (nodeId == 0)
		return std::vector<int64_t>();

	auto parents_ids_str_opt = 
		db.execScalarString(L"SELECT parents FROM nodes WHERE id = @nodeId", { {L"@nodeId", nodeId} });
	
	if (!parents_ids_str_opt.has_value())
		throw nldberr("get_parents_node_ids: Node not found: " + std::to_string(nodeId));
	else
		return str_to_ids(parents_ids_str_opt.value(), '/');
}

node nodes::create(db& db, int64_t parentNodeId, int64_t nameStringId, int64_t typeStringId, const std::wstring& payload) 
{
	auto parent_node_ids = get_parents_node_ids(db, parentNodeId);
	if (parentNodeId != 0)
		parent_node_ids.push_back(parentNodeId);

	std::wstring parents_str = ids_to_parents_str(parent_node_ids);

	int64_t new_id = -1;
	if (parents_str.empty() && payload.empty())
	{
		new_id =
			db.execInsert
			(
				L"INSERT INTO nodes (parent_id, name_string_id, type_string_id) "
				L"VALUES (@parentNodeId, @nameStringId, @typeStringId)",
				{
					{ L"@parentNodeId", parentNodeId },
					{ L"@nameStringId", nameStringId },
					{ L"@typeStringId", typeStringId },
				}
			);
	}
	else if (payload.empty())
	{
		new_id =
			db.execInsert
			(
				L"INSERT INTO nodes (parent_id, name_string_id, type_string_id, parents) " 
				L"VALUES (@parentNodeId, @nameStringId, @typeStringId, @parents)",
				{
					{ L"@parentNodeId", parentNodeId },
					{ L"@nameStringId", nameStringId },
					{ L"@typeStringId", typeStringId },
					{ L"@parents", parents_str },
				}
			);
	}
	else if (parents_str.empty())
	{
		new_id =
			db.execInsert
			(
				L"INSERT INTO nodes (parent_id, name_string_id, type_string_id, payload) " 
				L"VALUES (@parentNodeId, @nameStringId, @typeStringId, @payload)",
				{
					{ L"@parentNodeId", parentNodeId },
					{ L"@nameStringId", nameStringId },
					{ L"@typeStringId", typeStringId },
					{ L"@payload", payload }
				}
			);
	}
	else
	{
		new_id =
			db.execInsert
			(
				L"INSERT INTO nodes (parent_id, name_string_id, type_string_id, parents, payload) "
				L"VALUES (@parentNodeId, @nameStringId, @typeStringId, @parents, @payload)",
				{
					{ L"@parentNodeId", parentNodeId },
					{ L"@nameStringId", nameStringId },
					{ L"@typeStringId", typeStringId },
					{ L"@parents", parents_str },
					{ L"@payload", payload }
				}
			);
	}
	return node(new_id, parentNodeId, nameStringId, typeStringId);
}

// build LIKE path to node where it exists initially as we'll need to update its children
static std::wstring get_child_nodes_like(db& db, int64_t nodeId)
{
	auto original_node_parents_opt =
		db.execScalarString(L"SELECT parents FROM nodes WHERE id = @id", { { L"@id", nodeId } });
	if (!original_node_parents_opt.has_value())
		throw nldberr("nodes::move: Node not found: " + std::to_string(nodeId));

	std::wstring original_node_parents = original_node_parents_opt.value();
	if (original_node_parents.empty() || original_node_parents.back() != '/')
		original_node_parents = L"/";
	original_node_parents += std::to_wstring(nodeId) + L"/%";
	return original_node_parents;
}

void nodes::move(db& db, int64_t nodeId, int64_t newParentNodeId) 
{
	// check inputs
	if (nodeId == 0)
		throw nldberr("nodes::move: Cannot move null node");

	// collect all children nodes
	std::wstring child_nodes_like = get_child_nodes_like(db, nodeId);
	std::vector<int64_t> child_node_ids;
	{
		auto child_reader = 
			db.execReader
			(
				L"SELECT id FROM nodes WHERE parents LIKE @childParents", 
				{ { L"@childParents", child_nodes_like } }
			);
		while (child_reader->read())
			child_node_ids.push_back(child_reader->getInt64(0));
	}

	// compute the new parents for the node
	auto parents_node_ids = get_parents_node_ids(db, newParentNodeId);
	if (newParentNodeId != 0)
		parents_node_ids.push_back(newParentNodeId);
	std::wstring new_parents_str = ids_to_parents_str(parents_node_ids);

	// update the nodes parent and parents
	{
		int rows_affected =
			db.execSql
			(
				L"UPDATE nodes SET parent_id = @newParentNodeId, parents = @newParents WHERE id = @nodeId",
				{
					{ L"@nodeId", nodeId },
					{ L"@newParentNodeId", newParentNodeId },
					{ L"@newParents", new_parents_str },
				}
				);
		if (rows_affected != 1)
			throw nldberr("nodes::move: Node not moved: " + std::to_string(nodeId));
	}

	// update the parents of all children nodes
	for (auto child_id : child_node_ids)
	{
		std::wstring parents_str = ids_to_parents_str(get_parents_node_ids(db, child_id));
		int rows_affected =
			db.execSql
			(
				L"UPDATE nodes SET parents = @newParents WHERE id = @nodeId",
				{
					{ L"@nodeId", child_id },
					{ L"@newParents", new_parents_str },
				}
				);
		if (rows_affected != 1)
			throw nldberr("nodes::move: Child node not updated: " + std::to_string(child_id));
	}
}

void nodes::remove(db& db, int64_t nodeId)
{
	// check inputs
	if (nodeId == 0)
		throw nldberr("nodes::remove: Cannot remove node: " + std::to_string(nodeId));

	// collect all children nodes
	std::wstring child_nodes_like = get_child_nodes_like(db, nodeId);
	std::vector<int64_t> child_node_ids;
	{
		auto child_reader = 
			db.execReader
			(
				L"SELECT id FROM nodes WHERE parents LIKE @childParents", 
				{ { L"@childParents", child_nodes_like } }
			);
		while (child_reader->read())
			child_node_ids.push_back(child_reader->getInt64(0));
	}

	// delete the children nodes
	if (!child_node_ids.empty())
	{
		int rows_affected = db.execSql(L"DELETE FROM nodes WHERE id IN (@nodeIds)", { { L"@nodeIds", ids_to_sql_in(child_node_ids) } });
		if (rows_affected != static_cast<int64_t>(child_node_ids.size()))
			throw nldberr("nodes::remove: Not all child nodes removed: " + std::to_string(rows_affected));
	}

	// delete the node
	int rows_affected = db.execSql(L"DELETE FROM nodes WHERE id = @nodeId", { { L"@nodeId", nodeId } });
	if (rows_affected != 1)
		throw nldberr("nodes::remove: Node not removed: " + std::to_string(nodeId));
}

std::optional<node> nodes::get_node(db& db, int64_t nodeId) 
{
	auto reader =
		db.execReader
		(
			L"SELECT parent_id, name_string_id, type_string_id FROM nodes WHERE id = @nodeId",
			{ { L"@nodeId", nodeId } }
		);
	if (!reader->read())
		return std::nullopt;
	else
		return node(nodeId, reader->getInt64(0), reader->getInt64(1), reader->getInt64(2));
}

std::optional<node> nodes::get_node_in_parent(db& db, int64_t parentNodeId, int64_t nameStringId) 
{
	auto reader =
		db.execReader
		(
			L"SELECT id, type_string_id " 
			L"FROM nodes " 
			L"WHERE parent_id = @parentNodeId " 
			L"AND name_string_id = @nameStringId", 
			{
				{ L"@parentNodeId", parentNodeId, },
				{ L"@nameStringId", nameStringId }
			}
		);
	if (!reader->read())
		return std::nullopt;

	int64_t id = reader->getInt64(0);
	int64_t type_string_id = reader->getInt64(1);

	return node(id, parentNodeId, nameStringId, type_string_id);
}

std::optional<node> nodes::get_parent(db& db, int64_t nodeId) 
{
	if (nodeId == 0)
		return std::nullopt;

	auto reader =
		db.execReader
		(
			L"SELECT id, parent_id, name_string_id, type_string_id FROM nodes WHERE id = (SELECT parent_id FROM nodes WHERE id = @id)",
			{ { L"@id", nodeId } }
		);
	if (!reader->read())
		return std::nullopt;
	else
		return node(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
}

std::vector<node> nodes::get_parents(db& db, int64_t nodeId)
{
	if (nodeId == 0)
		return std::vector<node>();

	auto parents_node_ids = get_parents_node_ids(db, nodeId);
	if (parents_node_ids.empty())
		return std::vector<node>();

	std::unordered_map<int64_t, node> collector;
	collector.reserve(parents_node_ids.size());
	auto reader =
		db.execReader
		(
			L"SELECT id, parent_id, name_string_id, type_string_id FROM nodes WHERE id IN (@nodeIds)",
			{ { L"@nodeIds", ids_to_sql_in(parents_node_ids) } }
		);
	while (reader->read())
		collector.insert({ reader->getInt64(0), node(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3)) });

	std::vector<node> output;
	output.reserve(parents_node_ids.size());
	for (auto id : parents_node_ids)
	{
		auto it = collector.find(id);
		if (it == collector.end())
			throw nldberr("nodes::get_node_parents: Path to node not found: " + std::to_string(id));
		else
			output.emplace_back(it->second);
	}
	return output;
}

std::vector<node> nodes::get_children(db& db, int64_t nodeId)
{
	// handle null parent, the parent of all children
	if (nodeId == 0)
	{
		std::vector<node> all_child_nodes;
		auto all_reader = db.execReader(L"SELECT id, parent_id, name_string_id, type_string_id FROM nodes WHERE id <> 0", {});
		while (all_reader->read())
			all_child_nodes.emplace_back
			(
				all_reader->getInt64(0),
				all_reader->getInt64(1),
				all_reader->getInt64(2),
				all_reader->getInt64(3)
			);
		return all_child_nodes;
	}

	// build LIKE path to node where it exists initially as we'll need to update its children
	auto original_node_parents_opt =
		db.execScalarString(L"SELECT parents FROM nodes WHERE id = @id", { { L"@id", nodeId } });
	if (!original_node_parents_opt.has_value())
		throw nldberr("nodes::get_children: Node not found: " + std::to_string(nodeId));

	std::wstring original_node_parents_str = original_node_parents_opt.value();

	std::wstring original_node_parents_like;
	if (original_node_parents_str.empty())
		original_node_parents_like = L"/" + original_node_parents_str + std::to_wstring(nodeId) + L"/%";
	else
		original_node_parents_like = original_node_parents_str + std::to_wstring(nodeId) + L"/%";

	// collect all children nodes
	std::vector<node> child_nodes;
	auto reader =
		db.execReader
		(
			L"SELECT id, parent_id, name_string_id, type_string_id FROM nodes " 
			L"WHERE id <> 0 AND parents LIKE @childParents AND id <> @nodeId",
			{ { L"@childParents", original_node_parents_like }, { L"@nodeId", nodeId } }
		);
	while (reader->read())
		child_nodes.emplace_back
		(
			reader->getInt64(0), 
			reader->getInt64(1), 
			reader->getInt64(2), 
			reader->getInt64(3)
		);
	return child_nodes;
}

std::vector<node> nodes::get_path(db& db, const node& cur)
{
	std::vector<node> output;
	std::unordered_set<int64_t> seen_node_ids;
	node cur_node = cur;
	do
	{
		if (cur_node.m_id != 0)
			output.push_back(cur_node);

		auto new_node_opt = get_parent(db, cur_node.m_id);
		if (!new_node_opt.has_value())
			break;
		cur_node = new_node_opt.value();

		if (seen_node_ids.find(cur_node.m_id) != seen_node_ids.end())
			break;
		else
			seen_node_ids.insert(cur_node.m_id);
	} while (cur_node.m_id != 0);
	std::reverse(output.begin(), output.end());
	return output;
}

std::wstring nodes::get_path_str(db& db, const node& cur)
{
	auto path_nodes = get_path(db, cur);

	std::vector<int64_t> path_str_ids;
	path_str_ids.reserve(path_nodes.size());
	for (const auto& pnode : path_nodes)
		path_str_ids.push_back(pnode.m_nameStringId);

	auto strs_map = strings::get_vals(db, path_str_ids);

	std::wstring path_str;
	for (int64_t path_str_id : path_str_ids) 
	{
		path_str += L'/';
		path_str += strs_map[path_str_id];
	}
	return path_str;
}

std::vector<node> nodes::get_path_nodes(db& db, const std::wstring& path) 
{
	std::vector<node> output;

	std::vector<std::wstring> splits;
	std::wstring builder;
	for (wchar_t c : path) 
	{
		if (c == '/') 
		{
			if (!builder.empty())
			{
				splits.push_back(builder);
				builder.clear();
			}
		}
		else
			builder += c;
	}
	if (!builder.empty())
		splits.push_back(builder);

	if (splits.empty())
		return { };
	else
		output.reserve(splits.size());

	int64_t cur_node_id = 0;
	for (const auto& part : splits) {
		int64_t cur_name_string_id = strings::get_id(db, part);

		auto node_opt = get_node_in_parent(db, cur_node_id, cur_name_string_id);
		if (!node_opt.has_value())
			throw nldberr("nodes::get_path_nodes: Node not found: " + toNarrowStr(part));

		output.emplace_back(node_opt.value());

		cur_node_id = node_opt.value().m_id;
	}

	return output;
}
