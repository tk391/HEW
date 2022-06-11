#pragma once
#include "main.h"

class ISceneBase {
public:
	virtual HRESULT Init()=0;
	virtual void Uninit()=0;
	virtual void Update()=0;
	virtual void Draw()=0;
};