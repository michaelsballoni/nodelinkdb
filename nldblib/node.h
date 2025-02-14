#pragma once

namespace nldb
{
	struct node 
	{
		int64_t id;
		int64_t parentId;
		int64_t nameStringId;
		int64_t typeStringId;
		std::optional<std::wstring> payload;

		node(int64_t id = 0, int64_t parentId = 0, int64_t nameStringId = 0, int64_t typeStringId = 0, const std::optional<std::wstring>& payload = std::nullopt)
			: id(id)
			, parentId(parentId)
			, nameStringId(nameStringId)
			, typeStringId(typeStringId)
			, payload(payload)
		{
		}

		bool operator==(const node& other) const 
		{
			return
				id == other.id
				&&
				parentId == other.parentId
				&&
				nameStringId == other.nameStringId
				&&
				typeStringId == other.typeStringId;
		}
		bool operator!=(const node& other) const {
			return !operator==(other);
		}
	};
}
