#pragma once

class IModel
{
public:
	virtual int Setup() = 0;
	virtual int Draw() = 0;
};