#pragma once

// CRC-16�㷨�Ĳ���
#define POLY 0x8005 // ���ɶ���ʽ
#define INIT 0x0000 // ��ʼֵ
#define XOROUT 0x0000 // ������ֵ
#define REFIN true // ���뷴ת
#define REFOUT true // �����ת

// ��תһ���ֽڵ�λ
unsigned char reverse_byte(unsigned char x);

// ����CRC-16У����
unsigned short crc16(char* data, int len);