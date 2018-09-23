#include "Encoder.h"

#include <iostream>
#include <fstream>

bool Encoder::Encode(std::string inFilePath, std::string outFilePath) 
{
    // build frequency table for each byte          hash table: time: O(n) space: O(n)
    std::cout << "Building Frequency Table\n";
	FreqTablePtr freqTable = buildFrequencyTable(inFilePath);

	if (freqTable == nullptr)
	{
		return false;
	}

    /*for (auto it = freqTable->begin(); it != freqTable->end(); ++it)
    {
        std::cout << "byte: " << it->first << " count: " << it->second.count << std::endl;
    }*/

	// assign bit sequence to each char  
        // create leaf nodes                        list: time O(n) space O(n)  maximum for n is number of possible chars from this point on
        // build tree                               time: O((n^2)/2) space: O((n^2)/2)
	std::cout << "Building Frequency Tree\n";
    std::shared_ptr<TreeNode> root = buildFrequencyTree(freqTable);

    //printTree(root);

	// store the bit representation of each leaf    hash table: time O(nlogn) space: none if reuse frequency table
	std::cout << "Saving Bit Representations\n";
    saveBitReps(root, "", freqTable);

    /*for (auto it = freqTable->begin(); it != freqTable->end(); ++it)
    {
        std::cout << "byte: " << it->first << " bit rep: " << it->second.bitRep << std::endl;
    }*/
	// write tree into start of file                time: O((n^2)/2)
	std::cout << "Writing Tree to File\n";
    writeTreeToFile(root, outFilePath);

	// write out compressed bits to new file        time: O(n)
	std::cout << "Writing Bits to File\n";
    writeDataToFile(inFilePath, outFilePath, freqTable);

	std::cout << "Finished Encoding\n";
    return true;
}

Encoder::FreqTablePtr Encoder::buildFrequencyTable(std::string inFilePath)
{
	// read file into memory
	std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

	if (!inFile.is_open())
	{
        std::cout << "File isn't open\n";
		return nullptr;
	}

    FreqTablePtr freqTable = std::make_shared<std::unordered_map<char, FrequencyNode> >();
    std::streampos maxReadSize = 2048;
    std::streampos fileSize = inFile.tellg();

	if (fileSize < 2)
	{
		std::cout << "File is too small to benefit from compression\n";
		return nullptr;
	}

    inFile.seekg(0, std::ios::beg);

	std::streampos totalBytesRead = 0;
    //std::vector<unsigned char> memblock;
	while (totalBytesRead < fileSize)
	{
		//std::cout << "Processing chunk\n";
        std::streampos readChunkSize = maxReadSize;
		if (fileSize - totalBytesRead < readChunkSize)
		{
            readChunkSize = fileSize - totalBytesRead;
		}
		
        //std::vector<unsigned char> memblock;
        //memblock.reserve(readChunkSize);
        char* memblock = new char[readChunkSize];
        inFile.read(memblock, readChunkSize);
        //inFile.read((char*)(&(memblock.front())), readChunkSize);
        //memblock.resize(inFile.gcount());
        totalBytesRead += readChunkSize;

        // build frequency table for each byte
		for (int i = 0; i < readChunkSize; i++)
		{
            auto it = freqTable->find(memblock[i]);
            if (it == freqTable->end())
            {
                //std::cout << "New byte\n";
                FrequencyNode node(1, "");
                freqTable->emplace(memblock[i], node);
            }
            else
            {
                //std::cout << "Update count\n";
                it->second.count++;
            }
		}

        delete[] memblock;
		//memblock.clear();
	}

	inFile.close();

    return freqTable;
}

std::shared_ptr<Encoder::TreeNode> Encoder::buildFrequencyTree(FreqTablePtr freqTable)
{
    // create leaf nodes                        list: time O(n) space O(n)  maximum for n is number of possible chars
    std::list<std::shared_ptr<TreeNode>> nodeList;
    for (auto it = freqTable->begin(); it != freqTable->end(); ++it)
    {
		//std::cout << "Pushing node\n";
        nodeList.push_back(std::make_shared<TreeNode>(it->first, it->second.count));
    }

    // build tree                               time: O((n^2)/2) space: O((n^2)/2)
    // array - can calculate total number of
    // nodes needed and start filling up array
    // from the end
    while (nodeList.size() > 1)
    {
		//std::cout << "Noodles: " << nodeList.size() <<std::endl;
        int min = 99999998;
        int secondMin = 99999999;
        std::shared_ptr<TreeNode> minNode = nullptr, secondMinNode = nullptr;

        for (auto node : nodeList)
        {
            if (node->frequency < min)
            {
				//std::cout << "Updating min node: " << node->frequency << std::endl;
				// Update second min node to be the current min
				if (minNode != nullptr)
				{
					//std::cout << "Updating second min node: " << minNode->frequency << std::endl;
					secondMin = minNode->frequency;
					secondMinNode = minNode;
				}

				// Save the new min
                min = node->frequency;
                minNode = node;
            }
			else if (node->frequency < secondMin)
			{
				//std::cout << "Updating second min node: " << node->frequency << std::endl;
				secondMin = node->frequency;
				secondMinNode = node;
			}
        }

        nodeList.push_back(std::make_shared<TreeNode>('\0', minNode->frequency + secondMinNode->frequency, minNode, secondMinNode));
        nodeList.remove(minNode);
        nodeList.remove(secondMinNode);
    }

    return nodeList.front();
}

