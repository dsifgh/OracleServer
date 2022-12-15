#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#include "ModuleBasic.h"
#include <cstddef>
#include <occi.h>
#include<iostream>
#include"DataStruct.h"
#include <include/occiData.h>
#include<list>
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
    vector<string> BookNameWest = { "简爱","白夜行","魔法电子基础","类人群星闪耀时刻","小郭学霸笔记","地铁2033" };
    vector<string> BookNameSouth = { "如何让小郭爱上我","罗生门","高效养猪秘籍","管理的实践","理工的阴暗","赤脚医生手册" };
    vector<string> FoodNameWest = { "六元餐","叫了个鸭","麻辣烫","牛肉面" };
    vector<int> FoodWestPrice = { 5,20,16,12 };
    vector<string> FoodNameSouth = { "四元餐","叫了个猪","麻辣拌","小面" };
    vector<int> FoodSouthPrice = { 5,20,16,12 };
    vector<string> ShopNameWest = { "牙膏","作业本","签字笔","手机壳" };
    vector<int> ShopWestPrice = { 5,20,16,12 };
    vector<string> ShopNameSouth = { "USB","U盘","数据线","小夜灯" };
    vector<int> ShopSouthPrice = { 5,20,16,12 };
    OracleConnectModule()
    {
    }
    static OracleConnectModule* ptr;
