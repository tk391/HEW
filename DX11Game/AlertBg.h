#pragma once
//�x������ʐԉ�
#include "main.h"

class Alert {
public:
	HRESULT Init();	//������
	void Uninit();		//�X�V
	void Update();
	void Draw();		//�`��

private:
	XMFLOAT2 m_vPos[2];	//�ʒu
	XMFLOAT2 m_vSize[2];	//�傫��

	ID3D11ShaderResourceView* m_pTexture[2];	//�e�N�X�`��
};

//������Ăяo���ď������J�n����
void AlertWarning(bool Flg);
//�����܂