#pragma once

#include "core.h"

namespace nldb
{
    class strnum
    {
	private:
		bool m_isStr;
		double m_num;
		std::wstring m_str;

	public:
        strnum() : m_isStr(false), m_str(L"0"), m_num(0.0) {}
        strnum(const std::wstring& str) : m_isStr(true), m_str(str), m_num(0.0) {}
        strnum(double num) : m_isStr(false), m_str(std::to_wstring(num)), m_num(num) {}

        bool operator==(const strnum& other) const
        {
            return m_isStr == other.m_isStr && m_str == other.m_str;
        }

        const std::wstring& str() const
        {
            if (!m_isStr)
                throw std::runtime_error("not a string");
			else
	            return m_str;
        }

        double num() const
        {
            if (m_isStr)
                throw std::runtime_error("not a number");
			else
	            return m_num;
        }

        std::wstring toSqlLiteral() const
        {
            if (m_isStr)
            {
                std::wstring retVal = m_str; // replace modifies param in place
                replace(retVal, L"\'", L"\'\'");
                retVal = L"'" + retVal + L"'";
                return retVal;
            }
            else
            {
                return num2str(m_num);
            }
        }
    };
}

namespace std
{
    template <>
    struct hash<nldb::strnum>
    {
        std::size_t operator()(const nldb::strnum& sn) const
        {
            const std::hash<std::wstring> hasher;
            return hasher(sn.str());
        }
    };
}
