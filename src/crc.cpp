#include "crc.h"

// 反转一个字节的位
unsigned char reverse_byte(unsigned char x) {
    x = (x & 0x55) << 1 | (x & 0xAA) >> 1; // 交换相邻的位
    x = (x & 0x33) << 2 | (x & 0xCC) >> 2; // 交换相邻的2位
    x = (x & 0x0F) << 4 | (x & 0xF0) >> 4; // 交换相邻的4位
    return x;
}

// 计算CRC-16校验码
unsigned short crc16(char* data, int len) {
    unsigned short crc = INIT; // CRC寄存器，初始值为INIT
    for (int i = 0; i < len; i++) { // 遍历数据的每个字节
        unsigned char byte = data[i]; // 取出一个字节
        if (REFIN) byte = reverse_byte(byte); // 如果输入反转，先反转字节的位
        crc ^= byte << 8; // 将字节与CRC寄存器的高8位异或
        for (int j = 0; j < 8; j++) { // 遍历字节的每一位
            if (crc & 0x8000) { // 如果CRC寄存器的最高位为1
                crc = (crc << 1) ^ POLY; // 左移一位，并与生成多项式异或
            }
            else { // 如果CRC寄存器的最高位为0
                crc = crc << 1; // 左移一位
            }
        }
    }
    if (REFOUT) crc = reverse_byte(crc >> 8) | reverse_byte(crc & 0xFF) << 8; // 如果输出反转，反转CRC寄存器的位
    crc ^= XOROUT; // 与结果异或值异或，得到最终的CRC校验码
    return crc;
}
