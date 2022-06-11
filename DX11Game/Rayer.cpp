//=============================================================================
// �w�i [bg.cpp]
//12/11 �V������
//=============================================================================
#include "Rayer.h"
#include "Texture.h"
#include "polygon.h"
#include "Select.h"
#include "Camera.h"

//=============================================================================
// �}�N����`
//=============================================================================
#define SEIZA_MOVE		40	//�w�i�̐����̓����₷��			01/15 �O�֑�O
#define STARS_MOVE		30	//�w�i�̐��̃��C���[�̓����₷��	01/15 �O�֑�O
#define PLANET_MOVE		20	//�w�i�̉ΐ��Ƃ��̓����₷��		01/15 �O�֑�O

static std::string fileName[]{
	"data/texture/selectMoon.png",
	"data/texture/selectMarse.png",
	"data/texture/selectJupiter.png",
	"data/texture/selectVinus.png",
	"data/texture/asteroids.png",

	//��������
	"data/texture/Aquarius.png",
	"data/texture/Aries.png",
	"data/texture/Cancer.png",
	"data/texture/Capricorn.png",
	"data/texture/Gemini.png",
	"data/texture/Leo.png",
	"data/texture/Libra.png",
	"data/texture/Pisces.png",
	"data/texture/Sagittarius.png",
	"data/texture/Scoepio.png",
	"data/texture/Taurus.png",
	"data/texture/Virgo.png",

	"data/texture/�����Ȃ��w�i2.png"	//(��)�����C���[�̑w�𑝂₷���߂ɒǉ�	01/15 �O�֑�O
};

//=============================================================================
// �O���[�o���ϐ�
//=============================================================================

static int OutSeiza[3];
XMFLOAT3 CameraPos = XMFLOAT3(0.0f, 0.0f, 0.0f);	//�J�����̈ʒu�����炤���߂̕ϐ�	01/15 �O�֑�O

//=============================================================================
// ������
//=============================================================================
HRESULT Rayer::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	// �e�N�X�`���Ǎ�
	for (int i = 0; i < MAX_RAYER_TEXTURE; ++i) {
		hr = CreateTextureFromFile(pDevice, fileName[i].c_str(), &m_pTexture[i]);
		if (FAILED(hr)) {
			return hr;
		}
	}
	for (int i = 0; i < 3; i++) {
		OutSeiza[i] = (rand() % 12) + 5;
	}

	//�ϐ�������
	for (int i = 0; i < 5; i++) {
		//��
		m_vPos[i] = XMFLOAT2(0.0f, -100.0f);
		m_vSize[i] = XMFLOAT2(700.0f, 700.0f);
	}
	//�����i���j
	m_vPos[OutSeiza[0]] = XMFLOAT2(300.0f, -200.0f);
	m_vSize[OutSeiza[0]] = XMFLOAT2(150.0f, 150.0f);
	m_vPos[OutSeiza[1]] = XMFLOAT2(1200.0f, 200.0f);
	m_vSize[OutSeiza[1]] = XMFLOAT2(200.0f, 200.0f);
	m_vPos[OutSeiza[2]] = XMFLOAT2(-300.0f, -200.0f);
	m_vSize[OutSeiza[2]] = XMFLOAT2(250.0f, 250.0f);

	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void Rayer::Uninit()
{
	for (int i = 0; i < MAX_RAYER_TEXTURE; ++i) {
		// �w�i�e�N�X�`�����
		SAFE_RELEASE(m_pTexture[i]);
	}
}

//=============================================================================
// �X�V
//=============================================================================
void Rayer::Update()
{
	//���ɂȂ�
}

//=============================================================================
// �`��
//=============================================================================
void Rayer::Draw()
{
	//�J�����̍��W���擾	01/15 �O�֑�O
	CameraPos = CCamera::Get()->GetPos();

	int stnum = GetStageNum();
	--stnum;

	ID3D11DeviceContext* pDC = GetDeviceContext();

	//����
	//�����_����3�����X�e�[�W���ɕ\��
	for (int i = 0; i < 3; ++i) {
		//�J�����������ɔ����A�w�i�`��̈ʒu�����������悤��		01/15 �O�֑�O
		//�u�J�����̍��W �� n(���ł�����30�j�v�̐��l���e�������Ă���Bn�̒l���傫���قǓ������݂��Ȃ�B
		SetPolygonPos(m_vPos[OutSeiza[i]].x - (CameraPos.x / SEIZA_MOVE), m_vPos[OutSeiza[i]].y - (CameraPos.y / SEIZA_MOVE));
		SetPolygonSize(m_vSize[OutSeiza[i]]);
		SetPolygonTexture(m_pTexture[OutSeiza[i]]);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonColor(1.0f, 1.0f, 1.0f);
		DrawPolygon(pDC);
	}

	//(��)�w�i���C���[1
	//�J�����������ɔ����A�w�i�`��̈ʒu�����������悤��	01/15 �O�֑�O
	SetPolygonPos(0.0f - (CameraPos.x / STARS_MOVE), 0.0f - (CameraPos.y / STARS_MOVE));
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonTexture(m_pTexture[17]);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

	//�X�^�[�g���̑傫�Ȑ�
	//�J�����������ɔ����A�w�i�`��̈ʒu�����������悤��	01/15 �O�֑�O
	SetPolygonPos(m_vPos[stnum].x - (CameraPos.x / PLANET_MOVE), m_vPos[stnum].y - (CameraPos.y / PLANET_MOVE));
	SetPolygonSize(m_vSize[stnum].x + CameraPos.z / 40, m_vSize[stnum].y + CameraPos.z / 40);
	SetPolygonTexture(m_pTexture[stnum]);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

}

//=============================================================================
// �ʒu�̃Z�b�g
//=============================================================================
void Rayer::Set(float moveX, float moveY, bool ClearFlag) {
	int stnum = GetStageNum();
	--stnum;

	m_vPos[stnum].x -= moveX;
	m_vPos[stnum].y -= moveY;
	if (ClearFlag) {
		for (int i = 0; i < 3; ++i) {
			m_vPos[OutSeiza[i]].x -= moveX;
			m_vPos[OutSeiza[i]].y -= moveY;
		}
		m_vPos[17].x -= moveX * 1.8f;
		m_vPos[17].y -= moveY * 1.8f;
	}
}