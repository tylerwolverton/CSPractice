#include "Encoder.h"

#include <iostream>
#include <fstream>
#include <memory>

//Encoder::Encoder() 
//{
//
//}
//
//Encoder::~Encoder() 
//{
//
//}

bool Encoder::Encode(std::string inFilePath, std::string outFilePath) 
{
    // build frequency table for each byte          hash table: time: O(n) space: O(n)
    std::cout << "Building Frequency Table\n";
	std::shared_ptr<std::unordered_map<char, FrequencyNode> > freqTablePtr = buildFrequencyTable(inFilePath);

    for (auto it = freqTablePtr->begin(); it != freqTablePtr->end(); ++it)
    {
        std::cout << "byte: " << it->first << " count: " << it->second.count << std::endl;
    }

	// assign bit sequence to each char  
        // create leaf nodes                        list: time O(n) space O(n)  maximum for n is number of possible chars from this point on
        // build tree                               time: O((n^2)/2) space: O((n^2)/2)
    std::shared_ptr<TreeNode> root = buildFrequencyTree(freqTablePtr);

    printTree(root);

	// store the bit representation of each leaf    hash table: time O(nlogn) space: none if reuse frequency table


	// write tree into start of file                time: O((n^2)/2)

	// write out compressed bits to new file        time: O(n)
    return true;
}

std::shared_ptr<std::unordered_map<char, Encoder::FrequencyNode> > Encoder::buildFrequencyTable(std::string inFilePath)
{
	// read file into memory
	std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

	if (!inFile.is_open())
	{
        std::cout << "File isn't open\n";
		return nullptr;
	}

	std::shared_ptr<std::unordered_map<char, FrequencyNode> > freqTablePtr = std::make_shared<std::unordered_map<char, FrequencyNode> >();
    std::streampos maxReadSize = 2048;
    std::streampos fileSize = inFile.tellg();
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
            auto it = freqTablePtr->find(memblock[i]);
            if (it == freqTablePtr->end())
            {
                //std::cout << "New byte\n";
                FrequencyNode node(1, "");
                freqTablePtr->emplace(memblock[i], node);
            }
            else
            {
                //std::cout << "Update count\n";
                it->second.count++;
            }
		}

		delete[] memblock;
	}

	inFile.close();

    return freqTablePtr;
}

 std::shared_ptr<Encoder::TreeNode> Encoder::buildFrequencyTree(std::shared_ptr<std::unordered_map<char, Encoder::FrequencyNode>> freqTablePtr)
{
    // create leaf nodes                        list: time O(n) space O(n)  maximum for n is number of possible chars
    std::list<std::shared_ptr<TreeNode>> nodeList;
    for (auto it = freqTablePtr->begin(); it != freqTablePtr->end(); ++it)
    {
        nodeList.push_back(std::make_shared<TreeNode>(it->first, it->second.count));
    }

    // build tree                               time: O((n^2)/2) space: O((n^2)/2)
    // array - can calculate total number of
    // nodes needed and start filling up array
    // from the end
    while (nodeList.size() > 1)
    {
        int min = 9999;
        int secondMin = 9998;
        std::shared_ptr<TreeNode> minNode, secondMinNode;

        for (auto node : nodeList)
        {
            if (node->frequency < min)
            {
                min = node->frequency;
                minNode = node;
            }
            else if (node->frequency < secondMin)
            {
                secondMin = node->frequency;
                secondMinNode = node;
            }
        }

        TreeNode newNode(NULL, min + secondMin, minNode, secondMinNode);
        nodeList.push_back(std::make_shared<TreeNode>(NULL, min + secondMin, minNode, secondMinNode));
        nodeList.remove(minNode);
        nodeList.remove(secondMinNode);
    }

    return nodeList.front();
}

void Encoder::printTree(std::shared_ptr<TreeNode> root)
{
    if (root == nullptr)
        return;

    std::cout << " | " << root->byteVal << " : " << root->frequency << " |\n";
    printTree(root->left);
    printTree(root->right);
}
