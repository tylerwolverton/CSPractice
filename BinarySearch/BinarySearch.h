#pragma once

#include <vector>

class BinarySearch {
public:
	static bool Find(const std::vector<int>& searchArray, const int& targetVal);

private:
	static bool find(const std::vector<int>& searchArray, const int& targetVal, int startIdx, int endIdx);
};