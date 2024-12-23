#include "HuffmanCoder.h"

HuffmanNode::HuffmanNode(char c, size_t w) : ch(c), weight(w), left(nullptr), right(nullptr), code("") {}

HuffmanNode::HuffmanNode() : ch('\0'), weight(0), left(nullptr), right(nullptr), code("") {}

bool HuffmanNode::operator < (const HuffmanNode& other) const {
    return weight > other.weight; // 权重越小，优先级越高
}

HuffmanCoder::HuffmanCoder() : root(new HuffmanNode()) {}

HuffmanCoder::HuffmanCoder(const std::string& str) {
    // 判断字符串是否为空
    if (str.empty()) {
        // 如果为空，打印出错误信息，并抛出异常
        std::cerr << "Error: empty string for Huffman coding" << std::endl;
        throw std::invalid_argument("empty string for Huffman coding");
    }

    // 定义一个哈希表，用于统计每个字符出现的次数
    std::unordered_map<char, size_t> freq;
    for (char c : str) {
        freq[c]++;
    }

    // 定义一个优先队列，用于存储哈夫曼树的节点，并按照权重从小到大排序
    std::priority_queue<HuffmanNode> pq;
    for (const auto& pair : freq) {
        // 将键值对作为参数，创建一个哈夫曼节点，并将其入队
        pq.push(HuffmanNode(pair.first, pair.second));
    }

    // 当优先队列中只剩下一个节点时，停止循环
    while (pq.size() > 1) {
        // 取出队首的两个节点，并将它们出队
        HuffmanNode left = pq.top();
        pq.pop();
        HuffmanNode right = pq.top();
        pq.pop();
        HuffmanNode h = HuffmanNode('\0', left.weight + right.weight);
        h.left = new HuffmanNode(left);
        h.right = new HuffmanNode(right);
        pq.push(h);
    }

    // 取出最后剩下的节点，并将其赋值给根节点
    root = new HuffmanNode(pq.top());
    // 调用generate_code函数，传入根节点和空字符串作为参数，生成编码表和解码表
    generate_code(root, "");
}

// 定义一个函数，接受一个哈夫曼节点和一个字符串作为参数，递归生成编码表和解码表
void HuffmanCoder::generate_code(HuffmanNode* node, const std::string& prefix) {
    // 判断节点是否为空
    if (node == nullptr) {
        // 如果为空，直接返回
        return;
    }

    // 判断节点是否是叶子节点
    if (node->left == nullptr && node->right == nullptr) {
        // 如果是叶子节点，将节点的字符和字符串作为键值对，存入编码表中
        encoder[node->ch] = prefix;
        // 将字符串和节点的字符作为键值对，存入解码表中
        decoder[prefix] = node->ch;
    } else {
        // 如果不是叶子节点，递归调用本函数，分别传入左右子节点和字符串加上0或1作为参数
        generate_code(node->left, prefix + "0");
        generate_code(node->right, prefix + "1");
    }
}

// 定义一个函数，接受一个哈夫曼节点作为参数，递归删除哈夫曼树的所有节点
void HuffmanCoder::delete_tree(HuffmanNode* node) {
    // 判断节点是否为空
    if (node == nullptr) {
        // 如果为空，直接返回
        return;
    }
    // 递归调用本函数，分别传入左右子节点作为参数
    HuffmanCoder::delete_tree(node->left);
    HuffmanCoder::delete_tree(node->right);
    // 删除当前节点
    delete node;
}

// 定义一个函数，接受一个字符串作为参数，返回该字符串经过哈夫曼编码后的结果
std::string HuffmanCoder::encode(const std::string& str) {
    // 定义一个空字符串，用于存储编码结果
    std::string result = "";
    // 遍历字符串中的每个字符
    for (char c : str) {
        // 在编码表中查找该字符对应的编码，并将其追加到结果字符串中
        result += encoder[c];
    }
    
    return result;
}

// 定义一个函数，接受一个编码字符串作为参数，返回该字符串经过哈夫曼解码后的结果
std::string HuffmanCoder::decode(const std::string& code) {
    // 定义一个空字符串，用于存储解码结果
    std::string result = "";
    // 定义一个临时字符串，用于存储当前的编码片段
    std::string temp = "";
    
    for (char c : code) {
        // 将字符追加到临时字符串中
        temp += c;
        // 在解码表中查找临时字符串对应的字符
        auto it = decoder.find(temp);
        // 判断是否找到了匹配的字符
        if (it != decoder.end()) {
            // 如果找到了匹配的字符，将其追加到结果字符串中，并清空临时字符串
            result += it->second;
            temp = "";
        }
    }
    // 返回结果字符串
    return result;
}

// 定义一个函数，将HuffmanCoder对象保存到文件中
void HuffmanCoder::save_to_file(const std::filesystem::path& file) {
    // 打开文件，如果不存在则创建。注意以二进制打开，末尾附加
    std::ofstream out(file, std::ios::app | std::ios::binary);
    // 检查文件是否打开成功
    if (out) {
        // 写入encoder的大小
        size_t encoder_size = encoder.size();
        out.write(reinterpret_cast<const char*>(&encoder_size), sizeof(encoder_size));
        // 遍历encoder中的每个键值对
        for (const auto& pair : encoder) {
            // 写入键的内容
            char key = pair.first;
            out.write(reinterpret_cast<const char*>(&key), sizeof(key));
            // 写入值的长度和内容
            size_t value_size = pair.second.size();
            out.write(reinterpret_cast<const char*>(&value_size), sizeof(value_size));
            out.write(pair.second.data(), value_size);
        }
        // 写入decoder的大小
        size_t decoder_size = decoder.size();
        out.write(reinterpret_cast<const char*>(&decoder_size), sizeof(decoder_size));
        // 遍历decoder中的每个键值对
        for (const auto& pair : decoder) {
            // 写入键的长度和内容
            size_t key_size = pair.first.size();
            out.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
            out.write(pair.first.data(), key_size);
            // 写入值的内容
            char value = pair.second;
            out.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
        // 关闭文件
        out.close();
    }
    else {
        // 文件打开失败，抛出异常
        throw std::runtime_error("Failed to open file: " + file.string());
    }
}

// 定义一个函数，从文件中读取一个HuffmanCoder对象
void HuffmanCoder::load_from_file(const std::filesystem::path& file) {
    // 清空encoder和decoder
    encoder.clear();
    decoder.clear();
    // 打开文件，如果不存在则抛出异常。注意二进制格式打开
    std::ifstream in(file, std::ios::binary);
    // 检查文件是否打开成功
    if (in) {
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
            encoder.insert({ key, value });
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
            decoder.insert({ key, value });
        }
        // 关闭文件
        in.close();
    }
    else {
        // 文件打开失败，抛出异常
        throw std::runtime_error("Failed to open file: " + file.string());
    }
}
