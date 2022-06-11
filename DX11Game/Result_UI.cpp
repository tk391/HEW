// ���U���g��� [ResultUI.cpp]
#include "Result_UI.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"
#include "fade.h"
#include "debugproc.h"
#include "bg.h"
#include "game.h"
#include "Scene.h"
#include "Select.h"
#include "Sound.h"

// �}�N����`
#define	STAR_ALPHA		(0.01f)		// �����D�F�����F�ɂȂ鑬�x
#define STAR_INTERVAL	(0.005f)		// ���̃A�j���[�V�������I����āA���̐��̃A�j���[�V��������������܂ł̊Ԋu
#define BUTTON_SIZE_X	(657.0f)
#define BUTTON_SIZE_Y	(192.0f)
#define HOSEI	(0.5f)
// �摜���
static std::string fileName[]{
	"data/texture/stageName.png",
	"data/texture/stage_clear2.png",
	"data/texture/gameover.png",
	"data/texture/nextbotan2.png",
	"data/texture/retrybotan2.png",
	"data/texture/stageselectbotan2.png",
	"data/texture/starGrey.png",
	"data/texture/star.png",
	"data/texture/bombeMeter.png",
	"data/texture/prottype_white.png",
	"data/texture/bombeEmpty.png"
};

// �񋓌^:�摜���		01/16 �O�֑�O
enum EResult {
	STAGE_NAME = 0,			// �摜:�X�e�[�W��(�X�v���b�h�V�[�g)
	GAME_CLEAR,				// �摜:"�X�e�[�W�N���A"����
	GAME_OVER,				// �摜:"�Q�[���I�[�o�["����
	BUTTON_NEXT,			// �{�^��:���̃X�e�[�W
	BUTTON_RETRY,			// �{�^��:���g���C
	BUTTON_SELECT,			// �{�^��:�X�e�[�W�Z���N�g
	GRAYSTAR,				//�D�F�̐�
	STAR,					//��
	CYLINDER,				//�_�f�{���x
	LIFEGAGE,				//�_�f�Q�[�W
	EMPTY,					//�_�f�{���x�@�i��j
	MAX_RESULT				// ���U���g�摜����
};

//���̏��
struct TGrayStar
{
	XMFLOAT3 pos;
	float Angle;
	float Apha;
};

// �O���[�o���ϐ�
const int max_star = 3;

static float StageNameY;
static long prevDW;//�\���L�[��1�t���[���O�̒l
static int stnum;
static float Life;
static TGrayStar g_Star[max_star];
static float	StarCnt;	//�����F�Â��n�߂�܂ł̎���

// ������
HRESULT ResultUI::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	//�e�N�X�`���ǂݍ���
	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; ++i) {
		hr = CreateTextureFromFile(pDevice, fileName[i].c_str(), &m_pTexture[i]);
		if (FAILED(hr))
			return hr;
	}

	// BGM�Đ�
	//CSound::Play(BGM_Result);


	m_vPos[STAGE_NAME] = XMFLOAT2(0.0f, 216.0f);
	m_vSize[STAGE_NAME] = XMFLOAT2(350.0f, 175.0f);

	m_vPos[GAME_CLEAR] = XMFLOAT2(0.0f, 0.0f);
	m_vSize[GAME_CLEAR] = XMFLOAT2(540.0f, 162.0f);
	m_vPos[GAME_OVER] = XMFLOAT2(0.0f, -128.0f);
	m_vSize[GAME_OVER] = XMFLOAT2(540.0f, 90.0f);
	m_vSize[GRAYSTAR] = XMFLOAT2(100.0f, 100.0f);
	m_vSize[STAR] = m_vSize[GRAYSTAR];


	m_vPos[BUTTON_NEXT] = XMFLOAT2(-200.0f, -240.0f);
	m_vPos[BUTTON_RETRY] = XMFLOAT2(-200.0f, -240.0f);
	m_vPos[BUTTON_SELECT] = XMFLOAT2(200.0f, -240.0f);

	m_vPos[LIFEGAGE] = XMFLOAT2(0.0, -110.0f);
	m_vSize[LIFEGAGE] = XMFLOAT2(500.0f, 50.0f);

	m_vPos[EMPTY] = XMFLOAT2(26.0, -110.0f);
	m_vSize[EMPTY] = XMFLOAT2(740.0f, 158.0f);
	m_vPos[CYLINDER] = XMFLOAT2(0.0f, -110.0f);
	m_vSize[CYLINDER] = XMFLOAT2(523.0f, 56.0f);

	m_vSize[BUTTON_NEXT] = XMFLOAT2(BUTTON_SIZE_X * HOSEI, BUTTON_SIZE_Y * HOSEI);
	m_vSize[BUTTON_RETRY] = XMFLOAT2(BUTTON_SIZE_X * HOSEI, BUTTON_SIZE_Y * HOSEI);
	m_vSize[BUTTON_SELECT] = XMFLOAT2(BUTTON_SIZE_X * HOSEI, BUTTON_SIZE_Y * HOSEI);

	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; i++) {
		m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_vDefaultPos[i] = m_vPos[i];	// ���ꂼ���Pos�̏����ʒu��ۑ�		01/16 �O�֑�O
	}

	stnum = GetStageNum();

	//���\���̏�����	01/24	������n
	g_Star[0].pos = XMFLOAT3(-170.0f, 75.0f, 0.0f);
	g_Star[1].pos = XMFLOAT3(0.0f, 90.0f, 0.0f);
	g_Star[2].pos = XMFLOAT3(170.0f, 75.0f, 0.0f);

	g_Star[0].Angle = 15.0f;
	g_Star[1].Angle = 0.0f;
	g_Star[2].Angle = -15.0f;

	for (int i = 0; i < max_star; i++) {
		g_Star[i].Apha = 0.0f;
	}

	StarCnt = 0.0f;
	Life = GetPlayer()->GetO2();//�_�f�ʎ擾
	//Life = 50.0f;//�_�f�ʎ擾
	return hr;
}

