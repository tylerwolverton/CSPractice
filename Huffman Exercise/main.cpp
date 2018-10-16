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
			if (!Decoder::Decode(tempFile, outFile))
			{
				std::cout << "Decoding Error\n";
			}
		}
		else
		{
			std::cout << "Encoding Error\n";
		}
	}
	else if (argc == 4)
	{
		std::string mode = argv[1];
		inFile = argv[2];
		outFile = argv[3];

		if (mode == "e")
		{
			if (!Encoder::Encode(inFile, outFile))
			{
				std::cout << "Encoding Error\n";
			}
		}
		else if (mode == "d")
		{
			if (!Decoder::Decode(inFile, outFile))
			{
				std::cout << "Decoding Error\n";
			}
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