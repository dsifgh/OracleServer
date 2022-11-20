#pragma once
#include "ModuleBasic.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include"DataStruct.h"
#include "JsonModule.h"
#include"OracleConnectModule.h"
#include<map>
#pragma comment(lib, "ws2_32.lib")
#include<string>
class ConnectModule :
    public ModuleBasic
{
public:
    static ConnectModule* Get_Instance()
    {
        if (ptr == NULL)
        {
            ptr = new ConnectModule();
        }
        return ptr;
    }
    virtual bool OnModuelStart()
    {
        cout << "Connect Module Start" << endl;
        UserMap.clear();
        IsOpen = false;
        WSADATA wsdata;
        if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0)
        {
            //输出出错信息
            cout << "载入socket库出错！" << endl;
            return false;
        }

        //创建Socket
        sockSer = socket(AF_INET, SOCK_STREAM, 0);
        //初始化地址
        addrSer.sin_port = htons(50000);
        addrSer.sin_family = AF_INET;
        addrSer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


        //绑定Socket
        bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));
        cout << "Create Socket Connect Success" << endl;
        IsOpen = true;
        return true;
    }
    virtual void OnModuleUpdate()
    {
        if (!IsOpen)
            return;
        listen(sockSer, 2);
        ReceiveConnectRequset();
    }
    virtual void OnModuleEnd()
    {
        cout << "Connect Module Exit" << endl;
        IsOpen = false;
        closesocket(sockSer);
        WSACleanup();
    }
private:
    void ReceiveConnectRequset()
    {
        //接受连接请求
        SOCKET sockCli = accept(sockSer, (SOCKADDR*)&addrCli, &naddr);
        if (sockCli != INVALID_SOCKET) {
            cout << "收到客户端的连接请求" << endl;
            //创建线程 并且传入与client通讯的套接字
            HANDLE hThread = CreateThread(NULL, 0, ThreadFun, (LPVOID)sockCli, 0, NULL);
            CloseHandle(hThread); //关闭对线程的引用
        }
    }
    void OnReceiveMessageHandler(SOCKET socket,DataStruct::MainData mainData)
    {
        if (mainData.MessageType == DataStruct::Login)
        {
            DataStruct::LoginData loginData;
            loginData = JsonModule::Get_Instance()->JsonToData<DataStruct::LoginData>(mainData.Message);
            bool loginState = OracleConnectModule::Get_Instance()->CheckUserLogin(loginData);
            DataStruct::LoginStateData data;
            data.State = loginState;
            string loginStateString = JsonModule::Get_Instance()->DataToJson<DataStruct::LoginStateData>(data);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::LoginState, loginStateString));
        }
    }
    void SendMessage(SOCKET socket, string message)
    {
        strcpy_s(sendbuf,message.c_str());
        send(socket, sendbuf, sizeof(sendbuf), 0);
        cout << "发送消息" << socket << " " << sendbuf << endl;
    }
    DWORD static WINAPI ThreadFun(LPVOID lpThreadParameter)
    {
        //与客户端通讯 先发送再接收数据
        SOCKET sock = (SOCKET)lpThreadParameter;
        cout << "成功和" << sock << "建立连接！" << endl;
        char msgbuffer[1000];
        strcpy_s(msgbuffer, "欢迎登录");
        int size = send(sock, msgbuffer, sizeof(msgbuffer), 0);//给客户端发送一段信息
        if (size == SOCKET_ERROR || size == 0)
        {
            cout << "发送信息失败！错误代码：" << WSAGetLastError() << endl;
            closesocket(sock);
            return 0;
        }
        while (ConnectModule::Get_Instance()->IsOpen)
        {
            //接收客户端数据
            memset(msgbuffer, 0, sizeof(msgbuffer));
            int ret = recv(sock, msgbuffer, sizeof(msgbuffer), 0);
            if (ret == SOCKET_ERROR || ret == 0)
            {
                cout << sock << "断开了连接！" << endl;
                break;
            }
            else 
            {
                cout << "接收到：" << sock << " " << msgbuffer << endl;
                DataStruct::MainData mainData;
                mainData = JsonModule::Get_Instance()->JsonToData<DataStruct::MainData>(msgbuffer);
                Get_Instance()->OnReceiveMessageHandler(sock,mainData);
            }
        }
        closesocket(sock);
        return 0;
    }
private:
    ConnectModule()
    {
    }
    static ConnectModule* ptr;
    char sendbuf[1000],receivebuf[1000];
    SOCKET sockSer;
    SOCKADDR_IN addrSer, addrCli;
    int naddr = sizeof(SOCKADDR_IN);
    map<string,SOCKET> UserMap;
};