void Encoder::saveBitReps(std::shared_ptr<TreeNode> node, std::string bitStr, FreqTablePtr freqTable)
{
    if (node->left == nullptr
        && node->right == nullptr)
    {
        auto it = freqTable->find(node->byteVal);
		// If only 1 node is in the list, set its value to 0
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

bool Encoder::writeTreeToFile(std::shared_ptr<TreeNode> node, std::string outFilePath)
{
    std::shared_ptr<std::string> treeStrPtr = std::make_shared<std::string>();
    buildTreeString(node, treeStrPtr);

    //std::cout << "treeString size " << (*treeStrPtr).size() << "  value: " << *treeStrPtr <<"!" <<std::endl;

    std::ofstream outfile;
    outfile.open(outFilePath, std::ios::out | std::ios::binary);

    if (outfile.is_open())
    {
        outfile.write((*treeStrPtr).c_str(), (*treeStrPtr).size());
        outfile.write("!", sizeof("!"));
		return 1;// (*treeStrPtr).size() + sizeof("\\!");
    }

    return 0;
}

void Encoder::buildTreeString(std::shared_ptr<TreeNode> node, std::shared_ptr<std::string> treeStrPtr)
{
    if (node->left != nullptr)
    {
        buildTreeString(node->left, treeStrPtr);
    }
    if (node->right != nullptr)
    {
        buildTreeString(node->right, treeStrPtr);
    }

    if (node->byteVal == '\0' 
		&& (node->left != nullptr
		|| node->right != nullptr))
	{
        (*treeStrPtr) += '1';
    }
    else
    {
		(*treeStrPtr) += '0';
        (*treeStrPtr) += node->byteVal;
    }
}

bool Encoder::writeDataToFile(std::string inFilePath, std::string outFilePath, FreqTablePtr freqTable)
{
    // read file into memory
    std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

    if (!inFile.is_open())
    {
        std::cout << "File isn't open\n";
        return nullptr;
    }

    std::ofstream outfile;
    outfile.open(outFilePath, std::ios::out | std::ios::binary | std::ios::ate | std::ios::app);

    std::streampos maxReadSize = 2048;
    std::streampos fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    std::streampos totalBytesRead = 0;
    char byteToWrite = 0;
    int bitsRead = 0;
    // TODO: Support variable byte size
    const int BIT_COUNT = 8;
    while (totalBytesRead < fileSize)
    {
        std::streampos readChunkSize = maxReadSize;
        if (fileSize - totalBytesRead < readChunkSize)
        {
            readChunkSize = fileSize - totalBytesRead;
        }

        char* memblock = new char[readChunkSize];
        //std::vector<unsigned char> memblock;
        //memblock.resize(readChunkSize);
        inFile.read(memblock, readChunkSize);
        totalBytesRead += readChunkSize;

        // write each bit to file in 1 byte chunks
        for (int i = 0; i < readChunkSize; i++)
        {
            auto it = freqTable->find(memblock[i]);
            //std::cout << "processing bit string: " << it->second.bitRep << std::endl;
            for (char bit : it->second.bitRep)
            {
                if (bitsRead == BIT_COUNT)
                {
					//std::cout << "byteToWrite: " << byteToWrite << std::endl;
                    outfile.write(&byteToWrite, 1);
                    byteToWrite = 0;
                    bitsRead = 0;
                    //std::cout << "writing byte" << std::endl;
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

    // Write out any remaining bits
    if (bitsRead != 0)
    {
        for (int i = 0; i < BIT_COUNT - bitsRead; i++)
        {
            byteToWrite <<= 1;
        }
		//std::cout << "remaining byteToWrite: " << byteToWrite << std::endl;
    
        outfile.write(&byteToWrite, 1);
    }
    
    inFile.close();
    outfile.close();

    return true;
}

void Encoder::printTree(std::shared_ptr<TreeNode> root, int depth)
{
    if (root == nullptr)
        return;

    std::cout << " | " << root->byteVal << " : " << root->frequency << " depth: " <<depth <<" |\n";
    printTree(root->left, depth + 1);
    printTree(root->right, depth + 1);
}
