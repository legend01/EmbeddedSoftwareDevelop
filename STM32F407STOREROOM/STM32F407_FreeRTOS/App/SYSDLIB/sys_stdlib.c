/*
 * File:        stdlib.c
 * Purpose:     Functions normally found in a standard C lib.
 *
 * Notes:       This supports ASCII only!!!
 *
 */
#include "sys_stdlib.h"
#include "APP_USART.h"

/****************************************************************/
unsigned long int isspace (unsigned long int ch)
{
    if ((ch == ' ') || (ch == '\t'))    /* \n ??? */
        return TRUE;
    else
        return FALSE;
}

/**************************检查ASCII是否是字符**************************************/
unsigned long int isalnum (unsigned long int ch)
{
    /* ASCII only */
    if (((ch >= '0') && (ch <= '9')) ||
            ((ch >= 'A') && (ch <= 'Z')) ||
            ((ch >= 'a') && (ch <= 'z')))
        return TRUE;
    else
        return FALSE;
}

/*****************************检查是否是数字***********************************/
unsigned long int isdigit (unsigned long int ch)
{
    /* ASCII only */
    if ((ch >= '0') && (ch <= '9'))
        return TRUE;
    else
        return FALSE;
}

/****************************************************************/
/*
	@brief:判断字符c是否为大写英文字母
	@retval:返回真(TRUE)或者假(FALSE)
	@Notice:当参数c为大写英文字母(A-Z)时，返回非零值，否则返回零。
*/
unsigned long int isupper (unsigned long int ch)
{
    /* ASCII only */
    if ((ch >= 'A') && (ch <= 'Z'))
        return TRUE;
    else
        return FALSE;
}

/****************************************************************/
/*
	@brief:比较参数s1和s2字符串，比较时会自动忽略大小写的差异。
	@retval:返回值 若参数s1和s2字符串相等则返回0。s1大于s2则返回大于0 的值
			，s1 小于s2 则返回小于0的值。
*/
unsigned long int sys_strcasecmp (const unsigned char *s1, const unsigned char *s2)
{
    unsigned char c1, c2;
    unsigned long int result = 0;

    while (result == 0)
    {
        c1 = *s1++;
        c2 = *s2++;
        if ((c1 >= 'a') && (c1 <= 'z'))
            c1 = (char)(c1 - ' ');
        if ((c2 >= 'a') && (c2 <= 'z'))
            c2 = (char)(c2 - ' ');
        if ((result = (c1 - c2)) != 0)
            break;
        if ((c1 == 0) || (c2 == 0))
            break;
    }
    return result;
}


/****************************************************************/
/*
	@brief:比较参数s1和s2字符串前n个字符，比较时会自动忽略大小写的差异。
	@retval:若参数s1和s2字符串相同，则返回0；
			若s1大于s2，则返回大于0的值；
			若s1小于s2，则返回小于0的值。
*/
unsigned long int sys_strncasecmp (const unsigned char *s1, const unsigned char *s2, unsigned long int n)
{
    unsigned char c1, c2;
    unsigned long int k = 0;
    unsigned long int result = 0;

    while ( k++ < n )
    {
        c1 = *s1++;
        c2 = *s2++;
        if ((c1 >= 'a') && (c1 <= 'z'))
            c1 = (char)(c1 - ' ');
        if ((c2 >= 'a') && (c2 <= 'z'))
            c2 = (char)(c2 - ' ');
        if ((result = (c1 - c2)) != 0)
            break;
        if ((c1 == 0) || (c2 == 0))
            break;
    }
    return result;
}

