#include "pch.h"
#include "cloud.h"
#include "links.h"
#include "nodes.h"

using namespace nldb;

void cloud::init()
{
	m_db.execSql(L"DROP TABLE IF EXISTS " + m_tableName, {});
	m_db.execSql
	(
		L"CREATE TABLE " + m_tableName + L" "
		L"("
		L"gen INTEGER NOT NULL, "
		L"id INTEGER UNIQUE NOT NULL, "
		L"from_node_id INTEGER NOT NULL, "
		L"to_node_id INTEGER NOT NULL, "
		L"type_string_id INTEGER NOT NULL"
		L")",
		{}
	);
	m_db.execSql(L"CREATE INDEX link_from_" + std::to_wstring(m_seedNodeId) + L" ON " + m_tableName + L" (from_node_id, to_node_id)", {});
	m_db.execSql(L"CREATE INDEX link_to_" + std::to_wstring(m_seedNodeId) + L" ON " + m_tableName + L" (to_node_id, from_node_id)", {});
	m_db.execSql(L"CREATE INDEX gen_linkid_" + std::to_wstring(m_seedNodeId) + L" ON " + m_tableName + L" (gen, id)", {});
}

int64_t cloud::seed()
{
	std::wstring sql =
		L"INSERT INTO " + m_tableName + " (gen, id, from_node_id, to_node_id, type_string_id)"
		L"SELECT 0, id, from_node_id, to_node_id, type_string_id "
		L"FROM links "
		L"WHERE from_node_id = @seed_node_id OR to_node_id = @seed_node_id";
	int seed_link_count = m_db.execSql(sql, {{ L"@seed_node_id", m_seedNodeId }});
	if (seed_link_count <= 0)
		throw nldberr("Seed node has no links; cloud building is not possible; call again when there are links");
	else
		return seed_link_count;
}

int cloud::max_generation() const
{
	auto gen_opt = m_db.execScalarInt32(L"SELECT MAX(gen) FROM " + m_tableName, {});
	if (gen_opt.has_value())
		return gen_opt.value();
	else
		throw nldberr("No data loaded; call seed() first");
}

int64_t cloud::expand()
{
	int new_gen = max_generation() + 1;

	// from the cloud and not to the cloud
	int64_t affected = 0;
	{
		std::wstring sql =
			L"INSERT INTO " + m_tableName + " (gen, id, from_node_id, to_node_id, type_string_id)"
			L"SELECT @gen, id, from_node_id, to_node_id, type_string_id "
			L"FROM links "
			L"WHERE "
			L"("
			L"from_node_id IN (SELECT from_node_id FROM " + m_tableName + L") "
			L"OR "
			L"from_node_id IN (SELECT to_node_id FROM " + m_tableName + L") "
			L") "
			L"AND to_node_id NOT IN (SELECT to_node_id FROM " + m_tableName + L")";
		affected += m_db.execSql(sql, {{ L"@gen", new_gen }});
	}

	// to the cloud and not from the cloud
	{
		std::wstring sql =
			L"INSERT INTO " + m_tableName + " (gen, id, from_node_id, to_node_id, type_string_id)"
			L"SELECT @gen, id, from_node_id, to_node_id, type_string_id "
			L"FROM links "
			L"WHERE "
			L"("
			L"to_node_id IN (SELECT from_node_id FROM " + m_tableName + L") "
			L"OR "
			L"to_node_id IN (SELECT to_node_id FROM " + m_tableName + L") "
			L") "
			L"AND from_node_id NOT IN (SELECT from_node_id FROM " + m_tableName + L") ";
		affected += m_db.execSql(sql, {{ L"@gen", new_gen }});
	}

	return affected;
}

std::vector<link> cloud::links(int minGeneration) const
{
	std::wstring sql =
		L"SELECT id, from_node_id, to_node_id, type_string_id FROM " + m_tableName + L" WHERE gen >= @minGen";
	auto reader = m_db.execReader(sql, {{ L"@minGen", minGeneration }});
	std::vector<link> output;
	while (reader->read())
		output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	return output;
}
