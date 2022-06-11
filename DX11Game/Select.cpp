// �^�C�g����� [title.cpp]
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

// �}�N����`
#define MAX_STAGE 5

// �O���[�o���ϐ�
static int stagenum;
static int animation;	//�����ړ�����܂ł̎��ԁi�C���^�[�o���j
static long prevDW;		//�\���L�[��1�t���[���O�̒l
static int TriggerKey;	//�ǂ���̖���I��������

//�N���X
Star Select_Star;
SelectUI Select_UI;

// ������
HRESULT Select::Init()
{
	HRESULT hr = S_OK;
	stagenum = 1;
	animation = 0;

	// ���b�V��������
	hr = InitMesh();
	if (FAILED(hr))
		return hr;

	//��
	hr = Select_Star.Init();
	if (FAILED(hr))
		return hr;

	//UI
	hr = Select_UI.Init();
	if (FAILED(hr))
		return hr;

	CCamera::Get()->SetPos(0.0f, 20.0f, -250.0f);
	CCamera::Get()->SetTarget(0.0f, -20.0f, 0.0f);


	// BGM�Đ�
	CSound::Play(BGM_STAGESELECT);
	prevDW = GetJoyState(0)->dwPOV;

	return hr;
}

// �I������
void Select::Uninit()
{
	// BGM�Đ���~
	CSound::Stop(BGM_STAGESELECT);

	Select_Star.Uninit();
	Select_UI.Uninit();

	// ���b�V���I������
	UninitMesh();
}

// �X�V
void Select::Update()
{
	PrintDebugProc("volume=%.2f\n", CSound::GetVolume(BGM_STAGESELECT));
	static bool SelectFlag = true;		//���������Ă��Ȃ�������true->�I���ł���
	static bool PushFlag = false;		//�\���L�[�������ꂽ�t���[���̂ݔ���
	JOYINFOEX* joystate = GetJoyState(0);
	if (joystate->dwPOV != prevDW && prevDW >= 65535) {
		PushFlag = true;
	}
	else {
		PushFlag = false;
	}
	//�L�[���������琯������
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


	//���������Ă���
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
	//���������I�����->�X�e�[�W�ԍ��ύX + �X�e�[�W���̃A�j���[�V����(01/23 �O�֑�O)
	if (abs(animation) >= abs(15)) {
		if (animation > 0) {
			stagenum++;
		}
		else {
			stagenum--;
		}
		// �X�e�[�W�ԍ����͂ݏo�����Ƃ��C��
		if (stagenum < 1) {
			stagenum = MAX_STAGE;
		}
		else if (stagenum > MAX_STAGE) {
			stagenum = 1;
		}
		//�X�e�[�W�ԍ����ς�����Ƃ��A�X�e�[�W�����ŕς��������o���A�j���[�V����
		if (stagenum <= GetProgress())
		{
			Select_UI.StageAnimSet();
		}
		SelectFlag = true;
		animation = 0;
	}

	// [Enter]�܂��̓X�y�[�X�L�[���� �f�o�b�O�̂��ߐi�s�x�̉e�����󂯂Ȃ��悤��
	if (stagenum <= GetProgress() && SelectFlag && ( GetJoyTrigger(0, XBOXA))) {


		CSound::Stop(SE_STAGEENTER);
		CSound::Play(SE_STAGEENTER);

		Select_UI.ShiningAnimSet();

		// �Q�[����ʂ�
		StartFadeOut(SCENE_GAME);
		SetStageNo(stagenum);
		return;
	}
	else if (!stagenum <= GetProgress() && SelectFlag && (GetKeyRelease(VK_RETURN) || GetKeyRelease(VK_SPACE) || GetJoyTrigger(0, XBOXA)) && !Select_UI.NoPlayAnimGet())
	{// �I�������X�e�[�W�����J���������ꍇ
		//�܂��V�ׂȂ���̕����𓮂����֐����Ăяo��
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

// �`��
void Select::Draw()
{
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);

	DrawBG("select");

	// Z�o�b�t�@�L��(Z�`�F�b�N�L&Z�X�V�L)
	SetZBuffer(true);

	//��
	Select_Star.Draw(stagenum - 1);


	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//���u�����h�L��
	Select_UI.Draw();
}

//�X�e�[�W�ԍ��̎擾
int GetStageNum() {
	return stagenum;
}

//�X�e�[�W�ԍ��̃Z�b�g
void SetStageNum(int stnum) {
	if (stnum >= 6) {
		stnum = 5;
	}
	stagenum = stnum;
}