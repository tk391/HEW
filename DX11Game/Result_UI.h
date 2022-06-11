#pragma once
#include "main.h"
#include "UI.h"

#define MAX_RESULTUI_TEXUTURE			11

class ResultUI :
	public UI
{
public:
	HRESULT Init();
	void Uninit();
	void Update(bool countUp);
	void Draw(bool clear);
	void Star(float nLife);	//�F�t�����̕\��	01/24	������n
	void SetLife(float nLife);//���C�t�o�[�̐F���� 01/24	������n
private:
	void ActionButton(int nUInum, int count);	//�{�^���̏㉺���铮���F�̕ω����ꊇ��	01/16 �O�֑�O

	XMFLOAT2 m_vPos[MAX_RESULTUI_TEXUTURE];
	XMFLOAT2 m_vDefaultPos[MAX_RESULTUI_TEXUTURE];	//���ꂼ���UI�̏����ʒu		01/16 �O�֑�O
	XMFLOAT2 m_vSize[MAX_RESULTUI_TEXUTURE];
	XMFLOAT4 m_vColor[MAX_RESULTUI_TEXUTURE];
	ID3D11ShaderResourceView* m_pTexture[MAX_RESULTUI_TEXUTURE];
};
