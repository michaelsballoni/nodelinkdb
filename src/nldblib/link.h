#pragma once

namespace nldb
{
	struct link 
	{
		int64_t m_id;
		int64_t m_fromNodeId;
		int64_t m_toNodeId;
		int64_t m_typeStringId;

		link(int64_t id = 0, int64_t fromNodeId = 0, int64_t toNodeId = 0, int64_t typeStringId = 0)
			: m_id(id)
			, m_fromNodeId(fromNodeId)
			, m_toNodeId(toNodeId)
			, m_typeStringId(typeStringId)
		{
		}

		bool operator==(const link& other) const
		{
			return
				m_id == other.m_id
				&&
				m_fromNodeId == other.m_fromNodeId
				&&
				m_toNodeId == other.m_toNodeId
				&&
				m_typeStringId == other.m_typeStringId;
		}
		bool operator!=(const link& other) const {
			return !operator==(other);
		}
	};
}
