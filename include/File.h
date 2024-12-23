#ifndef File_h
#define File_h

// 引入头文件
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <bitset>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <thread>
#include <unistd.h>
#define CALLBACK
typedef void* PVOID;
typedef bool BOOLEAN;
#endif
#include <unordered_map>
#include <queue>
#include <sstream>
#include <cmath>

namespace fs = std::filesystem;

struct File {
    std::string name; // 文件名
    size_t size; // 文件大小
    std::vector<char> data; // 文件内容
};

// 定义一个函数，接受一个文件路径作为参数，返回一个File结构体
File read_file(const fs::path& path);

// 定义一个函数，接受一个File结构体和一个文件路径作为参数，将File结构体写入到指定的文件中
void write_file(const File& file, const fs::path& path);

#endif // ! File