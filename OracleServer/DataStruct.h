#pragma once
#include<iostream>
#include<String>
#include"Include/xpack/json.h"
using namespace std;
class DataStruct
{
public:
	enum MessageType { Login, LoginState, Student_Info,Student_Card_Info,Student_Card_State_Info, Consume_Info,Books_Info,LendBookWest,ReturnBookWest, Books_Info_South, LendBookSouth, ReturnBookSouth,RequestShopItemListWest,RequestBuyShopItemWest, RequestShopItemListSouth, RequestBuyShopItemShouth,BuyItemBack};
	struct MainData
	{
		int MessageType;
		string Message;
		XPACK(O(MessageType, Message));
	};
	struct LoginData
	{
		string UserID;
		string UserPwd;
		XPACK(O(UserID, UserPwd));
	};
	struct LoginStateData
	{
		string UserID;
		string CardID;
		bool State;
		XPACK(O(UserID, CardID, State));
	};
	struct RequestInfoData
	{
		string UserID;
		int Type;
		XPACK(O(UserID,Type));
	};
	struct StudentInfoData
	{
		string Card_id;
		string Stu_id;
		string Stu_name;
		string Exp_date;
		string College_name;
		string Class;
		string Money;
		string State;
		string Bankcard_id;
		XPACK(O(Card_id, Stu_id, Stu_name, Exp_date, College_name, Class, Money, State, Bankcard_id));
	};
	struct StudentCardStateData
	{
		string Account_date;
		string Lost_date;
		string Find_date;
		string Cancel_data;
		XPACK(O(Account_date,Lost_date, Find_date, Cancel_data));
	};
	struct Consume_InfoData
	{
		string Serial;
		string Amount;
		string Balance;
		string Recharge;
		string Date;
		string Place;
		XPACK(O(Serial, Amount, Balance, Recharge,Date,Place));
	};
	struct Books_info
	{
		string Book_name;
		string Book_state;
		string URL;
		int LendState;//0 可借 1可归还 2其他
		XPACK(O(Book_name,Book_state,URL,LendState));
	};
	struct Lend_Book_Info
	{
		string UserID;
		int Book_ID;
		XPACK(O(UserID, Book_ID));
	};
	struct RequestShopListData
	{
		int ListType;//0 食堂 1超市
		XPACK(O(ListType));
	};
	struct RequestBuyItemData
	{
		string UserID;
		int ListType;//0 食堂 1超市
		int ItemID;
		XPACK(O(ListType, ItemID));
	};
	struct ShopItem
	{
		string ItemName;
		string PicURL;
		int ItemType;
		int ItemID;
		int ItemPrice;
		XPACK(O(ItemName, PicURL, ItemType, ItemID, ItemPrice));
	};
	struct BuyItemState 
	{
		bool State;
		XPACK(O(State));
	};
};

