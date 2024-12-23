#ifndef buf_h
#define buf_h

#include "File.h"
#include "HuffmanCoder.h"
#include "crc.h"
#include <map>
#include <bitset>

// BackUpFile，简称buf，将目录内所有文件打包为一个buf大文件
class buf {
public:
    std::vector<File> files; // 一个File结构体的向量，用来存储所有的文件信息
    HuffmanCoder huff_coder;

    // 定义一个默认构造函数，不做任何操作
    buf();

    
    // 定义一个构造函数，接受一个文件夹路径作为参数，递归遍历该文件夹下所有文件，
    //并将它们转换为File结构体并存储到files向量中
    // 保存source是为了在递归时得到相对路径
    buf(const fs::path& folder, const fs::path& source, 
        std::map<std::string, std::string> condition = std::map<std::string, std::string>());

    // 定义一个函数，对初始化后的rjh对象构造哈夫曼表。注意该函数不能在构造函数中调用，因为在构造函数递归遍历到子文件夹时不需要构造哈夫曼树
    void buildHuffCoder();

    // 定义一个函数，接受一个buf格式的文件路径作为参数，将当前对象中的files向量中的所有File结构体写入到该buf格式的文件中。
    // 返回crc校验码
    unsigned short write_buf(const fs::path& buf_file);

    // 定义一个函数，接受一个buf文件字符串作为参数，将该buf格式的文件解析为多个File结构体，并存储到当前对象中的files向量中
    void parse_buf_str(const std::string& buf_str);
};

#endif