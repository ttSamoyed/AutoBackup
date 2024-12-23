#include "utils.h"

// 定义一个函数，返回当前的系统时间转换为std::string对象
std::string get_current_time_string() {
    // 获取当前的系统时间，返回一个std::chrono::system_clock::time_point对象
    auto now = std::chrono::system_clock::now();
    // 将时间点转换为一个time_t对象，表示自1970年1月1日以来的秒数
    auto time = std::chrono::system_clock::to_time_t(now);
    // 将time_t对象转换为一个tm结构体，表示本地时间的各个分量
    auto local_time = std::localtime(&time);
    // 创建一个char数组，用于存储格式化后的时间字符串
    char buffer[20];
    // 使用strftime函数，将tm结构体转换为指定格式的字符串，存储在buffer中
    // 格式为"2023_10_19_15_31_15"
    std::strftime(buffer, sizeof(buffer), "%Y_%m_%d_%H_%M_%S", local_time);
    // 将char数组转换为std::string对象，返回该对象
    return std::string(buffer);
}

// 定义一个函数，接受两个文件夹路径作为参数，将源文件夹中的所有文件打包成一个buf格式的文件
//，并备份到目标文件夹中
void backup_folder_as_buf(const fs::path& source, const fs::path& destination, 
    std::map<std::string, std::string> condition) {
    // 判断源路径是否存在并且是一个文件夹
    if (fs::exists(source) && (fs::is_directory(source) || fs::is_regular_file(source))) {
        // 判断目标路径是否存在并且是一个文件夹
        if (fs::exists(destination) && fs::is_directory(destination)) {
            // 创建一个buf类的对象，传入源文件夹的路径作为参数，递归遍历该文件夹下所有文件
            //，并将它们转换为File结构体并存储到buf对象中
            buf r(source, source, condition);
            r.buildHuffCoder();
            // 定义一个buf格式的文件路径，以源文件夹的名称作为buf文件的名称，并添加".buf"作为扩展名
            fs::path buf_file = destination / (source.filename().string() + "_" + get_current_time_string() + ".buf");
            // 调用buf对象的write_buf函数，传入buf格式的文件路径作为参数，将buf对象中的files向量中的所有File结构体写入到该buf格式的文件中
            unsigned short crc_code = r.write_buf(buf_file);
            std::string ab = fs::absolute(buf_file).string();
            ab = ab.substr(0, ab.size() - 4) + "_" + std::to_string(crc_code) + ".buf";
            fs::path crc_buf_file = ab;
            fs::rename(buf_file, crc_buf_file);

            // 打印出备份成功的信息
            std::cout << "Backup: " << source << " to " << crc_buf_file << std::endl;
        }
        else {
            // 如果目标路径不存在或者不是一个文件夹，打印出错误信息，并抛出异常
            std::cerr << "Error: invalid destination folder path " << destination << std::endl;
            throw std::invalid_argument("invalid destination folder path");
        }
    }
    else {
        // 如果源路径不存在或者不是一个文件夹，打印出错误信息，并抛出异常
        std::cerr << "Error: invalid source folder path " << source << std::endl;
        throw std::invalid_argument("invalid source folder path");
    }
}

