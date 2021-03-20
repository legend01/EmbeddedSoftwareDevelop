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
#define UNIX_DEBUG

// #define u16_t unsigned int
#define NULL 0
#define TRUE 1
#define FALSE 0

unsigned long int isspace(unsigned long int);

unsigned long int isalnum(unsigned long int);

unsigned long int isdigit (unsigned long int);

unsigned long int isupper (unsigned long int);

unsigned long int sys_strcasecmp (const unsigned char *, const unsigned char *);

unsigned long int sys_strncasecmp (const unsigned char *, const unsigned char *, unsigned long int);

unsigned long int sys_strtoul (unsigned char *, unsigned char **, short int );

unsigned long int sys_strlen (const unsigned char *);

unsigned char * sys_strcat (unsigned char *, const unsigned char *);

unsigned char * sys_strncat (unsigned char *, const unsigned char *, unsigned long int);

unsigned char * sys_strcpy (unsigned char *, const unsigned char *);

unsigned char * sys_strncpy (unsigned char *, const unsigned char *, unsigned long int);

unsigned long int sys_strcmp (const unsigned char *, const unsigned char *);

unsigned long int sys_strncmp (const unsigned char *, const unsigned char *, unsigned long int);

unsigned char *sys_strstr(const unsigned char *,const unsigned char *);

unsigned char *sys_strchr(unsigned char * , unsigned char );

unsigned char *sys_strrchr(unsigned char * , unsigned char );

void * memcpy (void *, const void *, unsigned);

void * sys_memset (void *, unsigned long int, unsigned);

unsigned int atoi_my(const unsigned char * str);

/********************************************************************/
/********************************************************************/

typedef struct
{
    unsigned long int dest;
    void (*func)(unsigned char*);
    unsigned char *loc;
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
static void printk_putc(unsigned long int c, unsigned long int * count, PRINTK_INFO * info);
static unsigned long int printk_mknumstr(unsigned char * numstr, void * nump, unsigned long int neg, unsigned long int radix);
static void printk_pad_zero(unsigned long int curlen, unsigned long int field_width, unsigned long int * count, PRINTK_INFO * info);
static void printk_pad_space(unsigned long int curlen, unsigned long int field_width, unsigned long int* count, PRINTK_INFO * info);
unsigned long int printk(PRINTK_INFO * info, const unsigned char* fmt, va_list ap);
unsigned long int sys_sprintf(unsigned char* s, const unsigned char* fmt,...);
/*********************************************************************/

#endif
