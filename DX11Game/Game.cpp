#include "Game.h"
#include "mesh.h"
#include "bg.h"
#include "Player.h"
#include "Life.h"
#include "fade.h"
#include "input.h"
#include "Debly.h"
#include "Goal.h"
#include "AlertBg.h"
#include "Sound.h"
#include "debugproc.h"
#include "Pause.h"
#include"StartTelop.h"
#include "Select.h"
#include "ControlUI.h"

Player Game_Player;				//プレイヤー
Life Game_Life;						//酸素ボンベ
Debly Game_Debly;				//デブリ
Goal Game_Goal;					//ゴール（ロケット）
Rayer Game_Rayer;				//レイヤー
Meteo Game_Meteo;				//隕石
BlackHole Game_BlackHole;	//ブラックホール
StartTelop Game_StartTelop;	//スタートUI
Alert Game_Alert;					//アラート
ControlUI Game_ControlUI;			//操作UI

static int stageNum;
//グローバル定義
static int g_bPause;



//初期化
HRESULT Game::Init()
{
	HRESULT hr = S_OK;

	stageNum = GetStageNum();

	// メッシュ初期化
	hr = InitMesh();
	if (FAILED(hr))
		return hr;

	// モデル表示初期化
	hr = Game_Player.Init();
	if (FAILED(hr))
		return hr;

	// 障害物初期化
	hr = Game_Debly.Init(stageNum);
	if (FAILED(hr))
		return hr;

	//隕石初期化
	hr = Game_Meteo.Init(stageNum);
	if (FAILED(hr))
		return hr;

	// ゴール初期化
	hr = Game_Goal.Init(stageNum);
	if (FAILED(hr))
		return hr;

	// 背景初期化
	hr = Game_Rayer.Init();
	if (FAILED(hr))
		return hr;

	//画面赤化初期化処理
	hr = Game_Alert.Init();
	if (FAILED(hr))
		return hr;

	//ポーズ処理初期化
	//12/21 佐藤大地
	hr = InitPause();
	if (FAILED(hr))
		return hr;
	g_bPause = false;

	// ライフ初期化
	hr = Game_Life.Init();
	if (FAILED(hr))
		return hr;

	// ブラックホール初期化
	hr = Game_BlackHole.Init(stageNum);
	if (FAILED(hr))
		return hr;

	//スタートテロップ初期化
	hr = Game_StartTelop.Init();
	if (FAILED(hr))
		return hr;

	//操作方法UI初期化
	hr = Game_ControlUI.Init();
	if (FAILED(hr))
		return hr;
	//BGM再生
	CSound::Play(BGM_STAGE1);

	return hr;
}

// 終了処理
void Game::Uninit()
{
	//BGM停止
	CSound::Stop(BGM_STAGE1);

	// 操作方法UI終了処理
	Game_ControlUI.Uninit();

	// ライフ終了処理
	Game_Life.Uninit();

	// 背景終了処理
	Game_Rayer.Uninit();

	//画面赤化終了処理
	Game_Alert.Uninit();

	//ポーズ終了処理
	//12/21 佐藤大地
	UninitPause();

	//隕石終了処理
	Game_Meteo.Uninit();

	// 障害物終了処理
	Game_Debly.Uninit();

	// ゴール終了処理
	Game_Goal.Uninit();

	// モデル表示終了処理
	Game_Player.Uninit();

	Game_StartTelop.Uninit();

	// ブラックホール表示終了処理
	Game_BlackHole.Uninit();
	// メッシュ終了処理
	UninitMesh();

}