/****************************************************************/
/*
	@brief:strtoul()会将参数str字符串根据参数base来转换成无符号的长整型数
			。参数base范围从2至36，或0。参数base代表采用的进制方式
			，如base值为10则采用10进制，若base值为16则采用16进制数等
			。当base值为0时会根据情况选择用哪种进制：如果第一个字符是'0'
			，就判断第二字符如果是‘x’则用16进制，否则用8进制
			；第一个字符不是‘0’，则用10进制。一开始strtoul()会扫描参数
			str字符串，跳过前面的空格字符串，直到遇上数字或正负符号才开始做转换
			，再遇到非数字或字符串结束时('')结束转换，并将结果返回
			。若参数ptr不为NULL，则会将遇到不合条件而终止的str中的
			字符指针由ptr返回。
	@retval:返回转换后的长整型数，否则返回ERANGE并将错误代码存入errno中。
*/
unsigned long int strtoul (unsigned char *str, unsigned char **ptr, short int base)
{
    unsigned long int rvalue;
    unsigned long int c, err, neg;
    unsigned char *endp;
    unsigned char *startp;

    rvalue = 0;
    err = 0;
    neg = 0;

    /* 检查无效的参数 */
    if ((str == NULL) || (base < 0) || (base == 1) || (base > 36))
    {
        if (ptr != NULL)
        {
            *ptr = str;
        }
        return 0;
    }

    /* 跳过前导空格 */
    for (startp = str; isspace(*startp); ++startp)
        ;

    /* 检查符号 */
    switch (startp[0])
    {
    case '0':
        if ((startp[1] == 'x') || (startp[1] == 'X'))
        {
            if ((base == 0) || (base == 16))
            {
                base = 16;
                startp = &startp[2];
            }
        }
        break;
    case '-':
        neg = 1;
        startp = &startp[1];
        break;
    default:
        break;
    }

    if (base == 0)
        base = 10;

    /* 检查无效的str字符 */
    for ( endp = startp; (c = *endp) != '\0'; ++endp)
    {
        /* Check for 0..9,Aa-Zz */
        if (!isalnum(c))
        {
            err = 1;
            break;
        }

        /* Convert char to num in 0..36 */
        if (isdigit(c))
        {
            c = c - '0';
        }
        else
        {
            if (isupper(c))
            {
                c = c - 'A' + 10;
            }
            else
            {
                c = c - 'a' + 10;
            }
        }

        /* check c against base */
        if (c >= base)
        {
            err = 1;
            break;
        }

        if (neg)
        {
            rvalue = (rvalue * base) - c;
        }
        else
        {
            rvalue = (rvalue * base) + c;
        }
    }

    /* 退出时，endp指向有效信息所在的字符。Upon exit, endp points to the character at which valid info */
    /* STOPS.  No chars including and beyond endp are used.        */

    if (ptr != NULL)
        *ptr = endp;

    if (err)
    {
        if (ptr != NULL)
            *ptr = str;

        return 0;
    }
    else
    {
        return rvalue;
    }
}

/****************************计算字符串长度******************************/
unsigned long int sys_strlen (const unsigned char *str)
{
    unsigned char *s = (unsigned char *)str;
    unsigned long int len = 0;

    if (s == NULL)
        return 0;

    while (*s++ != '\0')
        ++len;

    return len;
}

/****************************字符串拼接函数************************************/
/*
	@brief:dest 为目标字符串指针，src 为源字符串指针。
	@retval:返回dest 字符串起始地址。
	@Notice:dest 与 src 所指的内存空间不能重叠
	        ，且 dest 要有足够的空间来容纳要复制的字符串
*/
unsigned char * sys_strcat (unsigned char *dest, const unsigned char *src)
{
    unsigned char *dp;
    unsigned char *sp = (unsigned char *)src;

    if ((dest != NULL) && (src != NULL))
    {
        dp = &dest[sys_strlen(dest)];

        while (*sp != '\0')
        {
            *dp++ = *sp++;
        }
        *dp = '\0';
    }
    return dest;
}

