#include "HuffmanCoder.h"

HuffmanNode::HuffmanNode(char c, size_t w) : ch(c), weight(w), left(nullptr), right(nullptr), code("") {}

HuffmanNode::HuffmanNode() : ch('\0'), weight(0), left(nullptr), right(nullptr), code("") {}

bool HuffmanNode::operator < (const HuffmanNode& other) const {
    return weight > other.weight; // Ȩ��ԽС�����ȼ�Խ��
}

HuffmanCoder::HuffmanCoder() : root(new HuffmanNode()) {}

HuffmanCoder::HuffmanCoder(const std::string& str) {
    // �ж��ַ����Ƿ�Ϊ��
    if (str.empty()) {
        // ���Ϊ�գ���ӡ��������Ϣ�����׳��쳣
        std::cerr << "Error: empty string for Huffman coding" << std::endl;
        throw std::invalid_argument("empty string for Huffman coding");
    }

    // ����һ����ϣ������ͳ��ÿ���ַ����ֵĴ���
    std::unordered_map<char, size_t> freq;
    for (char c : str) {
        freq[c]++;
    }

    // ����һ�����ȶ��У����ڴ洢���������Ľڵ㣬������Ȩ�ش�С��������
    std::priority_queue<HuffmanNode> pq;
    for (const auto& pair : freq) {
        // ����ֵ����Ϊ����������һ���������ڵ㣬���������
        pq.push(HuffmanNode(pair.first, pair.second));
    }

    // �����ȶ�����ֻʣ��һ���ڵ�ʱ��ֹͣѭ��
    while (pq.size() > 1) {
        // ȡ�����׵������ڵ㣬�������ǳ���
        HuffmanNode left = pq.top();
        pq.pop();
        HuffmanNode right = pq.top();
        pq.pop();
        HuffmanNode h = HuffmanNode('\0', left.weight + right.weight);
        h.left = new HuffmanNode(left);
        h.right = new HuffmanNode(right);
        pq.push(h);
    }

    // ȡ�����ʣ�µĽڵ㣬�����丳ֵ�����ڵ�
    root = new HuffmanNode(pq.top());
    // ����generate_code������������ڵ�Ϳ��ַ�����Ϊ���������ɱ����ͽ����
    generate_code(root, "");
}

// ����һ������������һ���������ڵ��һ���ַ�����Ϊ�������ݹ����ɱ����ͽ����
void HuffmanCoder::generate_code(HuffmanNode* node, const std::string& prefix) {
    // �жϽڵ��Ƿ�Ϊ��
    if (node == nullptr) {
        // ���Ϊ�գ�ֱ�ӷ���
        return;
    }

    // �жϽڵ��Ƿ���Ҷ�ӽڵ�
    if (node->left == nullptr && node->right == nullptr) {
        // �����Ҷ�ӽڵ㣬���ڵ���ַ����ַ�����Ϊ��ֵ�ԣ�����������
        encoder[node->ch] = prefix;
        // ���ַ����ͽڵ���ַ���Ϊ��ֵ�ԣ�����������
        decoder[prefix] = node->ch;
    } else {
        // �������Ҷ�ӽڵ㣬�ݹ���ñ��������ֱ��������ӽڵ���ַ�������0��1��Ϊ����
        generate_code(node->left, prefix + "0");
        generate_code(node->right, prefix + "1");
    }
}

// ����һ������������һ���������ڵ���Ϊ�������ݹ�ɾ���������������нڵ�
void HuffmanCoder::delete_tree(HuffmanNode* node) {
    // �жϽڵ��Ƿ�Ϊ��
    if (node == nullptr) {
        // ���Ϊ�գ�ֱ�ӷ���
        return;
    }
    // �ݹ���ñ��������ֱ��������ӽڵ���Ϊ����
    HuffmanCoder::delete_tree(node->left);
    HuffmanCoder::delete_tree(node->right);
    // ɾ����ǰ�ڵ�
    delete node;
}

// ����һ������������һ���ַ�����Ϊ���������ظ��ַ������������������Ľ��
std::string HuffmanCoder::encode(const std::string& str) {
    // ����һ�����ַ��������ڴ洢������
    std::string result = "";
    // �����ַ����е�ÿ���ַ�
    for (char c : str) {
        // �ڱ�����в��Ҹ��ַ���Ӧ�ı��룬������׷�ӵ�����ַ�����
        result += encoder[c];
    }
    
    return result;
}

