// タイトル画面 [title.cpp]
#include "Select.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"
#include "fade.h"
#include "Game.h"
#include "Select.h"
#include "Sound.h"
#include "debugproc.h"
#include "Star.h"
#include "SelectUI.h"
#include "Scene.h"

// マクロ定義
#define MAX_STAGE 5

// グローバル変数
static int stagenum;
static int animation;	//星が移動するまでの時間（インターバル）
static long prevDW;		//十字キーの1フレーム前の値
static int TriggerKey;	//どちらの矢印を選択したか

//クラス
Star Select_Star;
SelectUI Select_UI;

// 初期化
HRESULT Select::Init()
{
	HRESULT hr = S_OK;
	stagenum = 1;
	animation = 0;

	// メッシュ初期化
	hr = InitMesh();
	if (FAILED(hr))
		return hr;

	//星
	hr = Select_Star.Init();
	if (FAILED(hr))
		return hr;

	//UI
	hr = Select_UI.Init();
	if (FAILED(hr))
		return hr;

	CCamera::Get()->SetPos(0.0f, 20.0f, -250.0f);
	CCamera::Get()->SetTarget(0.0f, -20.0f, 0.0f);


	// BGM再生
	CSound::Play(BGM_STAGESELECT);
	prevDW = GetJoyState(0)->dwPOV;

	return hr;
}

// 終了処理
void Select::Uninit()
{
	// BGM再生停止
	CSound::Stop(BGM_STAGESELECT);

	Select_Star.Uninit();
	Select_UI.Uninit();

	// メッシュ終了処理
	UninitMesh();
}

// 更新
void Select::Update()
{
	PrintDebugProc("volume=%.2f\n", CSound::GetVolume(BGM_STAGESELECT));
	static bool SelectFlag = true;		//星が動いていなかったらtrue->選択できる
	static bool PushFlag = false;		//十字キーが押されたフレームのみ反応
	JOYINFOEX* joystate = GetJoyState(0);
	if (joystate->dwPOV != prevDW && prevDW >= 65535) {
		PushFlag = true;
	}
	else {
		PushFlag = false;
	}
	//キーを押したら星が動く
	if ((PushFlag&&joystate->dwPOV == 27000) && animation == 0) {
		animation--;
		TriggerKey = 0;
		SelectFlag = false;
		PrintDebugProc("push\n");
		CSound::Stop(SE_SELECTMOVE);
		CSound::Play(SE_SELECTMOVE);
	}
	else if ((PushFlag&&joystate->dwPOV == 9000) || GetKeyTrigger(VK_RIGHT) && animation == 0) {
		animation++;
		TriggerKey = 1;
		SelectFlag = false;

		CSound::Stop(SE_SELECTMOVE);
		CSound::Play(SE_SELECTMOVE);
	}


	//星が動いている
	if (animation != 0) {
		if (animation < 0) {
			animation--;
			Select_UI.ArrowAnim(1, animation);
		}
		else {
			animation++;
			Select_UI.ArrowAnim(0, animation);
		}
	}
	//星が動き終わった->ステージ番号変更 + ステージ名のアニメーション(01/23 三輪卓弘)
	if (abs(animation) >= abs(15)) {
		if (animation > 0) {
			stagenum++;
		}
		else {
			stagenum--;
		}
		// ステージ番号がはみ出したとき修正
		if (stagenum < 1) {
			stagenum = MAX_STAGE;
		}
		else if (stagenum > MAX_STAGE) {
			stagenum = 1;
		}
		//ステージ番号が変わったとき、ステージ文字で変えた感を出すアニメーション
		if (stagenum <= GetProgress())
		{
			Select_UI.StageAnimSet();
		}
		SelectFlag = true;
		animation = 0;
	}

	// [Enter]またはスペースキー押下 デバッグのため進行度の影響を受けないように
	if (stagenum <= GetProgress() && SelectFlag && ( GetJoyTrigger(0, XBOXA))) {


		CSound::Stop(SE_STAGEENTER);
		CSound::Play(SE_STAGEENTER);

		Select_UI.ShiningAnimSet();

		// ゲーム画面へ
		StartFadeOut(SCENE_GAME);
		SetStageNo(stagenum);
		return;
	}
	else if (!stagenum <= GetProgress() && SelectFlag && (GetKeyRelease(VK_RETURN) || GetKeyRelease(VK_SPACE) || GetJoyTrigger(0, XBOXA)) && !Select_UI.NoPlayAnimGet())
	{// 選択したステージが未開放だった場合
		//まだ遊べないよの文字を動かす関数を呼び出す
		Select_UI.NoPlayAnimSet();

		CSound::Stop(SE_STAGEENTERCANCEL);
		CSound::Play(SE_STAGEENTERCANCEL);
	}

	Select_UI.AstroFloating(ASTRONAUT, animation, stagenum - 1);

	Select_Star.Update(stagenum, animation);
	Select_UI.Update(stagenum);

	prevDW = joystate->dwPOV;

#if _DEBUG
	PrintDebugProc("stagenum=%d\n", stagenum);
	PrintDebugProc("animation=%d\n", animation);
#endif

}

// 描画
void Select::Draw()
{
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);

	DrawBG("select");

	// Zバッファ有効(Zチェック有&Z更新有)
	SetZBuffer(true);

	//星
	Select_Star.Draw(stagenum - 1);


	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//αブレンド有効
	Select_UI.Draw();
}

//ステージ番号の取得
int GetStageNum() {
	return stagenum;
}

//ステージ番号のセット
void SetStageNum(int stnum) {
	if (stnum >= 6) {
		stnum = 5;
	}
	stagenum = stnum;
}