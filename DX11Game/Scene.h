#pragma once
#include "main.h"
#include "EffectManager.h"

// 定数定義
enum EScene {
	SCENE_NONE = 0,	// 未設定
	SCENE_TITLE,		// タイトル画面
	SCENE_SELECT,		//選択画面
	SCENE_GAME,		// ゲーム画面
	SCENE_RESULT,		// リザルト画面

	MAX_SCENE		// シーン総数
};

class Scene {
public:
	// 関数プロトタイプ
	HRESULT Init();				// 初期化
	void Uninit();					// 終了処理
	void Update();					// 更新
	void Draw();					// 描画
private:
	void SetVolume();			//全部のサウンドの音量設定
};

int GetProgress(void);
void AddProgress(void);

HRESULT SetScene(EScene eScene);	// 切替