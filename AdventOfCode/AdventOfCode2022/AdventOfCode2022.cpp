// AdventOfCode2022.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>

std::string ReadInputAsString(const std::string& filename)
{
	FILE* fp;
	fopen_s(&fp, filename.c_str(), "r");
	if (fp == nullptr)
	{
		return std::string();
	}

	// get size of file
	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);

	char* buffer = new char[fileSize + 1];
	// Guarantee or die not null
	if (buffer != nullptr)
	{
		fseek(fp, 0, SEEK_SET);
		uint32_t bytesRead = (uint32_t)fread(buffer, 1, fileSize, fp);
		buffer[bytesRead] = NULL;
	}

	std::string newString( buffer, (uint32_t)fileSize );
	
	fclose(fp);
	delete buffer;

	return newString;
}

struct ElfInventory
{
public:
	std::vector<int> calorieCounts;
	int totalCalories = 0;
};

void SolvePuzzle1()
{
	std::string inputStr = ReadInputAsString("./Input1.txt");

	bool inNum = false;

	std::vector<ElfInventory> elfInventories;
	ElfInventory curElfInventory;
	std::string curNumStr;

	for (char c : inputStr)
	{
		if (c >= '0' && c <= '9')
		{
			inNum = true;
			curNumStr +=c;
		}
		else if (c == '\n')
		{
			// End of number
			if (inNum)
			{
				inNum = false;

				int curNum = atoi(curNumStr.c_str());
				curNumStr.clear();

				curElfInventory.calorieCounts.push_back(curNum);
				curElfInventory.totalCalories += curNum;

			}
			// Blank line between elves
			else
			{
				elfInventories.push_back(curElfInventory);
				curElfInventory = ElfInventory();
			}
		}
	}

	ElfInventory maxElfInventory1;
	ElfInventory maxElfInventory2;
	ElfInventory maxElfInventory3;
	for (const ElfInventory& elfInventory : elfInventories)
	{
		if (elfInventory.totalCalories > maxElfInventory1.totalCalories)
		{
			maxElfInventory3 = maxElfInventory2;
			maxElfInventory2 = maxElfInventory1;
			maxElfInventory1 = elfInventory;
		}
		else if (elfInventory.totalCalories > maxElfInventory2.totalCalories)
		{
			maxElfInventory3 = maxElfInventory2;
			maxElfInventory2 = elfInventory;
		}
		else if (elfInventory.totalCalories > maxElfInventory3.totalCalories)
		{
			maxElfInventory3 = elfInventory;
		}
	}

	std::cout << "1. Total Calories: " << maxElfInventory1.totalCalories << std::endl;
	std::cout << "2. Total Calories: " << maxElfInventory1.totalCalories + maxElfInventory2.totalCalories + maxElfInventory3.totalCalories << std::endl;
}

int main()
{
    SolvePuzzle1();
}
