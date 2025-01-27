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

std::wstring nldb::num2str(double num)
{
    std::wstringstream ss;
    ss << num;
    return ss.str();
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

bool nldb::isWord(const std::wstring& str)
{
    if (str.empty())
        return false;

    if (!std::isalpha(str[0]))
        return false;

    for (wchar_t c : str)
    {
        if (!std::isalnum(c) && c != '_' && c != '-')
            return false;
    }

    return true;
}

std::wstring nldb::toLower(const std::wstring& str)
{
    std::wstring retVal;
    for (wchar_t c : str)
        retVal += towlower(c);
    return retVal;
}

bool nldb::isNameReserved(const std::wstring& name)
{
    static std::unordered_set<std::wstring> names
    {
        L"select",
        L"from",
        L"where",
        L"order",
        L"limit",
        L"value",
        L"id",
        L"count",
        L"created",
        L"lastmodified",
        L"rank"
    };
    return names.find(toLower(name)) != names.end();
}

bool nldb::isValidOp(const std::wstring& op)
{
    static std::unordered_set<std::wstring> ops
    {
        L"=",
        L"==",
        L"!=",
        L"<>",
        L">",
        L">=",
        L"!>",
        L"<",
        L"<=",
        L"!<",
        L"matches",
        L"like"
    };
    return ops.find(toLower(op)) != ops.end();
}

std::wstring nldb::cleanseName(const std::wstring& name)
{
    std::wstring clean;
    for (auto c : name)
    {
        if (iswalnum(c))
            clean += c;
    }

    if (clean.empty())
        clean = L"a";

    return clean;
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
