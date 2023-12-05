#include "Challenges.h"
#include "FileUtils.h"

#include <iostream>

int charToInt(char c)
{
	return c - 48;
}

struct CharacterNode
{
public:
	char character = 0;
	std::vector<CharacterNode*> nextNodes;

public:
	CharacterNode(char c) :character(c) {}

	bool IsLeaf()
	{
		return nextNodes.empty();
	}
};

class WordChecker
{
public:
	WordChecker(const std::vector<std::string>& dictionary)
	{
		m_root = new CharacterNode(0);

		for (const std::string& word : dictionary)
		{
			AddWord(word);
		}
	}

	bool FindWord(const std::string& searchText, int startIdx, int& lengthSearched)
	{
		int curIdx = startIdx;
		bool foundWord = false;
		CharacterNode* curNode = m_root;
		while (curIdx < searchText.size())
		{
			bool foundChar = false;

			for (const auto& node : curNode->nextNodes)
			{
				if (node->character == searchText[curIdx])
				{
					curNode = node;
					++curIdx;
					foundChar = true;
					break;
				}
			}

			if (!foundChar)
			{
				if (curIdx - startIdx == 0)
				{
					++curIdx;
				}
				break;
			}

			if (curNode->IsLeaf())
			{
				foundWord = true;
				break;
			}
		}

		lengthSearched = curIdx - startIdx;
		return foundWord;
	}

	void DebugPrint()
	{
		PrintNode(m_root);
	}

private:
	void AddWord(const std::string& newWord)
	{
		if (newWord.empty())
		{
			std::cout << "Don't add empty words to dictionary :|\n";
			return;
		}

		int curCharIdx = 0;
		CharacterNode* curNode = m_root;
		
		while (curCharIdx < newWord.size())
		{
			bool foundChar = false;

			// Try to find existing node
			for (CharacterNode* c : curNode->nextNodes)
			{
				if (c->character == newWord[curCharIdx])
				{
					curNode = c;
					++curCharIdx;
					
					foundChar = true;
					break;
				}
			}

			if (foundChar)
			{
				continue;
			}

			// Add new node for this char
			CharacterNode* newNode = new CharacterNode(newWord[curCharIdx]);
			curNode->nextNodes.emplace_back(newNode);
			
			curNode = newNode;
			++curCharIdx;
		}
	}

	void PrintNode(CharacterNode* node)
	{
		for (const auto& node : node->nextNodes)
		{
			PrintNode(node);
		}

		std::cout << node->character;
	}

private:
	CharacterNode* m_root = nullptr;
};

int strToDigit(const std::string& digitWord)
{
	if (digitWord == "zero") return 0; 
	if (digitWord == "one") return 1; 
	if (digitWord == "two") return 2; 
	if (digitWord == "three") return 3; 
	if (digitWord == "four") return 4; 
	if (digitWord == "five") return 5; 
	if (digitWord == "six") return 6; 
	if (digitWord == "seven") return 7; 
	if (digitWord == "eight") return 8; 
	if (digitWord == "nine") return 9;

	return 0;
}

int SolveDay1()
{
	WordChecker wordChecker(std::vector<std::string>{ "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" });

	std::vector<std::string> inputStrings = SplitFileByLine("./Day1.txt");

	int sumOfValues = 0;

	for (const std::string& line : inputStrings)
	{
		int firstDigit = -1;
		int lastDigit = -1;

		int lineIdx = 0;
		while (lineIdx < line.size())
		{
			int nextDigit = -1;

			if (line[lineIdx] >= '0' && line[lineIdx] <= '9')
			{
				nextDigit = charToInt(line[lineIdx]);
				++lineIdx;
			}
			else
			{
				int lengthSearched = 0;
				if (wordChecker.FindWord(line, lineIdx, lengthSearched))
				{
					std::string digitWord = line.substr(lineIdx, lengthSearched);
					nextDigit = strToDigit(digitWord);
				}
				lineIdx++;
			}

			if (nextDigit == -1)
			{
				continue;
			}

			if (firstDigit == -1)
			{
				if (nextDigit == 0)
				{
					continue;
				}

				firstDigit = nextDigit;
			}

			lastDigit = nextDigit;
		}

		int newValue = firstDigit * 10 + lastDigit;
		std::cout <<line <<": " <<newValue <<"\n";
		sumOfValues += newValue;
	}

	return sumOfValues;
}
