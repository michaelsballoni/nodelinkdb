#include "pch.h"
#include "links.h"
#include "core.h"

using namespace nldb;

void links::setup(db& db)
{
	db.execSql(L"DROP TABLE IF EXISTS links", {});
	db.execSql
	(
		L"CREATE TABLE links "
		L"("
		L"id INTEGER PRIMARY KEY, "
		L"fromnodeid INTEGER NOT NULL, "
		L"tonodeid INTEGER NOT NULL, "
		L"typestrid INTEGER NOT NULL, "
		L"created TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, "
		L"last_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP"
		L")",
		{});

	db.execSql(L"CREATE UNIQUE INDEX link_from ON links (fromnodeid, tonodeid, typestrid)", {});
	db.execSql(L"CREATE UNIQUE INDEX link_to ON links (tonodeid, fromnodeid, typestrid)", {});

	db.execSql(L"INSERT INTO links (id, fromnodeid, tonodeid, typestrid) VALUES (0, 0, 0, 0)", {});
}

link links::create(db& db, int64_t fromNodeId, int64_t toNodeId, int64_t typeStringId)
{
	int64_t new_id =
		db.execInsert
		(
			L"INSERT INTO links (fromnodeid, tonodeid, typestrid) VALUES (@fromNodeId, @toNodeId, @typeStringId)",
			{
				{ L"@fromNodeId", (double)fromNodeId },
				{ L"@toNodeId", (double)toNodeId },
				{ L"@typeStringId", (double)typeStringId },
			}
			);
	return link(new_id, fromNodeId, toNodeId, typeStringId);
}

std::optional<link> links::get_link(db& db, int64_t linkId)
{
	auto reader =
		db.execReader
		(
			L"SELECT id, fromnodeid, tonodeid, typestrid FROM links WHERE id = @linkId",
			{ { L"@linkId", (double)linkId } }
		);
	if (!reader->read())
		return std::nullopt;
	else
		return link(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
}

std::vector<link> links::get_out_links(db& db, int64_t fromNodeId)
{
	std::vector<link> output;
	auto reader = 
		db.execReader
		(
			L"SELECT id, fromnodeid, tonodeid, typestrid FROM links WHERE fromnodeid = @fromNodeId",
			{ { L"@fromNodeId", (double)fromNodeId } }
		);
	while (reader->read())
		output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	return output;
}

std::vector<link> links::get_in_links(db& db, int64_t toNodeId)
{
	std::vector<link> output;
	auto reader =
		db.execReader
		(
			L"SELECT id, fromnodeid, tonodeid, typestrid FROM links WHERE tonodeid = @toNodeId",
			{ { L"@toNodeId", (double)toNodeId } }
		);
	while (reader->read())
		output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	return output;
}
