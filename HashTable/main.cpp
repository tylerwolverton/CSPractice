#include "HashTable.h"
#include <string>
#include <iostream>

int main() {
	HashTable<std::string> map;

	map.Insert("Tyler", "Wolverton");
	map.Insert("Lee Anne", "Opfer");
	map.Insert("Brian", "Wolverton4");

	std::string last = map.Find("Tyler");
	if (last.length() > 0) {
		std::cout << last <<std::endl;
	}

	map.Remove("Tyler");
	map.Remove("Tyler");
	map.Insert("Kylie", "Wolverton2");
	map.Insert("Lucky", "Wolverton3");

	last = map.Find("Tyler");
	if (last.length() > 0) {
		std::cout << last << std::endl;
	}

	last = map.Find("Lee Anne");
	if (last.length() > 0) {
		std::cout << last << std::endl;
	}
	last = map.Find("Kylie");
	if (last.length() > 0) {
		std::cout << last << std::endl;
	}

	/*HashTable<int, std::string> map2;

	map2.Insert(12, "Wolverton");

	std::string last2 = map2.Find(12);
	if (last2.length() > 0) {
		std::cout << last2 << std::endl;
	}*/

	return 0;
}