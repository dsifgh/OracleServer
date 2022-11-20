#pragma once
#include<iostream>
#include"Include/xpack/json.h"
#include "ModuleBasic.h"
#include"DataStruct.h"
class JsonModule :
    public ModuleBasic
{
public:
    static  JsonModule* Get_Instance()
    {
        if (ptr == NULL)
        {
            ptr = new  JsonModule();
        }
        return ptr;
    }
private:
    JsonModule()
    {
    }
    static JsonModule* ptr;
public:
    template <class T>
    string DataToJson(T const& data)
    {
        string json = xpack::json::encode(data);
        return json;
    }
    template <class T>
    T JsonToData(string dataJson)
    {
        T data;
        xpack::json::decode(dataJson, data);
        return data;
    }
    string BuildMainData(DataStruct::MessageType messageType,string message)
    {
        DataStruct::MainData mainData;
        mainData.MessageType = messageType;
        mainData.Message = message;
        return DataToJson<DataStruct::MainData>(mainData);
    }
};

