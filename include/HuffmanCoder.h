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
    char ch; // �ַ�
    size_t weight; // Ȩ��
    HuffmanNode* left; // ���ӽڵ�
    HuffmanNode* right; // ���ӽڵ�
    std::string code; // ����

    // ����Ĭ�Ϲ��캯��
    HuffmanNode();

    // ����һ�����캯��������һ���ַ���һ��Ȩ����Ϊ��������ʼ���ڵ�
    HuffmanNode(char c, size_t w);

    // ����һ���Ƚ�������������Ƚ������ڵ��Ȩ�أ��������ȶ��е�����
    bool operator < (const HuffmanNode& other) const;
};

class HuffmanCoder {
public:
    HuffmanNode* root; // ���������ĸ��ڵ�
    std::unordered_map<char, std::string> encoder; // ��������������ڴ洢ÿ���ַ���Ӧ�ı���
    std::unordered_map<std::string, char> decoder; // ��������������ڴ洢ÿ�������Ӧ���ַ�

public:
    // ����һ��Ĭ�Ϲ��캯��
    HuffmanCoder();

    // ����һ�����캯��������һ���ַ�����Ϊ�����������ַ����е��ַ�Ƶ�ʹ������������������ɱ����ͽ����
    HuffmanCoder(const std::string& str);

    // ����һ������������һ���������ڵ��һ���ַ�����Ϊ�������ݹ����ɱ����ͽ����
    void generate_code(HuffmanNode* node, const std::string& prefix);

    // ����һ������������һ���������ڵ���Ϊ�������ݹ�ɾ���������������нڵ�
    void delete_tree(HuffmanNode* node);

    // ����һ������������һ���ַ�����Ϊ���������ظ��ַ������������������Ľ��
    std::string encode(const std::string& str);

    // ����һ������������һ�������ַ�����Ϊ���������ظ��ַ������������������Ľ��
    std::string decode(const std::string& code);

    // ����һ����������HuffmanCoder���󱣴浽�ļ���
    void save_to_file(const std::filesystem::path& file);

    // ����һ�����������ļ��ж�ȡһ��HuffmanCoder����
    void load_from_file(const std::filesystem::path& file);
};

#endif