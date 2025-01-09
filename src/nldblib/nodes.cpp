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
		L"created NUMBER NOT NULL DEFAULT unixepoch('now','subsec'), " 
		L"last_modified NUMBER NOT NULL DEFAULT unixepoch('now','subsec')" 
		L")", 
	{});
	
	db.execSql(L"CREATE UNIQUE INDEX node_parents ON nodes (parent_id, id)", {});
	db.execSql(L"CREATE UNIQUE INDEX node_names ON nodes (parent_id, name_string_id)", {});

	db.execSql(L"INSERT INTO nodes (id, created, last_modified) VALUES (0, 0, 0)", {});
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
	// FORNOW
	(void)db;
	(void)nodeId;
	return std::nullopt;
}
