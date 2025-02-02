#include "pch.h"
#include "dbreader.h"

namespace nldb
{
    bool dbreader::read()
    {
        if (m_doneReading)
            return false;

        int rc = sqlite3_step(m_stmt);
        if (rc == SQLITE_ROW)
        {
            return true;
        }
        else if (rc == SQLITE_DONE)
        {
            m_doneReading = true;
            return false;
        }
        else
            throw nldberr(rc, m_db);
    }

    std::wstring dbreader::getString(unsigned idx)
    {
        const unsigned char* str = sqlite3_column_text(m_stmt, idx);
        if (str != nullptr)
            return toWideStr(str);

        int columnType = sqlite3_column_type(m_stmt, idx);
        switch (columnType)
        {
        case SQLITE_INTEGER:
            return std::to_wstring(sqlite3_column_int64(m_stmt, idx));
        case SQLITE_FLOAT:
            return std::to_wstring(sqlite3_column_double(m_stmt, idx));
        case SQLITE_NULL:
            return L"null";
        case SQLITE_BLOB:
            return L"blob";
        default:
            throw nldberr("Unknown column type: " + std::to_string(columnType));
        }
    }

    strnum dbreader::getStrNum(unsigned idx, bool& isNull)
    {
        isNull = false;

        int columnType = sqlite3_column_type(m_stmt, idx);
        switch (columnType)
        {
        case SQLITE_INTEGER:
            return sqlite3_column_int64(m_stmt, idx);
        case SQLITE_FLOAT:
            return static_cast<int64_t>(sqlite3_column_double(m_stmt, idx));
        case SQLITE_NULL:
            isNull = true;
            return toWideStr("null");
        case SQLITE_BLOB:
            return toWideStr("blob");
        default:
            const unsigned char* str = sqlite3_column_text(m_stmt, idx);
            if (str != nullptr)
                return toWideStr(str);
            else
                throw nldberr("Unknown column type: " + std::to_string(columnType));
        }
    }
}
