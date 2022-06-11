#pragma once
#include "main.h"
#include "SceneBase.h"


class Result :
	public ISceneBase
{
public:
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
};

