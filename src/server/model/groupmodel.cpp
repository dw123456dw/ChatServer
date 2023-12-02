#include "groupmodel.hpp"
#include "db.h"

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname,groupdesc) values('%s','%s');",
            group.getName().c_str(), group.getDesc().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取插入成功的用户数据生成的主键id
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false;
}
// 加入群组
void GroupModel::addGroup(int userid, int groupid, string role)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d,%d,'%s');",
            groupid,userid,role.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    } 
}
// 查询用户所在群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    //先根据userid在groupuser表中查出该用户所属的群组信息
    //在根据群组信息，查询属于该群组的所有用户id，并和user表进行联合查询，查询各个用户的具体信息

    //1.组装sql语句
    char sql[1024]={0};
    sprintf(sql,"select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid=%d;",userid);
    
    vector<Group> groupVec;

    MySQL mysql;
    if(mysql.connect())
    {   
        MYSQL_RES* res = mysql.query(sql);

        if(res != nullptr)
        {
            MYSQL_ROW row;
            //把userid用户的所有离线消息放入数组，返回
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }

            mysql_free_result(res);
        }
    }

    for(Group &group : groupVec)
    {
        sprintf(sql,"select a.id,a.name,a.state,b.grouprole from user a inner join groupuser b on b.userid = a.id where b.groupid=%d;",group.getId());

        MYSQL_RES* res = mysql.query(sql);

        if(res != nullptr)
        {
            MYSQL_ROW row;
            //把userid用户的所有离线消息放入数组，返回
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUSers().push_back(user);
            }

            mysql_free_result(res);
        }
    }

    return groupVec;
}
// 根据groupid查询群组用户id列表，除userid,主要用来发群聊消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d;", groupid,userid);

    vector<int> idVec;

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);

        if (res != nullptr)
        {
            MYSQL_ROW row;

            while((row = mysql_fetch_row(res))!= nullptr)
            {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }

    return idVec;
}