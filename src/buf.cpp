#include "buf.h"

buf::buf() {};

// 定义一个构造函数，接受一个文件夹路径作为参数，递归遍历该文件夹下所有文件，并将它们转换为File结构体并存储到files向量中
buf::buf(const fs::path& folder, const fs::path& source, 
    std::map<std::string, std::string> condition) {
    // 判断路径是否存在并且是一个文件夹或文件
    if (fs::exists(folder) && (fs::is_directory(folder) || fs::is_regular_file(folder))) {
        if (fs::is_directory(folder)) {
            // 遍历文件夹下的所有条目
            for (const auto& entry : fs::directory_iterator(folder)) {
                // 获取条目的路径
                const auto& path = entry.path();
                // 判断条目是否是一个文件夹
                if (fs::is_directory(path)) {
                    // 如果是一个文件夹，递归调用本构造函数，传入子文件夹的路径作为参数，并将返回的buf对象中的files向量合并到当前对象中的files向量中
                    buf sub_buf(path, source, condition);
                    files.insert(files.end(), sub_buf.files.begin(), sub_buf.files.end());
                }
                else {
                    // 先判断文件名/时间/大小/类型对不对，不对的话直接跳过
                    if (condition.find("file_name") != condition.end()) {
                        if (path.filename().string() != condition["file_name"]) {
                            continue;
                        }
                    }
                    if (condition.find("file_type") != condition.end()) {
                        // 得到类型名
                        std::string type = "", name = path.filename().string();
                        for (int i = 0; i < name.size(); i++) {
                            if (name[i] == '.') {
                                i++;
                                for (; i < name.size(); i++) {
                                    type += name[i];
                                }
                            }
                        }
                        if (type != condition["file_type"]) {
                            continue;
                        }
                    }
                    if (condition.find("file_time") != condition.end()) {
                        // 此处std::chrono的几个API需要 C++ 20
                        fs::file_time_type fs_time = fs::last_write_time(path);
                        auto time = std::chrono::system_clock::now() + (fs_time - std::filesystem::file_time_type::clock::now());
                        auto now = std::chrono::system_clock::now();
                        auto diff = now - time;
                        double diff_days = std::chrono::duration<double, std::ratio<86400>>(diff).count();
                        if (diff_days > (double)std::stoi(condition["file_time"])) {
                            continue;
                        }
                    }
                    if (condition.find("file_size") != condition.end()) {
                        size_t fs_size = fs::file_size(path);
                        if (std::stoi(condition["file_size"]) < fs_size / 1024) {
                            // 输入默认单位是 KB
                            continue;
                        }
                    }

                    // 如果是一个文件，读取该文件的信息，返回一个File结构体，并将其添加到当前对象中的files向量中
                    File file = read_file(path);
                    // 只需要相对路径，因为将来恢复时只需要将目录内的子目录搞出来
                    file.name = file.name.substr(source.string().length() + 1);
                    files.push_back(file);
                }
            }
        } else {
            // 如果是一个文件，读取该文件的信息，返回一个File结构体，并将其添加到当前对象中的files向量中
            File file = read_file(folder);
            // 只需要相对路径，因为将来恢复时只需要将目录内的子目录搞出来
            file.name = folder.filename().string();
            files.push_back(file);
        }
        
        
    } else {
        // 如果路径不存在或者不是一个文件夹，打印出错误信息，并抛出异常
        std::cerr << "Error: invalid folder path " << folder << std::endl;
        throw std::invalid_argument("invalid folder path");
    }
}

// 定义一个函数，对初始化后的buf对象构造哈夫曼表。注意该函数不能在构造函数中调用，因为在构造函数递归遍历到子文件夹时不需要构造哈夫曼树
void buf::buildHuffCoder() {
    std::string all_data = "";
    for (const auto& file : files) {
        // 写入File结构体的name成员到文件中，以'\0'作为结束符
        all_data.append(file.name.c_str(), file.name.size() + 1);
        // 写入File结构体的size成员到文件中，以4个字节表示无符号整数
        all_data.append(reinterpret_cast<const char*>(&file.size), sizeof(file.size));
        // 写入File结构体的data成员到文件中，以file.size个字节表示内容
        all_data.append(file.data.data(), file.size);
    }
    huff_coder = HuffmanCoder(all_data);
}

