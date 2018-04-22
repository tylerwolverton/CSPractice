#include "BinarySearch.h"
#include <assert.h>
#include <string>
#include <iostream>

int main() {
	std::vector<int> sortedArray {1, 2, 5, 7, 9, 13, 15, 21, 29, 32, 35};

	// Even number of vals
	for (auto val : sortedArray) {
		assert(BinarySearch::Find(sortedArray, val) == true);
	}

	assert(BinarySearch::Find(sortedArray, -3) == false);
	assert(BinarySearch::Find(sortedArray, 3) == false);
	assert(BinarySearch::Find(sortedArray, 60) == false);

	// Odd number of vals
	sortedArray.push_back(42);
	for (auto val : sortedArray) {
		assert(BinarySearch::Find(sortedArray, val) == true);
	}

	assert(BinarySearch::Find(sortedArray, -3) == false);
	assert(BinarySearch::Find(sortedArray, 3) == false);
	assert(BinarySearch::Find(sortedArray, 60) == false);

	sortedArray.clear();
	assert(BinarySearch::Find(sortedArray, 5) == false);

	std::cout << "Woohoo! Passed!" << std::endl;
	return 0;
}