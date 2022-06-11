#include "Scene.h"
#include "Fade.h"
#include "Title.h"
#include "Select.h"
#include "Game.h"
#include "Result.h"
#include "SceneBase.h"
#include "bg.h"
#include "Sound.h"

// �O���[�o���ϐ�
static EScene g_eScene = SCENE_NONE;

static int g_nProgress;//�i�s�x

//�N���X�錾
ISceneBase* SceneBase[MAX_SCENE];

HRESULT Scene::Init() {
	HRESULT hr = S_OK;

	SceneBase[SCENE_NONE] = nullptr;
	SceneBase[SCENE_TITLE] = new Title;
	SceneBase[SCENE_SELECT] = new Select;
	SceneBase[SCENE_GAME] = new Game;
	SceneBase[SCENE_RESULT] = new Result;

	//�V���O���g���̃I�u�W�F�N�g���쐬
	EffectManager::Init();		//�S�ẴG�t�F�N�g�̏�����

	InitBG();						//�w�i������
	InitFade();
	g_nProgress = 5;//�X�e�[�W�P����v���C�\
	hr = SetScene(SCENE_TITLE);	// �ŏ��̓^�C�g����ʂ�
	
	//���ʂ̐ݒ�
	SetVolume();

	return hr;
}
void Scene::Uninit() {
	SetScene(SCENE_NONE);	// ���݂̉�ʂ��I��
	UninitFade();
	UninitBG();		//�w�i�I��
	EffectManager::Uninit();//�S�ẴG�t�F�N�g�̍폜

	for (int i = 0; i < MAX_SCENE; ++i) {
		delete SceneBase[i];
		SceneBase[i] = nullptr;
	}
}

void Scene::Update() {
	//�V�[�����Ȃ������珈�������Ȃ�
	if (g_eScene == SCENE_NONE) {
		return;
	}

	//�V�[�����ƂɍX�V
	SceneBase[g_eScene]->Update();

	UpdateFade();		// �t�F�[�h�C��/�A�E�g�X�V
}

void Scene::Draw() {
	//�V�[�����Ȃ������珈�������Ȃ�
	if (g_eScene == SCENE_NONE) {
		return;
	}
	//�V�[�����Ƃɕ`��
	SceneBase[g_eScene]->Draw();
	DrawFade();			// �t�F�[�h�C��/�A�E�g�`��
}

HRESULT SetScene(EScene eScene) {
	HRESULT hr = S_OK;
	//�V�[�����Ȃ������珈�������Ȃ�
	if (g_eScene != SCENE_NONE) {
		// ���݂̉�ʂ��I��
		//�V�[������
		SceneBase[g_eScene]->Uninit();
	}
	// ��ʂ����ւ�
	g_eScene = eScene;

	//�V�[�����Ȃ������珈�������Ȃ�
	if (g_eScene != SCENE_NONE) {
		// ���̉�ʂ�������
		SceneBase[g_eScene]->Init();
		return hr;
	}
	return hr;
}

//�i�s�x�̊Ǘ�
int GetProgress(void) {
	return g_nProgress;
}
void AddProgress(void) {
	g_nProgress++;
}



//�S���̉��̉��ʐݒ�
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