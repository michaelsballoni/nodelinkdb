#include "pch.h"
#include "strings.h"
#include "core.h"

using namespace nldb;

std::shared_mutex g_toIdCacheLock;
std::unordered_map<std::wstring, int64_t> g_toIdCache;

int64_t strings::get_id(db& db, const std::wstring& str) 
{
	if (str.empty())
		return 0;

	{
		std::shared_lock<std::shared_mutex> read_lock(g_toIdCacheLock);
		auto it = g_toIdCache.find(str);
		if (it != g_toIdCache.end())
			return it->second;
	}

	auto opt = db.execScalarInt64(L"SELECT id FROM strings WHERE val = @val", { { L"@val", str } });
	if (opt.has_value()) 
	{
		int64_t id = opt.value();
		std::unique_lock<std::shared_mutex> write_lock(g_toIdCacheLock);
		g_toIdCache.insert({ str, id });
		return id;
	}
	else 
	{
		try 
		{
			int64_t id = db.execInsert(L"INSERT INTO strings (val) VALUES (@val)", { { L"@val", str } });;
			std::unique_lock<std::shared_mutex> write_lock(g_toIdCacheLock);
			g_toIdCache.insert({ str, id });
			return id;
		}
		catch (const nldberr&) 
		{
			return get_id(db, str); // loop around and try again
		}
	}
}

std::shared_mutex g_toValCacheLock;
std::unordered_map<int64_t, std::wstring> g_toValCache;

std::wstring strings::get_val(db& db, int64_t id) 
{
	if (id == 0)
		return std::wstring();

	{
		std::shared_lock<std::shared_mutex> read_lock(g_toValCacheLock);
		auto it = g_toValCache.find(id);
		if (it != g_toValCache.end())
			return it->second;
	}

	auto opt = db.execScalarString(L"SELECT val FROM strings WHERE id = @id", { { L"@id", id } });
	if (!opt.has_value())
		throw nldberr("strings::get_val: String not found: " + std::to_string(id));

	std::wstring val = opt.value();
	{
		std::unique_lock<std::shared_mutex> write_lock(g_toValCacheLock);
		g_toValCache.insert({ id, val });
	}
	return val;
}

std::unordered_map<int64_t, std::wstring> strings::get_vals(db& db, const std::vector<int64_t>& ids) 
{
	std::unordered_map<int64_t, std::wstring> ret_val;
	if (ids.empty())
		return ret_val;
	else
		ret_val.reserve(ids.size());

	std::wstring ids_str;
	{
		std::shared_lock<std::shared_mutex> read_lock(g_toValCacheLock);
		for (int64_t id : ids) 
		{
			auto it = g_toValCache.find(id);
			if (it != g_toValCache.end()) 
			{
				ret_val[id] = it->second;
			}
			else 
			{
				if (!ids_str.empty())
					ids_str += ',';
				ids_str += std::to_wstring(id);
			}
		}
	}

	if (!ids_str.empty()) 
	{
		auto reader = db.execReader(std::wstring(L"SELECT id, val FROM strings WHERE id IN (") + ids_str + L")", {});
		{
			std::unique_lock<std::shared_mutex> write_lock(g_toValCacheLock);
			while (reader->read()) 
			{
				int64_t id = reader->getInt64(0);
				std::wstring val = reader->getString(1);
				ret_val[id] = val;
				g_toValCache[id] = val;
			}
		}
	}
	
	for (int64_t id : ids)
		if (ret_val.find(id) == ret_val.end())
			throw nldberr("strings::get_vals: String not found: " + std::to_string(id));

	return ret_val;
}

void strings::flush_caches()
{
	{
		std::unique_lock<std::shared_mutex> write_lock(g_toIdCacheLock);
		g_toIdCache.clear();
	}

	{
		std::unique_lock<std::shared_mutex> write_lock(g_toValCacheLock);
		g_toValCache.clear();
	}
}
