// �^�C�g����� [title.cpp]
#include "Title.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"
#include "fade.h"
#include "debugproc.h"
#include "bg.h"
#include "Sound.h"

//===========================================
// �}�N����`
//===========================================
#define LOGO_PARPORSE
#define LOGO_SIZE_X		1260.0f	//���S�̉摜�T�C�YX	01/15 �O�֑�O
#define LOGO_SIZE_Y		600.0f	//���S�̉摜�T�C�YY	01/15 �O�֑�O

#define PRESS_SIZE_X    539.0f * 0.75f // �Ղꂷ�����񂽁[�@X
#define PRESS_SIZE_Y    94.0f  * 0.75f // �Ղꂷ�����񂽁[�@Y

#define PUSHA_TIMER 60.0f
#define BLINK_TIMER 60
#define BLINK_START_TIMER 5

// �摜���
static std::string fileName_Title[]{
	"data/texture/astronaut.png",
	"data/texture/title.png",
	"data/texture/pushA.png",
	"data/texture/pushAShining.png"
};

enum eTITLE {
	TITLE_ASTRONAUT = 0,// �F����s�m
	TITLE_LOGO,			// �^�C�g�����S
	TITLE_PUSHA,		// A�����Ă�
	TITLE_SHINING,		// ��������

	MAX_TITLE
};
//�O���[�o���ϐ�
static bool g_bAnimFlag = false;			//�A�j���[�V�����������������𔻒肷��t���O
static bool g_bOldAnimFlag = false;			//1�t���[���O�̓��t���O
static bool g_bFloatingFlag = false;		//�t���O:�F����s�m���ŏ��̕����オ�������������	01/15 �O�֑�O

static float fMoveY = 2.8f;
static bool g_bStart;
static int g_nTitleShiningAnimTime;

static bool g_bPushAAlpha = true;		// PushA�̕s�����x���v���X���邩�}�C�i�X���邩
static float g_fPushATimer = 0.0f;		// PushA�̓_�ŊԊu
static bool g_bPushAFlag = false;			// PushA�����������ǂ���

// ������
HRESULT Title::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`���ǂݍ���
	for (int i = 0; i < MAX_TITLE_TEXUTURE; ++i) {
		hr = CreateTextureFromFile(pDevice, fileName_Title[i].c_str(), &m_pTexture[i]);
		if (FAILED(hr))
			return hr;
	}

	//�����o�ϐ��̏�����
	//�F����s�m
	m_vPos[TITLE_ASTRONAUT] = XMFLOAT2(-370.0f, -40.0f);
	m_vSize[TITLE_ASTRONAUT] = XMFLOAT2(211.11f, 144.44f);
	m_vColor[TITLE_ASTRONAUT].w = 0.0f;

	//���S
	m_vPos[TITLE_LOGO] = XMFLOAT2(0.0f, 0.0f);
	m_vSize[TITLE_LOGO] = XMFLOAT2(LOGO_SIZE_X, 0.0f); //X�T�C�Y���摜�̉����Ɠ������l��	01/15 �O�֑�O
	m_vColor[TITLE_LOGO].w = 1.0f;
	g_bAnimFlag = false;

	// �Ղꂷ�����񂽁[
	m_vPos[TITLE_PUSHA] = XMFLOAT2(0.0f, -250.0f);
	m_vSize[TITLE_PUSHA] = XMFLOAT2(PRESS_SIZE_X, PRESS_SIZE_Y);
	m_vColor[TITLE_PUSHA].w = 0.0f;

	// A�����Ăˁ@����o�[�W���� 01/25 �O�֑�O
	m_vPos[TITLE_SHINING] = XMFLOAT2(0.0f, -250.0f);
	m_vSize[TITLE_SHINING] = XMFLOAT2(PRESS_SIZE_X, PRESS_SIZE_Y);
	m_vColor[TITLE_SHINING].w = 0.0f;

	// BGM�Đ�
	CSound::Play(BGM_TITLE);
	CSound::Play(SE_GOGOGO);

	return hr;
}

// �I������
void Title::Uninit()
{
	// BGM�Đ���~
	CSound::Stop(BGM_TITLE);
	CSound::Stop(SE_STAGEENTER);
	for (int i = 0; i < MAX_TITLE_TEXUTURE; ++i) {
		//�w�i�e�N�X�`�����
		SAFE_RELEASE(m_pTexture[i]);
	}
}

