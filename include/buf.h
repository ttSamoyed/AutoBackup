#ifndef buf_h
#define buf_h

#include "File.h"
#include "HuffmanCoder.h"
#include "crc.h"
#include <map>
#include <bitset>

// BackUpFile�����buf����Ŀ¼�������ļ����Ϊһ��buf���ļ�
class buf {
public:
    std::vector<File> files; // һ��File�ṹ��������������洢���е��ļ���Ϣ
    HuffmanCoder huff_coder;

    // ����һ��Ĭ�Ϲ��캯���������κβ���
    buf();

    
    // ����һ�����캯��������һ���ļ���·����Ϊ�������ݹ�������ļ����������ļ���
    //��������ת��ΪFile�ṹ�岢�洢��files������
    // ����source��Ϊ���ڵݹ�ʱ�õ����·��
    buf(const fs::path& folder, const fs::path& source, 
        std::map<std::string, std::string> condition = std::map<std::string, std::string>());

    // ����һ���������Գ�ʼ�����rjh�������������ע��ú��������ڹ��캯���е��ã���Ϊ�ڹ��캯���ݹ���������ļ���ʱ����Ҫ�����������
    void buildHuffCoder();

    // ����һ������������һ��buf��ʽ���ļ�·����Ϊ����������ǰ�����е�files�����е�����File�ṹ��д�뵽��buf��ʽ���ļ��С�
    // ����crcУ����
    unsigned short write_buf(const fs::path& buf_file);

    // ����һ������������һ��buf�ļ��ַ�����Ϊ����������buf��ʽ���ļ�����Ϊ���File�ṹ�壬���洢����ǰ�����е�files������
    void parse_buf_str(const std::string& buf_str);
};

#endif