/*****************************字符串拼接函数***********************************/
/*
	@brief:把src所指字符串的前n个字符添加到dest所指字符串的结尾处
			，并覆盖dest所指字符串结尾的'\0'，从而实现字符串的连接。
	@retval:返回指向dest的指针。
	@Notice:src和dest所指内存区域不可以重叠
			，并且dest必须有足够的空间来容纳src的字符串。
*/
unsigned char * sys_strncat (unsigned char *dest, const unsigned char *src, unsigned long int n)
{
    unsigned char *dp;
    unsigned char *sp = (unsigned char *)src;

    if ((dest != NULL) && (src != NULL) && (n > 0))
    {
        dp = &dest[sys_strlen(dest)];

        while ((*sp != '\0') && (n-- > 0))
        {
            *dp++ = *sp++;
        }
        *dp = '\0';
    }
    return dest;
}
/*****************************************************************/
/*
	@brief:atoi (表示 ascii to integer)是把字符串转换成整型数的一个函数，应用在计算机程序和办公软件中,
			直到遇上数字或正负符号才开始做转换，而在遇到非数字或字符串结束时('\0')才结束转换，并将结果返回。
	@retval:遇到非数字或字符串结束时('\0')才结束转换，并将结果返回。
			如果 nptr不能转换成 int 或者 nptr为空字符串，那么将返回 0。
	@NOTICE:每个函数返回 int 值，此值由将输入字符作为数字解析而生成。 
			如果该输入无法转换为该类型的值，则atoi的返回值为 0。
*/
u16_t atoi_my(const unsigned char *str)
{
	u16_t s = 0;
	unsigned long int flag = FALSE;

	while(*str == ' ')
	{
		str++;
	}
	if(*str == '-' || *str == '+')
	{
		if(*str == '-')
		{
			flag = TRUE;
		}
		str++;
	}
	while(*str >= '0' && *str <='9')
	{
		s = s*10 + *str - '0';
		str ++;
	}
	return s * (flag ? -1:1);
}
/****************************************************************/
/*
	@brief:把从src地址开始且含有NULL结束符的字符串复制到以dest开始的地址空间
	@retval:返回指向dest的指针
	@Notice:src和dest所指内存区域不可以重叠且dest必须有足够的空间来容纳src的字符串。
*/
unsigned char * sys_strcpy (unsigned char *dest, const unsigned char *src)
{
    unsigned char *dp = (unsigned char *)dest;
    unsigned char *sp = (unsigned char *)src;

    if ((dest != NULL) && (src != NULL))
    {
        while (*sp != '\0')
        {
            *dp++ = *sp++;
        }
        *dp = '\0';
    }
    return dest;
}

/****************************************************************/
/*
	@brief:把src所指向的字符串中以src地址开始的前n个字节复制到dest所指的数组中
			，并返回dest。
	@retval:返回dest地址
	@Notice:src和dest所指内存区域不可以重叠且dest必须有
			足够的空间来容纳src的字符长度+'\0'
*/
unsigned char * sys_strncpy (unsigned char *dest, const unsigned char *src, unsigned long int n)
{
    char *dp = (char *)dest;
    char *sp = (char *)src;

    if ((dest != NULL) && (src != NULL) && (n > 0))
    {
        while ((*sp != '\0') && (n-- > 0))
        {
            *dp++ = *sp++;
        }
        *dp = '\0';
    }
    return dest;
}

/****************************************************************/
/*
	@brief:比较两个字符串
	@retval:若str1==str2，则返回零；
			若str1<str2，则返回负数；
			若str1>str2，则返回正数。
	@Notice:strcmp(s1，s2) 判断两个字符串s1和s2是否相同，相同返回true ,不同返回false
*/
unsigned long int sys_strcmp (const unsigned char *s1, const unsigned char *s2)
{
    /* No checks for NULL */
    unsigned char *s1p = (unsigned char *)s1;
    unsigned char *s2p = (unsigned char *)s2;

    while (*s2p != '\0')
    {
        if (*s1p != *s2p)
		{
			return FALSE;
			//break;
		}
        ++s1p;
        ++s2p;
    }
    return TRUE;
}

