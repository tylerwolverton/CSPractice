#include"Decoder.h"

#include <iostream>
#include <fstream>
#include <stack>

bool Decoder::Decode(std::string inFilePath, std::string outFilePath)
{
    // Build tree from file
        // Read in tree from file
    std::shared_ptr<TreeNode> root = std::make_shared<TreeNode>('!');
    std::streampos bytesRead = buildDataTree(inFilePath, root);

    printTree(root);

    // Read the remainder of the file and build output file
    buildOutputFile(inFilePath, outFilePath, root, bytesRead);

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
    bool seenSlash = false;
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

        // build frequency tree one byte at a time
        for (int i = 0; i < readChunkSize; i++)
        {
            treeBytes++;

            // TODO: Fix edge case where a leaf slash comes right before a leaf ! or leaf | in file
            if (memblock[i] == '\\')
            {
                // Add a slash leaf node since last one we saw isn't escape character
                if (seenSlash)
                {
                    std::cout << "Seen slash\n";
                    nodeStack.push(std::make_shared<TreeNode>('\\'));
                }
                seenSlash = true;
            }
            else
            {
                if (seenSlash)
                {
                    // End of tree seen
                    if (memblock[i] == '!')
                    {
                        break;
                    }
                    // Internal node seen
                    else if (memblock[i] == '|')
                    {
                        std::cout << "Adding internal node\n";
                        auto node1 = nodeStack.top();
                        nodeStack.pop();
                        auto node2 = nodeStack.top();
                        nodeStack.pop();
                        nodeStack.push(std::make_shared<TreeNode>('\0', node2, node1));
                    }
                }
                // Add new leaf node
                else
                {
                    std::cout << "Adding leaf: " << memblock[i] << std::endl;
                    nodeStack.push(std::make_shared<TreeNode>(memblock[i]));
                }
                
                seenSlash = false;
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

    std::cout << "bytesRead: " <<bytesRead << "\n";

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
            std::cout << "Processing byte\n";
            for (int j = 7; j >= 0; j--)
            {
                //int mask = 1 << j;
				//std::cout << "mask: " << mask << std::endl;
                //int bit = atoi(&(memblock[i])) & (mask);
				auto bit = (memblock[i] >> j) & 1;
				std::cout << "bit: " << bit << std::endl;
				if (bit == 0)
                {
                    curNode = curNode->left;
                }
                else if (bit == 1)
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