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
#define FBUFFSIZE	2048
// размер входного буфера
#define IBUFFSIZE	2048
// размер активного буфера
#define ABUFFSIZE	2048
// размер пассивного буфера
#define PBUFFSIZE	2048
// размер выходного буфера
#define OBUFFSIZE	2048
// максимальное количество форм
#define MAXFORMS	1024
#endif // TARGET == LINUX

#if TARGET == ESP8266
// размер файлового буфера
#define FBUFFSIZE	512
// размер входного буфера
#define IBUFFSIZE	512
// размер активного буфера
#define ABUFFSIZE	512
// размер пассивного буфера
#define PBUFFSIZE	512
// размер выходного буфера
#define OBUFFSIZE	512
// максимальное количество форм
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