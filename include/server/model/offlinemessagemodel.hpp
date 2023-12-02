#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

using namespace std;
#include <string>
#include <vector>


//离线消息表的操作接口方法
class  OfflineMsgModel
{   
public:
    //存储用户离线消息
    void insert(int userid,string msg);

    //删除用户离线的消息
    void remove(int userid);

    //查询用户的离线消息
    vector<string> query(int userid);
};


#endif
