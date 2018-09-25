#include"Decoder.h"

#include <iostream>
#include <fstream>
#include <stack>

bool Decoder::Decode(std::string inFilePath, std::string outFilePath)
{
    // Build tree from file
        // Read in tree from file
	std::cout << "Reading Tree from File\n";
    std::shared_ptr<TreeNode> root = std::make_shared<TreeNode>('!');
    std::streampos bytesRead = buildDataTree(inFilePath, root);

    //printTree(root);

    // Read the remainder of the file and build output file
	std::cout << "Reading Remainder of File\n";
    buildOutputFile(inFilePath, outFilePath, root, bytesRead);

	std::cout << "Decoding Complete\n";
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
            if (memblock[i] == '0')
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
			else if (memblock[i] == '1')
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


void Decoder::buildOutputFile(std::string inFilePath, std::string outFilePath, std::shared_ptr<Decoder::TreeNode> root, std::streampos bytesRead)
{
    std::ifstream inFile(inFilePath, std::ios::binary | std::ios::ate);

    if (!inFile.is_open())
    {
        std::cout << "File isn't open\n";
        return;
    }

    //std::cout << "bytesRead: " <<bytesRead << "\n";

    std::ofstream outfile;
    outfile.open(outFilePath, std::ios::out | std::ios::binary);

    std::stack<std::shared_ptr<TreeNode>> nodeStack;
    std::streampos maxReadSize = 2048;
    std::streampos fileSize = inFile.tellg() - bytesRead;
    inFile.seekg(bytesRead, std::ios::beg);

    std::streampos totalBytesRead = 0;
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

        // write each byte to file
        for (int i = 0; i < readChunkSize; i++)
        {
            //std::cout << "Processing byte\n";
            for (int j = 7; j >= 0; j--)
            {
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
}

void Decoder::printTree(std::shared_ptr<Decoder::TreeNode> root)
{
    if (root == nullptr)
        return;

    std::cout << " | " << root->byteVal << " |\n";
    printTree(root->left);
    printTree(root->right);
}