//�x������ʐԉ�
#include "AlertBg.h"
#include "Texture.h"
#include "polygon.h"
#include "Sound.h"
#include "debugproc.h"

//�萔
const std::string ArtBGTexture[] = {
	"data/texture/RedBg.png",
	"data/texture/OutOfArea.png" 
};//�e�N�X�`����

//�O���[�o���ϐ�
static float g_fAlphaAlt;				//�����x
static bool AltFlg;				//�A���[�g�����Ă��邩
static bool StartAFlg;		//�A���[�g�̃X�^�[�g

//������
HRESULT Alert::Init() {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	// �e�N�X�`���Ǎ�
	for (int i = 0; i < 2; i++) {
		hr = CreateTextureFromFile(pDevice, ArtBGTexture[i].c_str(), &m_pTexture[i]);
	}

	//�����o�ϐ��i�O���[�o���ϐ��j������
	m_vPos[0] = XMFLOAT2(0.0f, 0.0f);
	m_vSize[0] = XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT);
	m_vPos[1] = XMFLOAT2(0.0f, 0.0f);
	m_vSize[1] = XMFLOAT2(700.0f, 132.0f);
	g_fAlphaAlt = 0.0f;
	StartAFlg = false;

	return hr;
}

//�I��
void Alert::Uninit() {
	// �w�i�e�N�X�`�����
	for (int i = 0; i < 2; i++) {
		SAFE_RELEASE(m_pTexture[i]);
	}
}

//�A�b�v�f�[�g
void Alert::Update() {
	static int Count = 0;
	//�A���[�g����
	if (StartAFlg) {
		Count++;
		if (Count >= 60) {
			//���l��������SE�Đ�
			Count = 0;
			CSound::Play(SE_WARNING);
		}
	}
}

//�`��
void Alert::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();
	for (int i = 0; i < 2; i++) {
		SetPolygonSize(m_vSize[i]);
		SetPolygonPos(m_vPos[i]);
		SetPolygonAlpha(g_fAlphaAlt);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonUV(0.0f, 0.0f);
		DrawPolygon(pDC);
	}
}

//������Ăяo���ĉ�ʂ�Ԃ�����
void AlertWarning(bool Flg) {
	//��ʊO�̏���Player���炢������
	StartAFlg = Flg;

	//��ʊO��񂪁u�O(true)�v�̎��ɏ������s��
	if (StartAFlg) {
		if (g_fAlphaAlt <= 0.9f && AltFlg) {
			g_fAlphaAlt += 0.007f;
		}
		else {
			AltFlg = false;
		}

		if (g_fAlphaAlt >= 0.5f && !AltFlg) {
			g_fAlphaAlt -= 0.007f;
		}
		else {
			AltFlg = true;
		}
	}

	//��ʊO��񂪁u��(false)�v�̎��ɏ������s��
	if (!StartAFlg) {
		//��ʓ��ɖ߂�����A��ʂ�Ԃ��疳�F�ɖ߂��������s��
		if (g_fAlphaAlt > 0.0f) {
			g_fAlphaAlt -= 0.01f;
		}
		else if (g_fAlphaAlt <= 0.0f) {
			//�t���O����߂��Ď��ɉ�ʊO�ɏo������
			//���񂾂�Ԃ��Ȃ鏈������n�߂�
			AltFlg = true;
		}
	}
}