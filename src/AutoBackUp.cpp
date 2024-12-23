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

// ����һ�����������ڼ����ļ��еı仯
void watch_folder(const fs::path& folder, const fs::path& desti) {
    // ����һ��map���������ڴ洢�ļ���������޸�ʱ��Ķ�Ӧ��ϵ
    std::map<std::string, fs::file_time_type> files;

    // �����ļ����е��ļ���������뵽map��
    for (const auto& entry : fs::recursive_directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            files[entry.path().string()] = fs::last_write_time(entry);
        }
    }

    // ����һ��ѭ����ÿ��һ����һ���ļ��еı仯
    while (true) {
        // ����һ����־�������ж��Ƿ���Ҫ�����ļ���
        bool need_backup = false;

        // �����ļ����е��ļ�����map�е����ݽ��бȽ�
        for (const auto& entry : fs::recursive_directory_iterator(folder)) {
            if (entry.is_regular_file()) {
                // ��ȡ�ļ���������޸�ʱ��
                std::string name = entry.path().string();
                fs::file_time_type time = fs::last_write_time(entry);

                // ���map��û�и��ļ���˵��������ӵ��ļ��������Ϣ�����뵽map��
                if (files.find(name) == files.end()) {
                    std::cout << "New file added: " << name << "\n";
                    files[name] = time;
                    need_backup = true;
                }
                // ���map���и��ļ���������޸�ʱ�䲻ͬ��˵�����޸Ĺ����ļ��������Ϣ������map�е�����
                else if (files[name] != time) {
                    std::cout << "File modified: " << name << "\n";
                    files[name] = time;
                    need_backup = true;
                }
            }
        }

        // ����map�е����ݣ����ļ����е��ļ����бȽ�
        for (auto it = files.begin(); it != files.end();) {
            // ��ȡ�ļ���������޸�ʱ��
            std::string name = it->first;

            // ����ļ�����û�и��ļ���˵����ɾ���˵��ļ��������Ϣ����map��ɾ��
            if (!fs::exists(name)) {
                std::cout << "File deleted: " << name << "\n";
                it = files.erase(it);
                need_backup = true;
            } else {
                it++;
            }
        }

        // ����ļ��з����˱仯������backup_folder_as_buf���������䱸�ݵ���һ���ļ�����
        if (need_backup) {
            backup_folder_as_buf(folder, desti);
            std::cout << "Folder " << folder << " backed up to " << desti << "\n";
        }

        // �ȴ�һ��
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// ����һ���ص�����������ִ�ж�ʱ����
void TimerCallback(void* lpParameter) {
    fs::path* paths = reinterpret_cast<fs::path*>(lpParameter);
    // ���ñ��ݺ������������
    backup_folder_as_buf(paths[0], paths[1]);
}

void TimerBackUp(fs::path source, fs::path destination, std::string interval) {
    int t = std::stoi(interval) * 1000;

#ifdef _WIN32
    // Windows ��ʱ������ʵ��
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
    // macOS/Linux ʹ�� std::thread ģ�ⶨʱ��
    std::thread([source, destination, t]() {
        fs::path paths[2] = {source, destination};
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(t));
            TimerCallback(paths);
        }
    }).detach();
#endif
}
