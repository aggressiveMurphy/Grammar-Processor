#ifndef LINKNODE_H
#define LINKNODE_H
#include<vector>
#include<string>
#include<iostream>
using namespace std;
//使用'@'标识EPSILON
#define EPSILON "@"
class LinkNode
{//定义存放文法的结构
public:
    //规则左部
    string left;
    //规则右部分 abc | def | hji，一个vector<string>元素代表一种可能
    vector<vector<string>> right;
    //记录处理后的正规文法规则的类型
    int t;
    //记录每条文法规则的类型
    int level;
    //构造函数，str是左边部分
    LinkNode(string str)
    {
        left = str;
        right.clear();
        level=0;
        t=0;
    }
    //右边添加规则
    void insert(vector<string> &nodes)
    {
        right.push_back(nodes);
    }
    //判断是否有epsilon存在
    bool includesEpsilon()
    {
        for(size_t i = 0; i < right.size(); i++)
        {
            if(right[i].size() == 1 && right[i][0] == EPSILON)
            {
                return true;
            }
        }
        return false;
    }
    //获取当前语法规则
    string toString()
    {
        string message = "";
        for(string s : this -> right[0])
        {
            message += s;
            cout<< s;
        }
        for(size_t i = 1; i < right.size(); i++)
        {
            message += " | ";
            cout<< " | ";
            for(string s : this -> right[i])
            {
                message += s;
                cout << s;
            }
            cout << endl;
        }
        return message;
    }
};
#endif // LINKNODE_H
