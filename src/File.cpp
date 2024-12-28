#include "File.h"

// ���ļ�����������һ���ļ�·����Ϊ����������һ��File�ṹ��
File read_file(const fs::path& path) {
    // ����File�ṹ�����
    File file;
    // ��ȡ�ļ�������ֵ���ṹ���name��Ա
    // file.name = path.filename().string();
    file.name = path.string();
    // ���ļ���
    std::ifstream in(path, std::ios::binary);
    // �ж��ļ��Ƿ�򿪳ɹ�
    if (in.is_open()) {
        // ��ȡ�ļ���С����ֵ���ṹ���size��Ա
        file.size = fs::file_size(path);
        // Ϊ�ṹ���data��Ա����ռ�
        file.data.resize(file.size);
        // ��ȡ�ļ����ݲ��洢���ṹ���data��Ա��
        in.read(file.data.data(), file.size);
        // �ر��ļ���
        in.close();
    }
    else {
        // �ļ���ʧ��:��ӡ��������Ϣ�������ṹ���size��Ա��Ϊ0��ʾ��Ч�����׳��쳣
        std::cerr << "Error: cannot open file " << path << std::endl;
        file.size = 0;
        throw std::runtime_error("cannot open file");
    }
    // ����File�ṹ�����
    return file;
}

// д�ļ�����������һ��File�ṹ���һ���ļ�·����Ϊ��������File�ṹ��д�뵽ָ�����ļ���
void write_file(const File& file, const fs::path& path) {
    fs::create_directories(path.parent_path());

    // ���ļ���
    std::ofstream out(path, std::ios::app | std::ios::binary); // ע��ʹ��std::ios::appģʽ����ʾÿ��д�붼��׷�ӵ��ļ�ĩβ
    // �ж��ļ��Ƿ�򿪳ɹ�
    if (out.is_open()) {
        out.write(file.data.data(), file.size);
        // �ر��ļ���
        out.close();
    }
    else {
        // ����ļ���ʧ�ܣ���ӡ��������Ϣ�����׳��쳣
        std::cerr << "Error: cannot write file " << path << std::endl;
        throw std::runtime_error("cannot write file");
    }
}