#pragma once
#include "main.h"
#include"Player.h"
#include "Debly.h"
#include "Rayer.h"
#include "SceneBase.h"
#include "Meteo.h"
#include "Goal.h"
#include "BlackHole.h"



//�ق��̃v���O�����ɏ���n��
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
	HRESULT Init()override;//������
	void Uninit()override; // �I������
	void Update()override; // �X�V
	void Draw()override; // �`��
};