// �I������
void ResultUI::Uninit() {
	//�e�N�X�`�����
	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; i++) {
		SAFE_RELEASE(m_pTexture[i]);
	}
}

// �X�V
void ResultUI::Update(bool countUp)
{
	static int g_count = 0;

	//�_�f�Q�[�W�̑傫���ݒ�
	m_vSize[LIFEGAGE] = XMFLOAT2(Life*5.0f, 50.0f);
	m_vPos[LIFEGAGE].x = -250.0f + Life * 2.5f;

	static bool PushFlag = false;		//�\���L�[�������ꂽ�t���[���̂ݔ���
	JOYINFOEX* joystate = GetJoyState(0);
	if (joystate->dwPOV != prevDW && prevDW >= 65535) {
		PushFlag = true;
	}
	else {
		PushFlag = false;
	}

	//�X�e�[�W�Z��\�L���邽�߂�1���炷
	StageNameY = 1.0f / 5.0f * (stnum - 1);

	// check��bool�� �悭�Ȃ���������ǂ��܂�		01/18 �O�֑�O
	static bool check = 0;
	static bool nOldCheck = 0;	// 1�t���[���O��check			01/16 �O�֑�O
	static int count = 0;

	count += 3;

	//�J�[�\�����ǂ���ɂ����Ă��A���E�L�[�������ł����Е��Ɉڂ�悤��	01/18 �O�֑�O
	if ((PushFlag&&joystate->dwPOV == 9000)) { check = !check; }
	if ((PushFlag&&joystate->dwPOV == 27000)) { check = !check; }
	//if (check >= 2) { check = 1; }
	//if (check <  0) { check = 0; }

	// �I�𒆂łȂ��{�^����S�ď����F��
	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; i++) {
		m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	if (check == 0) {
		//m_vPos[3].y = -240.0f + SinDeg(count) * 3;	//�I�𒆂̃{�^���̐F��ς��鏈���ƈꊇ�����������߁AActionButton�֐���		01/16 �O�֑�O
		//m_vPos[4].y = -240.0f + SinDeg(count) * 3;
		ActionButton(BUTTON_NEXT, count);
		ActionButton(BUTTON_RETRY, count);
		// [Enter]�܂��̓X�y�[�X�L�[����
		if (GetJoyTrigger(0, XBOXA)) {
			CSound::Stop(SE_RESULTENTER);
			CSound::Play(SE_RESULTENTER);
			if (countUp) {
				SetStageNum(stnum + 1);
			}
			else {
				SetStageNum(stnum);
			}
			StartFadeOut(SCENE_GAME);
			return;
		}
	}
	else {
		ActionButton(BUTTON_SELECT, count);
		// [Enter]�܂��̓X�y�[�X�L�[����
		if (GetJoyTrigger(0, XBOXA)) {
			CSound::Stop(SE_RESULTENTER);
			CSound::Play(SE_RESULTENTER);
			StartFadeOut(SCENE_SELECT);
			return;
		}
	}
	prevDW = joystate->dwPOV;
	//	PrintDebugProc("pos={%.2f,%.2f},size={%.2f,%.2f}\n", m_vPos[check].x, m_vPos[check].y, m_vSize[check].x, m_vSize[check].y);

	if (nOldCheck != check)	//check��1�t���[���O�̏��ƈႤ�ꍇ			01/16 �O�֑�O
	{
		for (int i = 0; i <= BUTTON_SELECT; i++) {
			count = 0.0f;					//�h���������
			m_vPos[i] = m_vDefaultPos[i];	//�S�Ă�UI�������l��

			CSound::Stop(SE_RESULTMOVE);
			CSound::Play(SE_RESULTMOVE);
		}
	}
	nOldCheck = check;		//���t���[����check�l���擾				01/16 �O�֑�O
}

