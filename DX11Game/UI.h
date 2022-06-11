#pragma once
#include "main.h"

class UI
{
public:
	virtual HRESULT Init() = 0;
	virtual void Uninit() = 0;
};