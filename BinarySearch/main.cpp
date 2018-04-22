#include "BinarySearch.h"
#include <assert.h>
#include <string>
#include <iostream>

int main() {
	std::vector<int> sortedArray{ 1, 2, 5, 7, 9, 13, 15, 21, 29, 32, 35 };
	std::vector<int> notExistArray{ -3, 3, 60 };

	// Even number of vals
	for (auto val : sortedArray) {
		assert(BinarySearch::Find(sortedArray, val) == true);
	}

	for (auto val : notExistArray) {
		assert(BinarySearch::Find(sortedArray, val) == false);
	}

	// Odd number of vals
	sortedArray.push_back(42);
	for (auto val : sortedArray) {
		assert(BinarySearch::Find(sortedArray, val) == true);
	}

	for (auto val : notExistArray) {
		assert(BinarySearch::Find(sortedArray, val) == false);
	}

	// Empty array
	sortedArray.clear();
	assert(BinarySearch::Find(sortedArray, 5) == false);

	// 1 value
	sortedArray.push_back(4);
	assert(BinarySearch::Find(sortedArray, 4) == true);
	for (auto val : notExistArray) {
		assert(BinarySearch::Find(sortedArray, val) == false);
	}

	// 2 values
	sortedArray.push_back(6);
	assert(BinarySearch::Find(sortedArray, 4) == true);
	assert(BinarySearch::Find(sortedArray, 6) == true);
	for (auto val : notExistArray) {
		assert(BinarySearch::Find(sortedArray, val) == false);
	}

	std::cout << "Woohoo! Passed!" << std::endl;
	return 0;
}