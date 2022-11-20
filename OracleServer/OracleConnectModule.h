#pragma once
#include "ModuleBasic.h"
#include <cstddef>
#include <occi.h>
#include<iostream>
#include"DataStruct.h"
using namespace std;
using namespace oracle::occi;
class OracleConnectModule :
    public ModuleBasic
{
public:
    static  OracleConnectModule* Get_Instance()
    {
        if (ptr == NULL)
        {
            ptr = new  OracleConnectModule();
        }
        return ptr;
    }
private:
    OracleConnectModule()
    {
    }
    static OracleConnectModule* ptr;
public:
    virtual bool OnModuelStart()
    {
        env = Environment::createEnvironment();
        if (NULL == env) {
            printf("CreateOracleDataBaseEnvironment Error.\n");
            return false;
        }
        else
            cout << "CreateOracleDataBaseEnvironment Success" << endl;

        string name = "moyu";
        string pass = "guojiayi520";
        string srvName = "127.0.0.1:1521/orcl";

        //�������ݿ�����  
        try
        {
            conn = env->createConnection(name, pass, srvName);//�û��������룬���ݿ���  
            if (NULL == conn) {
                printf("CreateOracleDataBaseConnection Error.\n");
                return false;
            }
            else
                cout << "CreateOracleDataBaseConnection Success" << endl; \

            //  ���ݲ���,����Statement����  
            pStmt = NULL;    // Statement����  
            pStmt = conn->createStatement();
            if (NULL == pStmt) {
                printf("CreateOracleDataBaseStatement error.\n");
                return false;
            }
        }
        catch (SQLException e)
        {
            cout << e.getMessage() << endl;
            return false;
        }
        return true;
    }
    virtual void OnModuleEnd()
    {
        conn->cancel();
    }
    bool CheckUserLogin(DataStruct::LoginData loginData)
    {
        string userID = loginData.UserID;
        string userPwd = loginData.UserPwd;
        return true;
    }
private:
    Environment* env;
    Connection* conn;
    Statement* pStmt;
};

