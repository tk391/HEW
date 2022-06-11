//=============================================================================
//
// 爆発処理 [billboard.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "Star.h"
#include "debugproc.h"
#include "Scene.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define M_DIFFUSE				XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR				XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER					(1.0f)
#define M_AMBIENT				XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE				XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

#define MAX_BILLBOARD			(100)

#define BILLBOARD_FRAME_X		(1)		// 横フレーム数(テクスチャに合)
#define BILLBOARD_FRAME_Y		(1)		// 縦フレーム数(わせて変更する)
#define BILLBOARD_ANIM_COUNT	(4)		// 一コマ当たりの表示回数

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

static int count;

static std::string fileName[]{
	"data/texture/honjissouMoon.png",
	"data/texture/honjissouMarse.png",
	"data/texture/honjissouJupiter.png",
	"data/texture/selectVinus.png",
	"data/texture/asteroids.png"
};


//=============================================================================
// 初期化
//=============================================================================
HRESULT Star::Init(void)
{
	HRESULT hr = S_OK;
	for (int i = 0; i < maxStar; i++) {

		ID3D11Device* pDevice = GetDevice();
		// 頂点情報の作成
		hr = MakeVertex(pDevice, i);
		if (FAILED(hr)) {
			return hr;
		}

		// マテリアルの設定
		g_material[i].Diffuse = M_DIFFUSE;
		g_material[i].Ambient = M_AMBIENT;
		g_material[i].Specular = M_SPECULAR;
		g_material[i].Power = M_POWER;
		g_material[i].Emissive = M_EMISSIVE;
		g_mesh[i].pMaterial = &g_material[i];

		// テクスチャの読み込み
		hr = CreateTextureFromFile(pDevice, fileName[i].c_str(), &g_mesh[i].pTexture);
		if (FAILED(hr))
			return hr;
		XMStoreFloat4x4(&g_mesh[i].mtxTexture, XMMatrixIdentity());

		// ワールド マトリックス初期化
		XMStoreFloat4x4(&g_mesh[i].mtxWorld, XMMatrixIdentity());

		// 爆発情報初期化
		pos[i] = XMFLOAT3(CosDeg((72.0f*i) - 90.0f)*100.0f, 10.0f, SinDeg((72.0f*i) - 90.0f)*100.0f);
		m_size[i] = XMFLOAT2(100.0f, 100.0f);
		color[i] = M_DIFFUSE;
	}
	count = 0;

	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void Star::Uninit(void)
{
	for (int i = 0; i < maxStar; i++) {
		ReleaseMesh(&g_mesh[i]);
	}
}

//=============================================================================
// 更新
//=============================================================================
void Star::Update(int StageNum, int animation)
{
	static float count = 0.0f;
	int x = StageNum - 1;
	for (int i = 0; i < maxStar; i++) {
		pos[i] = XMFLOAT3(CosDeg((i - x)*72.0f - (72.0f / 15.0f*animation) - 90.0f)*100.0f,
			0.0f,
			SinDeg((i - x)*72.0f - (72.0f / 15.0f*animation) - 90.0f)*100.0f);
		if (i < GetProgress()) {
			if (i == x) {
				//選択していて、且つプレイできるステージ
				color[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else {
				//選択はできないが、プレイできるステージ
				color[i] = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			}
		}
		else {
			//選択できないステージ
			color[i] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		}
	}
	if (animation == 0) {
		count += 2.0f;
		pos[x].y = SinDeg(count);
	}
	//バグるので一応
	m_size[0] = XMFLOAT2(100.0f, 100.0f);
}

//=============================================================================
// 描画
//=============================================================================
void Star::Draw(int selectStage)
{
	int NowDraw;
	ID3D11DeviceContext* pDC = GetDeviceContext();

	CLight::Get()->SetDisable();	// 光源無効
	SetBlendState(BS_ALPHABLEND);	// αブレンディング有効
	for (int i = 0; i < maxStar; i++) {
		switch (i)
		{
		case 0:
			NowDraw = OverFix(selectStage + 2);
			break;
		case 1:
			NowDraw = OverFix(selectStage + 3);
			break;
		case 2:
			NowDraw = OverFix(selectStage + 1);
			break;
		case 3:
			NowDraw = OverFix(selectStage + 4);
			break;
		case 4:
			NowDraw = OverFix(selectStage);
			break;
		}
		XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();

		// テクスチャ マトリックス更新
		XMMATRIX mtxTex = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		mtxTex = XMMatrixMultiply(mtxTex, XMMatrixTranslation(0.0f, 0.0f, 0.0f));
		XMStoreFloat4x4(&g_mesh[NowDraw].mtxTexture, mtxTex);


		// ビュー行列の回転成分の転置行列を設定(+拡縮)
		g_mesh[NowDraw].mtxWorld._11 = mView._11 * m_size[NowDraw].x;
		g_mesh[NowDraw].mtxWorld._12 = mView._21 * m_size[NowDraw].x;
		g_mesh[NowDraw].mtxWorld._13 = mView._31 * m_size[NowDraw].x;
		g_mesh[NowDraw].mtxWorld._14 = 0.0f;
		g_mesh[NowDraw].mtxWorld._21 = mView._12 * m_size[NowDraw].y;
		g_mesh[NowDraw].mtxWorld._22 = mView._22 * m_size[NowDraw].y;
		g_mesh[NowDraw].mtxWorld._23 = mView._32 * m_size[NowDraw].y;
		g_mesh[NowDraw].mtxWorld._24 = 0.0f;
		g_mesh[NowDraw].mtxWorld._31 = mView._13;
		g_mesh[NowDraw].mtxWorld._32 = mView._23;
		g_mesh[NowDraw].mtxWorld._33 = mView._33;
		g_mesh[NowDraw].mtxWorld._34 = 0.0f;

		// 位置を反映
		g_mesh[NowDraw].mtxWorld._41 = pos[NowDraw].x;
		g_mesh[NowDraw].mtxWorld._42 = pos[NowDraw].y;
		g_mesh[NowDraw].mtxWorld._43 = pos[NowDraw].z;
		g_mesh[NowDraw].mtxWorld._44 = 1.0f;
		// 色を反映
		g_material[NowDraw].Diffuse = color[NowDraw];

		// 描画
		DrawMesh(pDC, &g_mesh[NowDraw]);
	}
	SetBlendState(BS_NONE);		// αブレンディング無効
	CLight::Get()->SetEnable();	// 光源有効
}


//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT Star::MakeVertex(ID3D11Device* pDevice, int count)
{
	// 一時的な頂点配列を生成
	g_mesh[count].nNumVertex = 4;
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_mesh[count].nNumVertex];

	// 頂点配列の中身を埋める
	pVertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	pVertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	pVertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	pVertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);
	pVertexWk[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWk[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	pVertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	pVertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	pVertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	// 一時的なインデックス配列を生成
	g_mesh[count].nNumIndex = 4;
	int* pIndexWk = new int[4];

	// インデックス配列の中身を埋める
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	// 頂点バッファ/インデックス バッファ生成
	HRESULT hr = MakeMeshVertex(pDevice, &g_mesh[count], pVertexWk, pIndexWk);

	// 一時的な頂点配列/インデックス配列を解放
	delete[] pIndexWk;
	delete[] pVertexWk;
	return hr;
}


int Star::OverFix(int num)
{
	if (num >= 5)
	{
		num -= 5;
	}

	return num;
}