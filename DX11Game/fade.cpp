//=============================================================================
// �t�F�[�h�C��/�A�E�g���� [fade.cpp]
//12/11 �V������
//=============================================================================
#include "fade.h"
#include "polygon.h"
//#include "Sound.h"

// �}�N����`
#define FADE_RATE	0.02f	// �t�F�[�h�C��/�A�E�g�̍���

// �O���[�o���ϐ�
static float g_fRed = 0.0f;		// �t�F�[�h�A�E�g�F
static float g_fGreen = 0.0f;
static float g_fBlue = 0.0f;
static float g_fAlpha;	// �s�����x
static EFade g_eFade;	// ���
static EScene g_eNext;	// ���̃V�[��

//=============================================================================
// ������
//=============================================================================
HRESULT InitFade()
{
	g_eFade = FADE_IN;
	g_fAlpha = 1.0f;
	g_eNext = SCENE_TITLE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFade()
{
	g_eFade = FADE_NONE;
	g_fAlpha = 0.0f;
}

//=============================================================================
// �X�V
//=============================================================================
void UpdateFade()
{
	switch (g_eFade) {
	case FADE_NONE:
		break;
	case FADE_OUT:
		g_fAlpha += FADE_RATE;	// �s�����x�𑝂�
		if (g_fAlpha >= 1.0f) {
			// �t�F�[�h�C�������ɐ؂�ւ�
			g_fAlpha = 1.0f;
			g_eFade = FADE_IN;
			// �V�[���ؑ�
			SetScene(g_eNext);
		}
		// �{�����[�����t�F�[�h�A�E�g
		//CSound::SetVolume(1.0f - g_fAlpha);
		break;
	case FADE_IN:
		g_fAlpha -= FADE_RATE;	// �����x�𑝂�
		if (g_fAlpha <= 0.0f) {
			// �t�F�[�h�C�����I������
			g_fAlpha = 0.0f;

			g_eFade = FADE_NONE;
		}
		// �{�����[�����t�F�[�h�C��
		//CSound::SetVolume(1.0f - g_fAlpha);
		break;
	}
}

//=============================================================================
// �`��
//=============================================================================
void DrawFade()
{
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//���u�����h�L��
	// ��ʑS�̂ɔ������̋�`��`��
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonTexture(nullptr);
	SetPolygonColor(g_fRed, g_fGreen, g_fBlue);
	SetPolygonAlpha(g_fAlpha);
	DrawPolygon(GetDeviceContext());
	// ���ɖ߂�
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonAlpha(1.0f);

	SetBlendState(BS_NONE);//���u�����h����
	// Z�o�b�t�@�L��(Z�`�F�b�N�L&Z�X�V�L)
	SetZBuffer(true);
}

//=============================================================================
// �t�F�[�h�A�E�g�J�n
//=============================================================================
void StartFadeOut(EScene eNext)
{
	if (g_eFade != FADE_OUT) {
		g_eFade = FADE_OUT;
		g_fAlpha = 0.0f;
		g_eNext = eNext;
	}
}

//=============================================================================
// ��Ԏ擾
//=============================================================================
EFade GetFade()
{
	return g_eFade;
}

//=============================================================================
// �t�F�[�h�A�E�g�F�ݒ�
//=============================================================================
void SetFadeColor(float fR, float fG, float fB)
{
	g_fRed = fR;
	g_fGreen = fG;
	g_fBlue = fB;
}