//=============================================================================
//
// �����N���X [Light.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "Light.h"

using namespace DirectX;

// �O���[�o���ϐ�
namespace {
	const XMFLOAT3 LIGHT0_DIRECTION = XMFLOAT3(0.0f, -1.0f, 1.0f);
	const XMFLOAT4 LIGHT0_DIFFUSE = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	const XMFLOAT4 LIGHT0_AMBIENT = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	const XMFLOAT4 LIGHT0_SPECULAR = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	CLight g_light;
};

// �R���X�g���N�^
CLight::CLight()
{
	Init();
}

// ������
void CLight::Init()
{
	XMFLOAT3 vDir = LIGHT0_DIRECTION;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vDir)));
	m_diffuse = LIGHT0_DIFFUSE;
	m_ambient = LIGHT0_AMBIENT;
	m_specular = LIGHT0_SPECULAR;
	m_bEnable = true;
}

// ���������擾
XMFLOAT3& CLight::GetDir()
{
	if (m_bEnable) return m_direction;
	static XMFLOAT3 off(0.0f, 0.0f, 0.0f);
	return off;
}

// �����C���X�^���X�擾
CLight* CLight::Get()
{
	return &g_light;
}
