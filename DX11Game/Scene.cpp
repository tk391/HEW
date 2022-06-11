#include "Scene.h"
#include "Fade.h"
#include "Title.h"
#include "Select.h"
#include "Game.h"
#include "Result.h"
#include "SceneBase.h"
#include "bg.h"
#include "Sound.h"

// グローバル変数
static EScene g_eScene = SCENE_NONE;

static int g_nProgress;//進行度

//クラス宣言
ISceneBase* SceneBase[MAX_SCENE];

HRESULT Scene::Init() {
	HRESULT hr = S_OK;

	SceneBase[SCENE_NONE] = nullptr;
	SceneBase[SCENE_TITLE] = new Title;
	SceneBase[SCENE_SELECT] = new Select;
	SceneBase[SCENE_GAME] = new Game;
	SceneBase[SCENE_RESULT] = new Result;

	//シングルトンのオブジェクトを作成
	EffectManager::Init();		//全てのエフェクトの初期化

	InitBG();						//背景初期化
	InitFade();
	g_nProgress = 5;//ステージ１からプレイ可能
	hr = SetScene(SCENE_TITLE);	// 最初はタイトル画面へ
	
	//音量の設定
	SetVolume();

	return hr;
}
void Scene::Uninit() {
	SetScene(SCENE_NONE);	// 現在の画面を終了
	UninitFade();
	UninitBG();		//背景終了
	EffectManager::Uninit();//全てのエフェクトの削除

	for (int i = 0; i < MAX_SCENE; ++i) {
		delete SceneBase[i];
		SceneBase[i] = nullptr;
	}
}

void Scene::Update() {
	//シーンがなかったら処理をしない
	if (g_eScene == SCENE_NONE) {
		return;
	}

	//シーンごとに更新
	SceneBase[g_eScene]->Update();

	UpdateFade();		// フェードイン/アウト更新
}

void Scene::Draw() {
	//シーンがなかったら処理をしない
	if (g_eScene == SCENE_NONE) {
		return;
	}
	//シーンごとに描画
	SceneBase[g_eScene]->Draw();
	DrawFade();			// フェードイン/アウト描画
}

HRESULT SetScene(EScene eScene) {
	HRESULT hr = S_OK;
	//シーンがなかったら処理をしない
	if (g_eScene != SCENE_NONE) {
		// 現在の画面を終了
		//シーンごと
		SceneBase[g_eScene]->Uninit();
	}
	// 画面を入れ替え
	g_eScene = eScene;

	//シーンがなかったら処理をしない
	if (g_eScene != SCENE_NONE) {
		// 次の画面を初期化
		SceneBase[g_eScene]->Init();
		return hr;
	}
	return hr;
}

//進行度の管理
int GetProgress(void) {
	return g_nProgress;
}
void AddProgress(void) {
	g_nProgress++;
}



//全部の音の音量設定
void Scene::SetVolume() {
	//BGM
	CSound::SetVolume(BGM_TITLE, 80.0f);
	CSound::SetVolume(BGM_STAGESELECT, 70.0f);
	CSound::SetVolume(BGM_STAGE1, 70.0f);
	CSound::SetVolume(BGM_RESULTGAMEOVER	, 100.0f);
	//SE
	CSound::SetVolume(SE_JET							, 100.0f);
	CSound::SetVolume(SE_COLLISION					, 100.0f);
	CSound::SetVolume(SE_HOLD						, 85.0f);
	CSound::SetVolume(SE_SELECTMOVE				, 60.0f);
	CSound::SetVolume(SE_STAGEENTER				, 100.0f);
	CSound::SetVolume(SE_PAUSESELECT			, 100.0f);
	CSound::SetVolume(SE_PAUSEENTER				, 100.0f);
	CSound::SetVolume(SE_STAGEENTERCANCEL	, 100.0f);
	CSound::SetVolume(SE_RESULTENTER			, 100.0f);
	CSound::SetVolume(SE_TITLEPOP					, 10.0f);
	CSound::SetVolume(SE_GOGOGO					, 40.0f);
	CSound::SetVolume(SE_GOSHIP						, 100.0f);
	CSound::SetVolume(SE_WARNING, 100.0f);
}