#include "Encoder.h"

#include <iostream>
#include <fstream>
#include <cstdint>
#include <climits>

bool Encoder::Encode(const std::string& inFilePath, const std::string& outFilePath) 
{
    // build frequency table for each byte
    std::cout << "Building Frequency Table\n";
	FreqTablePtr freqTable = buildFrequencyTable(inFilePath);

	if (freqTable == nullptr)
	{
		return false;
	}

	// assign bit sequence to each char              
	std::cout << "Building Frequency Tree\n";
    std::shared_ptr<TreeNode> root = buildFrequencyTree(freqTable);

	// store the bit representation of each leaf
	// hash table: time O(nlogn) space: none (reuse frequency table)
	std::cout << "Saving Bit Representations\n";
    saveBitReps(root, "", freqTable);

	// write tree into start of file
	std::cout << "Writing Tree to File\n";
	if (!writeTreeToFile(root, outFilePath))
	{
		return false;
	}

	// write out compressed bits to new file
	std::cout << "Writing Bits to File\n";
	if (!writeDataToFile(inFilePath, outFilePath, freqTable))
	{
		return false;
	}

	std::cout << "Finished Encoding\n";
    return true;
}

Encoder::FreqTablePtr Encoder::buildFrequencyTable(const std::string& inFilePath)
{
	// read file into memory
	std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

	if (!inFile.is_open())
	{
		std::cout << "Input file couldn't be opened\n";
		return nullptr;
	}

    FreqTablePtr freqTable = std::make_shared<std::unordered_map<char, FrequencyNode> >();
    std::streampos maxReadSize = 2048;
    std::streampos fileSize = inFile.tellg();

	if (fileSize < 1)
	{
		std::cout << "File is empty\n";
		return nullptr;
	}

    inFile.seekg(0, std::ios::beg);

	std::streampos totalBytesRead = 0;
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

        // build frequency table for each byte
		for (int i = 0; i < readChunkSize; i++)
		{
            auto it = freqTable->find(memblock[i]);
            if (it == freqTable->end())
            {
                FrequencyNode node(1, "");
                freqTable->emplace(memblock[i], node);
            }
            else
            {
                it->second.count++;
            }
		}

        delete[] memblock;
	}

	inFile.close();

    return freqTable;
}

std::shared_ptr<Encoder::TreeNode> Encoder::buildFrequencyTree(const FreqTablePtr& freqTable)
{
    // create leaf nodes            
    // list: time O(n) space O(n) - maximum for n is number of possible chars (256)
    std::list<std::shared_ptr<TreeNode>> nodeList;
    for (auto it = freqTable->begin(); it != freqTable->end(); ++it)
    {
        nodeList.push_back(std::make_shared<TreeNode>(it->first, it->second.count));
    }

    // build tree 
	// time: O((n ^ 2) / 2) space : O(n ^ 2)
    while (nodeList.size() > 1)
    {
        int min = INT_MAX - 1;
        int secondMin = INT_MAX;
        std::shared_ptr<TreeNode> minNode = nullptr, secondMinNode = nullptr;

        for (auto node : nodeList)
        {
            if (node->frequency < min)
            {
				// update second min node to be the current min
				if (minNode != nullptr)
				{
					secondMin = minNode->frequency;
					secondMinNode = minNode;
				}

				// save the new min
                min = node->frequency;
                minNode = node;
            }
			else if (node->frequency < secondMin)
			{
				secondMin = node->frequency;
				secondMinNode = node;
			}
        }

		// make a new internal node with '\0' as the char value
        nodeList.push_back(std::make_shared<TreeNode>('\0', minNode->frequency + secondMinNode->frequency, minNode, secondMinNode));
        nodeList.remove(minNode);
        nodeList.remove(secondMinNode);
    }

    return nodeList.front();
}

