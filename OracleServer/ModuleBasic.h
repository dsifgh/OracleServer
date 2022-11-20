#pragma once
using namespace std;
class ModuleBasic
{
public :
	bool IsOpen;
	virtual bool OnModuelStart()
	{
		return false;
	}
	virtual void OnModuleUpdate()
	{
		if (IsOpen)
			return;
	}
	virtual void OnModuleEnd()
	{

	}
};

