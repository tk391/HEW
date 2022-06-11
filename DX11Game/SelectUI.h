#pragma once
#include "UI.h"
#include "main.h"

#define MAX_SELECTUI_TEXUTURE			21
/*
��5��
�F����s�m
�X�e�[�W��
���*/

// �񋓌^:�摜���		01/16 �O�֑�O
enum ESelectUI {
	MOON_1 = 0,		// �摜:�X�e�[�W1
	MARSE_2,		// �摜:�X�e�[�W2
	JUPITER_3,		// �摜:�X�e�[�W3
	VINUS_4,		// �摜:�X�e�[�W4
	ASTEROIDS_5,	// �摜:�X�e�[�W5

	ASTRONAUT,		// ����:�F����s�m
	ARROW_RIGHT,	// UI  :���
	ARROW_LEFT,		// UI  :���
	NOPLAY,			// ����:�܂��V�ׂȂ���
	STARFLAME,		// ���g
	SHUTTER,		// �V���b�^�[

	STAGENAME1,		// ����:�X�e�[�W��
	STAGENAME2,		// ����:�X�e�[�W��
	STAGENAME3,		// ����:�X�e�[�W��
	STAGENAME4,		// ����:�X�e�[�W��
	STAGENAME5,		// ����:�X�e�[�W��
	NAMESHINING1,	// ��������
	NAMESHINING2,	// ��������
	NAMESHINING3,	// ��������
	NAMESHINING4,	// ��������
	NAMESHINING5,	// ��������

	MAX_SELECTUI	// �Z���N�g���UI�摜����
};

class SelectUI :
	public UI
{
public:
	HRESULT Init();
	void Uninit();
	void Update(int StageNum);
	void Draw();
	void AstroFloating(int num, int animation, int stageNum);		//��ʏ�̉F����s�m�̈ʒu��I�𒆂̃X�e�[�W�Ɠ�������֐�	01/16 �O�֑�O
	void ArrowAnim(int LR, int animation);		//Select.cpp�̓��e�Ɠ������āA���L�[��������Ɠ������֐�	01/16 �O�֑�O
	void NoPlayAnimSet();	//"�܂��V�ׂȂ���"�̗h���t���O��ݒ�	01/23�O�֑�O
	bool NoPlayAnimGet();	//"�܂��V�ׂȂ���"�̗h���t���O���擾	01/23�O�֑�O
	void StageAnimSet();	//�X�e�[�W���̗h���t���O��ݒ�		01/23�O�֑�O
	void ShiningAnimSet();	//�X�e�[�W���̔����t���O��ݒ�			01/23�O�֑�O
private:
	XMFLOAT2 m_vPos[MAX_SELECTUI_TEXUTURE];
	XMFLOAT2 m_vSize[MAX_SELECTUI_TEXUTURE];
	XMFLOAT4 m_vColor[MAX_SELECTUI_TEXUTURE];
	ID3D11ShaderResourceView* m_pTexture[MAX_SELECTUI_TEXUTURE];
};