/****************************************************************/
/*
	@brief:str1, str2 为需要比较的两个字符串，n为要比较的字符的数目。
			
	@retval:若str1与str2的前n个字符相同，则返回0
			；若s1大于s2，则返回大于0的值
			；若s1 若小于s2，则返回小于0的值。
	@Notice:该函数区分大小写。
*/
unsigned long int sys_strncmp (const unsigned char *s1, const unsigned char *s2, unsigned long int n)
{
    /* No checks for NULL */
    unsigned char *s1p = (unsigned char *)s1;
    unsigned char *s2p = (unsigned char *)s2;

    if (n <= 0)
        return 0;

    while (*s2p != '\0')
    {
        if (*s1p != *s2p)
            break;

        if (--n == 0)
            break;

        ++s1p;
        ++s2p;
    }
    return (*s1p - *s2p);
}
/****************************************************************/
/*
	@brief:strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串
			。如果是，则该函数返回str2在str1中首次出现的地址；否则，返回NULL。
	@retval:若str2是str1的子串，则返回str2在str1的首次出现的地址
			；如果str2不是str1的子串，则返回NULL。
	@Notice:str1: 被查找目标  str2: 要查找对象
*/
unsigned char *sys_strstr(const unsigned char *str1,const unsigned char *str2)
{
	unsigned long int n;
	if(*str2)
	{
		while(*str1)
		{
			for(n = 0;*(str1 + n) == *(str2 + n);n++)
			{
				if(!*(str2 + n + 1))
				{
					return(unsigned char *)str1;
				}
			}
			str1 ++;
		}
		return NULL;
	}
	else
	{
		return (unsigned char*)str1;	
	}
}
/****************************************************************/
/*
	@brief:查找字符串_Str中首次出现字符_Val的位置,返回首次出现_Val的位置的指针
	    ，返回的地址是被查找字符串指针开始的第一个与Val相同字符的指针
	    ，如果Str中不存在Val则返回NULL。
	@retval:成功则返回要查找字符第一次出现的位置，失败返回NULL
	@Notice:str1: 被查找目标  str2: 要查找对象
*/
unsigned char* sys_strchr(unsigned char *s, unsigned char c)
{
	if(s == NULL)
    {
        return NULL;
    }

    while(*s != '\0')
    {
        if(*s == (unsigned char)c )
        {
            return (unsigned char*)s;
        }
        s++;
    }
    return NULL;
}
/****************************************************************/
/*
	@brief:查找一个字符c在另一个字符串str中末次出现的位置（
			也就是从str的右侧开始查找字符c首次出现的位置）
			，并返回这个位置的地址。如果未能找到指定字符，那么函数将返回NULL
			。使用这个地址返回从最后一个字符c到str末尾的字符串。
	@retval:成功返回查找成功字符的位置，未能找到指定字符，函数返回NULL
	@Notice:str1:被查找目标 c:要查找的字符
*/
unsigned char *sys_strrchr(unsigned char *s, unsigned char c)
{
	unsigned char *p_char = NULL;
	if(s == NULL)
    {
        return NULL;
    }
    while(*s != '\0')
    {
        if(*s == (unsigned char)c)
        {
            p_char = (unsigned char *)s;
        }
        s++;
    }

    return p_char;
}
/****************************************************************/
/*
	@brief:memcpy函数的功能是从源src所指的内存地址的起始位置开始拷贝
			n个字节到目标dest所指的内存地址的起始位置中。
	@retval:函数返回指向dest的指针。
	@Notice:strcpy和memcpy主要有以下3方面的区别。
			1、复制的内容不同。strcpy只能复制字符串
			，而memcpy可以复制任意内容，例如字符数组、整型、结构体、类等。
			2、复制的方法不同。strcpy不需要指定长度
			，它遇到被复制字符的串结束符"\0"才结束，所以容易溢出
			。memcpy则是根据其第3个参数决定复制的长度。
			3、用途不同。通常在复制字符串时用strcpy
			，而需要复制其他类型数据时则一般用memcpy
*/
void * memcpy (void *dest, const void *src, unsigned n)
{
    int longs, bytes;
    unsigned long int *dpl = (unsigned long int *)dest;
    unsigned long int *spl = (unsigned long int *)src;
    unsigned char  *dpb, *spb;

    if ((dest != NULL) && (src != NULL) && (n > 0))
    {
        bytes = (n & 0x3);
        longs = (n - bytes) >> 2;

        while (longs--)
            *dpl++ = *spl++;

        dpb = (unsigned char *)dpl;
        spb = (unsigned char *)spl;

        while (bytes--)
            *dpb++ = *spb++;
    }
    return dest;
}

