#pragma once

namespace nldb
{
	struct node 
	{
		int64_t m_id;
		int64_t m_parentId;
		int64_t m_nameStringId;
		int64_t m_typeStringId;

		node(int64_t id = 0, int64_t parentId = 0, int64_t nameStringId = 0, int64_t typeStringId = 0)
			: m_id(id)
			, m_parentId(parentId)
			, m_nameStringId(nameStringId)
			, m_typeStringId(typeStringId)
		{
		}

		bool operator==(const node& other) const 
		{
			return
				m_id == other.m_id
				&&
				m_parentId == other.m_parentId
				&&
				m_nameStringId == other.m_nameStringId
				&&
				m_typeStringId == other.m_typeStringId;
		}
		bool operator!=(const node& other) const {
			return !operator==(other);
		}
	};
}
