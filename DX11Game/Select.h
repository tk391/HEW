#pragma once
#include "main.h"
#include "SceneBase.h"

class Select:public ISceneBase {
public:
	HRESULT Init();	// ������
	void Uninit();		// �I������
	void Update();		// �X�V
	void Draw();		// �`��
};

int GetStageNum();	//���U���g�Ŏg�p
void SetStageNum(int stnum);	//�X�e�[�W���Z�b�g����