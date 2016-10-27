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
#define FBUFSIZE	2048
// ������ �������� ������
#define IBUFSIZE	2048
// ������ ��������� ������
#define ABUFSIZE	2048
// ������ ������������ ������
#define NBUFSIZE	2048
// ������ ��������� ������
#define OBUFSIZE	2048
// ������������ ���������� ����
#define MAXFORMS	1024
#include <stdint.h>
#endif // TARGET == LINUX

#if TARGET == ESP8266
// ������ ��������� ������
#define FBUFSIZE	512
// ������ �������� ������
#define IBUFSIZE	512
// ������ ��������� ������
#define ABUFSIZE	512
// ������ ���������� ������
#define NBUFSIZE	512
// ������ ������������ ������
#define OBUFSIZE	512
// ������������ ���������� ����
#define MAXFORMS	256
#endif // TARGET == ESP8266

#if CHARSIZE == ANSI
typedef uint8_t		litera;
#define NEXTPARAM	((litera)17)
#define ACTIVEFUN	((litera)18)
#define NEYTRALFUN	((litera)19)
#define ENDFUN		((litera)20)
#endif

#if CHARSIZE == UNICODE
typedef uint16_t	litera;
#define NEXTPARAM	((litera)0xee17)
#define ACTIVEFUN	((litera)0xee18)
#define NEYTRALFUN	((litera)0xee19)
#define ENDFUN		((litera)0xee20)
#endif


#endif // __PROJECTCFG_H__