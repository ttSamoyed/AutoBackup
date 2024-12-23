#ifndef utils_h
#define utils_h

#include "File.h"
#include "HuffmanCoder.h"
#include "buf.h"
#include "xxtea.h"
#include "crc.h"

// 定义一个函数，返回当前的系统时间转换为std::string对象
std::string get_current_time_string();

// 定义一个函数，接受两个文件夹路径作为参数，将源文件夹中的所有文件打包成一个buf格式的文件
//，并备份到目标文件夹中
void backup_folder_as_buf(const fs::path& source, const fs::path& destination,
	std::map<std::string, std::string> condition = std::map<std::string, std::string>());

// 定义一个函数，将一个磁盘中的buf文件读到内存中，同时检测CRC码
buf load_buf(const fs::path& buf_file, std::string crc);

// 定义一个函数，接受一个文件夹路径和一个文件名作为参数，删除该文件夹中的指定文件
void delete_file_in_folder(const std::filesystem::path& folder_path, const std::string& file_name);

// 定义一个函数，接受两个文件夹路径作为参数，将一个buf格式的文件还原为若干个原文件，并保存到目标文件夹中
void restore_folder_from_buf(const fs::path& buf_file, const fs::path& destination);

// 定义一个函数，对磁盘上buf文件加密，产生加密后的文件ebuf(encrypted_buf)
void encrypt_buf(const fs::path& buf_file, const char* key);

// 定义一个函数，对磁盘上ebuf文件解密，还原到指定位置
void decrypt_ebuf(const fs::path& ebuf_file, const char* key, const fs::path& destination);

// 定义一个函数，对磁盘上buf文件计算CRC
// std::string calc_buf_crc(const fs::path& buf_file);
#endif