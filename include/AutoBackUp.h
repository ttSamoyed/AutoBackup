#pragma once

#include "buf.h"
#include "utils.h"
#include <map>
#include <chrono>
#include <thread>

// ʵ�ֶ�ʱ���ݺ�ʵʱ����

// ����һ�����������ڼ����ļ��еı仯ʵʱ����
void watch_folder(const fs::path& folder, const fs::path& desti);

// ����һ���ص�����������ִ�ж�ʱ����
void CALLBACK TimerCallback(PVOID lpParameter);

// ִ�ж�ʱ����
void TimerBackUp(fs::path source, fs::path destination, std::string interval);