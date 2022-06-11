//=============================================================================
//
// �������� [billboard.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "Star.h"
#include "debugproc.h"
#include "Scene.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define M_DIFFUSE				XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR				XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER					(1.0f)
#define M_AMBIENT				XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE				XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

#define MAX_BILLBOARD			(100)

#define BILLBOARD_FRAME_X		(1)		// ���t���[����(�e�N�X�`���ɍ�)
#define BILLBOARD_FRAME_Y		(1)		// �c�t���[����(�킹�ĕύX����)
#define BILLBOARD_ANIM_COUNT	(4)		// ��R�}������̕\����

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

static int count;

static std::string fileName[]{
	"data/texture/honjissouMoon.png",
	"data/texture/honjissouMarse.png",
	"data/texture/honjissouJupiter.png",
	"data/texture/selectVinus.png",
	"data/texture/asteroids.png"
};


//=============================================================================
// ������
//=============================================================================
HRESULT Star::Init(void)
{
	HRESULT hr = S_OK;
	for (int i = 0; i < maxStar; i++) {

		ID3D11Device* pDevice = GetDevice();
		// ���_���̍쐬
		hr = MakeVertex(pDevice, i);
		if (FAILED(hr)) {
			return hr;
		}

		// �}�e���A���̐ݒ�
		g_material[i].Diffuse = M_DIFFUSE;
		g_material[i].Ambient = M_AMBIENT;
		g_material[i].Specular = M_SPECULAR;
		g_material[i].Power = M_POWER;
		g_material[i].Emissive = M_EMISSIVE;
		g_mesh[i].pMaterial = &g_material[i];

		// �e�N�X�`���̓ǂݍ���
		hr = CreateTextureFromFile(pDevice, fileName[i].c_str(), &g_mesh[i].pTexture);
		if (FAILED(hr))
			return hr;
		XMStoreFloat4x4(&g_mesh[i].mtxTexture, XMMatrixIdentity());

		// ���[���h �}�g���b�N�X������
		XMStoreFloat4x4(&g_mesh[i].mtxWorld, XMMatrixIdentity());

		// ������񏉊���
		pos[i] = XMFLOAT3(CosDeg((72.0f*i) - 90.0f)*100.0f, 10.0f, SinDeg((72.0f*i) - 90.0f)*100.0f);
		m_size[i] = XMFLOAT2(100.0f, 100.0f);
		color[i] = M_DIFFUSE;
	}
	count = 0;

	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void Star::Uninit(void)
{
	for (int i = 0; i < maxStar; i++) {
		ReleaseMesh(&g_mesh[i]);
	}
}

//=============================================================================
// �X�V
//=============================================================================
void Star::Update(int StageNum, int animation)
{
	static float count = 0.0f;
	int x = StageNum - 1;
	for (int i = 0; i < maxStar; i++) {
		pos[i] = XMFLOAT3(CosDeg((i - x)*72.0f - (72.0f / 15.0f*animation) - 90.0f)*100.0f,
			0.0f,
			SinDeg((i - x)*72.0f - (72.0f / 15.0f*animation) - 90.0f)*100.0f);
		if (i < GetProgress()) {
			if (i == x) {
				//�I�����Ă��āA���v���C�ł���X�e�[�W
				color[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else {
				//�I���͂ł��Ȃ����A�v���C�ł���X�e�[�W
				color[i] = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			}
		}
		else {
			//�I���ł��Ȃ��X�e�[�W
			color[i] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		}
	}
	if (animation == 0) {
		count += 2.0f;
		pos[x].y = SinDeg(count);
	}
	//�o�O��̂ňꉞ
	m_size[0] = XMFLOAT2(100.0f, 100.0f);
}

//=============================================================================
// �`��
//=============================================================================
void Star::Draw(int selectStage)
{
	int NowDraw;
	ID3D11DeviceContext* pDC = GetDeviceContext();

	CLight::Get()->SetDisable();	// ��������
	SetBlendState(BS_ALPHABLEND);	// ���u�����f�B���O�L��
	for (int i = 0; i < maxStar; i++) {
		switch (i)
		{
		case 0:
			NowDraw = OverFix(selectStage + 2);
			break;
		case 1:
			NowDraw = OverFix(selectStage + 3);
			break;
		case 2:
			NowDraw = OverFix(selectStage + 1);
			break;
		case 3:
			NowDraw = OverFix(selectStage + 4);
			break;
		case 4:
			NowDraw = OverFix(selectStage);
			break;
		}
		XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();

		// �e�N�X�`�� �}�g���b�N�X�X�V
		XMMATRIX mtxTex = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		mtxTex = XMMatrixMultiply(mtxTex, XMMatrixTranslation(0.0f, 0.0f, 0.0f));
		XMStoreFloat4x4(&g_mesh[NowDraw].mtxTexture, mtxTex);


		// �r���[�s��̉�]�����̓]�u�s���ݒ�(+�g�k)
		g_mesh[NowDraw].mtxWorld._11 = mView._11 * m_size[NowDraw].x;
		g_mesh[NowDraw].mtxWorld._12 = mView._21 * m_size[NowDraw].x;
		g_mesh[NowDraw].mtxWorld._13 = mView._31 * m_size[NowDraw].x;
		g_mesh[NowDraw].mtxWorld._14 = 0.0f;
		g_mesh[NowDraw].mtxWorld._21 = mView._12 * m_size[NowDraw].y;
		g_mesh[NowDraw].mtxWorld._22 = mView._22 * m_size[NowDraw].y;
		g_mesh[NowDraw].mtxWorld._23 = mView._32 * m_size[NowDraw].y;
		g_mesh[NowDraw].mtxWorld._24 = 0.0f;
		g_mesh[NowDraw].mtxWorld._31 = mView._13;
		g_mesh[NowDraw].mtxWorld._32 = mView._23;
		g_mesh[NowDraw].mtxWorld._33 = mView._33;
		g_mesh[NowDraw].mtxWorld._34 = 0.0f;

		// �ʒu�𔽉f
		g_mesh[NowDraw].mtxWorld._41 = pos[NowDraw].x;
		g_mesh[NowDraw].mtxWorld._42 = pos[NowDraw].y;
		g_mesh[NowDraw].mtxWorld._43 = pos[NowDraw].z;
		g_mesh[NowDraw].mtxWorld._44 = 1.0f;
		// �F�𔽉f
		g_material[NowDraw].Diffuse = color[NowDraw];

		// �`��
		DrawMesh(pDC, &g_mesh[NowDraw]);
	}
	SetBlendState(BS_NONE);		// ���u�����f�B���O����
	CLight::Get()->SetEnable();	// �����L��
}


//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT Star::MakeVertex(ID3D11Device* pDevice, int count)
{
	// �ꎞ�I�Ȓ��_�z��𐶐�
	g_mesh[count].nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_mesh[count].nNumVertex];

	// ���_�z��̒��g�𖄂߂�
	pVertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	pVertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	pVertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	pVertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);
	pVertexWk[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	pVertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	pVertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	pVertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	// �ꎞ�I�ȃC���f�b�N�X�z��𐶐�
	g_mesh[count].nNumIndex = 4;
	int* pIndexWk = new int[4];

	// �C���f�b�N�X�z��̒��g�𖄂߂�
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@����
	HRESULT hr = MakeMeshVertex(pDevice, &g_mesh[count], pVertexWk, pIndexWk);

	// �ꎞ�I�Ȓ��_�z��/�C���f�b�N�X�z������
	delete[] pIndexWk;
	delete[] pVertexWk;
	return hr;
}


int Star::OverFix(int num)
{
	if (num >= 5)
	{
		num -= 5;
	}

	return num;
}