#pragma once

#include "core.h"
#include "strnum.h"

namespace nldb
{
    class dbreader
    {
	private:
		sqlite3* m_db;
		sqlite3_stmt* m_stmt;
		bool m_doneReading;

	public:
		dbreader(sqlite3* db, const std::wstring& sql)
			: m_db(db)
			, m_stmt(nullptr)
			, m_doneReading(false)
		{
			int rc = sqlite3_prepare_v3(m_db, toNarrowStr(sql).c_str(), -1, 0, &m_stmt, nullptr);
			if (rc != SQLITE_OK)
				throw nldberr(rc, db);
		}

		~dbreader()
		{
			sqlite3_finalize(m_stmt);
		}

		unsigned getColCount()
		{
			return static_cast<unsigned>(sqlite3_column_count(m_stmt));
		}

		std::wstring getColName(unsigned idx)
		{
			return toWideStr(sqlite3_column_name(m_stmt, idx));
		}

		double getDouble(unsigned idx)
		{
			return sqlite3_column_double(m_stmt, idx);
		}

		int64_t getInt64(unsigned idx)
		{
			return sqlite3_column_int64(m_stmt, idx);
		}

		int getInt32(unsigned idx)
		{
			return sqlite3_column_int(m_stmt, idx);
		}

		bool getBoolean(unsigned idx)
		{
			return getInt32(idx) != 0;
		}

		bool isNull(unsigned idx)
		{
			return sqlite3_column_type(m_stmt, idx) == SQLITE_NULL;
		}

        bool read();
        std::wstring getString(unsigned idx); // best-effort string conversion
		strnum getStrNum(unsigned idx, bool& isNull);
	};
}
