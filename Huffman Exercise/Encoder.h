#include <string>
#include <unordered_map>
#include <memory>

class Encoder {
public:
	// Since no state can be shared between runs of encode, make it static
	static bool Encode(const std::string& inFilePath, const std::string& outFilePath);

private:
    class TreeNode {
    public:
        unsigned char byteVal;
        int frequency;
        std::shared_ptr<TreeNode> left;
        std::shared_ptr<TreeNode> right;

        TreeNode(unsigned char p_byteVal,
                 int p_frequency, 
                 const std::shared_ptr<TreeNode>& p_left = nullptr, 
                 const std::shared_ptr<TreeNode>& p_right = nullptr)
            : byteVal(p_byteVal),
              frequency(p_frequency),
              left(p_left),
              right(p_right) {}
    };

    class FrequencyNode {
    public:
        int count;
        std::string bitRep;

        FrequencyNode(int p_count, const std::string& p_bitRep)
            : count(p_count),
              bitRep(p_bitRep) {}
    };
    typedef std::shared_ptr<std::unordered_map<char, Encoder::FrequencyNode>> FreqTablePtr;

    static FreqTablePtr buildFrequencyTable(const std::string& inFilePath);
    static std::shared_ptr<Encoder::TreeNode> buildFrequencyTree(const FreqTablePtr& freqTable);
    static void saveBitReps(const std::shared_ptr<TreeNode>& node, const std::string& bitStr, const FreqTablePtr& freqTable);
    static bool writeTreeToFile(const std::shared_ptr<TreeNode>& node, const std::string& outFilePath);
    static void buildTreeString(const std::shared_ptr<TreeNode>& node, const std::shared_ptr<std::string>& treeStrPtr);
    static bool writeDataToFile(const std::string& inFilePath, const std::string& outFilePath, const FreqTablePtr& freqTable);
    static void printTree(const std::shared_ptr<TreeNode>& root, int depth = 0);
};