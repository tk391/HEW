//=============================================================================
//�u���b�N�z�[��
//1/3 �O�֑�O
//=============================================================================
#include "BlackHole.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "collision.h"
#include <vector>
#include "input.h"
#include "main.h"
#include "texture.h"
#include "mesh.h"
#include "Light.h"

//�}�N��
#define BlackHoleTexture			"data/texture/brackholl_T_Sprit.png"
#define BLACKHOLE_SIZE		90.0f			//�u���b�N�z�[���̑傫��(�c��)

#define BLACKHOLE_COUNT_X		4		//�摜�̉��̃t���[������
#define BLACKHOLE_COUNT_Y		4		//�摜�̏c�̃t���[������
#define BLACKHOLE_FLAME_COUNT	5	//���t���[���Ɉ�x�A�j���[�V�������X�V���邩



//�u���b�N�z�[���̏������\����
struct TBlackHole {
	XMFLOAT3	m_Pos;		// �ʒu
	float		m_Size;			// �u���b�N�z�[���̑傫��(�c��)
	XMFLOAT4	m_Col;		// �F
	int			anim;			//�A�j���[�V�����Đ��ʒu
	int			count;			//�A�j���[�V�����t���[����
};


//�O���[�o���ϐ�
static MESH g_mesh;							//���b�V�����
static MATERIAL g_material;				//�}�e���A��
static ID3D11ShaderResourceView* g_pTexture;	//�e�N�X�`�����
static std::vector<TBlackHole> BlackHoleArray;	//�u���b�N�z�[�����Ǘ����铮�I�z��


//=============================================================================
//����������
//=============================================================================
HRESULT BlackHole::Init(int stageNum) {
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	// ���_���̍쐬
	hr = MakeVertex(pDevice);
	if (FAILED(hr)) {
		MessageBoxA(GetMainWnd(), "�u���b�N�z�[���ǂݍ��݃G���[", "InitBlackHole", MB_OK);
		return hr;
	}

	// �}�e���A��
	g_material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Power = 0.0f;
	g_mesh.pMaterial = &g_material;

	// �e�N�X�`���ǂݍ���
	hr = CreateTextureFromFile(pDevice, BlackHoleTexture, &g_mesh.pTexture);
	if (FAILED(hr)) {
		return hr;
	}
	XMStoreFloat4x4(&g_mesh.mtxTexture, XMMatrixIdentity());

	//���[���h�}�g���N�X������
	XMStoreFloat4x4(&g_mesh.mtxWorld, XMMatrixIdentity());

	//������
	Create(stageNum);

	return hr;
}

//=============================================================================
//�I��
//=============================================================================
void BlackHole::Uninit(void) {
	// ���b�V���̊J��
	ReleaseMesh(&g_mesh);
	
	//���I�z��̉��
	BlackHoleArray.clear();
}

//=============================================================================
//�X�V
//=============================================================================
void BlackHole::Update(void) {
	//	XMMATRIX mtxWorld, mtxScl, mtxPos;
	ID3D11Device* pDevice = GetDevice();
	//�C�e���[�^�[
	std::vector<TBlackHole>::iterator it;
	it = BlackHoleArray.begin();
	while (it != BlackHoleArray.end()) {
		//�J�E���^�X�V
		it->count--;
		if (it->count <= 0) { //�t���[�����Ō�܂Ői�񂾂�
			it->anim = (it->anim + 1) % (BLACKHOLE_COUNT_X * BLACKHOLE_COUNT_Y); // it->anim + 1�̒l���t���[�������ɒB���Ă��Ȃ������� (�B���Ă����ꍇ�A0�Ƀ��Z�b�g)
			it->count = BLACKHOLE_FLAME_COUNT;
		}
		it++;
	}
}

//=============================================================================
//�`��
//=============================================================================
void BlackHole::Draw(void)
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	//XMMATRIX mtxWorld, mtxScale, mtxTranslate;

	CLight::Get()->SetDisable();	// ��������
	SetBlendState(BS_ALPHABLEND);	// ���u�����f�B���O�L��

	// �r���[�}�g���b�N�X���擾
	XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();

	//�C�e���[�^�[
	std::vector<TBlackHole>::iterator it;
	it = BlackHoleArray.begin();
	while (it != BlackHoleArray.end()) {

		// �e�N�X�`�� �}�g���b�N�X�X�V
		XMMATRIX mtxTex = XMMatrixScaling(1.0f / BLACKHOLE_COUNT_X, 1.0f / BLACKHOLE_COUNT_Y, 1.0f);
		int u = it->anim % BLACKHOLE_COUNT_X;
		int v = it->anim / BLACKHOLE_COUNT_X;
		mtxTex = XMMatrixMultiply(mtxTex, XMMatrixTranslation((float)u / BLACKHOLE_COUNT_X, (float)v / BLACKHOLE_COUNT_Y, 0.0f));
		XMStoreFloat4x4(&g_mesh.mtxTexture, mtxTex);

		//�r���[�s��̉�]�����̓]�u�s���ݒ�
		g_mesh.mtxWorld._11 = mView._11 * it->m_Size;
		g_mesh.mtxWorld._12 = mView._21 * it->m_Size;
		g_mesh.mtxWorld._13 = mView._31 * it->m_Size;
		g_mesh.mtxWorld._14 = 0.0f;
		g_mesh.mtxWorld._21 = mView._12 * it->m_Size;
		g_mesh.mtxWorld._22 = mView._22 * it->m_Size;
		g_mesh.mtxWorld._23 = mView._32 * it->m_Size;
		g_mesh.mtxWorld._24 = 0.0f;
		g_mesh.mtxWorld._31 = mView._13;
		g_mesh.mtxWorld._32 = mView._23;
		g_mesh.mtxWorld._33 = mView._33;
		g_mesh.mtxWorld._34 = 0.0f;

		//�ʒu�𔽉f
		g_mesh.mtxWorld._41 = it->m_Pos.x;
		g_mesh.mtxWorld._42 = it->m_Pos.y;
		g_mesh.mtxWorld._43 = it->m_Pos.z;
		g_mesh.mtxWorld._44 = 1.0f;

		// �|���S���̕`��
		DrawMesh(pDC, &g_mesh);
		it++;
	}

	//SetBlendState(BS_NONE);		// ���u�����f�B���O����
	CLight::Get()->SetEnable();	// �����L��
}

