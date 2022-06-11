// ���U���g��� [Result.cpp]
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

// �O���[�o���ϐ�
static bool countUpFlag;//�X�e�[�W���i�s���邩����
static bool clearFlag;//�N���A������
static float ScoreResult;//�v���C�����X�e�[�W�̎c��_�f��
static long prevDW;//�\���L�[��1�t���[���O�̒l

static float cameraTargetZ;//�J�����̒����_Z���W
static bool animFlag;//�A�j���[�V���������Ă��邩


//�N���X
ResultUI result_UI;
Result_Rocket result_Rocket;
Result_Astronaut result_Astronaut;

// ������
HRESULT Result::Init()
{
	HRESULT hr = S_OK;

	ScoreResult = GetPlayer()->GetO2();//�_�f�ʎ擾

	//�X�e�[�W�N���A���X�e�[�W���i��
	if (ScoreResult > 0.0f) {
		if (GetStageNum() == GetProgress() && GetProgress() < 5) {
			AddProgress();//�i�s�x���Z
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

	// BGM�Đ�
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

	//UI�̏�����
	hr = result_UI.Init();
	if (FAILED(hr))
		return hr;

	//���o�p���P�b�g�̏�����
	hr = result_Rocket.Init();
	if (FAILED(hr))
		return hr;

	//���o�p�F����s�m�̏�����
	hr = result_Astronaut.Init();
	if (FAILED(hr))
		return hr;


	return hr;
}

// �I������
void Result::Uninit()
{
	// BGM�Đ���~
	CSound::Stop(BGM_RESULTCLEAR);
	CSound::Stop(BGM_RESULTGAMEOVER);
	
	//���o�p���P�b�g�̏I��
	result_Rocket.Uninit();

	//���o�p�F����s�m�̏I��
	result_Astronaut.Uninit();

	//UI�̏I��
	result_UI.Uninit();

}

// �X�V
void Result::Update()
{
	if (clearFlag) {
		cameraTargetZ = result_Rocket.GetPos().z;
		if (cameraTargetZ <= -180.0f) {
			cameraTargetZ = -180.0f;
			animFlag = false;
		}
		//�J�����̈ʒu�ƒ����_���Z�b�g
		CCamera::Get()->SetPos(0.0f, -30.0f, -220.0f);
		CCamera::Get()->SetTarget(0.0f, 0.0f, cameraTargetZ);
	}
	else {
		//�J�����̈ʒu�ƒ����_���Z�b�g
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

	//�R���g���[���\���{�^���̑���
	static bool PushFlag = false;		//�\���L�[�������ꂽ�t���[���̂ݔ���
	JOYINFOEX* joystate = GetJoyState(0);
	if (joystate->dwPOV != prevDW && prevDW >= 65535) {
		PushFlag = true;
	}
	else {
		PushFlag = false;
	}

	if (!animFlag) {
		//UI�̍X�V
		result_UI.Update(clearFlag);
	}

	//���o�p���P�b�g(�F����s�m)�̍X�V
	if (clearFlag) {
		result_Rocket.Update();
	}
	else {
		result_Astronaut.Update();
	}
}

// �`��
void Result::Draw()
{
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);

	//�w�i�{UI�������ŕ`��
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



	// Z�o�b�t�@�L��(Z�`�F�b�N�L&Z�X�V�L)
	SetZBuffer(true);

	//���o�p���P�b�g�̕`��
	if (!clearFlag) {
		result_Astronaut.Draw();
	}
	result_Rocket.Draw();
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//���u�����h�L��
}