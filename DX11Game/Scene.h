#pragma once
#include "main.h"
#include "EffectManager.h"

// �萔��`
enum EScene {
	SCENE_NONE = 0,	// ���ݒ�
	SCENE_TITLE,		// �^�C�g�����
	SCENE_SELECT,		//�I�����
	SCENE_GAME,		// �Q�[�����
	SCENE_RESULT,		// ���U���g���

	MAX_SCENE		// �V�[������
};

class Scene {
public:
	// �֐��v���g�^�C�v
	HRESULT Init();				// ������
	void Uninit();					// �I������
	void Update();					// �X�V
	void Draw();					// �`��
private:
	void SetVolume();			//�S���̃T�E���h�̉��ʐݒ�
};

int GetProgress(void);
void AddProgress(void);

HRESULT SetScene(EScene eScene);	// �ؑ