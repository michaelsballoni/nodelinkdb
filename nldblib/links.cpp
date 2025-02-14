#include "pch.h"
#include "links.h"
#include "core.h"

using namespace nldb;

link links::create(db& db, int64_t fromNodeId, int64_t toNodeId, int64_t typeStringId, const std::optional<std::wstring>& payload)
{
	int64_t new_id = -1;
	if (payload.has_value())
	{
		new_id = 
			db.execInsert
			(
				L"INSERT INTO links (from_node_id, to_node_id, type_string_id, payload) VALUES (@fromNodeId, @toNodeId, @typeStringId, @payload)",
				{
					{ L"@fromNodeId", fromNodeId },
					{ L"@toNodeId", toNodeId },
					{ L"@typeStringId", typeStringId },
					{ L"@payload", payload.value()},
				}
			);
	}
	else
	{
		new_id =
			db.execInsert
			(
				L"INSERT INTO links (from_node_id, to_node_id, type_string_id) VALUES (@fromNodeId, @toNodeId, @typeStringId)",
				{
					{ L"@fromNodeId", fromNodeId },
					{ L"@toNodeId", toNodeId },
					{ L"@typeStringId", typeStringId },
				}
			);
	}
	return link(new_id, fromNodeId, toNodeId, typeStringId);
}

bool links::remove(db& db, int64_t fromNodeId, int64_t toNodeId, int64_t typeStringId)
{
	int affected =
		db.execSql
		(
			L"DELETE FROM links WHERE " 
			L"from_node_id = @fromNodeId AND " 
			L"to_node_id = @toNodeId AND " 
			L"type_string_id = @typeStringId",
			{
				{ L"@fromNodeId", fromNodeId },
				{ L"@toNodeId", toNodeId },
				{ L"@typeStringId", typeStringId },
			}
		);
	return affected > 0;
}

link links::get(db& db, int64_t linkId)
{
	auto reader =
		db.execReader
		(
			L"SELECT from_node_id, to_node_id, type_string_id FROM links WHERE id = @linkId",
			{ { L"@linkId", linkId } }
		);
	if (!reader->read())
		throw nldberr("links::get: Link not found: " + std::to_string(linkId));
	else
		return link(linkId, reader->getInt64(0), reader->getInt64(1), reader->getInt64(2));
}

std::wstring links::get_payload(db& db, int64_t linkId)
{
	auto reader =
		db.execReader
		(
			L"SELECT payload FROM links WHERE id = @linkId",
			{ { L"@linkId", linkId } }
		);
	if (!reader->read())
		throw nldberr("links::get_payload: Link not found: " + std::to_string(linkId));
	else
		return reader->getString(0);
}

std::vector<link> links::get_out_links(db& db, int64_t fromNodeId)
{
	std::vector<link> output;
	auto reader = 
		db.execReader
		(
			L"SELECT id, from_node_id, to_node_id, type_string_id FROM links WHERE from_node_id = @fromNodeId",
			{ { L"@fromNodeId", fromNodeId } }
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
			L"SELECT id, from_node_id, to_node_id, type_string_id FROM links WHERE to_node_id = @toNodeId",
			{ { L"@toNodeId", toNodeId } }
		);
	while (reader->read())
		output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	return output;
}
