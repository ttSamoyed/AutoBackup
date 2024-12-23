#include "buf.h"

buf::buf() {};

// ����һ�����캯��������һ���ļ���·����Ϊ�������ݹ�������ļ����������ļ�����������ת��ΪFile�ṹ�岢�洢��files������
buf::buf(const fs::path& folder, const fs::path& source, 
    std::map<std::string, std::string> condition) {
    // �ж�·���Ƿ���ڲ�����һ���ļ��л��ļ�
    if (fs::exists(folder) && (fs::is_directory(folder) || fs::is_regular_file(folder))) {
        if (fs::is_directory(folder)) {
            // �����ļ����µ�������Ŀ
            for (const auto& entry : fs::directory_iterator(folder)) {
                // ��ȡ��Ŀ��·��
                const auto& path = entry.path();
                // �ж���Ŀ�Ƿ���һ���ļ���
                if (fs::is_directory(path)) {
                    // �����һ���ļ��У��ݹ���ñ����캯�����������ļ��е�·����Ϊ�������������ص�buf�����е�files�����ϲ�����ǰ�����е�files������
                    buf sub_buf(path, source, condition);
                    files.insert(files.end(), sub_buf.files.begin(), sub_buf.files.end());
                }
                else {
                    // ���ж��ļ���/ʱ��/��С/���ͶԲ��ԣ����ԵĻ�ֱ������
                    if (condition.find("file_name") != condition.end()) {
                        if (path.filename().string() != condition["file_name"]) {
                            continue;
                        }
                    }
                    if (condition.find("file_type") != condition.end()) {
                        // �õ�������
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
                        // �˴�std::chrono�ļ���API��Ҫ C++ 20
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
                            // ����Ĭ�ϵ�λ�� KB
                            continue;
                        }
                    }

                    // �����һ���ļ�����ȡ���ļ�����Ϣ������һ��File�ṹ�壬��������ӵ���ǰ�����е�files������
                    File file = read_file(path);
                    // ֻ��Ҫ���·������Ϊ�����ָ�ʱֻ��Ҫ��Ŀ¼�ڵ���Ŀ¼�����
                    file.name = file.name.substr(source.string().length() + 1);
                    files.push_back(file);
                }
            }
        } else {
            // �����һ���ļ�����ȡ���ļ�����Ϣ������һ��File�ṹ�壬��������ӵ���ǰ�����е�files������
            File file = read_file(folder);
            // ֻ��Ҫ���·������Ϊ�����ָ�ʱֻ��Ҫ��Ŀ¼�ڵ���Ŀ¼�����
            file.name = folder.filename().string();
            files.push_back(file);
        }
        
        
    } else {
        // ���·�������ڻ��߲���һ���ļ��У���ӡ��������Ϣ�����׳��쳣
        std::cerr << "Error: invalid folder path " << folder << std::endl;
        throw std::invalid_argument("invalid folder path");
    }
}

// ����һ���������Գ�ʼ�����buf�������������ע��ú��������ڹ��캯���е��ã���Ϊ�ڹ��캯���ݹ���������ļ���ʱ����Ҫ�����������
void buf::buildHuffCoder() {
    std::string all_data = "";
    for (const auto& file : files) {
        // д��File�ṹ���name��Ա���ļ��У���'\0'��Ϊ������
        all_data.append(file.name.c_str(), file.name.size() + 1);
        // д��File�ṹ���size��Ա���ļ��У���4���ֽڱ�ʾ�޷�������
        all_data.append(reinterpret_cast<const char*>(&file.size), sizeof(file.size));
        // д��File�ṹ���data��Ա���ļ��У���file.size���ֽڱ�ʾ����
        all_data.append(file.data.data(), file.size);
    }
    huff_coder = HuffmanCoder(all_data);
}

