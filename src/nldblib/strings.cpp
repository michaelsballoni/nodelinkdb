#include "pch.h"
#include "strings.h"
#include "core.h"

using namespace nldb;

void strings::setup(db& db) {
	db.execSql(L"DROP TABLE IF EXISTS strings", {});
	db.execSql(L"CREATE TABLE strings (id INTEGER PRIMARY KEY, val STRING UNIQUE NOT NULL)", {});
	db.execSql(L"INSERT INTO strings (id, val) VALUES (0, '')", {});
}

int64_t strings::get_id(db& db, const std::wstring& str) {
	auto opt = db.execScalarInt64(L"SELECT id FROM strings WHERE val = @val", { { L"@val", str } });
	if (opt.has_value())
		return opt.value();
	else
		return db.execInsert(L"INSERT INTO strings (val) VALUES (@val)", { { L"@val", str } });
}

std::wstring strings::get_val(db& db, int64_t id) {
	auto opt = db.execScalarString(L"SELECT val FROM strings WHERE id = @id", { { L"@id", (double)id } });
	if (!opt.has_value())
		throw nldberr("String not found: " + std::to_string(id));
	else
		return opt.value();
}

std::unordered_map<int64_t, std::wstring> strings::get_vals(db& db, std::vector<int64_t> ids) {
	std::unordered_map<int64_t, std::wstring> ret_val;
	if (ids.empty())
		return ret_val;
	else
		ret_val.reserve(ids.size());

	std::wstring ids_str;
	for (auto id : ids) {
		if (!ids_str.empty())
			ids_str += ',';
		ids_str += std::to_wstring(id);
	}

	auto reader = db.execReader(std::wstring(L"SELECT id, val FROM strings WHERE id IN (") + ids_str + L")", {});
	while (reader->read())
		ret_val[reader->getInt64(0)] = reader->getString(1);
	
	for (auto id : ids)
		if (ret_val.find(id) == ret_val.end())
			throw nldberr("String not found: " + std::to_string(id));

	return ret_val;
}