// 定义一个函数，将一个磁盘中的buf文件读到内存中
buf load_buf(const fs::path& buf_file, std::string crc) {
    buf loaded_file;
    // 打开buf文件流
    std::ifstream in(buf_file, std::ios::binary);
    // 判断buf文件是否打开成功
    if (in.is_open()) {
        // 先读HuffmanCoder
        // 读取encoder的大小
        size_t encoder_size;
        in.read(reinterpret_cast<char*>(&encoder_size), sizeof(encoder_size));
        // 循环读取每个键值对
        for (size_t i = 0; i < encoder_size; i++) {
            // 读取键的内容
            char key;
            in.read(reinterpret_cast<char*>(&key), sizeof(key));
            // 读取值的长度和内容
            size_t value_size;
            in.read(reinterpret_cast<char*>(&value_size), sizeof(value_size));
            std::string value(value_size, '\0');
            in.read(value.data(), value_size);
            // 将键值对插入到encoder中
            loaded_file.huff_coder.encoder.insert({ key, value });
        }
        // 读取decoder的大小
        size_t decoder_size;
        in.read(reinterpret_cast<char*>(&decoder_size), sizeof(decoder_size));
        // 循环读取每个键值对
        for (size_t i = 0; i < decoder_size; i++) {
            // 读取键的长度和内容
            size_t key_size;
            in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
            std::string key(key_size, '\0');
            in.read(key.data(), key_size);
            // 读取值的内容
            char value;
            in.read(reinterpret_cast<char*>(&value), sizeof(value));
            // 将键值对插入到decoder中
            loaded_file.huff_coder.decoder.insert({ key, value });
        }

        std::string result = "";
        // 计算除掉HuffCoder后文件的大小。注意还要减去两个size_t的大小。
        // 获取当前的读取位置
        std::streampos cur_pos = in.tellg();
        // 移动到文件的末尾
        in.seekg(0, std::ios::end);
        // 获取文件的总大小
        std::streampos total_size = in.tellg();
        // 计算剩下未读部分的大小
        std::streampos remain_size = total_size - cur_pos;
        // 恢复原来的读取位置
        in.seekg(cur_pos);
        size_t files_size = remain_size;
        char* buffer = new char[files_size];
        in.read(buffer, files_size);
        // 关闭文件
        in.close();

        std::string crc_code = std::to_string(crc16(buffer, files_size));
        if (crc == crc_code) {
            std::cout << "CRC 校验正确！将解析 buf 文件..." << '\n';
        }
        else {
            std::cout << "CRC 校验错误！是否继续解析？（按 1 继续）" << '\n';
            char op;
            std::cin >> op;
            if (op != '1') {
                exit(0);
            }
        }

        // 遍历buffer中的每个字节
        for (size_t i = 0; i < files_size; i++) {
            // 把每个字节转换成8位的二进制字符串
            std::bitset<BITS_PER_BYTE> bits(buffer[i]);
            // 把二进制字符串追加到result中
            result += bits.to_string();
        }
        delete[] buffer;

        // std::string loaded_encoded_string = buffer.str();
        std::string decoded_files_string = loaded_file.huff_coder.decode(result);

        /*
        std::cout << "#########  show decoded_files_string ########" << '\n';
        std::cout << decoded_files_string << '\n';
        */
        
        loaded_file.parse_buf_str(decoded_files_string);
        return loaded_file;
    } else {
        // 如果buf文件打开失败，打印出错误信息，并抛出异常
        std::cerr << "Error: cannot open buf file " << buf_file << std::endl;
        throw std::runtime_error("cannot open buf file");
    }
}

// 定义一个函数，接受一个文件夹路径和一个文件名作为参数，删除该文件夹中的指定文件
void delete_file_in_folder(const std::filesystem::path& folder_path, const std::string& file_name) {
    // 检查文件夹路径是否存在并且是一个目录
    if (std::filesystem::exists(folder_path) && std::filesystem::is_directory(folder_path)) {
        // 遍历文件夹中的所有条目
        for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
            // 检查条目是否是一个文件，并且文件名是否与参数匹配
            if (std::filesystem::is_regular_file(entry) && entry.path().filename() == file_name) {
                // 删除该文件
                std::filesystem::remove(entry);
                // 跳出循环
                break;
            }
        }
    }
    else {
        // 如果文件夹路径不存在或者不是一个目录，抛出异常
        throw std::runtime_error("Invalid folder path: " + folder_path.string());
    }
}