void Encoder::saveBitReps(const std::shared_ptr<TreeNode>& node, const std::string& bitStr, const FreqTablePtr& freqTable)
{
    if (node->left == nullptr
        && node->right == nullptr)
    {
        auto it = freqTable->find(node->byteVal);
		// if only 1 node is in the list, set its value to 0
		if (bitStr == "")
		{
			it->second.bitRep = "0";
		}
		else
		{
			it->second.bitRep = bitStr;
		}
        return;
    }

    if (node->left != nullptr)
    {
        saveBitReps(node->left, bitStr + '0', freqTable);
    }

    if (node->right != nullptr)
    {
        saveBitReps(node->right, bitStr + '1', freqTable);
    }
}

bool Encoder::writeTreeToFile(const std::shared_ptr<TreeNode>& node, const std::string& outFilePath)
{
    std::shared_ptr<std::string> treeStrPtr = std::make_shared<std::string>();
    buildTreeString(node, treeStrPtr);

    std::ofstream outfile;
    outfile.open(outFilePath, std::ios::out | std::ios::binary);

    if (outfile.is_open())
    {
        outfile.write((*treeStrPtr).c_str(), (*treeStrPtr).size());
		// write marker for end of tree
        outfile.write("!", sizeof("!"));
		return true;
    }

	std::cout << "Output file couldn't be opened\n";
    return false;
}

void Encoder::buildTreeString(const std::shared_ptr<TreeNode>& node, const std::shared_ptr<std::string>& treeStrPtr)
{
    if (node->left != nullptr)
    {
        buildTreeString(node->left, treeStrPtr);
    }
    if (node->right != nullptr)
    {
        buildTreeString(node->right, treeStrPtr);
    }

	// check for internal node marker '\0'
    if (node->byteVal == '\0' 
		&& (node->left != nullptr
		|| node->right != nullptr))
	{
        (*treeStrPtr) += 'I';
    }
    else
    {
		(*treeStrPtr) += 'L';
        (*treeStrPtr) += node->byteVal;
    }
}

bool Encoder::writeDataToFile(const std::string& inFilePath, const std::string& outFilePath, const FreqTablePtr& freqTable)
{
    // read file into memory
    std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

    if (!inFile.is_open())
    {
        std::cout << "Input file couldn't be opened\n";
        return false;
    }

    std::ofstream outfile;
    outfile.open(outFilePath, std::ios::out | std::ios::binary | std::ios::ate | std::ios::app);

    std::streampos maxReadSize = 2048;
    std::streampos fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    std::streampos totalBytesRead = 0;
    char byteToWrite = 0;
	uint8_t bitsRead = 0;
    const uint8_t BIT_COUNT = 8;
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

        // write each bit to file in 1 byte chunks
        for (int i = 0; i < readChunkSize; i++)
        {
            auto it = freqTable->find(memblock[i]);
            for (char bit : it->second.bitRep)
            {
                if (bitsRead == BIT_COUNT)
                {
                    outfile.write(&byteToWrite, 1);
                    byteToWrite = 0;
                    bitsRead = 0;
                }

                byteToWrite <<= 1;
                if(bit == '1')
                {
                    byteToWrite |= 1;
                }

                ++bitsRead;
            }
        }

        delete[] memblock;
    }

    // write out any remaining bits
    if (bitsRead != 0)
    {
		char paddingBits = BIT_COUNT - bitsRead;
        for (int i = 0; i < paddingBits; i++)
        {
            byteToWrite <<= 1;
        }
    
        outfile.write(&byteToWrite, 1);

		// write number of padding bits to be removed in decode
		outfile.write(&paddingBits, 1);
    }
	// no padding bits to ignore during decode
	else
	{
		outfile.write(reinterpret_cast<const char*>(&bitsRead), sizeof(uint8_t));
	}
    
    inFile.close();
    outfile.close();

    return true;
}

// debug method to print the encoding tree
void Encoder::printTree(const std::shared_ptr<TreeNode>& root, int depth)
{
    if (root == nullptr)
        return;

    std::cout << " | " << root->byteVal << " : " << root->frequency << " depth: " <<depth <<" |\n";
    printTree(root->left, depth + 1);
    printTree(root->right, depth + 1);
}
