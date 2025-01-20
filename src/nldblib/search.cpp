#include "pch.h"
#include "search.h"
#include "strings.h"
#include "core.h"

using namespace nldb;

struct find_params
{
	std::wstring m_itemTable;
	std::wstring m_itemType;
	std::wstring m_columns;
	const search_query* m_pQuery = nullptr;
};

std::wstring get_find_sql(db& db, const find_params& findParams, paramap& sqlParams)
{
	sqlParams[L"@node_item_type_id"] = strings::get_id(db, findParams.m_itemType);
	sqlParams[L"@order_by_string_id"] = strings::get_id(db, findParams.m_pQuery->m_orderBy);

	std::wstring sql = L"SELECT " + findParams.m_columns + L" FROM " + findParams.m_itemTable + L" Items ";
	if (!findParams.m_pQuery->m_orderBy.empty())
		sql += L"JOIN props ItemProps ON ItemProps.itemid = Items.id JOIN strings ItemStrings ON ItemString.id = ItemProps.valstrid ";

	sql += L"WHERE ";
	int param_num = 1;
	for (const auto& crit : findParams.m_pQuery->m_criteria)
	{
		if (param_num > 1)
			sql += L"\nAND ";

		std::wstring param_num_str = std::to_wstring(param_num);
		++param_num;

		sqlParams[L"@namestrid" + param_num_str] = crit.m_nameStringId;

		std::wstring new_sql = L"id IN (SELECT itemid FROM props WHERE itemtypstrid = @node_item_type_id AND namestrid = @namestrid" + param_num_str;
		if (crit.m_useLike)
		{
			sqlParams[L"@valstr" + param_num_str] = crit.m_valueString;
			new_sql += L" AND valstrid IN (SELECT id FROM strings WHERE val LIKE @valstr" + param_num_str + L')';
		}
		else
		{
			sqlParams[L"@valstrid" + param_num_str] = strings::get_id(db, crit.m_valueString);
			new_sql += L" AND valstrid = @valstrid" + param_num_str;
		}
		new_sql += L')';

		sql += new_sql;
	}

	if (!findParams.m_pQuery->m_orderBy.empty())
	{
		sql += L"\nAND ItemProps.namestrid = @order_by_string_id AND ItemProps.itemtypstrid = @node_item_type_id";
		sql += L"\nORDER BY ItemStrings.val " + std::wstring(findParams.m_pQuery->m_orderAscending ? L"ASC" : L"DESC");
	}

	if (findParams.m_pQuery->m_limit > 0)
		sql += L"\nLIMIT " + std::to_wstring(findParams.m_pQuery->m_limit);

	return sql;
}

std::vector<node> search::find_nodes(db& db, const search_query& query)
{
	std::vector<node> output;
	if (query.m_criteria.empty())
		return output;

	find_params find_params;
	find_params.m_itemType = L"node";
	find_params.m_itemTable = L"nodes";
	find_params.m_columns = L"id, parent_id, name_string_id, type_string_id";
	find_params.m_pQuery = &query;

	paramap sql_params;
	std::wstring sql = get_find_sql(db, find_params, sql_params);
	auto reader = db.execReader(sql, sql_params);
	while (reader->read())
		output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	return output;
}

std::vector<link> search::find_links(db& db, const search_query& query)
{
	std::vector<link> output;
	if (query.m_criteria.empty())
		return output;

	find_params find_params;
	find_params.m_itemType = L"link";
	find_params.m_itemTable = L"links";
	find_params.m_columns = L"id, from_node_id, to_node_id, type_string_id";
	find_params.m_pQuery = &query;

	paramap sql_params;
	std::wstring sql = get_find_sql(db, find_params, sql_params);
	auto reader = db.execReader(sql, sql_params);
	while (reader->read())
		output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	return output;
}
