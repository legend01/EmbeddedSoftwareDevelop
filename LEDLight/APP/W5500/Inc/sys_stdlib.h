/*
 * File:        stdlib.h
 * Purpose:     Function prototypes for standard library functions
 *
 * Notes:
 */

#ifndef SYS_STDLIB_H
#define SYS_STDLIB_H

/********************************************************************/
#include <stdarg.h> /*stdarg.h是C语言中C标准函数库的头文件，
                     stdarg是由standard（标准） arguments（参数）
                     简化而来，主要目的为让函数能够接收可变参数*/
/********************************************************************
 * Standard library functions
 ********************************************************************/
#include "w5500.h"
#define NULL 0

 int isspace ( int);

 int isalnum ( int);

 int isdigit ( int);

 int isupper ( int);

 int sys_strcasecmp (const  char *, const  char *);

 int sys_strncasecmp (const  char *, const  char *,  int);

 int strtoul ( char *,  char **, int );

 int sys_strlen (const  char *);

 char * sys_strcat ( char *, const  char *);

 char * sys_strncat ( char *, const  char *,  int);

 char * sys_strcpy ( char *, const  char *);

 char * sys_strncpy ( char *, const  char *,  int);

 int sys_strcmp (const  char *, const  char *);

 int sys_strncmp (const  char *, const  char *,  int);

 char *sys_strstr(const  char *,const  char *);

 char *sys_strchr( char * ,  char );

 char *sys_strrchr( char * ,  char );

void * memcpy (void *, const void *, int);

void * sys_memset (void *,  int, int);

void free (void *);

void * malloc ();

short atoi_my(const  char * str);

/********************************************************************/
/********************************************************************/

typedef struct
{
     int dest;
    void (*func)( char*);
     char *loc;
} PRINTK_INFO;

/********************************************************************/

#define DEST_CONSOLE    (1)
#define DEST_STRING     (2)

#define FLAGS_MINUS     (0x01)
#define FLAGS_PLUS      (0x02)
#define FLAGS_SPACE     (0x04)
#define FLAGS_ZERO      (0x08)
#define FLAGS_POUND     (0x10)

#define IS_FLAG_MINUS(a)    (a & FLAGS_MINUS)
#define IS_FLAG_PLUS(a)     (a & FLAGS_PLUS)
#define IS_FLAG_SPACE(a)    (a & FLAGS_SPACE)
#define IS_FLAG_ZERO(a)     (a & FLAGS_ZERO)
#define IS_FLAG_POUND(a)    (a & FLAGS_POUND)

#define LENMOD_h        (0x01)
#define LENMOD_l        (0x02)
#define LENMOD_L        (0x04)

#define IS_LENMOD_h(a)  (a & LENMOD_h)
#define IS_LENMOD_l(a)  (a & LENMOD_l)
#define IS_LENMOD_L(a)  (a & LENMOD_L)

#define FMT_d   (0x0001)
#define FMT_o   (0x0002)
#define FMT_x   (0x0004)
#define FMT_X   (0x0008)
#define FMT_u   (0x0010)
#define FMT_c   (0x0020)
#define FMT_s   (0x0040)
#define FMT_p   (0x0080)
#define FMT_n   (0x0100)

#define IS_FMT_d(a)     (a & FMT_d)
#define IS_FMT_o(a)     (a & FMT_o)
#define IS_FMT_x(a)     (a & FMT_x)
#define IS_FMT_X(a)     (a & FMT_X)
#define IS_FMT_u(a)     (a & FMT_u)
#define IS_FMT_c(a)     (a & FMT_c)
#define IS_FMT_s(a)     (a & FMT_s)
#define IS_FMT_p(a)     (a & FMT_p)
#define IS_FMT_n(a)     (a & FMT_n)

/********************************************************************/
//static void printk_putc( int c,  int * count, PRINTK_INFO * info);
//static  int printk_mknumstr( char * numstr, void * nump,  int neg,  int radix);
//static void printk_pad_zero( int curlen,  int field_width,  int * count, PRINTK_INFO * info);
//static void printk_pad_space( int curlen,  int field_width,  int* count, PRINTK_INFO * info);
 int printk(PRINTK_INFO * info, const  char* fmt, va_list ap);
// int printf(const  char * fmt,...);
 int sys_sprintf( char* s, const  char* fmt,...);
void out_char( char *ch);
/*********************************************************************/

#endif
