#pragma once
#include "main.h"
#include "SceneBase.h"

#define MAX_TITLE_TEXUTURE			4
/*
??5??
?F?????s?m
?X?e?[?W??
????*/
class Title :public ISceneBase {
public:
	HRESULT Init();	// ??????
	void Uninit();		// ?I??????
	void Update();		// ?X?V
	void Draw();		// ?`??

private:
	XMFLOAT2 m_vPos[MAX_TITLE_TEXUTURE];
	XMFLOAT2 m_vSize[MAX_TITLE_TEXUTURE];
	XMFLOAT4 m_vColor[MAX_TITLE_TEXUTURE];
	ID3D11ShaderResourceView* m_pTexture[MAX_TITLE_TEXUTURE];
};