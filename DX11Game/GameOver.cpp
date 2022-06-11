#include"GameOver.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"
#include "fade.h"
#include "debugproc.h"
//#include "Sound.h"

// �}�N����`
#define BG_POS_X		0.0f
#define BG_POS_Y		0.0f
#define BG_WIDTH		SCREEN_WIDTH
#define BG_HEIGHT		SCREEN_HEIGHT

// �O���[�o���ϐ�
static LPCWSTR g_pszTexFName = L"data/texture/prottype/NULL";
static ID3D11ShaderResourceView* g_pTexture;

// ������
HRESULT GameOver::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	// �e�N�X�`���Ǎ�

	hr = CreateTextureFromFile(pDevice, g_pszTexFName, &g_pTexture);
	if (FAILED(hr)) {
		return hr;
	}

	// BGM�Đ�
	//CSound::Play(BGM_GameOver);

	return hr;
}

// �I������
void GameOver::Uninit()
{
	// BGM�Đ���~
	//CSound::Stop(BGM_GameOver);

	// �e�N�X�`�����
	SAFE_RELEASE(g_pTexture);

}

// �X�V
void GameOver::Update()
{
	// [Enter]�܂��̓X�y�[�X�L�[����
	if (GetKeyRelease(VK_RETURN) || GetKeyRelease(VK_SPACE) || GetJoyTrigger(0, PS4CIRCLE)) {
		StartFadeOut(SCENE_RESULT);
		return;
	}
}

// �`��
void GameOver::Draw()
{
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);
	ID3D11DeviceContext* pDC = GetDeviceContext();

	SetPolygonSize(BG_WIDTH, BG_HEIGHT);
	SetPolygonPos(BG_POS_X, BG_POS_Y);
	SetPolygonTexture(g_pTexture);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

	// Z�o�b�t�@�L��(Z�`�F�b�N�L&Z�X�V�L)
	SetZBuffer(true);
}