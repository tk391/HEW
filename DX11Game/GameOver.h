#pragma once
#include "main.h"
#include "SceneBase.h"

class GameOver :public ISceneBase {
public:
	HRESULT Init();	// ������
	void Uninit();		// �I������
	void Update();		// �X�V
	void Draw();		// �`��
};