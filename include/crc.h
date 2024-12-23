#pragma once

// CRC-16算法的参数
#define POLY 0x8005 // 生成多项式
#define INIT 0x0000 // 初始值
#define XOROUT 0x0000 // 结果异或值
#define REFIN true // 输入反转
#define REFOUT true // 输出反转

// 反转一个字节的位
unsigned char reverse_byte(unsigned char x);

// 计算CRC-16校验码
unsigned short crc16(char* data, int len);