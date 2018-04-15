//#include "HashTable.h"
//
//template<typename T, typename U>
//HashTable<T, U>::HashTable() 
//: m_tableSize(2048) {
//	m_hashTable.reserve(m_tableSize);
//}
//
//template<typename T, typename U>
//HashTable<T, U>::~HashTable() {
//
//}
//
//template<typename T, typename U>
//bool HashTable<T, U>::Insert(T key, U value) {
//	m_hashTable[hash(key)].push_back(std::make_pair(key, value));
//}
//
//template<typename T, typename U>
//U* HashTable<T, U>::Find(T key) {
//	std::vector<std::pair<T, U>> bucket = m_hashTable[hash(key)];
//
//	U* foundElem = nullptr;
//	for (auto element : bucket)	{
//		if (element.first() == key)	{
//			foundElem = &element.second();
//		}
//	}
//
//	return foundElem;
//}
//
//template<typename T, typename U>
//int HashTable<T, U>::hash(T key) {
//	int hash = 0;
//	for (auto c : key) {
//		hash += key[i] * i;
//	}
//
//	return hash;
//}