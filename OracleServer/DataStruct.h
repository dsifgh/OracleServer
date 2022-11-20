#pragma once
#include<iostream>
#include<String>
#include"Include/xpack/json.h"
using namespace std;
class DataStruct
{
public:
	enum MessageType { Login,LoginState, Quit };
	struct MainData
	{
		MessageType MessageType;
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
		bool State;
		XPACK(O(State));
	};
};