// 定义一个函数，接受两个文件夹路径作为参数，将一个buf格式的文件还原为若干个原文件，并保存到目标文件夹中
void restore_folder_from_buf(const fs::path& buf_file, const fs::path& destination) {
    // 判断buf格式的文件是否存在并且是一个普通文件
    if (fs::exists(buf_file) && fs::is_regular_file(buf_file)) {
        // 判断目标路径是否存在并且是一个文件夹
        if (fs::exists(destination) && fs::is_directory(destination)) {
            // 先得到记录的CRC
            std::string buf_name = buf_file.filename().string();
            size_t lastUnderscorePos = buf_name.rfind("_");
            size_t dotPos = buf_name.rfind(".");
            std::string result = buf_name.substr(lastUnderscorePos + 1, dotPos - lastUnderscorePos - 1);
            std::cout << "crc: " << result << "正在校验中";
            // 创建一个buf类的对象，传入buf格式的文件路径作为参数，将该buf格式的文件解析为多个File结构体，并存储到buf对象中
            buf r = load_buf(buf_file, result);
            // 遍历buf对象中的files向量中的所有File结构体
            for (const auto& file : r.files) {
                // 定义一个原始格式的文件路径，以目标文件夹和File结构体的name成员作为路径
                fs::path original_file = destination / file.name;
                // 调用write_file函数，传入File结构体和原始格式的文件路径作为参数，将File结构体写入到指定的原始格式的文件中
                write_file(file, original_file);
                // 打印出还原成功的信息
                std::cout << "Restored: " << original_file << " from " << buf_file << std::endl;
            }
        }
        else {
            // 如果目标路径不存在或者不是一个文件夹，打印出错误信息，并抛出异常
            std::cerr << "Error: invalid destination folder path " << destination << std::endl;
            throw std::invalid_argument("invalid destination folder path");
        }
    } else {
        // 如果buf格式的文件不存在或者不是一个普通文件，打印出错误信息，并抛出异常
        std::cerr << "Error: invalid buf file path " << buf_file << std::endl;
        throw std::invalid_argument("invalid buf file path");
    }
}

// 定义一个函数，对磁盘上buf文件加密，产生加密后的文件ebuf(encrypted_buf)
void encrypt_buf(const fs::path& buf_file, const char* key) {
    // 打开文件，以二进制模式读取
    std::ifstream in(buf_file, std::ios::binary);
    // 获取文件的大小
    size_t file_size = fs::file_size(buf_file);

    // 创建一个std::string对象，指定大小
    std::string bin_data(file_size, '\0');

    // 读取文件的内容，一次性读取整个文件
    in.read(bin_data.data(), file_size);
    in.close();
    size_t len = 0;
    char* encrypt_data = (char*)xxtea_encrypt(bin_data.c_str(), bin_data.size(), key, &len);
    
    /*
    char* decrypt_data = (char*)xxtea_decrypt(encrypt_data, len, key, &len);
    if (strncmp(bin_data.c_str(), decrypt_data, len) == 0) {
        printf("success!\n");
    }
    else {
        printf("fail!\n");
    }
    */

    // 将加密后文件写入磁盘，路径为buf所在文件夹，后缀改成.ebuf
    fs::path ebuf_file = buf_file.string().substr(0, buf_file.string().size() - 4) + ".ebuf";
    std::ofstream out(ebuf_file, std::ios::binary);
    out.write(encrypt_data, len);
    out.close();

    // 删除原buf文件（不然还加什么密）
    fs::remove(buf_file);
}

// 定义一个函数，将磁盘上ebuf文件解密为buf文件，还原到指定位置
void decrypt_ebuf(const fs::path& ebuf_file, const char* key, const fs::path& destination) {
    // 打开文件，以二进制模式读取
    std::ifstream in(ebuf_file, std::ios::binary);
    // 获取文件的大小
    size_t file_size = fs::file_size(ebuf_file);

    // 创建一个std::string对象，指定大小
    std::string bin_data(file_size, '\0');

    // 读取文件的内容，一次性读取整个文件
    in.read(bin_data.data(), file_size);
    in.close();

    size_t len = 0;
    char* decrypt_data = (char*)xxtea_decrypt(bin_data.c_str(), bin_data.size(), key, &len);

    // 将解密后文件写入磁盘
    fs::path buf_file = ebuf_file.string().substr(0, ebuf_file.string().size() - 5) + ".buf";
    std::ofstream out(buf_file, std::ios::binary);
    out.write(decrypt_data, len);
    out.close();

    restore_folder_from_buf(buf_file, destination);
}


std::string calc_buf_crc(const fs::path& buf_file) {
    // 读取文件，计算CRC校验码
    std::ifstream in(buf_file, std::ios::binary);
    size_t file_size = fs::file_size(buf_file);
    std::string bin_data(file_size, '\0');
    // 读取文件的内容，一次性读取整个文件
    in.read(bin_data.data(), file_size);
    in.close();

    unsigned short crc_code = crc16((char *)bin_data.c_str(), bin_data.size());
    return std::to_string(crc_code);
}