// �t�F�[�h�C��/�A�E�g���� [fade.h]
#pragma once
#include "scene.h"

// ���
enum EFade {
	FADE_NONE = 0,	// �������Ă��Ȃ�
	FADE_IN,		// �t�F�[�h�C��������
	FADE_OUT,		// �t�F�[�h�A�E�g������

	MAX_FADE
};

// �֐��v���g�^�C�v
HRESULT InitFade();		// ������
void UninitFade();		// �I������
void UpdateFade();		// �X�V
void DrawFade();		// �`��

void StartFadeOut(EScene eNext);
						// �t�F�[�h�A�E�g�J�n
EFade GetFade();		// ��Ԏ擾
void SetFadeColor(float fR, float fG, float fB);
						// �t�F�[�h�A�E�g�F�ݒ�
