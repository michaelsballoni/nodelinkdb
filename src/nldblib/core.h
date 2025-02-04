// Utility functions used throughout the class library and unit tests.
#pragma once

#pragma warning(disable : 4996) // doesn't work when camped in includes.h...so here it is...
#pragma warning(disable : 4005) // thanks SQLite!

#include "includes.h"

namespace nldb
{
    class nldberr : public std::runtime_error
    {
    public:
		nldberr(const std::string& msg) : std::runtime_error(msg) {}
		nldberr(int rc, sqlite3* db) : std::runtime_error(getExceptionMsg(rc, db)) {}
    private:
        static std::string getExceptionMsg(int rc, sqlite3* db);
    };

    inline std::string toNarrowStr(const std::wstring& str)
    {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(str);
    }

    inline std::wstring toWideStr(const std::string& str)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(str);
    }

    inline std::wstring toWideStr(const void* bytes)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(reinterpret_cast<const char*>(bytes));
    }

	void replace(std::wstring& str, const std::wstring& from, const std::wstring& to);
	std::wstring toLower(const std::wstring& str);
    std::wstring join(const std::vector<std::wstring>& strs, const wchar_t* seperator);
    std::vector<std::wstring> extractParamNames(const std::wstring& sql);
    std::wstring trim(const std::wstring& str);
}
