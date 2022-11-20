#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <occi.h>  
#include "JsonModule.h"
#include "ConnectModule.h"
#include "OracleConnectModule.h"
#include"DataStruct.h"
#pragma comment(lib,"WS2_32.lib")

using namespace std;
using namespace oracle::occi;

bool ApplicaitonStartSuccess = true;

void OnApplicationUpdate()
{
    ConnectModule::Get_Instance()->OnModuleUpdate();
}

void OnApplicationExitHandler()
{
    cout << "Exit" << endl;
    ConnectModule::Get_Instance()->OnModuelStart();
    OracleConnectModule::Get_Instance()->OnModuleEnd();
}

int main()
{
    atexit(OnApplicationExitHandler);
    OracleConnectModule::Get_Instance()->OnModuelStart();
    ConnectModule::Get_Instance()->OnModuelStart();

    while (ApplicaitonStartSuccess)
    {
        OnApplicationUpdate();
    }
}