/////////////////////////////////////////////////
//ブラックホール
//1/3 三輪卓弘
////////////////////////////////////////////////
#pragma once
#include "main.h"
#include "mesh.h"

#define MAX_BLACKHOLE			(10)	

class BlackHole
{
public:
	//4大処理
	HRESULT Init(int stageNum);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	HRESULT MakeVertex(ID3D11Device* pDevice);

	XMFLOAT4 Collision(XMFLOAT3 pos, XMFLOAT3 size);

	MESH			g_mesh;						// メッシュ情報
	MATERIAL		g_material;					// マテリアル

private:
	void Create(int StageNum);


};
