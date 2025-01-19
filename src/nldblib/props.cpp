#include "pch.h"
#include "props.h"
#include "strings.h"
#include "core.h"

using namespace nldb;

void props::setup(db& db) 
{
	db.execSql(L"DROP TABLE IF EXISTS props", {});
	db.execSql(L"CREATE TABLE props (itemtypstrid INTEGER, itemid INTEGER, namestrid INTEGER, valstrid INTEGER)", {});
	db.execSql(L"CREATE UNIQUE INDEX item_props ON props (itemtypstrid, itemid, namestrid)", {});
	db.execSql(L"CREATE INDEX prop_vals ON props (valstrid, namestrid, itemtypstrid, itemid)", {});
}

void props::set(db& db, int64_t item_type_string_id, int64_t item_id, int64_t name_string_id, int64_t value_string_id) 
{
	if (value_string_id >= 0)
	{
		db.execSql
		(
			L"INSERT INTO props (itemtypstrid, itemid, namestrid, valstrid) "
			L"VALUES(@item_type_string_id, @item_id, @name_string_id, @value_string_id) "
			L"ON CONFLICT(itemtypstrid, itemid, namestrid) DO UPDATE SET valstrid = @value_string_id",
			{
				{ L"@item_type_string_id", (double)item_type_string_id },
				{ L"@item_id", (double)item_id },
				{ L"@name_string_id", (double)name_string_id },
				{ L"@value_string_id", (double)value_string_id },
			}
		);
	}
	else // delete the value for the name
	{
		if (name_string_id >= 0)
		{
			db.execSql
			(
				L"DELETE FROM props WHERE itemtypstrid = @item_type_string_id AND itemid = @item_id AND namestrid = @name_string_id",
				{
					{ L"@item_type_string_id", (double)item_type_string_id },
					{ L"@item_id", (double)item_id },
					{ L"@name_string_id", (double)name_string_id },
				}
			);
		}
		else // delete all values for the node
		{
			db.execSql
			(
				L"DELETE FROM props WHERE itemtypstrid = @item_type_string_id AND itemid = @item_id",
				{
					{ L"@item_type_string_id", (double)item_type_string_id },
					{ L"@item_id", (double)item_id },
				}
			);
		}
	}
}

std::unordered_map<int64_t, int64_t> props::get(db& db, int64_t item_type_string_id, int64_t item_id) 
{
	std::unordered_map<int64_t, int64_t> output;
	auto reader = 
		db.execReader
		(
			L"SELECT namestrid, valstrid FROM props WHERE itemtypstrid = @item_type_string_id AND itemid = @item_id", 
			{ 
				{L"@item_type_string_id", (double)item_type_string_id },
				{L"@item_id", (double)item_id },
			}
		);
	while (reader->read()) 
		output.insert({ reader->getInt64(0), reader->getInt64(1) });
	return output;
}

std::map<std::wstring, std::wstring> props::fill(db& db, const std::unordered_map<int64_t, int64_t>& map)
{
	std::map<std::wstring, std::wstring> output;
	for (const auto& it : map)
		output.insert({ strings::get_val(db, it.first), strings::get_val(db, it.second) });
	return output;
}

std::wstring props::summarize(db& db, const std::unordered_map<int64_t, int64_t>& map)
{
	auto sorted_map = fill(db, map);

	std::wstring output;
	for (const auto& it : sorted_map)
	{
		if (!output.empty())
			output += '\n';
		output += it.first + L" " + it.second;
	}
	return output;
}
