#include "pch.h"
#include "nodes.h"
#include "core.h"

using namespace nldb;

void nodes::setup(db& db) {
	db.execSql(L"DROP TABLE IF EXISTS nodes", {});
	db.execSql
	(
		L"CREATE TABLE nodes "
		L"(" 
		L"id INTEGER PRIMARY KEY, "
		L"parent_id INTEGER NOT NULL DEFAULT 0, " 
		L"type_string_id INTEGER NOT NULL DEFAULT 0, " 
		L"name_string_id INTEGER NOT NULL DEFAULT 0, "
		L"created TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, " 
		L"last_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP" 
		L")", 
	{});
	
	db.execSql(L"CREATE UNIQUE INDEX node_parents ON nodes (parent_id, id)", {});
	db.execSql(L"CREATE UNIQUE INDEX node_names ON nodes (parent_id, name_string_id)", {});

	db.execSql(L"INSERT INTO nodes (id, created, last_modified) VALUES (0, 0, 0)", {});
}

node nodes::create(db& db, int64_t parentNodeId, int64_t nameStringId, int64_t typeStringId) {
	int64_t new_id =
		db.execInsert
		(
			L"INSERT INTO nodes (parent_id, name_string_id, type_string_id) VALUES (@parent_id, @name_string_id, @type_string_id)",
			{
				{ L"@parent_id", (double)parentNodeId },
				{ L"@name_string_id", (double)nameStringId },
				{ L"@type_string_id", (double)typeStringId },
			}
			);
	return node(new_id, parentNodeId, nameStringId, typeStringId);
}

void nodes::move(db& db, int64_t nodeId, int64_t newParentNodeId) {
	int rows_affected =
		db.execSql
		(
			L"UPDATE nodes SET parent_id = @newParentNodeId WHERE id = @nodeId",
			{
				{ L"@nodeId", (double)nodeId },
				{ L"@newParentNodeId", (double)newParentNodeId }
			}
		);
	if (rows_affected != 1)
		throw nldberr("Node not moved");
}

std::optional<node> nodes::get_node(db& db, int64_t nodeId) {
	auto reader =
		db.execReader
		(
			L"SELECT parent_id, name_string_id, type_string_id FROM nodes WHERE id = @nodeId",
			{ { L"@nodeId", (double)nodeId } }
		);
	if (!reader->read())
		return std::nullopt;

	int64_t parent_id = reader->getInt64(0);
	int64_t name_string_id = reader->getInt64(1);
	int64_t type_string_id = reader->getInt64(2);

	return node(nodeId, parent_id, name_string_id, type_string_id);
}

std::optional<node> nodes::get_node_in_parent(db& db, int64_t parentNodeId, int64_t nameStringId) {
	auto reader =
		db.execReader
		(
			L"SELECT id, type_string_id " 
			L"FROM nodes " 
			L"WHERE parent_id = @parent_id " 
			L"AND name_string_id = @name_string_id", 
			{
				{ L"@parent_id", (double)parentNodeId, },
				{ L"@name_string_id", (double)nameStringId }
			}
		);
	if (!reader->read())
		return std::nullopt;

	int64_t id = reader->getInt64(0);
	int64_t type_string_id = reader->getInt64(1);

	return node(id, parentNodeId, nameStringId, type_string_id);
}

std::optional<node> nodes::get_parent_node(db& db, int64_t nodeId) {
	auto reader =
		db.execReader
		(
			L"SELECT id, parent_id, name_string_id, type_string_id FROM nodes WHERE id = (SELECT parent_id FROM nodes WHERE id = @id)",
			{ { L"@id", (double)nodeId } }
		);
	if (!reader->read())
		return std::nullopt;

	int64_t id = reader->getInt64(0);
	int64_t parent_id = reader->getInt64(1);
	int64_t name_string_id = reader->getInt64(2);
	int64_t type_string_id = reader->getInt64(3);

	return node(id, parent_id, name_string_id, type_string_id);
}
