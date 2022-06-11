#pragma once
#include "main.h"
#include "bg.h"

#define MAX_RAYER_TEXTURE 18

class Rayer
{
public:
	HRESULT Init();	// ������
	void Uninit();	// �I������
	void Update();	// �X�V
	void Draw();		// �`��

	void Set(float moveX, float moveY, bool ClearFlag);//�ʒu�̃Z�b�g

private:

	XMFLOAT2 m_vPos[MAX_RAYER_TEXTURE];
	XMFLOAT2 m_vSize[MAX_RAYER_TEXTURE];
	ID3D11ShaderResourceView* m_pTexture[MAX_RAYER_TEXTURE];
};
