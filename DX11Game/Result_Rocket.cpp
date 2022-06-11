//=============================================================================
//�S�[���i�F���D�j[Result_Rocket.cpp]
//12/11 �V������
//=============================================================================
#include "Result_Rocket.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "input.h"
#include "collision.h"
#include "Camera.h"

#define SPACE_SHIP			"data/model/spaceship2.fbx"

//�O���[�o���ϐ�
static CAssimpModel	g_model;		// ���f��

static bool oasobi;

static XMFLOAT4X4	g_mtxWorld;		// ���[���h�}�g���b�N�X

//=============================================================================
//������
//=============================================================================
HRESULT Result_Rocket::Init() {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	//�����o�ϐ��̏�����
	m_Pos = XMFLOAT3(0.0f, 0.0f, 500.0f);
	m_Rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scl = XMFLOAT3(8.0f, 8.0f, 8.0f);
	// ���f���f�[�^�̓ǂݍ���
	if (!g_model.Load(pDevice, pDeviceContext, SPACE_SHIP)) {
		MessageBoxA(GetMainWnd(), "���f���f�[�^�ǂݍ��݃G���[", "InitModel", MB_OK);
		return E_FAIL;
	}

	oasobi = false;

	return hr;
}
//=============================================================================
//�I��
//=============================================================================
void Result_Rocket::Uninit() {
	// ���f���̉��
	g_model.Release();
}

//=============================================================================
//�X�V
//=============================================================================
void Result_Rocket::Update() {
	if (!oasobi) {
		m_Pos.z -= 5.0f;
	}
	if (m_Pos.z < -500.0f&&GetJoyTrigger(0, XBOXY)) {
		CCamera::Get()->SetPos(0.0f, 0.0f, -200.0f);
		CCamera::Get()->SetTarget(0.0f, 0.0f, 0.0f);
		m_Pos = XMFLOAT3(1000.0f, -80.0f, 0.0f);
		m_Scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
		m_Rot.y = 90.0f;
		oasobi = true;
	}
	if (oasobi) {
		m_Pos.x -= 5.0f;
	}

#if _DEBUG
	PrintDebugProc("pos.z=%.2f\n", m_Pos.z);
#endif

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
void Result_Rocket::Draw() {
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

XMFLOAT3 Result_Rocket::GetPos() {
	return m_Pos;
}