// �`��
void ResultUI::Draw(bool clear)
{
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);
	ID3D11DeviceContext* pDC = GetDeviceContext();
	if (clear) {
		//01/24		������n
		SetLife(Life);
		Star(Life);
	}

	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; i++) {
		if (i == GRAYSTAR || i == STAR) { continue; }
		if (clear) {
			if (stnum < 5) {
				if (i == 2 || i == 4) { continue; }
			}
			else {
				if (i == 2 || i == 3) { continue; }
			}
		}
		else {
			if (i == 1 || i == 3) { continue; }
			if (i >= GRAYSTAR) { continue; }
		}
		if (i == 0) {
			SetPolygonFrameSize(1.0f, 0.2f);
			SetPolygonUV(0.0f, StageNameY);
		}
		else {
			SetPolygonFrameSize(1.0f, 1.0f);
			SetPolygonUV(0.0f, 0.0f);
		}
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonColor(m_vColor[i]);
		DrawPolygon(pDC);
	}

	//���`��
	if (clear) {
		for (int i = 0; i < max_star; i++) {
			SetPolygonSize(m_vSize[GRAYSTAR].x, m_vSize[GRAYSTAR].y);
			SetPolygonPos(g_Star[i].pos.x, g_Star[i].pos.y);
			SetPolygonAngle(g_Star[i].Angle);
			SetPolygonAlpha(1.0f);
			SetPolygonTexture(m_pTexture[GRAYSTAR]);
			SetPolygonUV(0.0f, 0.0f);
			DrawPolygon(pDC);

			SetPolygonSize(m_vSize[STAR].x, m_vSize[STAR].y);
			SetPolygonPos(g_Star[i].pos.x, g_Star[i].pos.y);
			SetPolygonAngle(g_Star[i].Angle);
			SetPolygonAlpha(g_Star[i].Apha);
			SetPolygonTexture(m_pTexture[STAR]);
			SetPolygonUV(0.0f, 0.0f);
			DrawPolygon(pDC);
		}
	}
	SetPolygonAngle(0.0f);
	// Z�o�b�t�@�L��(Z�`�F�b�N�L&Z�X�V�L)
	SetZBuffer(true);
}

void ResultUI::ActionButton(int nUInum, int count) {
	m_vPos[nUInum].y = m_vDefaultPos[nUInum].y + SinDeg(count) * 3.0f;
	m_vColor[nUInum] = XMFLOAT4(1.2f, 1.2f, 1.2f, 1.0f);
}

//���̕`��	01/24	������n
void ResultUI::Star(float nLife)
{
	float LifeRem = nLife;
	//const float upAlpha = 0.0005f;
	const float upAlpha = 0.005f;

	//01/31�V������
	//debug���ƒx������release���Ƃ��傤�ǂ���
	if (StarCnt <= 1.0f /*100.0f*/) {
		StarCnt += 0.0015f/* 0.020f*/;
	}
	else if (LifeRem >= 0.0f && g_Star[0].Apha <= 1.0f) {
		if (g_Star[0].Apha == 0.0f)
		{
			CSound::Stop(SE_RESULTSTAR);
			CSound::Play(SE_RESULTSTAR);
		}
		g_Star[0].Apha += upAlpha;
		if (g_Star[0].Apha >= 1.0f)
		{
			StarCnt = 0.0f;
		}
	}
	else if (LifeRem >= 20.0f && g_Star[1].Apha <= 1.0f) {
		if (g_Star[1].Apha == 0.0f)
		{
			CSound::Stop(SE_RESULTSTAR);
			CSound::Play(SE_RESULTSTAR);
		}
		g_Star[1].Apha += upAlpha;
		if (g_Star[1].Apha >= 1.0f)
		{
			StarCnt = 0.0f;
		}
	}
	else if (LifeRem >= 50.0f && g_Star[2].Apha <= 1.0f) {
		if (g_Star[2].Apha == 0.0f)
		{
			CSound::Stop(SE_RESULTSTAR);
			CSound::Play(SE_RESULTSTAR);
		}
		g_Star[2].Apha += upAlpha;
	}
}
//���C�t�o�[�Z�k����	01/24	������n
void ResultUI::SetLife(float nLife)
{
	if (nLife < 0.0f) {
		m_vSize[LIFEGAGE].x = 0.0f;
		return;
	}
	float sabun;
	//�΁���ver	01/18 �O�֑�O
	if (nLife >= 50.0f) {
		m_vColor[LIFEGAGE] = XMFLOAT4(1.0f - ((nLife / 100) - (1.0f - nLife / 100)), 1.0f, 0.0f, 1.0f);
	}
	else {
		m_vColor[LIFEGAGE] = XMFLOAT4(1.0f, nLife / 100 + nLife / 100, 0.0f, 1.0f);
	}
}