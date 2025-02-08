#pragma once

#include "core.h"

#include "strnum.h"
#include "vectormap.h"

#include "dbreader.h"

namespace nldb
{
    typedef std::unordered_map<std::wstring, strnum> paramap;

    class db
	{
	private:
		sqlite3* m_db;

	public:
		db(const std::string& filePath) : m_db(nullptr)
		{
			int rc = sqlite3_open(filePath.c_str(), &m_db);
			if (rc != SQLITE_OK)
				throw nldberr(rc, m_db);
		}

		~db()
		{
			if (m_db != nullptr)
			{
				int rc = sqlite3_close(m_db);
				(void)rc; // release build
				assert(rc == SQLITE_OK);
			}
		}

		std::unique_ptr<dbreader> execReader(const std::wstring& sql, const paramap& params)
		{
			return std::make_unique<dbreader>(m_db, applyParams(sql, params));
		}

		int execSql(const std::wstring& sql, const paramap& params)
		{
			int row_count = 0;
			{
				auto reader = execReader(sql, params);
				while (reader->read())
					++row_count;
			}
			return row_count > 0 ? row_count : execScalarInt32(L"SELECT changes()", paramap()).value();
		}

		std::optional<int> execScalarInt32(const std::wstring& sql, const paramap& params)
		{
			auto reader = execReader(sql, params);
			if (reader->read())
				return reader->getInt32(0);
			else
				return std::nullopt;
		}

		std::optional<int64_t> execScalarInt64(const std::wstring& sql, const paramap& params)
		{
			auto reader = execReader(sql, params);
			if (reader->read())
				return reader->getInt64(0);
			else
				return std::nullopt;
		}

		std::optional<std::wstring> execScalarString(const std::wstring& sql, const paramap& params)
		{
			auto reader = execReader(sql, params);
			if (reader->read())
				return reader->getString(0);
			else
				return std::nullopt;
		}

		int64_t execInsert(const std::wstring& sql, const paramap& params)
		{
			execSql(sql, params);
			return execScalarInt64(L"select last_insert_rowid()", paramap()).value();
		}

		std::wstring applyParams(const std::wstring& sql, const paramap& params)
		{
			std::wstring ret_val = sql;
			for (const auto& it : params)
				replace(ret_val, it.first, it.second.toSqlLiteral());
			return ret_val;
		}
	};
}
