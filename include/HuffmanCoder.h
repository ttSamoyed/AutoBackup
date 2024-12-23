#ifndef HuffmanCoder_h
#define HuffmanCoder_h

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <cmath>

const int BITS_PER_BYTE = 8;

struct HuffmanNode {
    char ch; // 字符
    size_t weight; // 权重
    HuffmanNode* left; // 左子节点
    HuffmanNode* right; // 右子节点
    std::string code; // 编码

    // 定义默认构造函数
    HuffmanNode();

    // 定义一个构造函数，接受一个字符和一个权重作为参数，初始化节点
    HuffmanNode(char c, size_t w);

    // 定义一个比较运算符，用来比较两个节点的权重，用于优先队列的排序
    bool operator < (const HuffmanNode& other) const;
};

class HuffmanCoder {
public:
    HuffmanNode* root; // 哈夫曼树的根节点
    std::unordered_map<char, std::string> encoder; // 哈夫曼编码表，用于存储每个字符对应的编码
    std::unordered_map<std::string, char> decoder; // 哈夫曼解码表，用于存储每个编码对应的字符

public:
    // 定义一个默认构造函数
    HuffmanCoder();

    // 定义一个构造函数，接受一个字符串作为参数，根据字符串中的字符频率构建哈夫曼树，并生成编码表和解码表
    HuffmanCoder(const std::string& str);

    // 定义一个函数，接受一个哈夫曼节点和一个字符串作为参数，递归生成编码表和解码表
    void generate_code(HuffmanNode* node, const std::string& prefix);

    // 定义一个函数，接受一个哈夫曼节点作为参数，递归删除哈夫曼树的所有节点
    void delete_tree(HuffmanNode* node);

    // 定义一个函数，接受一个字符串作为参数，返回该字符串经过哈夫曼编码后的结果
    std::string encode(const std::string& str);

    // 定义一个函数，接受一个编码字符串作为参数，返回该字符串经过哈夫曼解码后的结果
    std::string decode(const std::string& code);

    // 定义一个函数，将HuffmanCoder对象保存到文件中
    void save_to_file(const std::filesystem::path& file);

    // 定义一个函数，从文件中读取一个HuffmanCoder对象
    void load_from_file(const std::filesystem::path& file);
};

#endif