// 更新
void Game::Update()
{
if (GetJoyTrigger(0, XBOXOPTION)) {
		//フェードイン/アウト中でなければ
		if (GetFade() == FADE_NONE) {
			//ポーズフラグ切り替え
			g_bPause = !g_bPause;
			//ポーズ画面の決定音再生
			CSound::Stop(SE_PAUSEENTER);
			CSound::Play(SE_PAUSEENTER);
			if (g_bPause) {
				ResetPauseMenu();
			}
		}
	}
	
	//一時停止中の判定
		if (g_bPause) {
		//一時停止更新
		UpdatePause();
	}
	else {

		Game_StartTelop.Update();

		//ライフ更新
		Game_Life.Update();

		// 障害物更新
		Game_Debly.Update();

		//隕石更新
		Game_Meteo.Update();
		// ゴール更新
		Game_Goal.Update();
		Game_Rayer.Update();

		// ブラックホール更新
		Game_BlackHole.Update();
		// モデル更新
		Game_Player.Update();

		//アラート更新
		Game_Alert.Update();

		//操作方法UI更新
		Game_ControlUI.Update();
#if _DEBUG
		PrintDebugProc("stageNum=%d\n", stageNum);
#endif

		if (GetJoyTrigger(0, PS4OPTION) ) {
			//フェードイン/アウト中でなければ
			if (GetFade() == FADE_NONE) {
				//ポーズフラグ切り替え
				g_bPause = !g_bPause;
				//ポーズ画面の決定音再生

				CSound::Stop(SE_PAUSEENTER);
				CSound::Play(SE_PAUSEENTER);
				if (g_bPause) {
					ResetPauseMenu();
				}
			}
		}
	}
	//一時停止中メニューの選択
	if (g_bPause&&GetFade() == FADE_NONE) {
		if (GetJoyTrigger(0, XBOXA) ) {
			switch (GetPauseMenu()) {
			case PAUSE_MENU_CONTINUE:
				//ポーズ画面の決定音再生
				CSound::Stop(SE_PAUSEENTER);
				CSound::Play(SE_PAUSEENTER);
				g_bPause = false;
				break;
			case PAUSE_MENU_RETRY:
				//ステージ決定音再生
				CSound::Stop(SE_STAGEENTER);
				CSound::Play(SE_STAGEENTER);
				StartFadeOut(SCENE_GAME);
				break;
			case PAUSE_MENU_QUIT:
				//ステージ決定音再生
				CSound::Stop(SE_STAGEENTER);
				CSound::Play(SE_STAGEENTER);
				StartFadeOut(SCENE_SELECT);
				break;
			}
			if (GetPauseMenu() != PAUSE_MENU_CONTINUE) {
				//エフェクトの強制終了
				EffectManager::Get()->End("all");
			}
		}
	}
}

// 描画
void Game::Draw()
{
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);
	// 背景描画
	DrawBG("game");
	Game_Rayer.Draw();

	// Zバッファ有効(Zチェック有&Z更新有)
	SetZBuffer(true);

	// フィールド描画
	//DrawMeshField();

	// 障害物描画
	Game_Debly.Draw();
	//隕石描画
	Game_Meteo.Draw();

	// ゴール描画
	Game_Goal.Draw();

	//ブラックホール描画
	Game_BlackHole.Draw();
	// プレイヤー描画
	Game_Player.Draw();

	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//αブレンド有効
	//ここでUI描画

	//ライフ描画
	Game_Life.Draw();
	//DrawTankEffect();

	//スタートテロップ描画
	Game_StartTelop.Draw();

	//画面赤化描画
	//12/17  佐藤大地
	Game_Alert.Draw();

	//操作方法UI描画
	Game_ControlUI.Draw();
	//一時停止描画	
	if (g_bPause) {
		DrawPause();
	}
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//αブレンド有効
}

//ほかのオブジェクトに渡す
//プレイヤークラス
Player* GetPlayer()
{
	return &Game_Player;
}
//ライフクラス
Life* GetLife()
{
	return &Game_Life;
}
//デブリクラス
Debly* GetDebly() {
	return &Game_Debly;
}

//レイヤークラス
Rayer* GetRayer() {
	return &Game_Rayer;
}

//メテオクラス
Meteo* GetMeteo() {
	return &Game_Meteo;
}

//ブラックホールクラス
BlackHole* GetBlackHole() {
	return &Game_BlackHole;
}

Goal* GetGoal() {
	return &Game_Goal;
}

StartTelop* GetStartTelop() {
	return &Game_StartTelop;
}

ControlUI* GetControlUI() {
	return &Game_ControlUI;
}

void SetStageNo(int StageNum) {
	stageNum = StageNum;
}