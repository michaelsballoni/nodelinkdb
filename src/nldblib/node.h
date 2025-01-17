#pragma once

namespace nldb
{
	struct node {
		int64_t m_id;
		int64_t m_parent_id;
		int64_t m_name_string_id;
		int64_t m_type_string_id;

		node(int64_t id = 0, int64_t parent_id = 0, int64_t name_string_id = 0, int64_t type_string_id = 0)
			: m_id(id)
			, m_parent_id(parent_id)
			, m_name_string_id(name_string_id)
			, m_type_string_id(type_string_id)
		{
		}

		bool operator==(const node& other) const 
		{
			return
				m_id == other.m_id
				&&
				m_parent_id == other.m_parent_id
				&&
				m_name_string_id == other.m_name_string_id
				&&
				m_type_string_id == other.m_type_string_id;
		}
		bool operator!=(const node& other) const {
			return !operator==(other);
		}
	};
}
