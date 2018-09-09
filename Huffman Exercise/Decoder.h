#include <string>
#include <memory>

class Decoder {
public:
    static bool Decode(std::string inFilePath, std::string outFilePath);

private:
    class TreeNode {
    public:
        char byteVal;
        int frequency;
        std::shared_ptr<TreeNode> left;
        std::shared_ptr<TreeNode> right;

        TreeNode(unsigned char p_byteVal,
                 std::shared_ptr<TreeNode> p_left = nullptr,
                 std::shared_ptr<TreeNode> p_right = nullptr)
            : byteVal(p_byteVal),
                left(p_left),
                right(p_right) {}
    };

    static int buildDataTree(std::string inFilePath, std::shared_ptr<Decoder::TreeNode> root);
    static void buildOutputFile(std::string inFilePath, std::string outFilePath, std::shared_ptr<Decoder::TreeNode> root, std::streampos bytesRead);
    static void printTree(std::shared_ptr<TreeNode> root);
};