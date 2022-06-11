//=============================================================================
//�_�f[Life.cpp]
//12/11 �V������
//=============================================================================
#include "Life.h"
#include"Texture.h"
#include"polygon.h"
#include "debugproc.h"
#include <math.h>

//�}�N��
#define MAX_TEXTURE		(2)		//�e�N�X�`����

#define PATH_LIFEGAGE_TEXTURE	 L"data/texture/prottype_white.png"	//�_�f�Q�[�W
#define PATH_CYLINDER_TEXTURE	 L"data/texture/bombe.png"				//�_�f�{���x�i�ׂ����̂ɕς��Ăق����j

#define CENTER_POS_X	(-500.0f)	//���S���WX
#define CENTER_POS_Y	(-150.0f)	//���S���WY

#define UNDER_POS	(-270.0f)		//��̈ʒu

#define MAX_SIZE		(200.0f)		//�_�f�Q�[�W�̍ő�T�C�Y

//�O���[�o���ϐ�
static XMFLOAT2 g_vPos[MAX_TEXTURE];
static XMFLOAT2 g_vSize[MAX_TEXTURE];
static XMFLOAT4 g_vColor[MAX_TEXTURE];
static ID3D11ShaderResourceView* g_pTexture[MAX_TEXTURE];

//�U���O���[�o���ϐ�
//01/10  ������n
static int vibX;
static int vibY;
static bool BeatFlg = true;
static float vibSize;
static bool	DanceFlg = false;
//�����܂�
//=============================================================================
//������
//=============================================================================
HRESULT Life::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`���ǂݍ���
	hr = CreateTextureFromFile(pDevice, PATH_LIFEGAGE_TEXTURE, &g_pTexture[0]);
	hr = CreateTextureFromFile(pDevice, PATH_CYLINDER_TEXTURE, &g_pTexture[1]);
	if (FAILED(hr))
		return hr;

	//�����o�ϐ��̏�����
	g_vPos[0] = XMFLOAT2(CENTER_POS_X, CENTER_POS_Y - 25.0f);
	g_vSize[0] = XMFLOAT2(30.0f, 200.0f);
	g_vColor[0] = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	g_vPos[1] = XMFLOAT2(CENTER_POS_X, CENTER_POS_Y);
	g_vSize[1] = XMFLOAT2(100.0f, 300.0f);
	g_vColor[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//�U���ϐ��̏�����
	//01/11  ������n
	vibX = 0;
	vibY = 0;
	vibSize = 0.0f;
	return hr;
}

//=============================================================================
//�I��
//=============================================================================
void Life::Uninit()
{
	for (int i = 0; i < MAX_TEXTURE; ++i) {
		//�w�i�e�N�X�`�����
		SAFE_RELEASE(g_pTexture[i]);
	}
}

//=============================================================================
//�X�V
//=============================================================================
void Life::Update()
{
#if _DEBUG
	//���̂Ƃ���͓��ɂȂ�
	PrintDebugProc("x,y={%.2f,%.2f}\n", g_vSize[1].x, g_vSize[1].y);
#endif
}

//=============================================================================
//�`��
//=============================================================================
void Life::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	//�_�f�Q�[�W
	SetPolygonSize(g_vSize[0].x + 25.0f, g_vSize[0].y);

	SetPolygonPos(g_vPos[0].x, g_vPos[0].y);
	//�U����ǉ�
	//01/10  ������n
	SetPolygonPos(g_vPos[0].x + vibX, g_vPos[0].y + vibY);
	SetPolygonTexture(g_pTexture[0]);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(g_vColor[0].x, g_vColor[0].y, g_vColor[0].z);
	DrawPolygon(pDC);

	//�_�f�{���x
	SetPolygonSize(g_vSize[1].x, g_vSize[1].y);
	//�ۓ���ǉ�
	//01/11  ������n
	SetPolygonSize(g_vSize[1].x + vibSize, g_vSize[1].y + vibSize);
	SetPolygonPos(g_vPos[1].x, g_vPos[1].y);
	//�U����ǉ�
	//01/10  ������n
	SetPolygonPos(g_vPos[1].x + vibX, g_vPos[1].y + vibY);
	SetPolygonTexture(g_pTexture[1]);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(g_vColor[1].x, g_vColor[1].y, g_vColor[1].z);
	DrawPolygon(pDC);
	//���ɖ߂�
	SetPolygonColor(1.0f, 1.0f, 1.0f);
}

//=============================================================================
//�v���C���[�̃��C�t��Ԃɂ���đ傫���𓮂���
//=============================================================================
void Life::Set(float fLife)
{
	if (fLife < 0.0f) {
		g_vSize[0].x = 0.0f;
		return;
	}
	g_vSize[0].y = fLife * 1.9f;
	g_vPos[0].y = UNDER_POS + (g_vSize[0].y / 2.0f);
#if _DEBUG
	PrintDebugProc("fLife=%.2f\n", fLife);
#endif
	//�΁���ver	01/18 �O�֑�O
	if (fLife >= 50.0f) {
		g_vColor[0] = XMFLOAT4(1.0f - ((fLife / 100) - (1.0f - fLife / 100)), 1.0f, 0.0f, 1.0f);
	}
	else {
		g_vColor[0] = XMFLOAT4(1.0f, fLife / 100 + fLife / 100, 0.0f, 1.0f);
	}
}

//�p�^�[��1;�U��
//01/10 ������n
void Life::Vib(float fLife)
{
	if (fLife >= 0)
	{
		if (fLife <= 50.0f && fLife > 25.0f)
		{
			vibX = (rand() % 4);
			vibY = (rand() % 1);
		}

		if (fLife <= 25.0f)
		{
			vibX = (rand() % 6);
			vibY = (rand() % 2);
		}
	}
	else
	{
		vibX = 0;
		vibY = 0;
	}
}

//�p�^�[��2:�ۓ�
//01/10  ������n
void Life::Beat(float fLife)
{
	if (fLife > 0)
	{
		if (fLife <= 50.0f && fLife > 25.0f && BeatFlg)
		{
			if (vibSize <= 9.5f)
			{
				vibSize += 0.4f;
			}
			else
			{
				BeatFlg = false;
			}
		}
		else if (fLife <= 50.0f && fLife > 25.0f && !BeatFlg)
		{
			if (vibSize >= -0.5f)
			{
				vibSize -= 0.4f;
			}
			else
			{
				BeatFlg = true;
			}
		}

		if (fLife <= 25.0f && BeatFlg)
		{
			if (vibSize <= 12.5f)
			{
				vibSize += 0.6f;
			}
			else
			{
				BeatFlg = false;
			}
		}
		else if (fLife <= 25.0f && !BeatFlg)
		{
			if (vibSize >= -0.5f)
			{
				vibSize -= 0.6f;
			}
			else
			{
				BeatFlg = true;
			}
		}
	}
	else
	{
		vibSize = 0.0f;
	}
	// �_�f�{���x�̖c��݂ɍ��킹�ĐԂ��ۂ��Ȃ鏈�� �s�v�Ȃ�����ĉ� 01/18 �O�֑�O
	g_vColor[1] = XMFLOAT4(1.0f, 1.0f - vibSize / 40, 1.0f - vibSize / 40, 1.0f);
}

void Life::Dance(float fLife)
{
	if (fLife >= 0)
	{
		if (fLife <= 50.0f)
		{
			DanceFlg = true;
		}
	}
	else
	{
		DanceFlg = false;
	}
}