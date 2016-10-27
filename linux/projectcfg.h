#ifndef __PROJECTCFG_H__
#define __PROJECTCFG_H__

//// Подо что собираем:
#define LINUX	0
#define ESP8266	1
#define STM32F1	2
#define STM32F4	3
#define MSDOS	4

#define TARGET LINUX

//// Размер символов 2 байта для UNICODE или 1 для ANSI.
#define UNICODE	2
#define ANSI 	1

#define CHARSIZE ANSI

//// Проверки


//// Вычисляемые

#if TARGET == LINUX
// размер файлового буфера
#define FBUFSIZE	2048
// размер входного буфера
#define IBUFSIZE	2048
// размер активного буфера
#define ABUFSIZE	2048
// размер нейтрального буфера
#define NBUFSIZE	2048
// размер выходного буфера
#define OBUFSIZE	2048
// максимальное количество форм
#define MAXFORMS	1024
#include <stdint.h>
#endif // TARGET == LINUX

#if TARGET == ESP8266
// размер файлового буфера
#define FBUFSIZE	512
// размер входного буфера
#define IBUFSIZE	512
// размер активного буфера
#define ABUFSIZE	512
// размер пассивного буфера
#define NBUFSIZE	512
// размер нейтрального буфера
#define OBUFSIZE	512
// максимальное количество форм
#define MAXFORMS	256
#endif // TARGET == ESP8266

#if CHARSIZE == ANSI
typedef litera		uint8_t;
#define NEXTPARAM	((litera)17)
#define ACTIVEFUN	((litera)18)
#define NEYTRALFUN	((litera)19)
#define ENDFUN		((litera)20)
#endif

#if CHARSIZE == UNICODE
typedef litera		uint16_t;
#define NEXTPARAM	((litera)0xee17)
#define ACTIVEFUN	((litera)0xee18)
#define NEYTRALFUN	((litera)0xee19)
#define ENDFUN		((litera)0xee20)
#endif


#endif // __PROJECTCFG_H__