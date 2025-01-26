#pragma once

namespace nldb
{
	struct link 
	{
		int64_t m_id;
		int64_t m_fromNodeId;
		int64_t m_toNodeId;
		int64_t m_typeStringId;
		std::optional<std::wstring> m_payload;

		link(int64_t id = 0, int64_t fromNodeId = 0, int64_t toNodeId = 0, int64_t typeStringId = 0, const std::optional<std::wstring>& payload = std::nullopt)
			: m_id(id)
			, m_fromNodeId(fromNodeId)
			, m_toNodeId(toNodeId)
			, m_typeStringId(typeStringId)
			, m_payload(payload)
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
				m_typeStringId == other.m_typeStringId
				&&
				((!m_payload.has_value() || !other.m_payload.has_value()) || m_payload.value() == other.m_payload.value());
		}
		bool operator!=(const link& other) const {
			return !operator==(other);
		}
	};
}
