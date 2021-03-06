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
	void Star(float nLife);	//色付き星の表示	01/24	佐藤大地
	void SetLife(float nLife);//ライフバーの色分け 01/24	佐藤大地
private:
	void ActionButton(int nUInum, int count);	//ボタンの上下する動作や色の変化を一括化	01/16 三輪卓弘

	XMFLOAT2 m_vPos[MAX_RESULTUI_TEXUTURE];
	XMFLOAT2 m_vDefaultPos[MAX_RESULTUI_TEXUTURE];	//それぞれのUIの初期位置		01/16 三輪卓弘
	XMFLOAT2 m_vSize[MAX_RESULTUI_TEXUTURE];
	XMFLOAT4 m_vColor[MAX_RESULTUI_TEXUTURE];
	ID3D11ShaderResourceView* m_pTexture[MAX_RESULTUI_TEXUTURE];
};

