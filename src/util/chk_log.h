#ifndef _CHK_LOG_H
#define _CHK_LOG_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "util/chk_singleton.h"
#include "../config.h"

/*
* 一个简单的日志系统
*/

enum{
    LOG_INFO = 0,
    LOG_DEBUG,
    LOG_WARN,
    LOG_ERROR,
    LOG_CRITICAL
};

typedef struct chk_logfile chk_logfile;

extern int32_t g_loglev;

chk_logfile *chk_create_logfile(const char *filename);

static inline void chk_set_loglev(int32_t loglev)
{
    if(loglev >= LOG_INFO && loglev <= LOG_CRITICAL)
        g_loglev = loglev;
}

static inline int32_t chk_current_loglev() {
    return g_loglev;
}


/*
*  以下两个函数中的context参数必须由malloc分配，并由日志系统负责free
*/

void    chk_log(chk_logfile*,int32_t loglev,char *context);
//写入系统日志,默认文件名由SYSLOG_NAME定义
void    chk_syslog(int32_t loglev,char *content);

int32_t chk_log_prefix(char *buf,uint8_t loglev);

int32_t chk_log_prefix_detail(char *buf,uint8_t loglev,const char *function,const char *file,int32_t line);


//日志格式[INFO|ERROR]yyyy-mm-dd-hh:mm:ss.ms:content
#define CHK_LOG(LOGFILE,LOGLEV,...)                                                             \
do{                                                                                             \
    if(LOGLEV >= g_loglev){                                                                     \
        char *xx___buf = malloc(CHK_MAX_LOG_SIZE);                                              \
        if(!xx___buf) break;                                                                    \
        int32_t size = chk_log_prefix_detail(xx___buf,LOGLEV,__FUNCTION__,__FILE__,__LINE__);   \
        snprintf(&xx___buf[size],CHK_MAX_LOG_SIZE-size,__VA_ARGS__);                            \
        chk_log(LOGFILE,LOGLEV,xx___buf);                                                       \
    }                                                                                           \
}while(0)

#define CHK_SYSLOG(LOGLEV,...)                                                                  \
do{                                                                                             \
    if(LOGLEV >= g_loglev){                                                                     \
        char *xx___buf = malloc(CHK_MAX_LOG_SIZE);                                              \
        if(!xx___buf) break;                                                                    \
        int32_t size = chk_log_prefix_detail(xx___buf,LOGLEV,__FUNCTION__,__FILE__,__LINE__);   \
        snprintf(&xx___buf[size],CHK_MAX_LOG_SIZE-size,__VA_ARGS__);                            \
        chk_syslog(LOGLEV,xx___buf);                                                            \
    }                                                                                           \
}while(0)

#define CHK_DEF_LOG(LOGNAME,LOGFILENAME)                                                        \
    typedef struct{chk_logfile *_logfile;}LOGNAME;                                              \
    static inline LOGNAME *LOGNAME##create_function(){                                          \
    	LOGNAME *tmp = calloc(1,sizeof(*tmp));                                                  \
        if(!tmp) return NULL;                                                                   \
    	tmp->_logfile = chk_create_logfile(LOGFILENAME);                                        \
    	return tmp;                                                                             \
    }                                                                                           \
    CHK_DECLARE_SINGLETON(LOGNAME)

#define CHK_IMP_LOG(LOGNAME) CHK_IMPLEMENT_SINGLETON(LOGNAME,LOGNAME##create_function,NULL)

#define CHK_GET_LOGFILE(LOGNAME) CHK_GET_INSTANCE(LOGNAME)->_logfile


#endif
