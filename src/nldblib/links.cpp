#include "pch.h"
#include "links.h"
#include "core.h"

using namespace nldb;

link links::create(db& db, int64_t fromNodeId, int64_t toNodeId, int64_t typeStringId)
{
	int64_t new_id =
		db.execInsert
		(
			L"INSERT INTO links (from_node_id, to_node_id, type_string_id) VALUES (@fromNodeId, @toNodeId, @typeStringId)",
			{
				{ L"@fromNodeId", fromNodeId },
				{ L"@toNodeId", toNodeId },
				{ L"@typeStringId", typeStringId },
			}
			);
	return link(new_id, fromNodeId, toNodeId, typeStringId);
}

std::optional<link> links::get_link(db& db, int64_t linkId)
{
	auto reader =
		db.execReader
		(
			L"SELECT id, from_node_id, to_node_id, type_string_id FROM links WHERE id = @linkId",
			{ { L"@linkId", linkId } }
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
