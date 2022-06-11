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

Player Game_Player;				//�v���C���[
Life Game_Life;						//�_�f�{���x
Debly Game_Debly;				//�f�u��
Goal Game_Goal;					//�S�[���i���P�b�g�j
Rayer Game_Rayer;				//���C���[
Meteo Game_Meteo;				//覐�
BlackHole Game_BlackHole;	//�u���b�N�z�[��
StartTelop Game_StartTelop;	//�X�^�[�gUI
Alert Game_Alert;					//�A���[�g
ControlUI Game_ControlUI;			//����UI

static int stageNum;
//�O���[�o����`
static int g_bPause;



//������
HRESULT Game::Init()
{
	HRESULT hr = S_OK;

	stageNum = GetStageNum();

	// ���b�V��������
	hr = InitMesh();
	if (FAILED(hr))
		return hr;

	// ���f���\��������
	hr = Game_Player.Init();
	if (FAILED(hr))
		return hr;

	// ��Q��������
	hr = Game_Debly.Init(stageNum);
	if (FAILED(hr))
		return hr;

	//覐Ώ�����
	hr = Game_Meteo.Init(stageNum);
	if (FAILED(hr))
		return hr;

	// �S�[��������
	hr = Game_Goal.Init(stageNum);
	if (FAILED(hr))
		return hr;

	// �w�i������
	hr = Game_Rayer.Init();
	if (FAILED(hr))
		return hr;

	//��ʐԉ�����������
	hr = Game_Alert.Init();
	if (FAILED(hr))
		return hr;

	//�|�[�Y����������
	//12/21 ������n
	hr = InitPause();
	if (FAILED(hr))
		return hr;
	g_bPause = false;

	// ���C�t������
	hr = Game_Life.Init();
	if (FAILED(hr))
		return hr;

	// �u���b�N�z�[��������
	hr = Game_BlackHole.Init(stageNum);
	if (FAILED(hr))
		return hr;

	//�X�^�[�g�e���b�v������
	hr = Game_StartTelop.Init();
	if (FAILED(hr))
		return hr;

	//������@UI������
	hr = Game_ControlUI.Init();
	if (FAILED(hr))
		return hr;
	//BGM�Đ�
	CSound::Play(BGM_STAGE1);

	return hr;
}

// �I������
void Game::Uninit()
{
	//BGM��~
	CSound::Stop(BGM_STAGE1);

	// ������@UI�I������
	Game_ControlUI.Uninit();

	// ���C�t�I������
	Game_Life.Uninit();

	// �w�i�I������
	Game_Rayer.Uninit();

	//��ʐԉ��I������
	Game_Alert.Uninit();

	//�|�[�Y�I������
	//12/21 ������n
	UninitPause();

	//覐ΏI������
	Game_Meteo.Uninit();

	// ��Q���I������
	Game_Debly.Uninit();

	// �S�[���I������
	Game_Goal.Uninit();

	// ���f���\���I������
	Game_Player.Uninit();

	Game_StartTelop.Uninit();

	// �u���b�N�z�[���\���I������
	Game_BlackHole.Uninit();
	// ���b�V���I������
	UninitMesh();

}

