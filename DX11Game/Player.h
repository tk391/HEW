#pragma once
#include "main.h"

//�O���錾
class Life;

class Player
{
public:
	//4�又��
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//4�又���ӊO��public�����o�֐�
	XMFLOAT3& GetPos();				//�ʒu�擾
	float GetO2();					//�_�f�ʎ擾�i���U���g�Ŏg�p�j
	void  SetAnimTelopFlag(bool);	//�e���b�v�A�j���[�V�����̃t���O�Z�b�g	// 01/22 �O�֑�O

private:
	//private�����o�֐�
	float RotZ(XMFLOAT2);							//�p�x�擾
	void Animator(int animNum);					//�A�j���[�V�������ꊇ�Ǘ�
	void GameOverAnimation();					//�_�f���؂ꂽ���̃A�j���[�V����
	void GameClearAnimation();					//�N���A�������̃A�j���[�V����
	void Repatriation(XMFLOAT3 Vector);		//�͈͊O�̋�������

	//�����o�ϐ�
	XMFLOAT3		m_Pos;			// ���݂̈ʒu
	XMFLOAT3		m_Rot;			// ���݂̌���
	XMFLOAT3		m_RotDest;	// �ړI�̌���
	XMFLOAT3		m_Move;		// �ړ���
	XMFLOAT3		m_Scl;			//�傫��
	float m_fLife;						//���C�t�Q�[�W
	bool m_bFlag;					//�ŏ����瓮���̂�h���t���O
	bool m_bCollision;				//�Ԃ����Ă���t���O
	bool m_bPull[2];				//�J�����������t���O�i0:�W�F�b�g���@1:�S�́j
	bool m_bHold;						//覐΂�����ł��锻��

	int m_nAnimNum;				//���݂̃A�j���[�V�����̔ԍ�
	XMFLOAT3		m_Accel;	//�����x 12/24 X

	XMFLOAT3        m_OldRot;  // 1�t���[���O�̊p�x�@12/26 X
};
