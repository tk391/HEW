#pragma once
#include "main.h"
#include "bg.h"

#define MAX_RAYER_TEXTURE 18

class Rayer
{
public:
	HRESULT Init();	// 初期化
	void Uninit();	// 終了処理
	void Update();	// 更新
	void Draw();		// 描画

	void Set(float moveX, float moveY, bool ClearFlag);//位置のセット

private:

	XMFLOAT2 m_vPos[MAX_RAYER_TEXTURE];
	XMFLOAT2 m_vSize[MAX_RAYER_TEXTURE];
	ID3D11ShaderResourceView* m_pTexture[MAX_RAYER_TEXTURE];
};

