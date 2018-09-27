#include"Decoder.h"

#include <iostream>
#include <fstream>
#include <stack>
#include <cstdint>
#include <cmath>

bool Decoder::Decode(std::string inFilePath, std::string outFilePath)
{
    // Build tree from file
        // Read in tree from file
	std::cout << "Decoding Tree from File\n";
    std::shared_ptr<TreeNode> root = std::make_shared<TreeNode>('!');
    std::streampos bytesRead = buildDataTree(inFilePath, root);
	if ((int)bytesRead == 0)
	{
		return false;
	}

    // Read the remainder of the file and build output file
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
    // If you read a leaf, put it on a stack, if you read a	node, take 2	
    // children	from the stack and put the node back on

    // create leaf nodes                        list: time O(n) space O(n)  maximum for n is number of possible chars
    //std::stack<std::shared_ptr<TreeNode>> nodeStack;

    // read file into memory
    std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

    if (!inFile.is_open())
    {
        std::cout << "File isn't open\n";
        return 0;
    }

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

			// Handle continuation of last chunk
			if (seenPrev0)
			{
				std::cout << "Finishing last chunk\n";
				nodeStack.push(std::make_shared<TreeNode>(memblock[i]));
				seenPrev0 = false;
				continue;
			}

			//std::cout << "Processing tree byte: " << treeBytes << std::endl;
            // Leaf node seen
            if (memblock[i] == 'L')
            {
				//std::cout << "Adding leaf node.\n";

				// Handle last byte in chunk
				if (i + 1 == readChunkSize)
				{
					std::cout << "Last byte in chunk\n";
					seenPrev0 = true;
					break;
				}

				// Save leaf node
				i++;
				treeBytes++;
				nodeStack.push(std::make_shared<TreeNode>(memblock[i]));
				//std::cout << "Leaf node: " << memblock[i] << " NodeStack size: " << nodeStack.size() << std::endl;
            }
			// Internal node seen
			else if (memblock[i] == 'I')
			{
				//std::cout << "Adding internal node. NodeStack size: " << nodeStack.size() << std::endl;
				auto node1 = nodeStack.top();
				nodeStack.pop();
				auto node2 = nodeStack.top();
				nodeStack.pop();
				nodeStack.push(std::make_shared<TreeNode>('\0', node2, node1));
			}
			else if (memblock[i] == '!')
			{
				//std::cout << "End of tree\n";
				// End of tree seen
				breakWhile = true;
				break;
			}
            else
            {
				std::cout << "Unknown byte sequence in tree.\n";
            }
        }

        delete[] memblock;
    }

    //root = (nodeStack.top());
    *root = *(nodeStack.top());
    return treeBytes + 1;
}


bool Decoder::buildOutputFile(std::string inFilePath, std::string outFilePath, std::shared_ptr<Decoder::TreeNode> root, std::streampos bytesRead)
{
    std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

    if (!inFile.is_open())
    {
        std::cout << "File isn't open\n";
        return false;
    }

	// Read number of padding bits from end of the file
	inFile.seekg(-1, std::ios::end);
	char* lastByte = new char[1];
	inFile.read(lastByte, 1);
	int paddingBits = 0;
	//inFile >> paddingBits;
	for (int i = 0; i < 8; i++)
	{
		auto bit = (lastByte[0] >> i) & 1;
		//std::cout << "bit: " << bit << std::endl;
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
	int bitsToRead = fileSize * 8 - paddingBits;
    std::shared_ptr<TreeNode> curNode = root;
    //std::vector<unsigned char> memblock;
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
            //std::cout << "Processing byte\n";
            for (int j = 7; j >= 0 && bitsToRead > 0; j--)
            {
				--bitsToRead;

				auto bit = (memblock[i] >> j) & 1;
				//std::cout << "bit: " << bit << std::endl;
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
                    //std::cout << "writing byte: " << curNode->byteVal << std::endl;
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