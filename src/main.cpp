#include "utils.h"
#include "AutoBackUp.h"
#include "xxtea.h"

int main() {
    /*
    fs::path source = "C:\\Users\\John\\Desktop\\test_backUp";
    fs::path destination = "C:\\Users\\John\\Desktop\\bing_shock";
    std::map<std::string, std::string> condition;
    //condition["file_name"] = "1.txt";
    //condition["file_type"] = "pdf";
    //condition["file_time"] = "3";
    //condition["file_size"] = "3";
    //TimerBackUp(source, destination);
    //backup_folder_as_buf(source, destination, condition);
    restore_folder_from_buf("C:\\Users\\John\\Desktop\\bing_shock\\test_backUp_2023_12_14_11_33_30_55329.buf", "C:\\Users\\John\\Desktop\\test1");
    //watch_folder(source, destination);
    //encrypt_buf("C:\\Users\\John\\Desktop\\bing_shock\\test_backUp.buf", "123");
    //decrypt_ebuf("C:\\Users\\John\\Desktop\\bing_shock\\test_backUp.ebuf", "123", "C:\\Users\\John\\Desktop\\test1");
    */
    
    std::cout << "*****************************************" << '\n';
    std::cout << "******                             ******" << '\n';
    std::cout << "*****       欢迎使用备份软件！      *****" << '\n';
    std::cout << "*****  开发者:张鹏成,曾楚童,李宝锟  *****" << '\n';
    std::cout << "******                             ******" << '\n';
    std::cout << "*****************************************" << '\n';

    char op = '1';

    while (op >= '1' && op <= '7') {
        std::cout << '\n';
        std::cout << "请输入您的需求：" << '\n';
        std::cout << "1:备份文件 2:还原文件 3:自定义备份 4:加密文件，" << '\n';
        std::cout << "5:解密文件 6:定时备份 7: 实时备份 8:退出软件" << '\n';
        std::cin >> op;
        if (op == '1') {
            fs::path source, middle;
            std::cout << "请输入想要备份的源文件（夹）：" << '\n';
            std::string tmp;
            std::cin >> tmp;
            source = tmp;
            std::cout << "请输入 buf 文件存放的文件夹：" << '\n';
            std::cin >> tmp;
            middle = tmp;
            backup_folder_as_buf(source, middle);
            std::cout << "备份成功！" << "\n\n";
        }
        else if (op == '2') {
            fs::path buf_file, target;
            std::cout << "请输入想要还原的 buf 文件的路径：" << '\n';
            std::string tmp;
            std::cin >> tmp;
            buf_file = tmp;
            std::cout << "请输入还原文件存放的文件夹：" << '\n';
            std::cin >> tmp;
            target = tmp;
            restore_folder_from_buf(buf_file, target);
            std::cout << "还原成功！" << std::endl;
        }
        else if (op == '3') {
            fs::path source;
            std::cout << "请输入想要备份的源文件夹：" << '\n';
            std::string tmp;
            std::cin >> tmp;
            source = tmp;
            std::map<std::string, std::string> condition;
            std::cout << "请输入文件名（若没有要求输入0）：" << '\n';
            std::string filename;
            std::cin >> filename;
            if (filename != "0") {
                condition["file_name"] = filename;
            }
            std::cout << "请输入文件类型（若没有要求输入0）：" << '\n';
            std::string filetype;
            std::cin >> filetype;
            if (filetype != "0") {
                condition["file_type"] = filetype;
            }
            std::cout << "请输入最后修改时间距今最多天数（若没有要求输入0）：" << '\n';
            std::string filetime;
            std::cin >> filetime;
            if (filetime != "0") {
                condition["file_time"] = filetime;
            }
            std::cout << "请输入最大大小（单位KB，若没有要求输入0）：" << '\n';
            std::string filesize;
            std::cin >> filesize;
            if (filesize != "0") {
                condition["file_size"] = filesize;
            }
            fs::path middle;
            std::cout << "请输入 buf 文件存放的文件夹：" << '\n';
            std::cin >> tmp;
            middle = tmp;
            backup_folder_as_buf(source, middle, condition);
            std::cout << "备份成功！" << "\n\n";
        }
        else if (op == '4') {
            std::cout << "请输入 buf 文件的地址：" << '\n';
            std::string source;
            std::cin >> source;
            std::cout << "请输入密码：" << '\n';
            std::string key;
            std::cin >> key;
            encrypt_buf(source, key.c_str());
            std::cout << "加密成功！" << "\n\n";
        }
        else if (op == '5') {
            std::cout << "请输入 ebuf 文件的地址：" << '\n';
            std::string source;
            std::cin >> source;
            std::cout << "请输入密码：" << '\n';
            std::string key;
            std::cin >> key;
            std::cout << "请输入还原文件存放的文件夹：" << '\n';
            std::string destination;
            std::cin >> destination;
            decrypt_ebuf(source, key.c_str(), destination);
            std::cout << "解密成功！" << "\n\n";
        }
        else if (op == '6') {
            fs::path source, middle;

            std::cout << "请输入想要备份的源文件夹：" << '\n';
            std::string tmp;
            std::cin >> tmp;
            source = tmp;

            std::cout << "请输入 buf 文件存放的文件夹：" << '\n';
            std::cin >> tmp;
            middle = tmp;

            std::cout << "请输入备份间隔时间（单位 秒）：" << '\n';
            std::string interval;
            std::cin >> interval;

            std::cout << "开始定时备份..." << '\n';
            TimerBackUp(source, middle, interval);
            system("pause");
        }
        else if (op == '7') {
            fs::path source, middle;

            std::cout << "请输入想要备份的源文件（夹）：" << '\n';
            std::string tmp;
            std::cin >> tmp;
            source = tmp;

            std::cout << "请输入 buf 文件存放的文件夹：" << '\n';
            std::cin >> tmp;
            middle = tmp;

            std::cout << "开始监测目标文件夹..." << '\n';
            watch_folder(source, middle);
            system("pause");
        }
        else {
            exit(0);
        }
    }
    return 0;
}