// 定义一个函数，接受一个buf格式的文件路径作为参数，将当前对象中的files向量中的所有File结构体写入到该buf格式的文件中。
unsigned short buf::write_buf(const fs::path& buf_file) {
    // 打开buf文件流
    std::ofstream buf(buf_file, std::ios::app | std::ios::binary);
    // 判断buf文件是否打开成功
    if (buf.is_open()) {
        // 先把HuffCoder存进去
        huff_coder.save_to_file(buf_file);
        std::string all_data = "";
        for (const auto& file : files) {
            all_data.append(file.name.c_str(), file.name.size() + 1);
            all_data.append(reinterpret_cast<const char*>(&file.size), sizeof(file.size));
            all_data.append(file.data.data(), file.size);
        }
        // std::cout << all_data << '\n';
        std::string encoded_data = huff_coder.encode(all_data);

        int len = encoded_data.size();
        // 计算需要写入的字节数，向下取整
        size_t bytes = len / BITS_PER_BYTE;
        // 循环写入每个字节
        // 使用bitset把每个0/1字符传唤为0/1比特。我的思路是，先把每8个字符转换为1个char
        // 再把这个char加到一个string后边，最后把string写入磁盘
        std::string transformed_encoded_data = "";
        for (int i = 0; i < bytes; i++) {
            // 计算当前字节对应的字符串的起始位置和结束位置
            int start = i * BITS_PER_BYTE;
            int end = start + BITS_PER_BYTE;
            // 截取当前字节对应的字符串片段
            std::string segment = encoded_data.substr(start, end - start);
            // 使用std::bitset类的构造函数，将字符串片段转换为一个二进制位集合
            std::bitset<BITS_PER_BYTE> bits(segment);
            // 使用std::bitset类的to_ulong方法，将二进制位集合转换为一个无符号长整数
            unsigned long value = bits.to_ulong();
            // 将无符号长整数转换为一个字符，并写入文件中
            char byte = static_cast<char>(value);
            transformed_encoded_data += byte;
            //buf.write(&byte, 1);
        }
        //buf.write(transformed_encoded_data.c_str(), transformed_encoded_data.size());
        // 上课讲到这里才发现，这里要处理下尾巴
        if (bytes * BITS_PER_BYTE != len) {
            std::string segment = encoded_data.substr(bytes * BITS_PER_BYTE, len);
            for (int j = 0; j < BITS_PER_BYTE - (len - bytes * BITS_PER_BYTE); j++) {
                segment += "0";
            }
            std::bitset<BITS_PER_BYTE> bits(segment);
            unsigned long value = bits.to_ulong();
            char byte = static_cast<char>(value);
            transformed_encoded_data += byte;
            //buf.write(&byte, 1);
        }
        buf.write(transformed_encoded_data.c_str(), transformed_encoded_data.size());
        // 关闭buf文件流
        buf.close();

        return crc16((char*)transformed_encoded_data.c_str(), transformed_encoded_data.size());
    } else {
        // 如果buf文件打开失败，打印出错误信息，并抛出异常
        std::cerr << "Error: cannot create buf file " << buf_file << std::endl;
        throw std::runtime_error("cannot create buf file");
    }
}

// 定义一个函数，接受一个buf格式的字符串作为参数，将该buf格式的字符串解析为多个File结构体，并存储到当前对象中的files向量中
void buf::parse_buf_str(const std::string& buf_str) { // 修改
    // 清空files向量，以便存储新的File结构体
    files.clear();
    // 创建buf内存流，注意二进制，不然会多几个换行
    std::istringstream in_buf(buf_str, std::ios::binary); // 修改
    // 判断buf内存流是否创建成功
    if (in_buf) { // 修改
        // 定义一个循环变量，用来判断是否到达buf字符串的末尾
        bool end_of_str = false; // 修改
        // 循环读取buf字符串中的每一个File结构体
        while (!end_of_str) { // 修改
            // 定义一个File结构体变量
            File file;
            // 读取File结构体的name成员，以'\0'作为结束符
            std::getline(in_buf, file.name, '\0');
            // 判断是否读取成功
            if (in_buf.good()) {
                // 读取File结构体的size成员，以4个字节表示无符号整数
                in_buf.read(reinterpret_cast<char*>(&file.size), sizeof(file.size));
                // 判断是否读取成功
                if (in_buf.good()) {
                    // 为结构体的data成员分配空间
                    file.data.resize(file.size);
                    // 读取File结构体的data成员，以file.size个字节表示内容
                    in_buf.read(file.data.data(), file.size);
                    // 判断是否读取成功
                    if (in_buf.good()) {
                        // 将File结构体添加到当前对象中的files向量中
                        files.push_back(file);
                    }
                    else {
                        // 如果读取失败，打印出错误信息，并抛出异常
                        std::cerr << "Error: cannot read file data from buf string " << buf_str << std::endl; // 修改
                        throw std::runtime_error("cannot read file data from buf string"); // 修改
                    }
                }
                else {
                    // 如果读取失败，打印出错误信息，并抛出异常
                    std::cerr << "Error: cannot read file size from buf string " << buf_str << std::endl; // 修改
                    throw std::runtime_error("cannot read file size from buf string"); // 修改
                }
            }
            else {
                // 如果读取失败，判断是否到达buf字符串的末尾，如果是，则退出循环，如果不是，则打印出错误信息，并抛出异常
                if (in_buf.eof()) {
                    end_of_str = true; // 修改
                }
                else {
                    std::cerr << "Error: cannot read file name from buf string " << buf_str << std::endl; // 修改
                    throw std::runtime_error("cannot read file name from buf string"); // 修改
                }
            }
        }
        // 不需要关闭buf内存流，它会自动销毁
    }
    else {
        // 如果buf内存流创建失败，打印出错误信息，并抛出异常
        std::cerr << "Error: cannot create buf memory stream " << buf_str << std::endl; // 修改
        throw std::runtime_error("cannot create buf memory stream"); // 修改
    }
}

