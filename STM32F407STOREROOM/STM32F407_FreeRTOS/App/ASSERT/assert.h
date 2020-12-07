#ifndef __ASSERT_H__
#define __ASSERT_H__

/*ʹ�ö��Բ���*/
#ifdef DEBUG
    /*������ԭ��*/
    void Assert(char * filename, unsigned int lineno);
    #define ASSERT(condition)\
    if(condition)\
        NULL; \
    else\
        Assert(__FILE__ , __LINE__)
/*��ʹ�ö��Բ���*/
#else
    #define ASSERT(condition) NULL
#endif
void Assert(char * filename, unsigned int lineno)
{
    fflush(stdout);
    fprintf(stderr,"\nAssert failed�� %s, line %u\n",filename, lineno);
    fflush(stderr);
    abort();
}

#endif // !__ASSERT_H__
