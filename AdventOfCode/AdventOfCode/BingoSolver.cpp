#include "BingoSolver.hpp"
#include <fstream>
#include <string>
#include <sstream>

int calledNumbers[] = { 94,21,58,16,4,1,44,6,17,48,20,92,55,36,40,63,62,2,47,7,46,72,85,24,66,49,34,56,98,41,84,23,86,64,28,90,39,97,73,81,12,69,35,26,75,8,32,77,52,50,5,96,14,31,70,60,29,71,9,68,19,65,99,57,54,61,33,91,27,78,43,95,42,3,88,51,53,30,89,87,93,74,18,15,80,38,82,79,0,22,13,67,59,11,83,76,10,37,25,45 };
int calledNumbersLen = sizeof(calledNumbers) / sizeof(calledNumbers[0]);

BingoCard5x5::BingoCard5x5(const std::vector<int>& boardNumbers)
{
	numbers = boardNumbers;
	isNumberMarked.resize( numbers.size(), false);
}

bool BingoCard5x5::MarkNumber(int newNumber)
{
	for (int numIdx = 0; numIdx < (int)numbers.size(); ++numIdx)
	{
		if (numbers[numIdx] == newNumber)
		{
			isNumberMarked[numIdx] = true;
			return CheckForWin();
		}
	}

	return false;
}

bool BingoCard5x5::CheckForWin()
{
	return false;
}

//int BingoCard5x5::GetRow(int index)
//{
//
//}
//
//int BingoCard5x5::GetColumn(int index)
//{
//
//}
//
//int BingoCard5x5::GetDiagonal(int index)
//{
//
//}

void BingoSolver::InitializeFromFile(const char* filename)
{
	std::ifstream inStream(filename);

	// Read off first 2 lines to get to boards
	std::string line;
	
	int precedingLines = 2;
	int curLine = 0;
	while (getline(inStream,line))
	{
		// Ignore first 2 lines to get to boards
		if (curLine < precedingLines)
		{
			++curLine;
			continue;
		}
		std::vector<int> boardNums;
		for (int rowNum = 0; rowNum < 5; ++rowNum)
		{
			std::istringstream iss(line);
			int a, b, c, d, e;
			iss >> a >> b >> c >> d >> e;
			boardNums.push_back(a);
			boardNums.push_back(b);
			boardNums.push_back(c);
			boardNums.push_back(d);
			boardNums.push_back(e);

			getline(inStream, line);
		}

		m_bingoCards.emplace_back(boardNums);

		// empty line
		getline(inStream, line);
	}

	inStream.close();
}

int BingoSolver::Solve()
{
	for (int calledNumIdx = 0; calledNumIdx < calledNumbersLen; ++calledNumIdx)
	{
		for (int cardIdx = 0; cardIdx < (int)m_bingoCards.size(); ++cardIdx)
		{
			if ( m_bingoCards[cardIdx].MarkNumber(calledNumbers[calledNumIdx]) )
			{
				return 1;
			}
		}
	}

	// Didn't find a match
	return -1;
}