/****************************************************************/
/*
	@brief:作用是在一段内存块中填充某个给定的值
			，它是对较大的结构体或数组进行清零操作的一种最快方法
	@retval:
	@Notice:
*/
void * sys_memset (void *s, unsigned long int c, unsigned n)
{
    /* 没有优化，但非常便携Not optimized, but very portable */
    unsigned char *sp = (unsigned char *)s;

    if ((s != NULL) && (n > 0))
    {
        while (n--)
        {
            *sp++ = (unsigned char)c;
        }
    }
    return s;
}
/****************************************************************/
static void printk_putc (unsigned long int c, unsigned long int*count, PRINTK_INFO *info)
{
    switch (info->dest)
    {
    case DEST_CONSOLE:
        info->func((unsigned char *)&c);
        break;
    case DEST_STRING:
        *(info->loc) = (unsigned char)c;
        ++(info->loc);
        break;
    default:
        break;
    }
    *count += 1;
}
/********************************************************************/
static unsigned long int printk_mknumstr (unsigned char *numstr, void *nump, unsigned long int neg, unsigned long int radix)
{
    int a, b, c;
    unsigned long int ua, ub, uc;

    unsigned long int nlen;
    unsigned char *nstrp;

    nlen = 0;
    nstrp = numstr;
    *nstrp++ = '\0';

    if (neg)
    {
        a = *(unsigned long int *)nump;
        if (a == 0)
        {
            *nstrp = '0';
            ++nlen;
            goto done;
        }
        while (a != 0)
        {
            b = (unsigned long int)a / (unsigned long int)radix;
            c = (unsigned long int)a - ((unsigned long int)b * (unsigned long int)radix);
            if (c < 0)
            {
                c = ~c + 1 + '0';
            }
            else
            {
                c = c + '0';
            }
            a = b;
            *nstrp++ = (unsigned char)c;
            ++nlen;
        }
    }
    else
    {
        ua = *(unsigned long int *)nump;
        if (ua == 0)
        {
            *nstrp = '0';
            ++nlen;
            goto done;
        }
        while (ua != 0)
        {
            ub = (unsigned long int)ua / (unsigned long int)radix;
            uc = (unsigned long int)ua - ((unsigned long int)ub * (unsigned long int)radix);
            if (uc < 10)
            {
                uc = uc + '0';
            }
            else
            {
                uc = uc - 10 + 'A';
            }
            ua = ub;
            *nstrp++ = (char)uc;
            ++nlen;
        }
    }
done:
    return nlen;
}
/********************************************************************/
static void printk_pad_zero (unsigned long int curlen, unsigned long int field_width, unsigned long int *count, PRINTK_INFO *info)
{
    unsigned long int i;

    for (i = curlen; i < field_width; i++)
    {
        printk_putc('0', count, info);
    }
}
/********************************************************************/
static void printk_pad_space (unsigned long int curlen, unsigned long int field_width, unsigned long int *count, PRINTK_INFO *info)
{
    unsigned long int i;
    for (i = curlen; i < field_width; i++)
    {
        printk_putc(' ', count, info);
    }
}
/********************************************************************/
unsigned long int printk (PRINTK_INFO *info, const unsigned char *fmt, va_list ap)
{
    /* va_list ap; */
    unsigned char*p;
    unsigned long int c;

    unsigned char vstr[33];
    unsigned char *vstrp;
    unsigned long int vlen;

    unsigned long int done;
    unsigned long int count = 0;

    unsigned long int flags_used;
    unsigned long int field_width;

    int ival;
    unsigned long int schar, dschar;
    unsigned long int *ivalp;
    unsigned char *sval;
    unsigned long int cval;
    unsigned long int uval;

    /*
     * Start parsing apart the format string and display appropriate
     * formats and data.
     * 开始解析格式字符串并适当地显示格式和数据
     */
    for (p = (unsigned char*)fmt; (c = *p) != 0; p++)
    {
        /*
         * All formats begin with a '%' marker.  Special chars like
         * '\n' or '\t' are normally converted to the appropriate
         * character by the __compiler__.  Thus, no need for this
         * routine to account for the '\' character.
         * 所有格式都以‘%’标记开头。特殊的字符如'\n'或者'\t'被汇编器正常地覆盖为特殊的字符
         * 因此,不需要为'\'字符计数
         */
        if (c != '%')
        {
            /*
             * This needs to be replaced with something like
             * 'out_char()' or call an OS routine.
             * 这需要用一些out_char()函数或者回调一个OS路径被取代
             */
#ifndef UNIX_DEBUG
            if (c != '\n')
            {
                printk_putc(c, &count, info);
            }
            else
            {
                printk_putc(0x0D /* CR */, &count, info);
                printk_putc(0x0A /* LF */, &count, info);
            }
#else
            printk_putc(c, &count, info);
#endif

            /*
             * By using 'continue', the next iteration of the loop
             * is used, skipping the code that follows.
             * 通过使用'continue',循环的下一次迭代被使用,跳过一下的代码
             */
            continue;
        }

        /*
         * First check for specification modifier flags.
         * 首先检查规范修饰符标志
         */
        flags_used = 0;
        done = FALSE;
        while (!done)
        {
            switch (/* c = */ *++p)
            {
            case '-':
                flags_used |= FLAGS_MINUS;
                break;
            case '+':
                flags_used |= FLAGS_PLUS;
                break;
            case ' ':
                flags_used |= FLAGS_SPACE;
                break;
            case '0':
                flags_used |= FLAGS_ZERO;
                break;
            case '#':
                flags_used |= FLAGS_POUND;
                break;
            default:
                /* we've gone one char too far 我们离开一个字符太远*/
                --p;
                done = TRUE;
                break;
            }
        }

        /*
         * Next check for minimum field width.
         * 下一次检查最小字段宽度
         */
        field_width = 0;
        done = FALSE;
        while (!done)
        {
            switch (c = *++p)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                field_width = (field_width * 10) + (c - '0');
                break;
            default:
                /* we've gone one char too far 离开一个字符太远*/
                --p;
                done = TRUE;
                break;
            }
        }

        /*
         * Next check for the width and precision field separator.
         * 下一步检查宽度和精度字段分隔符。
         */
        if (/* (c = *++p) */ *++p == '.')
        {
            /* precision_used = TRUE; */

            /*
             * Must get precision field width, if present.
             * 如果存在，必须获取精度区域长度
             */
            /* precision_width = 0; */
            done = FALSE;
            while (!done)
            {
                switch (/* c = uncomment if used below */ *++p)
                {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    break;
                default:
                    /* we've gone one char too far */
                    --p;
                    done = TRUE;
                    break;
                }
            }
        }
        else
        {
            /* we've gone one char too far */
            --p;
        }

        /*
         * Check for the length modifier.
         * 检查长度调节器
         */
        /* length_modifier = 0; */
        switch (/* c = */ *++p)
        {
        case 'h':
            /* length_modifier |= LENMOD_h; */
            break;
        case 'l':
            /* length_modifier |= LENMOD_l; */
            break;
        case 'L':
            /* length_modifier |= LENMOD_L; */
            break;
        default:
            /* we've gone one char too far */
            --p;
            break;
        }

        /*
         * Now we're ready to examine the format.
         * 现在我们已经准备好检查格式了。
         */
        switch (c = *++p)
        {
        case 'd':
        case 'i':
            ival = (unsigned long int)va_arg(ap, unsigned long int);
            vlen = printk_mknumstr(vstr, &ival, TRUE, 10);
            vstrp = &vstr[vlen];

            if (ival < 0)
            {
                schar = '-';
                ++vlen;
            }
            else
            {
                if (IS_FLAG_PLUS(flags_used))
                {
                    schar = '+';
                    ++vlen;
                }
                else
                {
                    if (IS_FLAG_SPACE(flags_used))
                    {
                        schar = ' ';
                        ++vlen;
                    }
                    else
                    {
                        schar = 0;
                    }
                }
            }
            dschar = FALSE;

            /*
             * do the ZERO pad.
             * 做零垫
             */
            if (IS_FLAG_ZERO(flags_used))
            {
                if (schar)
                    printk_putc(schar, &count, info);
                dschar = TRUE;

                printk_pad_zero (vlen, field_width, &count, info);
                vlen = field_width;
            }
            else
            {
                if (!IS_FLAG_MINUS(flags_used))
                {
                    printk_pad_space (vlen, field_width, &count, info);

                    if (schar)
                        printk_putc(schar, &count, info);
                    dschar = TRUE;
                }
            }

            /* the string was built in reverse order, now display in */
            /* correct order */
			/*
			 * 字符串是用相反的顺序构建的,现在用正确的顺序构建
			 */
            if (!dschar && schar)
            {
                printk_putc(schar, &count, info);
            }
            goto cont_xd;

        case 'x':
        case 'X':
            uval = (unsigned long int)va_arg(ap, unsigned long int);
            vlen = printk_mknumstr(vstr, &uval, FALSE, 16);
            vstrp = &vstr[vlen];

            dschar = FALSE;
            if (IS_FLAG_ZERO(flags_used))
            {
                if (IS_FLAG_POUND(flags_used))
                {
                    printk_putc('0', &count, info);
                    printk_putc('x', &count, info);
                    /*vlen += 2;*/
                    dschar = TRUE;
                }
                printk_pad_zero (vlen, field_width, &count, info);
                vlen = field_width;
            }
            else
            {
                if (!IS_FLAG_MINUS(flags_used))
                {
                    if (IS_FLAG_POUND(flags_used))
                    {
                        vlen += 2;
                    }
                    printk_pad_space (vlen, field_width, &count, info);
                    if (IS_FLAG_POUND(flags_used))
                    {
                        printk_putc('0', &count, info);
                        printk_putc('x', &count, info);
                        dschar = TRUE;
                    }
                }
            }

            if ((IS_FLAG_POUND(flags_used)) && !dschar)
            {
                printk_putc('0', &count, info);
                printk_putc('x', &count, info);
                vlen += 2;
            }
            goto cont_xd;

        case 'o':
            uval = (unsigned long int)va_arg(ap, unsigned long int);
            vlen = printk_mknumstr(vstr, &uval, FALSE, 8);
            goto cont_u;
        case 'b':
            uval = (unsigned long int)va_arg(ap, unsigned long int);
            vlen = printk_mknumstr(vstr, &uval, FALSE, 2);
            goto cont_u;
        case 'p':
            uval = (unsigned int)va_arg(ap, void *);
            vlen = printk_mknumstr(vstr, &uval, FALSE, 16);
            goto cont_u;
        case 'u':
            uval = (unsigned long int)va_arg(ap, unsigned long int);
            vlen = printk_mknumstr(vstr, &uval, FALSE, 10);

cont_u:
            vstrp = &vstr[vlen];

            if (IS_FLAG_ZERO(flags_used))
            {
                printk_pad_zero (vlen, field_width, &count, info);
                vlen = field_width;
            }
            else
            {
                if (!IS_FLAG_MINUS(flags_used))
                {
                    printk_pad_space (vlen, field_width, &count, info);
                }
            }

cont_xd:
            while (*vstrp)
                printk_putc(*vstrp--, &count, info);

            if (IS_FLAG_MINUS(flags_used))
            {
                printk_pad_space (vlen, field_width, &count, info);
            }
            break;

        case 'c':
            cval = (unsigned char)va_arg(ap, unsigned long int);
            printk_putc(cval, &count, info);
            break;
        case 's':
            sval = (unsigned char*)va_arg(ap, unsigned char*);
            if (sval)
            {
                vlen = sys_strlen(sval);
                if (!IS_FLAG_MINUS(flags_used))
                {
                    printk_pad_space (vlen, field_width, &count, info);
                }
                while (*sval)
                    printk_putc(*sval++, &count, info);
                if (IS_FLAG_MINUS(flags_used))
                {
                    printk_pad_space (vlen, field_width, &count, info);
                }
            }
            break;
        case 'n':
            ivalp = (unsigned long int *)va_arg(ap, unsigned long int*);
            *ivalp = count;
            break;
        default:
            printk_putc(c, &count, info);
            break;
        }
    }
    return count;
}
/********************************************************************/
unsigned long int log_printf (const unsigned char *fmt, ...) //!_TODO:修改函数名
{
    va_list ap;
    unsigned long int rvalue;
    PRINTK_INFO info;


    info.dest = DEST_CONSOLE;
    info.func = &out_char;
    /*
     * Initialize the pointer to the variable length argument list.
     * 初始化指向可变长度参数列表的指针
     */
    va_start(ap, fmt);
    rvalue = printk(&info, fmt, ap);
    /*
     * Cleanup the variable length argument list.
     */
    va_end(ap);
    return rvalue;
}
/********************************************************************/
void out_char(unsigned char *ch) //!_修改输出出口
{
	Uart5_DMA_Sent((uint8_t*)ch, 1);
}
/********************************************************************/
/*
	@brief:把格式化的数据写入某个字符串中
	@param:1.char型指针，指向将要写入的字符串缓冲区 2.格式化字符串 3.可选参数，可是任何类型的数据
	@retval:返回写入buffer 的字符数，出错则返回-1
			. 如果 buffer 或 format 是空指针，且不出错而继续
			，函数将返回-1，并且 errno 会被设置为 EINVAL。
			sprintf 返回以format为格式argument为内容组成的结果被写入buffer
			的字节数，结束字符‘\0’不计入内。即
			，如果“Hello”被写入空间足够大的buffer后，函数sprintf 返回5。
			同时buffer的内容将被改变。
	@Notice:sprintf格式的规格如下所示。[]中的部分是可选的。
			%[指定参数][标识符][宽度][.精度]指示符
			若想输出'%'本身时, 请使用'%%'处理。
			1. 处理字符方向。负号时表示从后向前处理。
			2. 填空字元。 0 的话表示空格填 0；空格是内定值，表示空格就放着。
			3. 字符总宽度。为最小宽度。
			4. 精确度。指在小数点后的浮点数位数。
			转换字符
			%% 印出百分比符号，不转换。
			%c 整数转成对应的 ASCII 字元。
			%d 整数转成十进位。
			%f 倍精确度数字转成浮点数。
			%o 整数转成八进位。
			%s 整数转成字符串。
			%x 整数转成小写十六进位。
			%X 整数转成大写十六进位。
			$money = 123.1
			$formatted = sprintf ("%06.2f", $money); // 此时变数 $ formatted 值为 "123.10"
			$formatted = sprintf ("%08.2f", $money); // 此时变数 $ formatted 值为 "00123.10"
			$formatted = sprintf ("%-08.2f", $money); // 此时变数 $ formatted 值为 "123.1000"
			$formatted = sprintf ("%.2f%%", 0.95 * 100); // 格式化为百分比
			%08.2f 解释:
			%开始符
			0是 "填空字元" 表示,如果长度不足时就用0来填满。
			8格式化后总长度
			2f小数位长度，即2位
			
*/
unsigned long int sys_sprintf (unsigned char *s, const unsigned char *fmt, ...)
{
    va_list ap;
    unsigned long int rvalue = 0;
    PRINTK_INFO info;

    /*
     * Initialize the pointer to the variable length argument list.
     * 初始化可变长参数列表指针
     */
    if (s != 0)
    {
        info.dest = DEST_STRING;
        info.loc = s;
        va_start(ap, fmt);
        rvalue = printk(&info, fmt, ap);
        *info.loc = '\0';
        va_end(ap);
    }
    return rvalue;
}

/********************************************************************/

