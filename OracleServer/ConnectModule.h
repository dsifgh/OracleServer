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
            //���������Ϣ
            cout << "����socket�����" << endl;
            return false;
        }

        //����Socket
        sockSer = socket(AF_INET, SOCK_STREAM, 0);
        //��ʼ����ַ
        addrSer.sin_port = htons(50000);
        addrSer.sin_family = AF_INET;
        addrSer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


        //��Socket
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
        //������������
        SOCKET sockCli = accept(sockSer, (SOCKADDR*)&addrCli, &naddr);
        if (sockCli != INVALID_SOCKET) {
            cout << "�յ��ͻ��˵���������" << endl;
            //�����߳� ���Ҵ�����clientͨѶ���׽���
            HANDLE hThread = CreateThread(NULL, 0, ThreadFun, (LPVOID)sockCli, 0, NULL);
            CloseHandle(hThread); //�رն��̵߳�����
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
        cout << "������Ϣ" << socket << " " << sendbuf << endl;
    }
    DWORD static WINAPI ThreadFun(LPVOID lpThreadParameter)
    {
        //��ͻ���ͨѶ �ȷ����ٽ�������
        SOCKET sock = (SOCKET)lpThreadParameter;
        cout << "�ɹ���" << sock << "�������ӣ�" << endl;
        char msgbuffer[1000];
        strcpy_s(msgbuffer, "��ӭ��¼");
        int size = send(sock, msgbuffer, sizeof(msgbuffer), 0);//���ͻ��˷���һ����Ϣ
        if (size == SOCKET_ERROR || size == 0)
        {
            cout << "������Ϣʧ�ܣ�������룺" << WSAGetLastError() << endl;
            closesocket(sock);
            return 0;
        }
        while (ConnectModule::Get_Instance()->IsOpen)
        {
            //���տͻ�������
            memset(msgbuffer, 0, sizeof(msgbuffer));
            int ret = recv(sock, msgbuffer, sizeof(msgbuffer), 0);
            if (ret == SOCKET_ERROR || ret == 0)
            {
                cout << sock << "�Ͽ������ӣ�" << endl;
                break;
            }
            else 
            {
                cout << "���յ���" << sock << " " << msgbuffer << endl;
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

