#include "chatserver.hpp"
#include "json.hpp"
#include <iostream>
#include "chatservice.hpp"

// 函数对象绑定器
#include <functional>
#include <string>

using namespace std;
// 参数占位符
using namespace placeholders;
//json库
using json = nlohmann::json;

#include <muduo/base/Logging.h>

// 初始化聊天服务器
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册回调连接
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    //一个I/O主Reactor线程3个逻辑工作线程
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 上报连接相关的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    //客户端断开连接
    if(!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }

    cout <<"连接111111"<<endl;
}
// 上报读写事件相关的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{

    //接收json
    string buf = buffer->retrieveAllAsString();
    //数据的反序列化
    json js = json::parse(buf);
    //通过js["msgid"] 获取-》业务处理handler-》conn   js    time
    //达到的目的：完全解耦网络模块的代码和业务模块的代码
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    //回调消息绑定好的事件处理器，执行业务执行。
    msgHandler(conn,js,time);
}