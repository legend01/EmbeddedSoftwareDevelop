#ifndef __ASSERT_H__
#define __ASSERT_H__

/*使用断言测试*/
#ifdef DEBUG
    /*处理函数原型*/
    void Assert(char * filename, unsigned int lineno);
    #define ASSERT(condition)\
    if(condition)\
        NULL; \
    else\
        Assert(__FILE__ , __LINE__)
/*不使用断言测试*/
#else
    #define ASSERT(condition) NULL
#endif
void Assert(char * filename, unsigned int lineno)
{
    fflush(stdout);
    fprintf(stderr,"\nAssert failed： %s, line %u\n",filename, lineno);
    fflush(stderr);
    abort();
}

#endif // !__ASSERT_H__
