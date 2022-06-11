//=============================================================================
//
// �J���� �N���X [Camera.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once
#include "main.h"

class CCamera
{
private:
	XMFLOAT3 m_vPos;				// ���_
	XMFLOAT3 m_vTarget;				// �����_
	XMFLOAT3 m_vUp;					// ����x�N�g��

	XMFLOAT3 m_vDestPos;			// ���_�̖ړI�ʒu
	XMFLOAT3 m_vDestTarget;			// �����_�̖ړI�ʒu
	XMFLOAT3 m_vVelocity;			// �ړ���

	float m_fAspectRatio;			// �c����
	float m_fFovY;					// ����p(Degree)
	float m_fNearZ;					// �O���N���b�v����
	float m_fFarZ;					// ����N���b�v����

	XMFLOAT4X4 m_mtxWorld;			// ���[���h �}�g���b�N�X
	XMFLOAT4X4 m_mtxView;			// �r���[ �}�g���b�N�X
	XMFLOAT4X4 m_mtxProj;			// �v���W�F�N�V���� �}�g���b�N�X

	XMFLOAT3 m_vAngle;				// �J�����̊p�x
	XMFLOAT3 m_vDestAngle;			// �J�����̖ړI�̌���
	XMFLOAT3 m_vDestTargetAngle;	// �����_�̖ړI�̌���
	float m_fLengthInterval;		// �J�����̎��_�ƒ����_�̋���

	static CCamera* m_pCamera;		// ���݂̃J����

public:
	CCamera();

	void Init();
	void Update();

	void SetPos(XMFLOAT3& vPos) { m_vPos = vPos; }
	XMFLOAT3& GetPos() { return m_vPos; }
	XMFLOAT3& GetTarget() { return m_vTarget; }
	XMFLOAT3& GetDestTarget() { return m_vDestTarget; }
	void SetPos(float x, float y, float z) { m_vPos.x = x, m_vPos.y = y, m_vPos.z = z; }
	void SetTarget(XMFLOAT3& vTarget) { m_vTarget = vTarget; }
	void SetTarget(float x, float y, float z) { m_vTarget.x = x, m_vTarget.y = y, m_vTarget.z = z; }
	void SetUpVector(XMFLOAT3& vUp) { m_vUp = vUp; }
	void SetUpVector(float x, float y, float z) { m_vUp.x = x, m_vUp.y = y, m_vUp.z = z; }
	void SetWorldMatrix(XMFLOAT4X4& mtxWorld);
	XMFLOAT4X4& GetWorldMatrix() { return m_mtxWorld; }
	XMFLOAT4X4& CalcWorldMatrix();
	void SetFovY(float fFovY) { m_fFovY = fFovY; }
	void SetAspectRatio(float fAspect) { m_fAspectRatio = fAspect; }
	void SetRangeZ(float fNearZ, float fFarZ) { m_fNearZ = fNearZ, m_fFarZ = fFarZ; }
	void UpdateMatrix();
	XMFLOAT4X4& GetViewMatrix() { return m_mtxView; }
	XMFLOAT4X4& GetProjMatrix() { return m_mtxProj; }
	XMFLOAT3& GetAngle() { return m_vAngle; }

	static CCamera* Get() { return m_pCamera; }
	static void Set(CCamera* pCamera) { m_pCamera = pCamera; }
};
