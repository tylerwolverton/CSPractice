#include <string>
#include <unordered_map>
#include <memory>

//class TreeNode;

class Encoder {
public:
	static bool Encode(std::string inFilePath, std::string outFilePath);

private:
    class TreeNode {
    public:
        unsigned char byteVal;
        int frequency;
        std::shared_ptr<TreeNode> left;
        std::shared_ptr<TreeNode> right;

        TreeNode(unsigned char p_byteVal,
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
    typedef std::shared_ptr<std::unordered_map<char, Encoder::FrequencyNode>> FreqTablePtr;

    static FreqTablePtr buildFrequencyTable(std::string inFilePath);
    static std::shared_ptr<Encoder::TreeNode> buildFrequencyTree(FreqTablePtr freqTable);
    static void saveBitReps(std::shared_ptr<TreeNode> node, std::string bitStr, FreqTablePtr freqTable);
    static bool writeTreeToFile(std::shared_ptr<TreeNode> node, std::string outFilePath);
    static void buildTreeString(std::shared_ptr<TreeNode> node, std::shared_ptr<std::string> treeStrPtr);
    static bool writeDataToFile(std::string inFilePath, std::string outFilePath, FreqTablePtr freqTable);
    static void printTree(std::shared_ptr<TreeNode> root, int depth = 0);
};