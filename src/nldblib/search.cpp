#include "pch.h"
#include "search.h"
#include "strings.h"
#include "core.h"

using namespace nldb;

struct find_params
{
	std::wstring itemTable;
	std::vector<search_criteria> criteria;
	std::wstring orderBy;
	bool orderAscending;
	int limit;
	paramap params;
	std::wstring itemType;
};

std::wstring get_find_sql(db& db, find_params& findParams)
{
	findParams.params[L"@node_item_type_id"] = (double)strings::get_id(db, findParams.itemType);

	std::wstring sql = L" FROM " + findParams.itemTable + L" I ";
	if (!findParams.orderBy.empty())
	{
		int64_t order_by_string_id = strings::get_id(db, findParams.orderBy);
		sql += L"JOIN props P ON P.itemid = I.id AND P.namestrid = " + std::to_wstring(order_by_string_id) + L" JOIN strings S ON id = P.valstrid ";
	}

	sql += L"WHERE ";
	int param_num = 1;
	for (const auto& crit : findParams.criteria)
	{
		if (param_num > 1)
			sql += L"\nAND ";

		std::wstring param_num_str = std::to_wstring(param_num);
		++param_num;

		findParams.params[L"@namestrid" + param_num_str] = (double)crit.m_nameStringId;

		std::wstring new_sql = L"id IN (SELECT itemid FROM props WHERE itemtypstrid = @node_item_type_id AND namestrid = @namestrid" + param_num_str;
		if (crit.m_useLike)
		{
			findParams.params[L"@valstr" + param_num_str] = crit.m_valueString;
			new_sql += L" AND valstrid IN (SELECT id FROM strings WHERE val LIKE @valstr" + param_num_str + L')';
		}
		else
		{
			findParams.params[L"@valstrid" + param_num_str] = (double)strings::get_id(db, crit.m_valueString);
			new_sql += L" AND valstrid = @valstrid" + param_num_str;
		}
		new_sql += L')';

		sql += new_sql;
	}

	if (!findParams.orderBy.empty())
	{
		sql += L"\nORDER BY S.val " + std::wstring(findParams.orderAscending ? L"ASC" : L"DESC");
	}

	if (findParams.limit > 0)
		sql += L"\nLIMIT " + std::to_wstring(findParams.limit);

	return sql;
}

std::vector<node> search::find_nodes(db& db, const std::vector<search_criteria>& criteria, const std::wstring& orderBy, bool orderAscending, int limit)
{
	std::vector<node> output;
	if (criteria.empty())
		return output;

	find_params find_params;
	find_params.criteria = criteria;
	find_params.itemTable = L"nodes";
	find_params.itemType = L"node";
	find_params.limit = limit;
	find_params.orderAscending = orderAscending;
	find_params.orderBy = orderBy;

	std::wstring sql = L"SELECT id, parent_id, name_string_id, type_string_id" + get_find_sql(db, find_params);
	auto reader = db.execReader(sql, find_params.params);
	while (reader->read())
		output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	return output;
}

std::vector<link> search::find_links(db& db, const std::vector<search_criteria>& criteria, const std::wstring& orderBy, bool orderAscending, int limit)
{
	std::vector<link> output;
	if (criteria.empty())
		return output;

	find_params find_params;
	find_params.criteria = criteria;
	find_params.itemTable = L"links";
	find_params.itemType = L"link";
	find_params.limit = limit;
	find_params.orderAscending = orderAscending;
	find_params.orderBy = orderBy;

	paramap params;
	std::wstring sql = L"SELECT id, from_node_id, to_node_id, type_string_id" + get_find_sql(db, find_params);
	auto reader = db.execReader(sql, params);
	while (reader->read())
		output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	return output;
}
