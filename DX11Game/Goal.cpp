//=============================================================================
//�S�[���i�F���D�j[Goal.cpp]
//12/11 �V������
//=============================================================================
#include "Goal.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "input.h"
#include "collision.h"

#define SPACE_SHIP			"data/model/spaceship2.fbx"

//�O���[�o���ϐ�
static CAssimpModel	g_model;		// ���f��

static XMFLOAT4X4	g_mtxWorld;		// ���[���h�}�g���b�N�X

//=============================================================================
//������
//=============================================================================
HRESULT Goal::Init(int stageNum) {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	switch (stageNum)
	{
	case 1:
		m_Pos = XMFLOAT3(465.0f, -110.0f, 0.0f);
		break;
	case 2:
		m_Pos = XMFLOAT3(517.0f, -186.0f, 0.0f);
		break;
	case 3:
		m_Pos = XMFLOAT3(400.0f, -180.0f, 0.0f);
		break;
	case 4:
		m_Pos = XMFLOAT3(574.0f, -160.0f, 0.0f);
		break;
	case 5:
		m_Pos = XMFLOAT3(149.0f, -340.0f, 0.0f);
		break;
	}
	//�����o�ϐ��̏�����
	m_Rot = XMFLOAT3(0.0f, -90.0f, 0.0f);
	//m_Scl = XMFLOAT3(66.0f, 30.0f, 23.0f);
	m_Scl = XMFLOAT3(8.0f, 8.0f, 8.0f);

	// ���f���f�[�^�̓ǂݍ���
	if (!g_model.Load(pDevice, pDeviceContext, SPACE_SHIP)) {
		MessageBoxA(GetMainWnd(), "���f���f�[�^�ǂݍ��݃G���[", "InitModel", MB_OK);
		return E_FAIL;
	}
	return hr;
}
//=============================================================================
//�I��
//=============================================================================
void Goal::Uninit() {
	// ���f���̉��
	g_model.Release();
}

//=============================================================================
//�X�V
//=============================================================================
void Goal::Update() {
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScl;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	//�X�P�[���𔽉f
	mtxScl = XMMatrixScaling(m_Scl.x, m_Scl.y, m_Scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);


	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rot.x),
		XMConvertToRadians(m_Rot.y), XMConvertToRadians(m_Rot.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	// ���[���h�}�g���b�N�X�ݒ�
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);
}

//=============================================================================
//�`��
//=============================================================================
void Goal::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// �s����������`��
	g_model.Draw(pDC, g_mtxWorld, eOpacityOnly);

	// ������������`��
	SetBlendState(BS_ALPHABLEND);	// �A���t�@�u�����h�L��
	SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�
	g_model.Draw(pDC, g_mtxWorld, eTransparentOnly);
	SetZWrite(true);				// Z�o�b�t�@�X�V����
	SetBlendState(BS_NONE);			// �A���t�@�u�����h����
}

//=============================================================================
//�Փ˔���
//=============================================================================
bool Goal::Collision(XMFLOAT3 pos, XMFLOAT3 size) {
	XMFLOAT3 Size = g_model.GetBBox();
	//�����蔻��̒���
	size.x *= 0.8f;
	size.y *= 0.8f;
	size.z *= 0.8f;
	Size = XMFLOAT3(size.x*m_Scl.x, size.y*m_Scl.y, size.z*m_Scl.z);

	int isGoal = CollisionAABB(pos, size, m_Pos, Size);
	if (isGoal >= 0) {
		return true;
	}
	else {
		return false;
	}
}

//�S�[�����W����点��
XMFLOAT3& Goal::GetPos() {
	return m_Pos;
}

//�S�[�����W���Z�b�g
void Goal::SetPos(float posX) {
	m_Pos.x += posX;
}