//=============================================================================
//�Փ˔��� �߂�l:�Փ˂����u���b�N�z�[���̍��W
//=============================================================================
XMFLOAT4 BlackHole::Collision(XMFLOAT3 pos, XMFLOAT3 size) {
	//�C�e���[�^�[
	std::vector<TBlackHole>::iterator it;
	it = BlackHoleArray.begin();
	int count = 0;
	while (it != BlackHoleArray.end()) {
		float m_Size = it->m_Size * 0.7f;
		bool collisilon = CollisionSphere(it->m_Pos, m_Size, pos, size.y); //�Ώە��ƃu���b�N�z�[�����ڐG���Ă�����
		if (collisilon == false) { // �Ώە��ƐڐG���Ă��Ȃ��ꍇ�A���̗v�f�֐i�s����
			it++;
			count++;
			continue;
		}

		//�ڐG�����u���b�N�z�[���̍��W��Ԃ�
		return XMFLOAT4(it->m_Pos.x, it->m_Pos.y, it->m_Pos.z, 1);
	}
	return XMFLOAT4(-1, -1, -1, -1);
}

//=============================================================================
//���̃Z�b�g
//=============================================================================
void BlackHole::Create(int StageNum) {
	switch (StageNum) {
	case 1:
		//�X�e�[�W5�ŕK�v�ȃu���b�N�z�[���𐶐����Ă���
		//                               �ʒu�@�@�@�@�@�@�@�@�@�@�@�傫���@�@�@�@�@�F
		BlackHoleArray.push_back({ XMFLOAT3(-354000.0f, -63.0f, 0.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	case 2:
		//�X�e�[�W5�ŕK�v�ȃu���b�N�z�[���𐶐����Ă���
		//                               �ʒu�@�@�@�@�@�@�@�@�@�@�@�傫���@�@�@�@�@�F
		BlackHoleArray.push_back({ XMFLOAT3(-354000.0f, -332.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	case 3:
		//�X�e�[�W5�ŕK�v�ȃu���b�N�z�[���𐶐����Ă���
		//                               �ʒu�@�@�@�@�@�@�@�@�@�@�@�傫���@�@�@�@�@�F
		BlackHoleArray.push_back({ XMFLOAT3(-354000.0f, -332.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	case 4:
		//�X�e�[�W5�ŕK�v�ȃu���b�N�z�[���𐶐����Ă���
		//                               �ʒu�@�@�@�@�@�@�@�@�@�@�@�傫���@�@�@�@�@�F
		BlackHoleArray.push_back({ XMFLOAT3(-354000.0f, -332.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	case 5:
		//�X�e�[�W5�ŕK�v�ȃu���b�N�z�[���𐶐����Ă���
		//                               �ʒu�@�@�@�@�@�@�@�@�@�@�@�傫���@�@�@�@�@�F
		BlackHoleArray.push_back({ XMFLOAT3(-354.0f, -332.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		BlackHoleArray.push_back({ XMFLOAT3(-164.0f,  -64.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		BlackHoleArray.push_back({ XMFLOAT3(150.0f, -130.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	default:
		break;
	}
}

//=============================================================================
//���_���̍쐬
//=============================================================================
HRESULT BlackHole::MakeVertex(ID3D11Device* pDevice) {
	//�ꎞ�I�Ȓ��_�z��𐶐�
	g_mesh.nNumVertex = 4;
	VERTEX_3D* pVertexWK = new VERTEX_3D[g_mesh.nNumVertex];

	//���_�z��̒��g�𖄂߂�
	pVertexWK[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	pVertexWK[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	pVertexWK[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	pVertexWK[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);
	pVertexWK[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[0].tex = XMFLOAT2(0.0f, 0.0f);
	pVertexWK[1].tex = XMFLOAT2(1.0f, 0.0f);
	pVertexWK[2].tex = XMFLOAT2(0.0f, 1.0f);
	pVertexWK[3].tex = XMFLOAT2(1.0f, 1.0f);

	//�ꎞ�I�ȃC���f�b�N�X�z��𐶐�
	g_mesh.nNumIndex = 4;
	int* pIndexWK = new int[4];

	//�C���f�b�N�X�z��̒��g�𖄂߂�
	for (int i = 0; i < 4; i++) {
		pIndexWK[i] = i;
	}

	//���_�o�b�t�@/�C���f�b�N�X�o�b�t�@����
	HRESULT hr = MakeMeshVertex(pDevice, &g_mesh, pVertexWK, pIndexWK);

	//�ꎞ�I�Ȓ��_�z��/�C���f�b�N�X�z������
	delete[] pIndexWK;
	delete[] pVertexWK;
	return hr;
}