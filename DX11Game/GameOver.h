#pragma once
#include "main.h"
#include "SceneBase.h"

class GameOver :public ISceneBase {
public:
	HRESULT Init();	// ‰Šú‰»
	void Uninit();		// I—¹ˆ—
	void Update();		// XV
	void Draw();		// •`‰æ
};