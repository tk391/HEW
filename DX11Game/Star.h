//=============================================================================
//
// 爆発処理 [explosion.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once

#include "main.h"
#include "Texture.h"
#include "mesh.h"
#include "Camera.h"
#include "Light.h"

//マクロ
#define maxStar	(5)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
class Star {
public:
	HRESULT Init(void);
	void Uninit(void);
	void Update(int StageNum, int animation);
	void Draw(int selectStage);
	int OverFix(int num);
private:
	HRESULT MakeVertex(ID3D11Device* pDevice, int count);

	XMFLOAT3	pos[maxStar];		// 位置
	XMFLOAT2	m_size[maxStar];		// サイズ
	XMFLOAT4	color[maxStar];		// 色

	MESH			g_mesh[maxStar];						// メッシュ情報
	MATERIAL		g_material[maxStar];					// マテリアル

};
