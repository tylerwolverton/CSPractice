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
		std::string tempFile = inFile.substr(0, ext) + "Encoded";// +inFile.substr(ext);

		if (Encoder::Encode(inFile, tempFile))
		{
			Decoder::Decode(tempFile, outFile);
		}
	}
	else if (argc == 4)
	{
		
	}
	else
	{
		std::cout << "Usage: Huffman [e|d] <input filepath> <output filepath>\n";
	}

    //Encoder::Encode(inFile, outFile);
    //Decoder::Decode(inFile, outFile);

	return 0;
}