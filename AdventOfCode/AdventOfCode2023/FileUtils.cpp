#include "FileUtils.h"

#include <fstream>

std::vector<std::string> SplitFileByLine(const std::string& filePath)
{
	std::vector<std::string> lines;

	std::ifstream inFile(filePath);
	if (inFile.is_open())
	{
		std::string line;
		while (getline(inFile, line))
		{
			lines.emplace_back(line);
		}
		inFile.close();
	}

	return lines;
}