// �X�V
void Game::Update()
{
if (GetJoyTrigger(0, XBOXOPTION)) {
		//�t�F�[�h�C��/�A�E�g���łȂ����
		if (GetFade() == FADE_NONE) {
			//�|�[�Y�t���O�؂�ւ�
			g_bPause = !g_bPause;
			//�|�[�Y��ʂ̌��艹�Đ�
			CSound::Stop(SE_PAUSEENTER);
			CSound::Play(SE_PAUSEENTER);
			if (g_bPause) {
				ResetPauseMenu();
			}
		}
	}
	
	//�ꎞ��~���̔���
		if (g_bPause) {
		//�ꎞ��~�X�V
		UpdatePause();
	}
	else {

		Game_StartTelop.Update();

		//���C�t�X�V
		Game_Life.Update();

		// ��Q���X�V
		Game_Debly.Update();

		//覐΍X�V
		Game_Meteo.Update();
		// �S�[���X�V
		Game_Goal.Update();
		Game_Rayer.Update();

		// �u���b�N�z�[���X�V
		Game_BlackHole.Update();
		// ���f���X�V
		Game_Player.Update();

		//�A���[�g�X�V
		Game_Alert.Update();

		//������@UI�X�V
		Game_ControlUI.Update();
#if _DEBUG
		PrintDebugProc("stageNum=%d\n", stageNum);
#endif

		if (GetJoyTrigger(0, PS4OPTION) ) {
			//�t�F�[�h�C��/�A�E�g���łȂ����
			if (GetFade() == FADE_NONE) {
				//�|�[�Y�t���O�؂�ւ�
				g_bPause = !g_bPause;
				//�|�[�Y��ʂ̌��艹�Đ�

				CSound::Stop(SE_PAUSEENTER);
				CSound::Play(SE_PAUSEENTER);
				if (g_bPause) {
					ResetPauseMenu();
				}
			}
		}
	}
	//�ꎞ��~�����j���[�̑I��
	if (g_bPause&&GetFade() == FADE_NONE) {
		if (GetJoyTrigger(0, XBOXA) ) {
			switch (GetPauseMenu()) {
			case PAUSE_MENU_CONTINUE:
				//�|�[�Y��ʂ̌��艹�Đ�
				CSound::Stop(SE_PAUSEENTER);
				CSound::Play(SE_PAUSEENTER);
				g_bPause = false;
				break;
			case PAUSE_MENU_RETRY:
				//�X�e�[�W���艹�Đ�
				CSound::Stop(SE_STAGEENTER);
				CSound::Play(SE_STAGEENTER);
				StartFadeOut(SCENE_GAME);
				break;
			case PAUSE_MENU_QUIT:
				//�X�e�[�W���艹�Đ�
				CSound::Stop(SE_STAGEENTER);
				CSound::Play(SE_STAGEENTER);
				StartFadeOut(SCENE_SELECT);
				break;
			}
			if (GetPauseMenu() != PAUSE_MENU_CONTINUE) {
				//�G�t�F�N�g�̋����I��
				EffectManager::Get()->End("all");
			}
		}
	}
}

// �`��
void Game::Draw()
{
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);
	// �w�i�`��
	DrawBG("game");
	Game_Rayer.Draw();

	// Z�o�b�t�@�L��(Z�`�F�b�N�L&Z�X�V�L)
	SetZBuffer(true);

	// �t�B�[���h�`��
	//DrawMeshField();

	// ��Q���`��
	Game_Debly.Draw();
	//覐Ε`��
	Game_Meteo.Draw();

	// �S�[���`��
	Game_Goal.Draw();

	//�u���b�N�z�[���`��
	Game_BlackHole.Draw();
	// �v���C���[�`��
	Game_Player.Draw();

	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//���u�����h�L��
	//������UI�`��

	//���C�t�`��
	Game_Life.Draw();
	//DrawTankEffect();

	//�X�^�[�g�e���b�v�`��
	Game_StartTelop.Draw();

	//��ʐԉ��`��
	//12/17  ������n
	Game_Alert.Draw();

	//������@UI�`��
	Game_ControlUI.Draw();
	//�ꎞ��~�`��	
	if (g_bPause) {
		DrawPause();
	}
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//���u�����h�L��
}

//�ق��̃I�u�W�F�N�g�ɓn��
//�v���C���[�N���X
Player* GetPlayer()
{
	return &Game_Player;
}
//���C�t�N���X
Life* GetLife()
{
	return &Game_Life;
}
//�f�u���N���X
Debly* GetDebly() {
	return &Game_Debly;
}

//���C���[�N���X
Rayer* GetRayer() {
	return &Game_Rayer;
}

//���e�I�N���X
Meteo* GetMeteo() {
	return &Game_Meteo;
}

//�u���b�N�z�[���N���X
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