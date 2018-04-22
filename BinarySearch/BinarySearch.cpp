#include "BinarySearch.h"
#include <iostream>

bool BinarySearch::Find(const std::vector<int>& searchArray, const int& targetVal) {
	if (searchArray.size() == 0) {
		return false;
	}
	
	return find(searchArray, targetVal, 0, searchArray.size() - 1);
}

bool BinarySearch::find(const std::vector<int>& searchArray, const int& targetVal, int startIdx, int endIdx) {
	// If the array only has 1 or 2 elems, check if one is the target
	if (endIdx - startIdx == 0) {
		if (searchArray[startIdx] == targetVal)
			return true;
		return false;
	}
	else if (endIdx - startIdx == 1) {
		if (searchArray[startIdx] == targetVal)
			return true;
		if (searchArray[endIdx] == targetVal)
			return true;
		return false;
	}

	int midIdx = (endIdx + startIdx) / 2;
	if (searchArray[midIdx] == targetVal) {
		return true;
	}
	else if (targetVal < searchArray[midIdx]) {
		return find(searchArray, targetVal, startIdx, midIdx);
	}
	else if (targetVal > searchArray[midIdx]) {
		return find(searchArray, targetVal, midIdx, endIdx);
	}
}