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
            data.UserID = loginData.UserID;
            data.State = loginState;
            data.CardID = OracleConnectModule::Get_Instance()->GetUserCardID(loginData.UserID);
            string loginStateString = JsonModule::Get_Instance()->DataToJson<DataStruct::LoginStateData>(data);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::LoginState, loginStateString));
        }
        else if (mainData.MessageType == DataStruct::Student_Info)
        {
            DataStruct::RequestInfoData requestData;
            requestData = JsonModule::Get_Instance()->JsonToData<DataStruct::RequestInfoData>(mainData.Message);
            if (requestData.Type == DataStruct::Student_Card_Info)
            {
                DataStruct::StudentInfoData studentData = OracleConnectModule::Get_Instance()->GetUserStudentData(requestData.UserID);
                string studentInfo = JsonModule::Get_Instance()->DataToJson<DataStruct::StudentInfoData>(studentData);
                SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::Student_Card_Info, studentInfo));
            }
            else if (requestData.Type == DataStruct::Student_Card_State_Info)
            {
                DataStruct::StudentCardStateData data = OracleConnectModule::Get_Instance()->GetUserStudentCardData(requestData.UserID);
                string cardInfo = JsonModule::Get_Instance()->DataToJson<DataStruct::StudentCardStateData>(data);
                SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::Student_Card_State_Info, cardInfo));
            }
            else if (requestData.Type == DataStruct::Consume_Info)
            {
                list<DataStruct::Consume_InfoData> data = OracleConnectModule::Get_Instance()->GetConsumeData(requestData.UserID);
                string cardInfo = JsonModule::Get_Instance()->DataToJson<list<DataStruct::Consume_InfoData>>(data);
                SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::Consume_Info, cardInfo));
            }
            else if (requestData.Type == DataStruct::Books_Info)
            {
                list<DataStruct::Books_info> data = OracleConnectModule::Get_Instance()->GetBooksInfo(requestData.UserID);
                string bookinfo = JsonModule::Get_Instance()->DataToJson<list<DataStruct::Books_info>>(data);
                SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::Books_Info, bookinfo));
            }
            else if (requestData.Type == DataStruct::Books_Info_South)
            {
                list<DataStruct::Books_info> data = OracleConnectModule::Get_Instance()->GetBooksInfoSouth(requestData.UserID);
                string bookinfo = JsonModule::Get_Instance()->DataToJson<list<DataStruct::Books_info>>(data);
                SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::Books_Info, bookinfo));
            }
        }
        else if (mainData.MessageType == DataStruct::LendBookWest)
        {
            DataStruct::Lend_Book_Info data = JsonModule::Get_Instance()->JsonToData<DataStruct::Lend_Book_Info>(mainData.Message);
            OracleConnectModule::Get_Instance()->LendBookWest(data.UserID, data.Book_ID);

            list<DataStruct::Books_info> bookData = OracleConnectModule::Get_Instance()->GetBooksInfo(data.UserID);
            string bookinfo = JsonModule::Get_Instance()->DataToJson<list<DataStruct::Books_info>>(bookData);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::Books_Info, bookinfo));
        }
        else if (mainData.MessageType == DataStruct::ReturnBookWest)
        {
            DataStruct::Lend_Book_Info data = JsonModule::Get_Instance()->JsonToData<DataStruct::Lend_Book_Info>(mainData.Message);
            OracleConnectModule::Get_Instance()->ReturnBookWest(data.UserID,data.Book_ID);

            list<DataStruct::Books_info> bookData = OracleConnectModule::Get_Instance()->GetBooksInfo(data.UserID);
            string bookinfo = JsonModule::Get_Instance()->DataToJson<list<DataStruct::Books_info>>(bookData);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::Books_Info, bookinfo));
        }
        else if (mainData.MessageType == DataStruct::LendBookSouth)
        {
            DataStruct::Lend_Book_Info data = JsonModule::Get_Instance()->JsonToData<DataStruct::Lend_Book_Info>(mainData.Message);
            OracleConnectModule::Get_Instance()->LendBookSouth(data.UserID, data.Book_ID);

            list<DataStruct::Books_info> bookData = OracleConnectModule::Get_Instance()->GetBooksInfoSouth(data.UserID);
            string bookinfo = JsonModule::Get_Instance()->DataToJson<list<DataStruct::Books_info>>(bookData);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::Books_Info, bookinfo));
        }
        else if (mainData.MessageType == DataStruct::ReturnBookSouth)
        {
            DataStruct::Lend_Book_Info data = JsonModule::Get_Instance()->JsonToData<DataStruct::Lend_Book_Info>(mainData.Message);
            OracleConnectModule::Get_Instance()->ReturnBookSouth(data.UserID, data.Book_ID);

            list<DataStruct::Books_info> bookData = OracleConnectModule::Get_Instance()->GetBooksInfoSouth(data.UserID);
            string bookinfo = JsonModule::Get_Instance()->DataToJson<list<DataStruct::Books_info>>(bookData);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::Books_Info, bookinfo));
        }
        else if (mainData.MessageType == DataStruct::RequestShopItemListWest)
        {
            DataStruct::RequestShopListData data = JsonModule::Get_Instance()->JsonToData<DataStruct::RequestShopListData>(mainData.Message);
            string dataString;
            list<DataStruct::ShopItem> shopData;
            if (data.ListType == 0)
            {
                shopData = OracleConnectModule::Get_Instance()->RequestWestFoodList();
            }
            else
            {
                shopData = OracleConnectModule::Get_Instance()->RequestWestShopList();
            }
            dataString = JsonModule::Get_Instance()->DataToJson<list<DataStruct::ShopItem>>(shopData);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::RequestShopItemListWest, dataString));
        }
        else if (mainData.MessageType == DataStruct::RequestShopItemListSouth)
        {
            DataStruct::RequestShopListData data = JsonModule::Get_Instance()->JsonToData<DataStruct::RequestShopListData>(mainData.Message);
            string dataString;
            list<DataStruct::ShopItem> shopData;
            if (data.ListType == 0)
            {
                shopData = OracleConnectModule::Get_Instance()->RequestSouthFoodList();
            }
            else
            {
                shopData = OracleConnectModule::Get_Instance()->RequestSouthShopList();
            }
            dataString = JsonModule::Get_Instance()->DataToJson<list<DataStruct::ShopItem>>(shopData);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::RequestShopItemListSouth, dataString));
        }
        else if (mainData.MessageType == DataStruct::RequestBuyShopItemWest)
        {
            DataStruct::RequestBuyItemData data = JsonModule::Get_Instance()->JsonToData<DataStruct::RequestBuyItemData>(mainData.Message);
            bool state= OracleConnectModule::Get_Instance()->RequestBuyShopWest(data.UserID, data.ListType, data.ItemID);

            DataStruct::BuyItemState backData;
            backData.State = state;
            string buyStateString = JsonModule::Get_Instance()->DataToJson<DataStruct::BuyItemState>(backData);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::BuyItemBack, buyStateString));
        }
        else if (mainData.MessageType == DataStruct::RequestBuyShopItemShouth)
        {
            DataStruct::RequestBuyItemData data = JsonModule::Get_Instance()->JsonToData<DataStruct::RequestBuyItemData>(mainData.Message);
            bool state = OracleConnectModule::Get_Instance()->RequestBuyShopSouth(data.UserID, data.ListType, data.ItemID);

            DataStruct::BuyItemState backData;
            backData.State = state;
            string buyStateString = JsonModule::Get_Instance()->DataToJson<DataStruct::BuyItemState>(backData);
            SendMessage(socket, JsonModule::Get_Instance()->BuildMainData(DataStruct::BuyItemBack, buyStateString));
        }
    }
    void SendMessage(SOCKET socket, string message)
    {
        //strcpy_s(sendbuf,message.c_str());
        send(socket, message.c_str(), message.length(), 0);
        cout << "发送消息" << socket << " " << message << endl;
    }
    DWORD static WINAPI ThreadFun(LPVOID lpThreadParameter)
    {
        //与客户端通讯 先发送再接收数据
        SOCKET sock = (SOCKET)lpThreadParameter;
        cout << "成功和" << sock << "建立连接！" << endl;
        char msgbuffer[1000];
        /*strcpy_s(msgbuffer, "欢迎登录");
        int size = send(sock, msgbuffer, sizeof(msgbuffer), 0);//给客户端发送一段信息
        if (size == SOCKET_ERROR || size == 0)
        {
            cout << "发送信息失败！错误代码：" << WSAGetLastError() << endl;
            closesocket(sock);
            return 0;
        }*/
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