// �X�V
void Title::Update()
{

	static float sindegCount = 0.0f;		//�F����s�m���ӂ�ӂ킳���邽�߂̃J�E���g

	// [Enter]�܂��̓X�y�[�X�L�[����
	if (g_bAnimFlag && (GetJoyTrigger(0, XBOXA)) && g_bPushAFlag == false) {
		g_bStart = true;
		//���������̕s�����x��1��		 01/25	�O�֑�O
		m_vColor[TITLE_SHINING].w = 1.0f;

		CSound::Stop(SE_STAGEENTER);
		CSound::Play(SE_STAGEENTER);
		//Time�ϐ����g������������		 01/25	�O�֑�O


		StartFadeOut(SCENE_SELECT);
		return;
	}

	//�A�j���[�V������������΂�
	if (GetJoyTrigger(0, XBOXA)) {
		g_bAnimFlag = true;
	}

	//�A�j���[�V��������
	if (!g_bAnimFlag&&m_vSize[1].y < LOGO_SIZE_Y) {
		m_vSize[1].y += 2.0f;
	}
	else {
		g_bAnimFlag = true;
	}

	//�A�j���[�V����������
	if (g_bAnimFlag) {
		CSound::Stop(SE_GOGOGO);
		//�c�ړ��͂��F����s�m�ɔ��f	01/15 �O�֑�O
		m_vPos[0].y += fMoveY;
		if (g_bFloatingFlag == false)
		{
			fMoveY -= 0.015f;
			if (m_vPos[TITLE_ASTRONAUT].y >= 200)
			{
				g_bFloatingFlag = true;
			}
		}

		if (g_bFloatingFlag == true)
		{
			if (m_vPos[TITLE_ASTRONAUT].y >= 200)
			{
				fMoveY -= 0.01f;
			}
			else
			{
				fMoveY += 0.01f;
			}
		}
		//���S�̂��������Œ�	01/15 �O�֑�O
		m_vSize[1].y = 600.0f;
	}
	//�ŏ��̃A�j���[�V�������I�������SE���Đ�
	if (g_bOldAnimFlag != g_bAnimFlag)
	{

		CSound::Stop(SE_TITLEPOP);
		CSound::Play(SE_TITLEPOP);
	}
	if (g_bStart && g_nTitleShiningAnimTime <= 30.0f)
	{
		//�|���S���̊g�又��
		m_vSize[TITLE_SHINING].x += PRESS_SIZE_X * 0.015f;
		m_vSize[TITLE_SHINING].y += PRESS_SIZE_Y * 0.015f;
		//���������̕s�����x���A�A�j���[�V�����̐i�݋�ɍ��킹�ĕs�����x��������	 01/23	�O�֑�O
		m_vColor[TITLE_SHINING].w = 1.0f - (g_nTitleShiningAnimTime / 30.0f);
		g_nTitleShiningAnimTime++;
	}
	//�A�j���[�V�����������̏ꍇ�A�F����s�m�݂̂����S�ɓ�����(=�����Ȃ�)����	01 / 15 �O�֑�O
	//�A�j���[�V�����������̏ꍇ�A�F����s�m��PRESSA�{�^�������S��������ɕύX�@01/24�@X
	if (!g_bAnimFlag)
	{
		m_vColor[TITLE_ASTRONAUT].w = 0.0f;
		m_vColor[TITLE_PUSHA].w = 0.0f;
	}
	else
	{
		m_vColor[TITLE_ASTRONAUT].w = 1.0f;
		//PushA�̕s�����x�A�j���[�V��������		//01/26	�O�֑�O
		m_vColor[TITLE_PUSHA].w = (g_fPushATimer / PUSHA_TIMER);
		if (g_bPushAAlpha)
		{
			if (g_fPushATimer >= PUSHA_TIMER)
			{
				g_bPushAAlpha = false;
			}
			g_fPushATimer++;
		}
		else
		{
			if (g_fPushATimer <= 0)
			{
				g_bPushAAlpha = true;
			}
			g_fPushATimer--;
		}
	}
#if _DEBUG
	PrintDebugProc("x,y={%.2f,%.2f}\n", m_vPos[0].x, m_vPos[0].y);
	PrintDebugProc("fMoveY={%.2f}\n", fMoveY);
	PrintDebugProc("m_vColor[TITLE_PUSHA].w:%.2f\n", m_vColor[TITLE_PUSHA].w);
#endif
	g_bOldAnimFlag = g_bAnimFlag;
}

// �`��
void Title::Draw()
{
	// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
	SetZBuffer(false);

	DrawBG("title");

	ID3D11DeviceContext* pDC = GetDeviceContext();
	//�`��
	for (int i = 0; i < MAX_TITLE_TEXUTURE; i++) {
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonAlpha(m_vColor[i].w);
		DrawPolygon(pDC);
	}
	// Z�o�b�t�@�L��(Z�`�F�b�N�L&Z�X�V�L)
	SetZBuffer(true);
}