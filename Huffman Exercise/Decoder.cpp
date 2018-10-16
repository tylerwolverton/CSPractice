#include"Decoder.h"

#include <iostream>
#include <fstream>
#include <stack>
#include <cstdint>
#include <cmath>

bool Decoder::Decode(std::string inFilePath, std::string outFilePath)
{
    // build tree from file
        // read in tree from file
	std::cout << "Decoding Tree from File\n";
    std::shared_ptr<TreeNode> root = std::make_shared<TreeNode>('!');
    std::streampos bytesRead = buildDataTree(inFilePath, root);
	if ((int)bytesRead == 0)
	{
		std::cout << "Didn't read any tree bytes in file\n";
		return false;
	}

    // read the remainder of the file and build output file
	std::cout << "Decoding Remainder of File\n";
	if (!buildOutputFile(inFilePath, outFilePath, root, bytesRead))
	{
		return false;
	}

	std::cout << "Finished Decoding\n";
    return true;
}

int Decoder::buildDataTree(std::string inFilePath, std::shared_ptr<Decoder::TreeNode> root)
{
    // read file into memory
    std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

    if (!inFile.is_open())
    {
		std::cout << "Input file couldn't be opened\n";
        return 0;
    }

	// create leaf nodes     list: time O(n) space O(n)  maximum for n is number of possible chars (256)
    std::stack<std::shared_ptr<TreeNode>> nodeStack;
    std::streampos maxReadSize = 2048;
    std::streampos fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    std::streampos totalBytesRead = 0;
    int treeBytes = 0;
    bool seenPrev0 = false;
	bool breakWhile = false;
    while (!breakWhile && totalBytesRead < fileSize)
    {
        std::streampos readChunkSize = maxReadSize;
        if (fileSize - totalBytesRead < readChunkSize)
        {
            readChunkSize = fileSize - totalBytesRead;
        }

        char* memblock = new char[readChunkSize];
        inFile.read(memblock, readChunkSize);
        totalBytesRead += readChunkSize;

        // build frequency tree one byte at a time
        for (int i = 0; i < readChunkSize; i++)
        {
			treeBytes++;

			// handle continuation of last chunk
			if (seenPrev0)
			{
				nodeStack.push(std::make_shared<TreeNode>(memblock[i]));
				seenPrev0 = false;
				continue;
			}

            // leaf node seen
            if (memblock[i] == 'L')
            {
				// handle last byte in chunk
				if (i + 1 == readChunkSize)
				{
					seenPrev0 = true;
					break;
				}

				// save leaf node
				i++;
				treeBytes++;
				nodeStack.push(std::make_shared<TreeNode>(memblock[i]));
            }
			// internal node seen
			else if (memblock[i] == 'I')
			{
				auto node1 = nodeStack.top();
				nodeStack.pop();
				auto node2 = nodeStack.top();
				nodeStack.pop();
				nodeStack.push(std::make_shared<TreeNode>('\0', node2, node1));
			}
			else if (memblock[i] == '!')
			{
				// end of tree seen
				breakWhile = true;
				break;
			}
            else
            {
				std::cout << "Unknown byte sequence in tree\n";
            }
        }

        delete[] memblock;
    }

    *root = *(nodeStack.top());
	// Return size plus one to account for null byte after end of tree
    return treeBytes + 1;
}

bool Decoder::buildOutputFile(std::string inFilePath, std::string outFilePath, std::shared_ptr<Decoder::TreeNode> root, std::streampos bytesRead)
{
    std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

    if (!inFile.is_open())
    {
		std::cout << "Input file couldn't be opened\n";
        return false;
    }

	// read number of padding bits from end of the file
	inFile.seekg(-1, std::ios::end);
	char* lastByte = new char[1];
	inFile.read(lastByte, 1);
	int paddingBits = 0;
	for (int i = 0; i < 8; i++)
	{
		auto bit = (lastByte[0] >> i) & 1;
		if (bit == 1)
		{
			paddingBits += std::pow(2, i);
		}
	}

    std::ofstream outfile;
    outfile.open(outFilePath, std::ios::out | std::ios::binary);

    std::stack<std::shared_ptr<TreeNode>> nodeStack;
    std::streampos maxReadSize = 2048;
    std::streampos fileSize = inFile.tellg() - bytesRead - sizeof(uint8_t);
    inFile.seekg(bytesRead, std::ios::beg);

    std::streampos totalBytesRead = 0;
    std::shared_ptr<TreeNode> curNode = root;
    while (totalBytesRead < fileSize)
    {
        std::streampos readChunkSize = maxReadSize;
        if (fileSize - totalBytesRead < readChunkSize)
        {
            readChunkSize = fileSize - totalBytesRead;
        }

        char* memblock = new char[readChunkSize];
        inFile.read(memblock, readChunkSize);
        totalBytesRead += readChunkSize;

        // read each byte and decode to file
        for (int i = 0; i < readChunkSize; i++)
        {
			int lastBitToReadPos = 0;
			// check if this is the last byte and account for padding bits
			if (totalBytesRead >= fileSize
				&& i == ((int)readChunkSize - 1)
				&& paddingBits != 0)
			{
				lastBitToReadPos = paddingBits;
			}

            for (int j = 7; j >= lastBitToReadPos; j--)
            {
				auto bit = (memblock[i] >> j) & 1;
				if (bit == 0 && curNode->left != nullptr)
                {
                    curNode = curNode->left;
                }
                else if (bit == 1 && curNode->right != nullptr)
                {
                    curNode = curNode->right;
                }

                if (curNode->left == nullptr
                    && curNode->right == nullptr)
                {
                    outfile.write(&(curNode->byteVal), 1);
                    curNode = root;
                }
            }
        }

        delete[] memblock;
    }

	return true;
}

void Decoder::printTree(std::shared_ptr<Decoder::TreeNode> root)
{
    if (root == nullptr)
        return;

    std::cout << " | " << root->byteVal << " |\n";
    printTree(root->left);
    printTree(root->right);
}