public:
    virtual bool OnModuelStart()
    {
        env = Environment::createEnvironment("ZHS16GBK", "ZHS16GBK");
        if (NULL == env) {
            printf("CreateOracleDataBaseEnvironment Error.\n");
            return false;
        }
        else
            cout << "CreateOracleDataBaseEnvironment Success" << endl;

        string name = "Scott";
        string pass = "tiger";
        string srvName = "127.0.0.1:1521/West";

        //创建数据库连接  
        try
        {
            conn = env->createConnection(name, pass, srvName);//用户名，密码，数据库名  
            if (NULL == conn) {
                printf("CreateOracleDataBaseConnection Error.\n");
                return false;
            }
            else
                cout << "CreateOracleDataBaseConnection Success" << endl; \

            //  数据操作,创建Statement对象  
            pStmt = NULL;    // Statement对象  
            pStmt = conn->createStatement();
            if (NULL == pStmt) {
                printf("CreateOracleDataBaseStatement error.\n");
                return false;
            }
            cout << "当前时间：" << GetTime() << endl;
            /*try
            {
                pStmt->execute("call lend_book()");
            }
            catch (SQLException e)
            {
                cout << e.getMessage() << endl;
            }*/
            /*try
            {
                string sql = "select to_char(stu_pwd) from sys.card where stu_id = 1804020404" ;
                Statement* stmt = conn->createStatement();
                ResultSet* pRs = stmt->executeQuery(sql);
                if (pRs == NULL)
                    return false;
                while (pRs->next())
                {
                    cout<<pRs->getString(1)<<endl;

                }
                pStmt->closeResultSet(pRs);
            }
            catch (SQLException e)
            {
                cout << e.getMessage() << endl;
                return false;
            }*/
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
        cout << "收到用户：" << userID << "登录请求密码为： " << userPwd << endl;
        try
        {
            string sql =  "select to_char(stu_pwd) from sys.card where stu_id = "+ userID;
            Statement* stmt = conn->createStatement();
            cout <<"执行数据库sql语句：" << sql << endl;
            ResultSet* pRs = stmt->executeQuery(sql);
            if (pRs == NULL)
                return false;
            string pwd = "";
            while (pRs->next())
            {
                pwd = pRs->getString(1);
                break;
            }
            stmt->closeResultSet(pRs);
            bool pwdCurrent = userPwd==pwd;
            cout << "数据库验证密码为：" << pwd << " 密码" << (pwdCurrent ? "正确": "错误" )<< endl;
            if (pwdCurrent)
            {
                return true;
            }
        }
        catch (SQLException e)
        {
            cout << e.getMessage() << endl;
            return false;
        }

        return false;
    }
    DataStruct::StudentInfoData GetUserStudentData(string userID)
    {
        DataStruct::StudentInfoData data;
        try
        {
            string sql = "select to_char(card_id),stu_id,stu_name,to_char(exp_date),to_char(college_name),to_char(class),to_char(balance),card_status,to_char(bankcar_id) from sys.card where stu_id = " + userID;
            pStmt = conn->createStatement();
            cout << "执行数据库sql语句：" << sql << endl;
            ResultSet* pRs = pStmt->executeQuery(sql);
            if (pRs == NULL)
                return data;
            while (pRs->next())
            {
                data.Card_id = pRs->getString(1);
                data.Stu_id = pRs->getString(2);
                data.Stu_name = pRs->getString(3);
                data.Exp_date = pRs->getString(4);
                data.College_name = pRs->getString(5);
                data.Class = pRs->getString(6);
                data.Money = pRs->getString(7);
                data.State = pRs->getString(8);
                data.Bankcard_id = pRs->getString(9);
                break;
            }
            pStmt->closeResultSet(pRs);
            return data;
        }
        catch(SQLException e)
        {
            cout << e.getMessage() << endl;
        }
        return data;
    }
    DataStruct::StudentCardStateData GetUserStudentCardData(string userID)
    {
        DataStruct::StudentCardStateData data;
        try
        {
            string sql = "select to_char(card_id) from sys.card where stu_id = " + userID;
            pStmt = conn->createStatement();
            cout << "执行数据库sql语句：" << sql << endl;
            ResultSet* pRs = pStmt->executeQuery(sql);
            if (pRs == NULL)
                return data;
            string card_id = "";
            while (pRs->next())
            {
                card_id = pRs->getString(1);
                break;
            }
            pStmt->closeResultSet(pRs);
            sql = "select to_char(account_date),to_char(lost_date),to_char(find_date),to_char(cancel_date) from sys.status where card_id = " + card_id;
            pRs = pStmt->executeQuery(sql);
            if (pRs == NULL)
                return data;
            while (pRs->next())
            {
                data.Account_date = pRs->getString(1);
                data.Find_date = pRs->getString(2);
                data.Lost_date = pRs->getString(3);
                data.Cancel_data = pRs->getString(4);
                break;
            }
            return data;
        }
        catch (SQLException e)
        {
            cout << e.getMessage() << endl;
        }
        return data;
    }
    list<DataStruct::Consume_InfoData> GetConsumeData(string  userID)
    {
        list<DataStruct::Consume_InfoData> dataList;
        dataList.clear();
        string card_id = GetUserCardID(userID);
        try
        {
            string sql = "select to_char(serial),to_char(amount),to_char(balance),to_char(recharge),to_char(c_date),place from sys.consume where card_id = " + card_id;
            pStmt = conn->createStatement();
            cout << "执行数据库sql语句：" << sql << endl;
            ResultSet* pRs = pStmt->executeQuery(sql);
            if (pRs == NULL)
                return dataList;
            while (pRs->next())
            {
                DataStruct::Consume_InfoData data;
                data.Serial = pRs->getString(1);
                data.Amount = pRs->getString(2);
                data.Balance = pRs->getString(3);
                data.Recharge = pRs->getString(4);
                data.Date = pRs->getString(5);
                data.Place = pRs->getString(6);
                dataList.push_back(data);
            }
            pStmt->closeResultSet(pRs);
            return dataList;
        }
        catch (SQLException e)
        {
            cout << e.getMessage() << endl;
        }
    }
    string GetUserCardID(string userID)
    {
        DataStruct::StudentCardStateData data;
        try
        {
            string sql = "select to_char(card_id) from sys.card where stu_id = " + userID;
            pStmt = conn->createStatement();
            cout << "执行数据库sql语句：" << sql << endl;
            ResultSet* pRs = pStmt->executeQuery(sql);
            if (pRs == NULL)
                return NULL;
            string card_id = "";
            while (pRs->next())
            {
                card_id = pRs->getString(1);
                break;
            }
            pStmt->closeResultSet(pRs);
            return card_id;
        }
        catch(SQLException e)
        {
            cout << e.getMessage() << endl;
        }
    }
    list<DataStruct::Books_info> GetBooksInfo(string userID)
    {
        list<DataStruct::Books_info> bookinfos;
        string card_id = GetUserCardID(userID);
        for (int i = 0; i < BookNameWest.size(); i++)
        {
            string bookstate = "";
            string lenderCardID = "";
            try
            {
                string sql = "select book_status,card_id from book where book_name = '" + BookNameWest[i]+"'";
                pStmt = conn->createStatement();
                cout << "执行数据库sql语句：" << sql << endl;
                ResultSet* pRs = pStmt->executeQuery(sql);
                if (pRs == NULL)
                    return  bookinfos;
                while (pRs->next())
                {
                    bookstate = pRs->getString(1);
                    lenderCardID = pRs->getString(2);
                    break;
                }
                pStmt->closeResultSet(pRs);
                cout << "bookstate：" + bookstate << endl;
            }
            catch (SQLException e)
            {
                cout << e.getMessage() << endl;
            }

            DataStruct::Books_info bookinfo;
            bookinfo.Book_name = BookNameWest[i];
            bookinfo.Book_state = bookstate;
            int lendstate = 2;
            if ((bookstate == "" || bookstate == "已归还"))
            {
                lendstate = 0;
            }
            else if (bookstate == "借出" && lenderCardID == card_id)
            {
                lendstate = 1;
            }
            else
            {
                lendstate = 2;
            }
            bookinfo.LendState = lendstate;
            int index = i;
            bookinfo.URL = "120.53.12.217/BookPicWest/"+to_string(i) + ".jpg";
            bookinfos.push_back(bookinfo);
        }
        return  bookinfos;
    }
    void LendBookWest(string userID,int bookID)
    {
        try
        {
            string cardID = GetUserCardID(userID);
            string serial_number = GetTime() + to_string(bookID);
            string bookName = BookNameWest[bookID];
            string sql = "call lend_book(:v1,:v2,:v3,:v4)";
            pStmt = conn->createStatement(sql);
            pStmt->setString(1, serial_number);
            pStmt->setString(2, to_string(bookID));
            pStmt->setString(3, bookName);
            pStmt->setString(4, cardID);
            pStmt->executeUpdate();
        }
        catch (SQLException e)
        {
            cout << e.getMessage() << endl;
        }
    }
    /*void RequestLendBookListWest(string userID)
    {
        try
        {
            string cardID = "";;
            string sql = "select to_char(card_id) from sys.card where stu_id = " + userID;
            pStmt = conn->createStatement();
            cout << "执行数据库sql语句：" << sql << endl;
            ResultSet* pRs = pStmt->executeQuery(sql);
            while (pRs->next())
            {
                cardID = pRs->getString(1);
                break;
            }
            pStmt->closeResultSet(pRs);
            
        }
        catch (SQLException e)
        {
            cout << e.getMessage() << endl;
        }
    }*/
    void ReturnBookWest(string userID, int bookID)
    {
        try
        {
            string cardID = GetUserCardID(userID);
            string bookName = BookNameWest[bookID];
            string sql = "call return_book(:v1,:v2)";
            pStmt = conn->createStatement(sql);
            pStmt->setString(1, cardID);
            pStmt->setString(2, bookName);
            pStmt->executeUpdate();
        }
        catch (SQLException e)
        {
            cout << e.getMessage() << endl;
        }
    }
    list<DataStruct::Books_info> GetBooksInfoSouth(string userID)
    {
        list<DataStruct::Books_info> bookinfos;
        string card_id = GetUserCardID(userID);
        for (int i = 0; i < BookNameSouth.size(); i++)
        {
            string bookstate = "";
            string lenderCardID = "";
            try
            {
                string sql = "select book_status,card_id from sys.book_@dblink where book_name = '" + BookNameSouth[i] + "'";
                pStmt = conn->createStatement();
                cout << "执行数据库sql语句：" << sql << endl;
                ResultSet* pRs = pStmt->executeQuery(sql);
                if (pRs == NULL)
                    return  bookinfos;
                while (pRs->next())
                {
                    bookstate = pRs->getString(1);
                    lenderCardID = pRs->getString(2);
                    break;
                }
                pStmt->closeResultSet(pRs);
                cout << "bookstate：" + bookstate << endl;
            }
            catch (SQLException e)
            {
                cout << e.getMessage() << endl;
            }

            DataStruct::Books_info bookinfo;
            bookinfo.Book_name = BookNameSouth[i];
            bookinfo.Book_state = bookstate;
            int lendstate = 2;
            if ((bookstate == "" || bookstate == "已归还"))
            {
                lendstate = 0;
            }
            else if (bookstate == "借出" && lenderCardID == card_id)
            {
                lendstate = 1;
            }
            else
            {
                lendstate = 2;
            }
            bookinfo.LendState = lendstate;
            int index = i;
            bookinfo.URL = "120.53.12.217/BookPicSouth/" + to_string(i) + ".jpg";
            bookinfos.push_back(bookinfo);
        }
        return  bookinfos;
    }
    void LendBookSouth(string userID, int bookID)
    {
        try
        {
            string cardID = GetUserCardID(userID);
            string serial_number = GetTime() + to_string(bookID);
            string bookName = BookNameWest[bookID];
            string sql = "call lend_book@dblink(:v1,:v2,:v3,:v4)";
            pStmt = conn->createStatement(sql);
            pStmt->setString(1, serial_number);
            pStmt->setString(2, to_string(bookID));
            pStmt->setString(3, bookName);
            pStmt->setString(4, cardID);
            pStmt->executeUpdate();
        }
        catch (SQLException e)
        {
            cout << e.getMessage() << endl;
        }
    }
    void ReturnBookSouth(string userID, int bookID)
    {
        try
        {
            string cardID = GetUserCardID(userID);
            string bookName = BookNameWest[bookID];
            string sql = "call return_book(:v1,:v2)";
            pStmt = conn->createStatement(sql);
            pStmt->setString(1, cardID);
            pStmt->setString(2, bookName);
            pStmt->executeUpdate();
        }
        catch (SQLException e)
        {
            cout << e.getMessage() << endl;
        }
    }
    list<DataStruct::ShopItem> RequestWestFoodList()
    {
        list<DataStruct::ShopItem> foodList;
        int foodCount = FoodNameWest.size();
        for (int i = 0; i < foodCount; i++)
        {
            int index = i;
            DataStruct::ShopItem shopItemData;
            shopItemData.ItemName = FoodNameWest[i];
            shopItemData.ItemID = index;
            shopItemData.ItemType = 0;
            shopItemData.ItemPrice = FoodWestPrice[i];
            shopItemData.PicURL = "120.53.12.217/FoodWest/" + to_string(i) + ".jpg";
            foodList.push_back(shopItemData);
        }
        return foodList;
    }
    list<DataStruct::ShopItem> RequestWestShopList()
    {
        list<DataStruct::ShopItem> shopList;
        int shopCount = ShopNameWest.size();
        for (int i = 0; i < shopCount; i++)
        {
            int index = i;
            DataStruct::ShopItem shopItemData;
            shopItemData.ItemName = ShopNameWest[i];
            shopItemData.ItemID = index;
            shopItemData.ItemType = 1;
            shopItemData.ItemPrice = ShopWestPrice[i];
            shopItemData.PicURL = "120.53.12.217/ShopWest/" + to_string(i) + ".jpg";
            shopList.push_back(shopItemData);
        }
        return shopList;
    }
    list<DataStruct::ShopItem> RequestSouthFoodList()
    {
        list<DataStruct::ShopItem> foodList;
        int foodCount = FoodNameSouth.size();
        for (int i = 0; i < foodCount; i++)
        {
            int index = i;
            DataStruct::ShopItem shopItemData;
            shopItemData.ItemName = FoodNameSouth[i];
            shopItemData.ItemID = index;
            shopItemData.ItemType = 0;
            shopItemData.ItemPrice = FoodSouthPrice[i];
            shopItemData.PicURL = "120.53.12.217/FoodSouth/" + to_string(i) + ".jpg";
            foodList.push_back(shopItemData);
        }
        return foodList;
    }
    list<DataStruct::ShopItem> RequestSouthShopList()
    {
        list<DataStruct::ShopItem> shopList;
        int shopCount = ShopNameSouth.size();
        for (int i = 0; i < shopCount; i++)
        {
            int index = i;
            DataStruct::ShopItem shopItemData;
            shopItemData.ItemName = ShopNameSouth[i];
            shopItemData.ItemID = index;
            shopItemData.ItemType = 1;
            shopItemData.ItemPrice = ShopSouthPrice[i];
            shopItemData.PicURL = "120.53.12.217/ShopSouth/" + to_string(i) + ".jpg";
            shopList.push_back(shopItemData);
        }
        return shopList;
    }
    bool RequestBuyShopWest(string userID,int itemType,int itemID)
    {
        string place = itemType == 0 ? "食堂" : "超市";
        int price = itemType == 0 ? FoodWestPrice[itemID] : ShopWestPrice[itemID];
        return true;
    }
    bool RequestBuyShopSouth(string userID, int itemType, int itemID)
    {
        string place = itemType == 0 ? "食堂" : "超市";
        int price = itemType == 0 ? FoodSouthPrice[itemID] : ShopSouthPrice[itemID];
        return true;
    }

private:
    Environment* env;
    Connection* conn;
    Statement* pStmt;
    string GetTime()
    {
        time_t now = time(0);
        string time = ctime(&now);
        vector<string> times;
        Split(time, times);
        return  times[0]+" "+times[1];
    }
    void Split(const std::string& s,
            std::vector<std::string>& sv,
            const char* delim = " ") {
        sv.clear();                                 // 1.
        char* buffer = new char[s.size() + 1];      // 2.
        buffer[s.size()] = '\0';
        std::copy(s.begin(), s.end(), buffer);      // 3.
        char* p = std::strtok(buffer, delim);       // 4.
        do {
            sv.push_back(p);                        // 5.
        } while ((p = std::strtok(NULL, delim)));   // 6.
        delete[] buffer;
        return;
    }
};

