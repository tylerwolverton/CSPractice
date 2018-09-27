#include "Encoder.h"
#include "Decoder.h"
#include <string>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) 
{
	std::string inFile, outFile;

	if (argc == 3)
	{
		inFile = argv[1];
		outFile = argv[2];

		// Write temporary encoded file to "<inFile>Encoded"
		std::size_t ext = inFile.find_last_of(".");
		std::string tempFile = inFile.substr(0, ext) + "Encoded";

		if (Encoder::Encode(inFile, tempFile))
		{
			Decoder::Decode(tempFile, outFile);
		}
	}
	else if (argc == 4)
	{
		std::string mode = argv[1];
		inFile = argv[2];
		outFile = argv[3];

		if (mode == "e")
		{
			Encoder::Encode(inFile, outFile);
		}
		else if (mode == "d")
		{
			Decoder::Decode(inFile, outFile);
		}
		else
		{
			std::cout << "Usage: Huffman [e|d] <input filepath> <output filepath>\n";
		}
	}
	else
	{
		std::cout << "Usage: Huffman [e|d] <input filepath> <output filepath>\n";
	}

	return 0;
}