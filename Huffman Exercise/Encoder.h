#include <string>
#include <unordered_map>

class Encoder {
public:
	/*Encoder();
	~Encoder();*/

	static bool Encode(std::string inFilePath, std::string outFilePath);

private:
    class TreeNode {
    public:
        char byteVal;
        int frequency;
        std::shared_ptr<TreeNode> left;
        std::shared_ptr<TreeNode> right;

        TreeNode(char p_byteVal, 
                 int p_frequency, 
                 std::shared_ptr<TreeNode> p_left = nullptr, 
                 std::shared_ptr<TreeNode> p_right = nullptr)
            : byteVal(p_byteVal),
              frequency(p_frequency),
              left(p_left),
              right(p_right) {}
    };

    class FrequencyNode {
    public:
        int count;
        std::string bitRep;

        FrequencyNode(int p_count, std::string p_bitRep)
            : count(p_count),
              bitRep(p_bitRep) {}
    };

    static std::shared_ptr<std::unordered_map<char, FrequencyNode>> buildFrequencyTable(std::string inFilePath);
    static std::shared_ptr<Encoder::TreeNode> Encoder::buildFrequencyTree(std::shared_ptr<std::unordered_map<char, Encoder::FrequencyNode>> freqTablePtr);
    static void printTree(std::shared_ptr<TreeNode> root);
};