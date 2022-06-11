// リザルト画面 [Result.cpp]
#include "Result.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"
#include "fade.h"
#include "debugproc.h"
#include "game.h"
#include "Scene.h"
#include "Select.h"
#include "Sound.h"
#include "Result_UI.h"
#include "Result_Rocket.h"
#include "Result_Astronaut.h"
#include "Camera.h"

// グローバル変数
static bool countUpFlag;//ステージが進行するか判定
static bool clearFlag;//クリアか判定
static float ScoreResult;//プレイしたステージの残り酸素量
static long prevDW;//十字キーの1フレーム前の値

static float cameraTargetZ;//カメラの注視点Z座標
static bool animFlag;//アニメーションをしているか


//クラス
ResultUI result_UI;
Result_Rocket result_Rocket;
Result_Astronaut result_Astronaut;

// 初期化
HRESULT Result::Init()
{
	HRESULT hr = S_OK;

	ScoreResult = GetPlayer()->GetO2();//酸素量取得

	//ステージクリア且つステージが進む
	if (ScoreResult > 0.0f) {
		if (GetStageNum() == GetProgress() && GetProgress() < 5) {
			AddProgress();//進行度加算
			countUpFlag = true;
		}
		else {
			countUpFlag = false;
		}
		clearFlag = true;
	}
	else {
		clearFlag = false;
	}

	animFlag = true;

	// BGM再生
	if (clearFlag)
	{
		CSound::Stop(BGM_RESULTCLEAR);
		CSound::Play(BGM_RESULTCLEAR);
	}
	else
	{
		CSound::Stop(BGM_RESULTGAMEOVER);
		CSound::Play(BGM_RESULTGAMEOVER);
	}

	//UIの初期化
	hr = result_UI.Init();
	if (FAILED(hr))
		return hr;

	//演出用ロケットの初期化
	hr = result_Rocket.Init();
	if (FAILED(hr))
		return hr;

	//演出用宇宙飛行士の初期化
	hr = result_Astronaut.Init();
	if (FAILED(hr))
		return hr;


	return hr;
}

// 終了処理
void Result::Uninit()
{
	// BGM再生停止
	CSound::Stop(BGM_RESULTCLEAR);
	CSound::Stop(BGM_RESULTGAMEOVER);
	
	//演出用ロケットの終了
	result_Rocket.Uninit();

	//演出用宇宙飛行士の終了
	result_Astronaut.Uninit();

	//UIの終了
	result_UI.Uninit();

}

// 更新
void Result::Update()
{
	if (clearFlag) {
		cameraTargetZ = result_Rocket.GetPos().z;
		if (cameraTargetZ <= -180.0f) {
			cameraTargetZ = -180.0f;
			animFlag = false;
		}
		//カメラの位置と注視点をセット
		CCamera::Get()->SetPos(0.0f, -30.0f, -220.0f);
		CCamera::Get()->SetTarget(0.0f, 0.0f, cameraTargetZ);
	}
	else {
		//カメラの位置と注視点をセット
		CCamera::Get()->SetPos(0.0f, 0.0f, -200.0f);
		CCamera::Get()->SetTarget(0.0f, 0.0f, 0.0f);
		animFlag = false;
	}

	int stnum = GetStageNum();
#if _DEBUG
	PrintDebugProc("cameraTargetZ=%.2f\n", cameraTargetZ);
	PrintDebugProc("StageNum=%d\n", GetStageNum());
	PrintDebugProc("Progress=%d\n", GetProgress());
#endif

	//コントローラ十字ボタンの操作
	static bool PushFlag = false;		//十字キーが押されたフレームのみ反応
	JOYINFOEX* joystate = GetJoyState(0);
	if (joystate->dwPOV != prevDW && prevDW >= 65535) {
		PushFlag = true;
	}
	else {
		PushFlag = false;
	}

	if (!animFlag) {
		//UIの更新
		result_UI.Update(clearFlag);
	}

	//演出用ロケット(宇宙飛行士)の更新
	if (clearFlag) {
		result_Rocket.Update();
	}
	else {
		result_Astronaut.Update();
	}
}

// 描画
void Result::Draw()
{
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);

	//背景＋UIをここで描画
	if (animFlag) {
		DrawBG("game");
	}
	else {
		if (clearFlag) {
			DrawBG("clear");
		}
		else {
			DrawBG("gameover");
		}
		result_UI.Draw(clearFlag);
	}



	// Zバッファ有効(Zチェック有&Z更新有)
	SetZBuffer(true);

	//演出用ロケットの描画
	if (!clearFlag) {
		result_Astronaut.Draw();
	}
	result_Rocket.Draw();
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//αブレンド有効
}