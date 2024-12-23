#include "crc.h"

// ��תһ���ֽڵ�λ
unsigned char reverse_byte(unsigned char x) {
    x = (x & 0x55) << 1 | (x & 0xAA) >> 1; // �������ڵ�λ
    x = (x & 0x33) << 2 | (x & 0xCC) >> 2; // �������ڵ�2λ
    x = (x & 0x0F) << 4 | (x & 0xF0) >> 4; // �������ڵ�4λ
    return x;
}

// ����CRC-16У����
unsigned short crc16(char* data, int len) {
    unsigned short crc = INIT; // CRC�Ĵ�������ʼֵΪINIT
    for (int i = 0; i < len; i++) { // �������ݵ�ÿ���ֽ�
        unsigned char byte = data[i]; // ȡ��һ���ֽ�
        if (REFIN) byte = reverse_byte(byte); // ������뷴ת���ȷ�ת�ֽڵ�λ
        crc ^= byte << 8; // ���ֽ���CRC�Ĵ����ĸ�8λ���
        for (int j = 0; j < 8; j++) { // �����ֽڵ�ÿһλ
            if (crc & 0x8000) { // ���CRC�Ĵ��������λΪ1
                crc = (crc << 1) ^ POLY; // ����һλ���������ɶ���ʽ���
            }
            else { // ���CRC�Ĵ��������λΪ0
                crc = crc << 1; // ����һλ
            }
        }
    }
    if (REFOUT) crc = reverse_byte(crc >> 8) | reverse_byte(crc & 0xFF) << 8; // ��������ת����תCRC�Ĵ�����λ
    crc ^= XOROUT; // �������ֵ��򣬵õ����յ�CRCУ����
    return crc;
}
