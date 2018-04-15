#pragma once
#include <vector>
#include <utility>

template<typename T>
class HashTable {

public:
    HashTable()
		: m_bucketCount(2048),
		  m_tableSize(0),
		  m_hashTable() {
		
		m_hashTable.reserve(m_bucketCount);

		for (int i = 0; i < m_bucketCount; i++) {
			m_hashTable.emplace_back();
		}
	}

	~HashTable() {
		for (int i = 0; i < m_bucketCount; i++) {
			m_hashTable[i].clear();
		}

		m_hashTable.clear();
	}
    
    void Insert(std::string key, T value) {
		m_hashTable[hash(key)].push_back(std::make_pair(key, value));
	}

	T Find(std::string key) {
		const std::vector<std::pair<std::string, T>>& bucket = m_hashTable[hash(key)];

		T foundElem;
		for (auto element : bucket) {
			if (element.first == key) {
				foundElem = element.second;
				break;
			}
		}

		return foundElem;
	}

	bool Remove(std::string key) {
		std::vector<std::pair<std::string, T>>& bucket = m_hashTable[hash(key)];

		int pos = 0;
		for (auto element : bucket) {
			if (element.first == key) {
				break;
			}
			pos++;
		}

		if (pos != bucket.size()) {
			bucket.erase(bucket.begin() + pos);
			return true;
		}

		return false;
	}

private:
	std::vector<std::vector<std::pair<std::string, T>>> m_hashTable;
	int m_bucketCount;
	int m_tableSize;

	int hash(std::string key) {
		int hash = 0;
		int i = 1;
		for (auto c : key) {
			hash += c * i;
			i++;
		}

		return hash % m_bucketCount;
	}

};