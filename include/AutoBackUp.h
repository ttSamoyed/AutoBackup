#pragma once

#include "buf.h"
#include "utils.h"
#include <map>
#include <chrono>
#include <thread>

// 实现定时备份和实时备份

// 定义一个函数，用于监视文件夹的变化实时备份
void watch_folder(const fs::path& folder, const fs::path& desti);

// 定义一个回调函数，用于执行定时任务
void CALLBACK TimerCallback(PVOID lpParameter);

// 执行定时备份
void TimerBackUp(fs::path source, fs::path destination, std::string interval);