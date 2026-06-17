#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <string.h>
#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed: name existed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed"

#define SEARCH_NO "no such user"
#define SEARCH_ONLINE "online"
#define SEARCH_OFFLINE "offline"

#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OFFLINE "user offline"
#define ADD_FRIEND_NOEXIST "user not exist"
#define DELETE_FRIEND_SUCCESS "delete friend success"
#define CREATE_DIR_SUCCESS "create dir success"
#define CREATE_DIR_FAIL "create dir fail!dir exists!"
#define UPLOAD_FILE_OK "upload file ok"
#define UPLOAD_FILE_FAIL "upload file fail"
#define DOWNLOAD_FILE_FAIL "download file fail"
typedef unsigned int uint;
#include<QString>
enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGIST_REQUEST,//注册请求
    ENUM_MSG_TYPE_REGIST_RESPONSE,//注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST,//登录请求
    ENUM_MSG_TYPE_LOGIN_RESPONSE,//登录回复

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,//在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE,//在线用户回复
    ENUM_MSG_TYPE_SEARCH_USER_REQUEST,//搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USER_RESPONSE,//搜索用户回复

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE,//添加好友回复

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE,
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,//同意好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,//拒绝好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,//删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE,//删除好友回复
    ENUM_MSG_TYPE_CHAT_ALONE_REQUEST,//私聊好友请求
    ENUM_MSG_TYPE_GROUP_REQUEST,//群聊

    ENUM_CREATE_DIR_REQUEST,//创建文件夹请求
    ENUM_CREATE_DIR_RESPONSE,//创建文件夹回复

    ENUM_CREATE_DIR_CLICKED_REQUEST,//手动创建文件夹请求
    ENUM_CREATE_DIR_CLICKED_RESPONSE,//手动创建文件夹回复

    ENUM_FLUSH_FILE_REQUEST,//刷新文件请求
    ENUM_FLUSH_FILE_RESPONSE,//刷新文件回复

    ENUM_UPLOAD_FILE_REQUEST,//上传文件请求
    ENUM_UPLOAD_FILE_RESPONSE,//上传文件回复

    ENUM_DOWNLOAD_FILE_REQUEST,
    ENUM_DOWNLOAD_FILE_RESPONSE,

    ENUM_SHARE_FILE_REQUEST,
    ENUM_SHARE_FILE_RESPONSE,
    ENUM_MSG_TYPE_MAX=0x00ffffff
};
struct fileInformation{
    char info[32];
    int isFile;
};

struct PDU
{
    uint uiPDULen;//总的消息长度
    uint uiMsgType;//消息的类型
    char caData[64];
    uint uiMsgLen;//实际消息长度
    int caMsg[];//实际消息
};
PDU* mkPDU(uint uiMsgLen);
#endif // PROTOCOL_H
