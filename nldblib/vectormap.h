#pragma once

#include <unordered_map>
#include <vector>

namespace nldb
{
    template <typename K, typename V>
    class vectormap
    {
	private:
		std::unordered_map<K, V> m_map;
		std::vector<std::pair<K, V>> m_vec;

	public:
        const std::vector<std::pair<K, V>>& vec() const { return m_vec; }
        const std::unordered_map<K, V>& map() const { return m_map; }
		size_t size() const { return m_vec.size(); }
		bool contains(const K& key) const { return m_map.find(key) != m_map.end(); }

        V get(const K& key) const
        {
            if (!contains(key))
                throw std::runtime_error("key not found");
			else
	            return m_map.find(key)->second;
        }

        void insert(const K& key, const V& val)
        {
            if (contains(key))
                throw std::runtime_error("key already exists");
            m_map.insert({ key, val });
            m_vec.emplace_back({ key, val });
        }

        bool tryGet(const K& key, V& val) const
        {
            auto it = m_map.find(key);
            if (it == m_map.end())
                return false;

            val = it->second;
            return true;
        }
   };
}
