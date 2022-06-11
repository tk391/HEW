#pragma once
#include "main.h"
#include"Player.h"
#include "Debly.h"
#include "Rayer.h"
#include "SceneBase.h"
#include "Meteo.h"
#include "Goal.h"
#include "BlackHole.h"



//ほかのプログラムに情報を渡す
Life* GetLife();
Player* GetPlayer();
Debly* GetDebly();
Rayer* GetRayer();
Meteo* GetMeteo();
Goal* GetGoal();
BlackHole* GetBlackHole();
void SetStageNo(int StageNum);

class Game :public ISceneBase
{
public:
	HRESULT Init()override;//初期化
	void Uninit()override; // 終了処理
	void Update()override; // 更新
	void Draw()override; // 描画
};