#ifndef __PROJECTCFG_H__
#define __PROJECTCFG_H__

//// ���� ��� ��������:
#define LINUX	0
#define ESP8266	1
#define STM32F1	2
#define STM32F4	3
#define MSDOS	4

#define TARGET LINUX

//// ������ �������� 2 ����� ��� UNICODE ��� 1 ��� ANSI.
#define UNICODE	2
#define ANSI 	1

#define CHARSIZE ANSI

//// ��������


//// �����������

#if TARGET == LINUX
// ������ ��������� ������
#define FBUFFSIZE	2048
// ������ �������� ������
#define IBUFFSIZE	2048
// ������ ��������� ������
#define ABUFFSIZE	2048
// ������ ���������� ������
#define PBUFFSIZE	2048
// ������ ��������� ������
#define OBUFFSIZE	2048
// ������������ ���������� ����
#define MAXFORMS	1024
#endif // TARGET == LINUX

#if TARGET == ESP8266
// ������ ��������� ������
#define FBUFFSIZE	512
// ������ �������� ������
#define IBUFFSIZE	512
// ������ ��������� ������
#define ABUFFSIZE	512
// ������ ���������� ������
#define PBUFFSIZE	512
// ������ ��������� ������
#define OBUFFSIZE	512
// ������������ ���������� ����
#define MAXFORMS	256
#endif // TARGET == ESP8266

#if CHARSIZE == ANSI
#define NEXTPARAM	((char)17)
#define ACTIVEFUN	((char)18)
#define NEYTRALFUN	((char)19)
#define ENDFUN		((char)20)
#endif

#if CHARSIZE == UNICODE
#define NEXTPARAM	(0xee17)
#define ACTIVEFUN	(0xee18)
#define NEYTRALFUN	(0xee19)
#define ENDFUN		(0xee20)
#endif


#endif // __PROJECTCFG_H__