#ifndef _CHK_REDIS_CLIENT_H
#define _CHK_REDIS_CLIENT_H

#include <stdint.h>
#include "event/chk_event_loop.h"
#include "socket/chk_socket_helper.h"  


#define REDIS_REPLY_STRING  1
#define REDIS_REPLY_ARRAY   2
#define REDIS_REPLY_INTEGER 3
#define REDIS_REPLY_NIL     4
#define REDIS_REPLY_STATUS  5
#define REDIS_REPLY_ERROR   6


#define REDIS_OK     0
#define REDIS_RETRY -1
#define REDIS_ERR   -2  


typedef struct redisReply redisReply;

typedef struct chk_redisclient chk_redisclient;

//连接被关闭后回调
typedef void (*chk_redis_disconnect_cb)(chk_redisclient*,void *ud,int32_t err);
//连接回调
typedef void (*chk_redis_connect_cb)(chk_redisclient*,void *ud,int32_t err);
//请求回调
typedef void (*chk_redis_reply_cb)(chk_redisclient*,redisReply*,void *ud);

struct redisReply {
    int32_t      type;    /* REDIS_REPLY_* */
    int64_t      integer; /* The integer when type is REDIS_REPLY_INTEGER */
    int32_t      len;     /* Length of string */
    char        *str;     /* Used for both REDIS_REPLY_ERROR and REDIS_REPLY_STRING */
    size_t       elements;/* number of elements, for REDIS_REPLY_ARRAY */
    redisReply **element; /* elements vector for REDIS_REPLY_ARRAY */
};

/**
 * 连接redis服务器
 * @param e 事件处理循环
 * @param addr 服务器地址结构
 * @param cntcb 连接回调
 * @param ud 传递给连接回调的用户数据
 * @param dcntcb 连接关闭回调
 */

int32_t chk_redis_connect(chk_event_loop *e,chk_sockaddr *addr,chk_redis_connect_cb cntcb,void *ud);

void    chk_redis_set_disconnect_cb(chk_redisclient *c,chk_redis_disconnect_cb cb,void *ud);

/**
 * 关闭redis连接,当连接结构销毁时回调dcntcb
 * @param c redis连接
 * @param err 错误码
 */

void chk_redis_close(chk_redisclient *c,int32_t err);

/**
 * 执行redis命令
 * @param c redis连接
 * @param str redis命令字符串
 * @param cb 请求执行回调(成功/出错)
 * @param ud 传递给cb的用户数据
 */

int32_t chk_redis_execute(chk_redisclient*,const char *str,chk_redis_reply_cb cb,void *ud);

#endif    
