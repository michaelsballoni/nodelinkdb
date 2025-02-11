#pragma once

namespace nldb
{
	struct link 
	{
		int64_t id;
		int64_t fromNodeId;
		int64_t toNodeId;
		int64_t typeStringId;
		std::optional<std::wstring> payload;

		link(int64_t id = 0, int64_t fromNodeId = 0, int64_t toNodeId = 0, int64_t typeStringId = 0, const std::optional<std::wstring>& payload = std::nullopt)
			: id(id)
			, fromNodeId(fromNodeId)
			, toNodeId(toNodeId)
			, typeStringId(typeStringId)
			, payload(payload)
		{
		}

		bool operator==(const link& other) const
		{
			return
				id == other.id
				&&
				fromNodeId == other.fromNodeId
				&&
				toNodeId == other.toNodeId
				&&
				typeStringId == other.typeStringId
				&&
				((!payload.has_value() || !other.payload.has_value()) || payload.value() == other.payload.value());
		}
		bool operator!=(const link& other) const {
			return !operator==(other);
		}
	};
}
