#include "pch.h"
#include "props.h"
#include "strings.h"
#include "core.h"

using namespace nldb;

void props::set(db& db, int64_t itemTypeStringId, int64_t itemId, int64_t nameStringId, int64_t valueStringId)
{
	if (valueStringId >= 0)
	{
		db.execSql
		(
			L"INSERT INTO props (itemtypstrid, itemid, namestrid, valstrid) "
			L"VALUES(@item_type_string_id, @item_id, @name_string_id, @value_string_id) "
			L"ON CONFLICT(itemtypstrid, itemid, namestrid) DO UPDATE SET valstrid = @value_string_id",
			{
				{ L"@item_type_string_id", itemTypeStringId },
				{ L"@item_id", itemId },
				{ L"@name_string_id", nameStringId },
				{ L"@value_string_id", valueStringId },
			}
		);
	}
	else // delete the value for the name
	{
		if (nameStringId >= 0)
		{
			db.execSql
			(
				L"DELETE FROM props WHERE itemtypstrid = @item_type_string_id AND itemid = @item_id AND namestrid = @name_string_id",
				{
					{ L"@item_type_string_id", itemTypeStringId },
					{ L"@item_id", itemId },
					{ L"@name_string_id", nameStringId },
				}
			);
		}
		else // delete all values for the node
		{
			db.execSql
			(
				L"DELETE FROM props WHERE itemtypstrid = @item_type_string_id AND itemid = @item_id",
				{
					{ L"@item_type_string_id", itemTypeStringId },
					{ L"@item_id", itemId },
				}
			);
		}
	}
}

std::unordered_map<int64_t, int64_t> props::get(db& db, int64_t itemTypeStringId, int64_t itemId)
{
	std::unordered_map<int64_t, int64_t> output;
	auto reader = 
		db.execReader
		(
			L"SELECT namestrid, valstrid FROM props WHERE itemtypstrid = @item_type_string_id AND itemid = @item_id", 
			{ 
				{L"@item_type_string_id", itemTypeStringId },
				{L"@item_id", itemId },
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
