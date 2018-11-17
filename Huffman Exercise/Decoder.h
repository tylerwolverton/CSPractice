#include <string>
#include <memory>

class Decoder {
public:
    static bool Decode(const std::string& inFilePath, const std::string& outFilePath);

private:
    class TreeNode {
    public:
        char byteVal;
        int frequency;
        std::shared_ptr<TreeNode> left;
        std::shared_ptr<TreeNode> right;

        TreeNode(unsigned char p_byteVal,
                 const std::shared_ptr<TreeNode>& p_left = nullptr,
                 const std::shared_ptr<TreeNode>& p_right = nullptr)
            : byteVal(p_byteVal),
                left(p_left),
                right(p_right) {}
    };

    static int buildDataTree(const std::string& inFilePath, const std::shared_ptr<Decoder::TreeNode>& root);
    static bool buildOutputFile(const std::string& inFilePath, const std::string& outFilePath, const std::shared_ptr<Decoder::TreeNode>& root, std::streampos bytesRead);
    static void printTree(const std::shared_ptr<TreeNode>& root);
};