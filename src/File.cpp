#include "File.h"

// 读文件函数，接受一个文件路径作为参数，返回一个File结构体
File read_file(const fs::path& path) {
    // 定义File结构体变量
    File file;
    // 获取文件名并赋值给结构体的name成员
    // file.name = path.filename().string();
    file.name = path.string();
    // 打开文件流
    std::ifstream in(path, std::ios::binary);
    // 判断文件是否打开成功
    if (in.is_open()) {
        // 获取文件大小并赋值给结构体的size成员
        file.size = fs::file_size(path);
        // 为结构体的data成员分配空间
        file.data.resize(file.size);
        // 读取文件内容并存储到结构体的data成员中
        in.read(file.data.data(), file.size);
        // 关闭文件流
        in.close();
    }
    else {
        // 文件打开失败:打印出错误信息，并将结构体的size成员设为0表示无效，并抛出异常
        std::cerr << "Error: cannot open file " << path << std::endl;
        file.size = 0;
        throw std::runtime_error("cannot open file");
    }
    // 返回File结构体变量
    return file;
}

// 写文件函数，接受一个File结构体和一个文件路径作为参数，将File结构体写入到指定的文件中
void write_file(const File& file, const fs::path& path) {
    fs::create_directories(path.parent_path());

    // 打开文件流
    std::ofstream out(path, std::ios::app | std::ios::binary); // 注意使用std::ios::app模式，表示每次写入都会追加到文件末尾
    // 判断文件是否打开成功
    if (out.is_open()) {
        out.write(file.data.data(), file.size);
        // 关闭文件流
        out.close();
    }
    else {
        // 如果文件打开失败，打印出错误信息，并抛出异常
        std::cerr << "Error: cannot write file " << path << std::endl;
        throw std::runtime_error("cannot write file");
    }
}