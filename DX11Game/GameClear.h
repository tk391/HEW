#pragma once
#include "main.h"
#include "SceneBase.h"

class GameClear :public ISceneBase {
public:
	HRESULT Init();	// 初期化
	void Uninit();		// 終了処理
	void Update();		// 更新
	void Draw();		// 描画
};