// ����һ������������һ�������ַ�����Ϊ���������ظ��ַ������������������Ľ��
std::string HuffmanCoder::decode(const std::string& code) {
    // ����һ�����ַ��������ڴ洢������
    std::string result = "";
    // ����һ����ʱ�ַ��������ڴ洢��ǰ�ı���Ƭ��
    std::string temp = "";
    
    for (char c : code) {
        // ���ַ�׷�ӵ���ʱ�ַ�����
        temp += c;
        // �ڽ�����в�����ʱ�ַ�����Ӧ���ַ�
        auto it = decoder.find(temp);
        // �ж��Ƿ��ҵ���ƥ����ַ�
        if (it != decoder.end()) {
            // ����ҵ���ƥ����ַ�������׷�ӵ�����ַ����У��������ʱ�ַ���
            result += it->second;
            temp = "";
        }
    }
    // ���ؽ���ַ���
    return result;
}

// ����һ����������HuffmanCoder���󱣴浽�ļ���
void HuffmanCoder::save_to_file(const std::filesystem::path& file) {
    // ���ļ�������������򴴽���ע���Զ����ƴ򿪣�ĩβ����
    std::ofstream out(file, std::ios::app | std::ios::binary);
    // ����ļ��Ƿ�򿪳ɹ�
    if (out) {
        // д��encoder�Ĵ�С
        size_t encoder_size = encoder.size();
        out.write(reinterpret_cast<const char*>(&encoder_size), sizeof(encoder_size));
        // ����encoder�е�ÿ����ֵ��
        for (const auto& pair : encoder) {
            // д���������
            char key = pair.first;
            out.write(reinterpret_cast<const char*>(&key), sizeof(key));
            // д��ֵ�ĳ��Ⱥ�����
            size_t value_size = pair.second.size();
            out.write(reinterpret_cast<const char*>(&value_size), sizeof(value_size));
            out.write(pair.second.data(), value_size);
        }
        // д��decoder�Ĵ�С
        size_t decoder_size = decoder.size();
        out.write(reinterpret_cast<const char*>(&decoder_size), sizeof(decoder_size));
        // ����decoder�е�ÿ����ֵ��
        for (const auto& pair : decoder) {
            // д����ĳ��Ⱥ�����
            size_t key_size = pair.first.size();
            out.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
            out.write(pair.first.data(), key_size);
            // д��ֵ������
            char value = pair.second;
            out.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
        // �ر��ļ�
        out.close();
    }
    else {
        // �ļ���ʧ�ܣ��׳��쳣
        throw std::runtime_error("Failed to open file: " + file.string());
    }
}

// ����һ�����������ļ��ж�ȡһ��HuffmanCoder����
void HuffmanCoder::load_from_file(const std::filesystem::path& file) {
    // ���encoder��decoder
    encoder.clear();
    decoder.clear();
    // ���ļ���������������׳��쳣��ע������Ƹ�ʽ��
    std::ifstream in(file, std::ios::binary);
    // ����ļ��Ƿ�򿪳ɹ�
    if (in) {
        // ��ȡencoder�Ĵ�С
        size_t encoder_size;
        in.read(reinterpret_cast<char*>(&encoder_size), sizeof(encoder_size));
        // ѭ����ȡÿ����ֵ��
        for (size_t i = 0; i < encoder_size; i++) {
            // ��ȡ��������
            char key;
            in.read(reinterpret_cast<char*>(&key), sizeof(key));
            // ��ȡֵ�ĳ��Ⱥ�����
            size_t value_size;
            in.read(reinterpret_cast<char*>(&value_size), sizeof(value_size));
            std::string value(value_size, '\0');
            in.read(value.data(), value_size);
            // ����ֵ�Բ��뵽encoder��
            encoder.insert({ key, value });
        }
        // ��ȡdecoder�Ĵ�С
        size_t decoder_size;
        in.read(reinterpret_cast<char*>(&decoder_size), sizeof(decoder_size));
        // ѭ����ȡÿ����ֵ��
        for (size_t i = 0; i < decoder_size; i++) {
            // ��ȡ���ĳ��Ⱥ�����
            size_t key_size;
            in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
            std::string key(key_size, '\0');
            in.read(key.data(), key_size);
            // ��ȡֵ������
            char value;
            in.read(reinterpret_cast<char*>(&value), sizeof(value));
            // ����ֵ�Բ��뵽decoder��
            decoder.insert({ key, value });
        }
        // �ر��ļ�
        in.close();
    }
    else {
        // �ļ���ʧ�ܣ��׳��쳣
        throw std::runtime_error("Failed to open file: " + file.string());
    }
}