// ����һ������������һ��buf��ʽ���ļ�·����Ϊ����������ǰ�����е�files�����е�����File�ṹ��д�뵽��buf��ʽ���ļ��С�
unsigned short buf::write_buf(const fs::path& buf_file) {
    // ��buf�ļ���
    std::ofstream buf(buf_file, std::ios::app | std::ios::binary);
    // �ж�buf�ļ��Ƿ�򿪳ɹ�
    if (buf.is_open()) {
        // �Ȱ�HuffCoder���ȥ
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
        // ������Ҫд����ֽ���������ȡ��
        size_t bytes = len / BITS_PER_BYTE;
        // ѭ��д��ÿ���ֽ�
        // ʹ��bitset��ÿ��0/1�ַ�����Ϊ0/1���ء��ҵ�˼·�ǣ��Ȱ�ÿ8���ַ�ת��Ϊ1��char
        // �ٰ����char�ӵ�һ��string��ߣ�����stringд�����
        std::string transformed_encoded_data = "";
        for (int i = 0; i < bytes; i++) {
            // ���㵱ǰ�ֽڶ�Ӧ���ַ�������ʼλ�úͽ���λ��
            int start = i * BITS_PER_BYTE;
            int end = start + BITS_PER_BYTE;
            // ��ȡ��ǰ�ֽڶ�Ӧ���ַ���Ƭ��
            std::string segment = encoded_data.substr(start, end - start);
            // ʹ��std::bitset��Ĺ��캯�������ַ���Ƭ��ת��Ϊһ��������λ����
            std::bitset<BITS_PER_BYTE> bits(segment);
            // ʹ��std::bitset���to_ulong��������������λ����ת��Ϊһ���޷��ų�����
            unsigned long value = bits.to_ulong();
            // ���޷��ų�����ת��Ϊһ���ַ�����д���ļ���
            char byte = static_cast<char>(value);
            transformed_encoded_data += byte;
            //buf.write(&byte, 1);
        }
        //buf.write(transformed_encoded_data.c_str(), transformed_encoded_data.size());
        // �Ͽν�������ŷ��֣�����Ҫ������β��
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
        // �ر�buf�ļ���
        buf.close();

        return crc16((char*)transformed_encoded_data.c_str(), transformed_encoded_data.size());
    } else {
        // ���buf�ļ���ʧ�ܣ���ӡ��������Ϣ�����׳��쳣
        std::cerr << "Error: cannot create buf file " << buf_file << std::endl;
        throw std::runtime_error("cannot create buf file");
    }
}

// ����һ������������һ��buf��ʽ���ַ�����Ϊ����������buf��ʽ���ַ�������Ϊ���File�ṹ�壬���洢����ǰ�����е�files������
void buf::parse_buf_str(const std::string& buf_str) { // �޸�
    // ���files�������Ա�洢�µ�File�ṹ��
    files.clear();
    // ����buf�ڴ�����ע������ƣ���Ȼ��༸������
    std::istringstream in_buf(buf_str, std::ios::binary); // �޸�
    // �ж�buf�ڴ����Ƿ񴴽��ɹ�
    if (in_buf) { // �޸�
        // ����һ��ѭ�������������ж��Ƿ񵽴�buf�ַ�����ĩβ
        bool end_of_str = false; // �޸�
        // ѭ����ȡbuf�ַ����е�ÿһ��File�ṹ��
        while (!end_of_str) { // �޸�
            // ����һ��File�ṹ�����
            File file;
            // ��ȡFile�ṹ���name��Ա����'\0'��Ϊ������
            std::getline(in_buf, file.name, '\0');
            // �ж��Ƿ��ȡ�ɹ�
            if (in_buf.good()) {
                // ��ȡFile�ṹ���size��Ա����4���ֽڱ�ʾ�޷�������
                in_buf.read(reinterpret_cast<char*>(&file.size), sizeof(file.size));
                // �ж��Ƿ��ȡ�ɹ�
                if (in_buf.good()) {
                    // Ϊ�ṹ���data��Ա����ռ�
                    file.data.resize(file.size);
                    // ��ȡFile�ṹ���data��Ա����file.size���ֽڱ�ʾ����
                    in_buf.read(file.data.data(), file.size);
                    // �ж��Ƿ��ȡ�ɹ�
                    if (in_buf.good()) {
                        // ��File�ṹ����ӵ���ǰ�����е�files������
                        files.push_back(file);
                    }
                    else {
                        // �����ȡʧ�ܣ���ӡ��������Ϣ�����׳��쳣
                        std::cerr << "Error: cannot read file data from buf string " << buf_str << std::endl; // �޸�
                        throw std::runtime_error("cannot read file data from buf string"); // �޸�
                    }
                }
                else {
                    // �����ȡʧ�ܣ���ӡ��������Ϣ�����׳��쳣
                    std::cerr << "Error: cannot read file size from buf string " << buf_str << std::endl; // �޸�
                    throw std::runtime_error("cannot read file size from buf string"); // �޸�
                }
            }
            else {
                // �����ȡʧ�ܣ��ж��Ƿ񵽴�buf�ַ�����ĩβ������ǣ����˳�ѭ����������ǣ����ӡ��������Ϣ�����׳��쳣
                if (in_buf.eof()) {
                    end_of_str = true; // �޸�
                }
                else {
                    std::cerr << "Error: cannot read file name from buf string " << buf_str << std::endl; // �޸�
                    throw std::runtime_error("cannot read file name from buf string"); // �޸�
                }
            }
        }
        // ����Ҫ�ر�buf�ڴ����������Զ�����
    }
    else {
        // ���buf�ڴ�������ʧ�ܣ���ӡ��������Ϣ�����׳��쳣
        std::cerr << "Error: cannot create buf memory stream " << buf_str << std::endl; // �޸�
        throw std::runtime_error("cannot create buf memory stream"); // �޸�
    }
}

