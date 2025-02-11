#pragma once

#include "node.h"
#include <vector>

inline bool hasNode(const std::vector<nldb::node>& vec, int64_t id)
{
	for (size_t i = 0; i < vec.size(); ++i)
		if (vec[i].id == id)
			return true;
	return false;
}

inline bool hasCloudLink(const std::vector<nldb::cloudlink>& vec, int64_t id)
{
	for (size_t i = 0; i < vec.size(); ++i)
		if (vec[i].baseLink.id == id)
			return true;
	return false;
}
