#pragma once
#include "main.h"
#include "SceneBase.h"

class Select:public ISceneBase {
public:
	HRESULT Init();	// 初期化
	void Uninit();		// 終了処理
	void Update();		// 更新
	void Draw();		// 描画
};

int GetStageNum();	//リザルトで使用
void SetStageNum(int stnum);	//ステージをセットする