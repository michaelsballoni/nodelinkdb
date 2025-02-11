#include "pch.h"
#include "search.h"
#include "strings.h"
#include "nodes.h"
#include "core.h"

using namespace nldb;

struct find_params
{
	find_params(const search_query& query) : m_query(query) {}

	const search_query& m_query;

	std::wstring m_itemTable;
	std::wstring m_itemType;
	std::wstring m_columns;
};

std::wstring get_find_sql(db& db, const find_params& findParams, paramap& sqlParams)
{
	int64_t parent_string_id = strings::get_id(db, L"parent");
	int64_t path_string_id = strings::get_id(db, L"path");
	int64_t name_string_id = strings::get_id(db, L"name");
	int64_t payload_string_id = strings::get_id(db, L"payload");
	int64_t type_string_id = strings::get_id(db, L"type");

	sqlParams[L"@node_item_type_id"] = strings::get_id(db, findParams.m_itemType);
	sqlParams[L"@order_by_string_id"] = strings::get_id(db, findParams.m_query.m_orderBy);

	std::wstring sql = L"SELECT " + findParams.m_columns + L" FROM " + findParams.m_itemTable + L" AS Items ";
	if (!findParams.m_query.m_orderBy.empty())
		sql += L"JOIN props AS ItemProps ON ItemProps.itemid = Items.id JOIN strings AS ItemStrings ON ItemStrings.id = ItemProps.valstrid ";

	std::wstring where;
	int param_num = 1;
	for (const auto& crit : findParams.m_query.m_criteria)
	{
		if (!where.empty())
			where += L"\nAND ";

		std::wstring param_num_str = std::to_wstring(param_num);
		++param_num;

		if (crit.m_nameStringId == name_string_id) // searching by name
		{
			std::wstring new_sql = L"Items.id IN (SELECT InnerNodes.id FROM nodes InnerNodes JOIN strings NameStrings ON NameStrings.id = InnerNodes.name_string_id WHERE ";
			if (crit.m_useLike)
			{
				sqlParams[L"@valstr" + param_num_str] = crit.m_valueString;
				new_sql += L"NameStrings.val LIKE @valstr" + param_num_str;
			}
			else
			{
				sqlParams[L"@valstrid" + param_num_str] = strings::get_id(db, crit.m_valueString);
				new_sql += L"NameStrings.id = @valstrid" + param_num_str;
			}
			new_sql += L')';

			where += new_sql;
		}
		else if (crit.m_nameStringId == type_string_id) // searching by type
		{
			sqlParams[L"@valstrid" + param_num_str] = strings::get_id(db, crit.m_valueString);
			where += L"type_string_id = @valstrid" + param_num_str;
		}
		else if (crit.m_nameStringId == payload_string_id) // search by payload
		{
			std::wstring new_sql;
			sqlParams[L"@valstr" + param_num_str] = crit.m_valueString;
			if (crit.m_useLike)
				new_sql += L"payload LIKE @valstr" + param_num_str;
			else
				new_sql += L"payload = @valstr" + param_num_str;
			where += new_sql;
		}
		else if (crit.m_nameStringId == parent_string_id) // search directly within a parent node
		{
			auto parent_path_opt = nodes::get_path_nodes(db, crit.m_valueString);
			if (parent_path_opt.has_value())
			{
				int64_t parent_id = parent_path_opt.value().back().id;
				where += L"Items.parent_id = " + std::to_wstring(parent_id);
			}
			else
				where += L"1 = 0"; // no path, no results
		}
		else if (crit.m_nameStringId == path_string_id) // search deeply within a parent node
		{
			auto child_like_opt = nodes::get_path_to_parent_like(db, crit.m_valueString);
			if (child_like_opt.has_value())
			{
				sqlParams[L"@valstr" + param_num_str] = child_like_opt.value();
				where += L"Items.parents LIKE @valstr" + param_num_str;
			}
			else
				where += L"1 = 0"; // no path, no results
		}
		else
		{
			sqlParams[L"@namestrid" + param_num_str] = crit.m_nameStringId;

			std::wstring new_sql = L"Items.id IN (SELECT itemid FROM props WHERE itemtypstrid = @node_item_type_id AND namestrid = @namestrid" + param_num_str;
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

			where += new_sql;
		}
	}
	sql += L"WHERE " + where;

	if (!findParams.m_query.m_orderBy.empty())
	{
		sql += L"\nAND ItemProps.itemtypstrid = @node_item_type_id AND ItemProps.namestrid = @order_by_string_id";
		sql += L"\nORDER BY ItemStrings.val " + std::wstring(findParams.m_query.m_orderAscending ? L"ASC" : L"DESC");
	}

	if (findParams.m_query.m_limit > 0)
		sql += L"\nLIMIT " + std::to_wstring(findParams.m_query.m_limit);

	return sql;
}

std::vector<node> search::find_nodes(db& db, const search_query& query)
{
	std::vector<node> output;
	if (query.m_criteria.empty())
		return output;

	find_params find_params(query);
	find_params.m_itemType = L"node";
	find_params.m_itemTable = L"nodes";
	find_params.m_columns = L"Items.id, parent_id, name_string_id, type_string_id";
	if (query.m_includePayload)
		find_params.m_columns += L", payload";

	paramap sql_params;
	std::wstring sql = get_find_sql(db, find_params, sql_params);
	auto reader = db.execReader(sql, sql_params);
	if (query.m_includePayload)
	{
		while (reader->read())
			output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3), reader->getString(4));
	}
	else
	{
		while (reader->read())
			output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	}
	return output;
}

std::vector<link> search::find_links(db& db, const search_query& query)
{
	std::vector<link> output;
	if (query.m_criteria.empty())
		return output;

	find_params find_params(query);
	find_params.m_itemType = L"link";
	find_params.m_itemTable = L"links";
	find_params.m_columns = L"Items.id, from_node_id, to_node_id, type_string_id";
	if (query.m_includePayload)
		find_params.m_columns += L", payload";

	paramap sql_params;
	std::wstring sql = get_find_sql(db, find_params, sql_params);
	auto reader = db.execReader(sql, sql_params);
	if (query.m_includePayload)
	{
		while (reader->read())
			output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3), reader->getString(4));
	}
	else
	{
		while (reader->read())
			output.emplace_back(reader->getInt64(0), reader->getInt64(1), reader->getInt64(2), reader->getInt64(3));
	}
	return output;
}
 