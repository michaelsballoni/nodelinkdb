#include "pch.h"
#include "nldb.h"

void nldb::setup_nldb(db& db)
{
	// strings
	nldb::strings::flush_caches();
	db.execSql(L"DROP TABLE IF EXISTS strings", {});
	db.execSql(L"CREATE TABLE strings (id INTEGER PRIMARY KEY, val STRING UNIQUE NOT NULL)", {});
	db.execSql(L"INSERT INTO strings (id, val) VALUES (0, '')", {});

	// nodes
	db.execSql(L"DROP TABLE IF EXISTS nodes", {});
	db.execSql
	(
		L"CREATE TABLE nodes "
		L"("
		L"id INTEGER PRIMARY KEY, "
		L"parent_id INTEGER NOT NULL, "
		L"type_string_id INTEGER NOT NULL, "
		L"name_string_id INTEGER NOT NULL, "
		L"parents STRING NOT NULL DEFAULT '', "
		L"payload STRING NOT NULL DEFAULT ''"
		L")",
		{});
	db.execSql(L"CREATE UNIQUE INDEX node_parents ON nodes (parent_id, id)", {});
	db.execSql(L"CREATE UNIQUE INDEX node_names ON nodes (parent_id, name_string_id)", {});
	db.execSql(L"CREATE UNIQUE INDEX node_parent_strs ON nodes (parents, id)", {});
	db.execSql(L"CREATE INDEX node_payloads ON nodes (payload, id)", {});
	db.execSql(L"INSERT INTO nodes (id, parent_id, type_string_id, name_string_id) VALUES (0, 0, 0, 0)", {});

	// links
	db.execSql(L"DROP TABLE IF EXISTS links", {});
	db.execSql
	(
		L"CREATE TABLE links "
		L"("
		L"id INTEGER PRIMARY KEY, "
		L"from_node_id INTEGER NOT NULL, "
		L"to_node_id INTEGER NOT NULL, "
		L"type_string_id INTEGER NOT NULL"
		L")",
		{});
	db.execSql(L"CREATE UNIQUE INDEX link_from ON links (from_node_id, to_node_id, type_string_id)", {});
	db.execSql(L"CREATE UNIQUE INDEX link_to ON links (to_node_id, from_node_id, type_string_id)", {});
	db.execSql(L"INSERT INTO links (id, from_node_id, to_node_id, type_string_id) VALUES (0, 0, 0, 0)", {});

	// props
	db.execSql(L"DROP TABLE IF EXISTS props", {});
	db.execSql(L"CREATE TABLE props (itemtypstrid INTEGER, itemid INTEGER, namestrid INTEGER, valstrid INTEGER)", {});
	db.execSql(L"CREATE UNIQUE INDEX item_props ON props (itemtypstrid, itemid, namestrid)", {});
	db.execSql(L"CREATE INDEX prop_vals ON props (valstrid, namestrid, itemtypstrid, itemid)", {});
}