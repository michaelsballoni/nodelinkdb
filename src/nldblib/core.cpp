#include "pch.h"
#include "core.h"

std::string nldb::nldberr::getExceptionMsg(int rc, sqlite3* db)
{
    std::string dbErrMsg;
    if (db != nullptr)
        dbErrMsg = sqlite3_errmsg(db);

    std::string retVal;
    if (dbErrMsg.empty())
        retVal = "SQLite error: " + std::to_string(rc);
    else
        retVal = "SQLite error: " + dbErrMsg + " (" + std::to_string(rc) + ")";
    return retVal;
}

void nldb::replace(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
    size_t pos;
    size_t offset = 0;
    const size_t fromSize = from.size();
    const size_t increment = to.size();
    while ((pos = str.find(from, offset)) != std::wstring::npos)
    {
        str.replace(pos, fromSize, to);
        offset = pos + increment;
    }
}

std::wstring nldb::toLower(const std::wstring& str)
{
	std::wstring ret_val;
	ret_val.reserve(str.length());
	for (wchar_t c : str)
		ret_val += towlower(c);
	return ret_val;

}

std::wstring nldb::join(const std::vector<std::wstring>& strs, const wchar_t* seperator)
{
    std::wstring retVal;
    for (const std::wstring& str : strs)
    {
        if (!retVal.empty())
            retVal += seperator;
        retVal += str;
    }
    return retVal;
}

std::vector<std::wstring> nldb::extractParamNames(const std::wstring& sql)
{
    std::vector<std::wstring> paramNames;
    std::wstring sb;
    size_t lookFrom = 0;
    while (true)
    {
        if (static_cast<unsigned>(lookFrom) >= sql.length())
            break;

        size_t at = sql.find('@', lookFrom);
        if (at == std::wstring::npos)
            break;

        sb.clear();
        size_t idx = at + 1;
        while (idx < sql.length())
        {
            wchar_t c = sql[idx++];
            if (iswalnum(c) || c == '_')
                sb += c;
            else
                break;
        }

        if (!sb.empty())
        {
            std::wstring paramName = L"@" + sb;
            if (std::find(paramNames.begin(), paramNames.end(), paramName) == paramNames.end())
                paramNames.emplace_back(paramName);
            sb.clear();
        }
        lookFrom = idx;
    }

    if (!sb.empty())
    {
        std::wstring paramName = L"@" + sb;
        if (std::find(paramNames.begin(), paramNames.end(), paramName) == paramNames.end())
            paramNames.emplace_back(paramName);
        sb.clear();
    }

    return paramNames;
}

std::wstring nldb::trim(const std::wstring& str)
{
    if (str.empty())
        return std::wstring();

    if (str.length() == 1)
        return iswspace(str[0]) ? std::wstring() : str;

    if (!iswspace(str.front()) && !iswspace(str.back()))
        return str;

    std::wstring retVal;
    retVal.reserve(str.length());

    // skip whitespace
    size_t c = 0;
    while (c < str.length() && iswspace(str[c]))
        ++c;

    // copy the rest
    while (c < str.length())
        retVal.push_back(str[c++]);

    // pop whitespace
    while (!retVal.empty() && iswspace(retVal.back()))
        retVal.pop_back();

    return retVal;
}