#ifndef __PROJECTCFG_H__
#define __PROJECTCFG_H__

#define TRUE 1
#define FALSE 0

//// Целевая платформа:
#define LINUX	0
#define ESP8266	1
#define STM32F1	2
#define STM32F4	3
#define MSDOS	4

#define TARGET LINUX

//// Размер символа 2 байта для UNICODE или 1 для ANSI.
#define UNICODE	2
#define ANSI 	1

#define CHARSIZE ANSI

//// Расширения базового синтаксиса (псевдонимы встроенных команд)
#define EXTENDEDSYNTAX TRUE

//// Разрешить выражения с плавающей точкой
#define FLOATINGPOINT TRUE

//// Проверки
#ifndef CHARSIZE
#define CHARSIZE ANSI
#endif


//// Прочее

#if TARGET == LINUX
// размер файлового буфера
#define FBUFSIZE	2048
// Размер входного буфера
#define IBUFSIZE	2048
// размер активного буфера
#define ABUFSIZE	2048
// Размер нейтрального буфера
#define NBUFSIZE	2048
// Размер выходного буфера
#define OBUFSIZE	2048
// Размер буфера результата функции
#define RBUFSIZE	1024
// Максимальное количество форм
#define MAXFORMS	1024
// Максимальная длина пути к файлу и имени файла
#define MAXFILEPATH	256
#include <stdint.h>
#endif // TARGET == LINUX

#if TARGET == ESP8266
// размер файлового буфера
#define FBUFSIZE	512
// Размер входного буфера
#define IBUFSIZE	512
// размер активного буфера
#define ABUFSIZE	512
// Размер нейтрального буфера
#define NBUFSIZE	512
// Размер выходного буфера
#define OBUFSIZE	512
// Размер буфера результата функции
#define RBUFSIZE	256
// Максимальное количество форм
#define MAXFORMS	256
// Максимальная длина пути к файлу и имени файла
#define MAXFILEPATH	64
#endif // TARGET == ESP8266

//// Вычисляемые

#if CHARSIZE == ANSI
typedef char		litera;
#define NEXTPARAM	((litera)17)
#define ACTIVEFUN	((litera)18)
#define NEYTRALFUN	((litera)19)
#define ENDFUN		((litera)20)
#define FORMBRAKE ((litera)17)

#elif CHARSIZE == UNICODE
typedef uint16_t	litera;
#define NEXTPARAM	((litera)0xee17)
#define ACTIVEFUN	((litera)0xee18)
#define NEYTRALFUN	((litera)0xee19)
#define ENDFUN		((litera)0xee20)
#define FORMBRAKE ((litera)0xee17)
#endif

#endif // __PROJECTCFG_H__
