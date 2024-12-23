#include "AutoBackUp.h"
#include <map>
#include <chrono>
#include <thread>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

// 定义一个函数，用于监视文件夹的变化
void watch_folder(const fs::path& folder, const fs::path& desti) {
    // 创建一个map容器，用于存储文件名和最后修改时间的对应关系
    std::map<std::string, fs::file_time_type> files;

    // 遍历文件夹中的文件，将其加入到map中
    for (const auto& entry : fs::recursive_directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            files[entry.path().string()] = fs::last_write_time(entry);
        }
    }

    // 定义一个循环，每隔一秒检查一次文件夹的变化
    while (true) {
        // 定义一个标志，用于判断是否需要备份文件夹
        bool need_backup = false;

        // 遍历文件夹中的文件，与map中的数据进行比较
        for (const auto& entry : fs::recursive_directory_iterator(folder)) {
            if (entry.is_regular_file()) {
                // 获取文件名和最后修改时间
                std::string name = entry.path().string();
                fs::file_time_type time = fs::last_write_time(entry);

                // 如果map中没有该文件，说明是新添加的文件，输出信息并加入到map中
                if (files.find(name) == files.end()) {
                    std::cout << "New file added: " << name << "\n";
                    files[name] = time;
                    need_backup = true;
                }
                // 如果map中有该文件，但最后修改时间不同，说明是修改过的文件，输出信息并更新map中的数据
                else if (files[name] != time) {
                    std::cout << "File modified: " << name << "\n";
                    files[name] = time;
                    need_backup = true;
                }
            }
        }

        // 遍历map中的数据，与文件夹中的文件进行比较
        for (auto it = files.begin(); it != files.end();) {
            // 获取文件名和最后修改时间
            std::string name = it->first;

            // 如果文件夹中没有该文件，说明是删除了的文件，输出信息并从map中删除
            if (!fs::exists(name)) {
                std::cout << "File deleted: " << name << "\n";
                it = files.erase(it);
                need_backup = true;
            } else {
                it++;
            }
        }

        // 如果文件夹发生了变化，调用backup_folder_as_buf函数，将其备份到另一个文件夹中
        if (need_backup) {
            backup_folder_as_buf(folder, desti);
            std::cout << "Folder " << folder << " backed up to " << desti << "\n";
        }

        // 等待一秒
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// 定义一个回调函数，用于执行定时任务
void TimerCallback(void* lpParameter) {
    fs::path* paths = reinterpret_cast<fs::path*>(lpParameter);
    // 调用备份函数，传入参数
    backup_folder_as_buf(paths[0], paths[1]);
}

void TimerBackUp(fs::path source, fs::path destination, std::string interval) {
    int t = std::stoi(interval) * 1000;

#ifdef _WIN32
    // Windows 定时器队列实现
    HANDLE hTimerQueue = CreateTimerQueue();
    if (hTimerQueue == NULL) {
        std::cerr << "CreateTimerQueue failed: " << GetLastError() << std::endl;
        return;
    }

    HANDLE hTimer = NULL;
    fs::path* paths = new fs::path[2];
    paths[0] = source;
    paths[1] = destination;

    if (!CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)TimerCallback, paths, 1000, t, 0)) {
        std::cerr << "CreateTimerQueueTimer failed: " << GetLastError() << std::endl;
        return;
    }
#else
    // macOS/Linux 使用 std::thread 模拟定时器
    std::thread([source, destination, t]() {
        fs::path paths[2] = {source, destination};
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(t));
            TimerCallback(paths);
        }
    }).detach();
#endif
}
