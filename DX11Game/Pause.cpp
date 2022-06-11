//�|�[�Y�@�\�̎���
//12/21  ������n
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "polygon.h"
#include "Texture.h"
#include "sound.h"

// �}�N����`
#define	NUM_PAUSE_MENU		(3)			// �|�[�Y���j���[��

#define	CONTINUE_WIDTH		(580.0f)	// �R���e�B�j���[��
#define	CONTINUE_HEIGHT		(84.0f)		// �R���e�B�j���[����
#define	RETRY_WIDTH			(337.0f)	// ���g���C��
#define	RETRY_HEIGHT		(88.0f)		// ���g���C����
#define	STAGESELECT_WIDTH	(586.0f)	// �X�e�[�W�Z���N�g��
#define	STAGESELECT_HEIGHT	(75.0f)		// �X�e�[�W�Z���N�g����
#define	NAME_CORRECTION		* 0.6f		// �����T�C�Y�␳

#define	PAUSE_MENU_POS_X	(0.0f)		// �|�[�Y���j���[�ʒu(X���W)
#define	PAUSE_MENU_POS_Y	(PAUSE_MENU_INTERVAL)	// �|�[�Y���j���[�ʒu(Y���W)
#define	PAUSE_MENU_INTERVAL	(90.0f)	// �|�[�Y���j���[�Ԋu

#define	PLATE_WIDTH			(400.0f * 1.3f)	// �v���[�g�̕�
#define	PLATE_HEIGHT		(400.0f * 1.1f)	// �v���[�g�̕�
#define	PLATE_POS_X			(0.0f)		// �v���[�g�̈ʒu(X���W)
#define	PLATE_POS_Y			(0.0f)		// �v���[�g�̈ʒu(Y���W)

#define PLATE_TEXTURE		L"data/texture/pose_screen.png"
//�|�[�Y��ʔw�i�@�@����͉��ł�

// �O���[�o���ϐ�
static long prevDW;//�\���L�[��1�t���[���O�̒l
static ID3D11ShaderResourceView*	g_pTextures[3] = { nullptr };	// �e�N�X�`���ւ̃|�C���^
static ID3D11ShaderResourceView*	g_pPlateTexture;			//�e�N�X�`���w�i�ւ̃|�C���^

static PAUSE_MENU g_nSelectMenu = PAUSE_MENU_CONTINUE;		// �I�𒆂̃��j���[No.
static float g_fCurve = 0.0f;
static float g_fCol = 0.0f;
static XMFLOAT2 g_vSize[NUM_PAUSE_MENU];

static LPCWSTR c_aFileNamePauseMenu[NUM_PAUSE_MENU] =
{
	L"data/texture/continue.png",	// �R���e�B�j���[
	L"data/texture/retry.png",	// ���g���C
	L"data/texture/stageselect.png",	// �N�C�b�g
	//���̉摜�ł�
};


// ����������
HRESULT InitPause(void)
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	for (int nCntPauseMenu = 0; nCntPauseMenu < NUM_PAUSE_MENU; ++nCntPauseMenu)
	{
		// �e�N�X�`���̓ǂݍ���
		hr = CreateTextureFromFile(pDevice,									// �f�o�C�X�ւ̃|�C���^
			c_aFileNamePauseMenu[nCntPauseMenu],	// �t�@�C���̖��O
			&g_pTextures[nCntPauseMenu]);			// �ǂݍ��ރ������[
	}

	hr = CreateTextureFromFile(pDevice, PLATE_TEXTURE, &g_pPlateTexture);

	g_nSelectMenu = PAUSE_MENU_CONTINUE;
	g_fCurve = 0.0f;
	g_vSize[0] = XMFLOAT2(CONTINUE_WIDTH NAME_CORRECTION, CONTINUE_HEIGHT NAME_CORRECTION);
	g_vSize[1] = XMFLOAT2(RETRY_WIDTH NAME_CORRECTION, RETRY_HEIGHT NAME_CORRECTION);
	g_vSize[2] = XMFLOAT2(STAGESELECT_WIDTH NAME_CORRECTION, STAGESELECT_HEIGHT NAME_CORRECTION);



	return hr;
}

// �I������
void UninitPause(void)
{
	// �e�N�X�`���̊J��
	for (int nCntPauseMenu = 0; nCntPauseMenu < NUM_PAUSE_MENU; ++nCntPauseMenu)
	{
		SAFE_RELEASE(g_pTextures[nCntPauseMenu]);
	}
	SAFE_RELEASE(g_pPlateTexture);
}

// �X�V����
void UpdatePause(void)
{
	//�\���L�[����
	static bool PushFlag = false;		//�\���L�[�������ꂽ�t���[���̂ݔ���
	JOYINFOEX* joystate = GetJoyState(0);
	if (joystate->dwPOV != prevDW && prevDW >= 65535) {
		PushFlag = true;
	}
	else {
		PushFlag = false;
	}

	if ((PushFlag&&joystate->dwPOV == 0))
	{

		CSound::Stop(SE_PAUSESELECT);
		CSound::Play(SE_PAUSESELECT);
		g_nSelectMenu = (PAUSE_MENU)((g_nSelectMenu + NUM_PAUSE_MENU - 1) % NUM_PAUSE_MENU);
		SetPauseMenu();
	}
	else if ((PushFlag&&joystate->dwPOV == 18000))
	{

		CSound::Stop(SE_PAUSESELECT);
		CSound::Play(SE_PAUSESELECT);
		g_nSelectMenu = (PAUSE_MENU)((g_nSelectMenu + 1) % NUM_PAUSE_MENU);
		SetPauseMenu();
	}
	prevDW = joystate->dwPOV;
}

// �`�揈��
void DrawPause(void)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	SetPolygonTexture(g_pPlateTexture);
	SetPolygonSize(PLATE_WIDTH, PLATE_HEIGHT);
	SetPolygonPos(PLATE_POS_X, PLATE_POS_Y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonAlpha(1.0f);
	DrawPolygon(pDeviceContext);

	for (int nCntPauseMenu = 0; nCntPauseMenu < NUM_PAUSE_MENU; ++nCntPauseMenu)
	{
		SetPolygonSize(g_vSize[nCntPauseMenu]);
		SetPolygonPos(PAUSE_MENU_POS_X, PAUSE_MENU_POS_Y - nCntPauseMenu * PAUSE_MENU_INTERVAL);

		if (nCntPauseMenu == g_nSelectMenu)
		{
			SetPolygonColor(1.0f, 1.0f, 1.0f);
		}
		else
		{
			SetPolygonColor(0.3f, 0.3f, 0.3f);
		}
		// �e�N�X�`���̐ݒ�
		SetPolygonTexture(g_pTextures[nCntPauseMenu]);
		// �|���S���̕`��
		DrawPolygon(pDeviceContext);
	}

	SetPolygonColor(1.0f, 1.0f, 1.0f);
}

// �|�[�Y���j���[�̐ݒ�
void SetPauseMenu(void)
{
	g_fCurve = 0.0f;
}

// �|�[�Y���j���[�̎擾
PAUSE_MENU GetPauseMenu(void)
{
	return g_nSelectMenu;
}

// �|�[�Y���j���[�̃��Z�b�g
void ResetPauseMenu(void)
{
	g_nSelectMenu = PAUSE_MENU_CONTINUE;
	SetPauseMenu();
}
//�����܂