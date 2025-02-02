#pragma once

#include "node.h"
#include <vector>

inline bool has(const std::vector<nldb::node>& vec, int64_t id)
{
	for (size_t i = 0; i < vec.size(); ++i)
		if (vec[i].m_id == id)
			return true;
	return false;
}
