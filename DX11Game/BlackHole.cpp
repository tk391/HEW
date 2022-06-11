//=============================================================================
//ブラックホール
//1/3 三輪卓弘
//=============================================================================
#include "BlackHole.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "collision.h"
#include <vector>
#include "input.h"
#include "main.h"
#include "texture.h"
#include "mesh.h"
#include "Light.h"

//マクロ
#define BlackHoleTexture			"data/texture/brackholl_T_Sprit.png"
#define BLACKHOLE_SIZE		90.0f			//ブラックホールの大きさ(縦横)

#define BLACKHOLE_COUNT_X		4		//画像の横のフレーム枚数
#define BLACKHOLE_COUNT_Y		4		//画像の縦のフレーム枚数
#define BLACKHOLE_FLAME_COUNT	5	//何フレームに一度アニメーションを更新するか



//ブラックホールの情報を持つ構造体
struct TBlackHole {
	XMFLOAT3	m_Pos;		// 位置
	float		m_Size;			// ブラックホールの大きさ(縦横)
	XMFLOAT4	m_Col;		// 色
	int			anim;			//アニメーション再生位置
	int			count;			//アニメーションフレーム数
};


//グローバル変数
static MESH g_mesh;							//メッシュ情報
static MATERIAL g_material;				//マテリアル
static ID3D11ShaderResourceView* g_pTexture;	//テクスチャ情報
static std::vector<TBlackHole> BlackHoleArray;	//ブラックホールを管理する動的配列


//=============================================================================
//初期化処理
//=============================================================================
HRESULT BlackHole::Init(int stageNum) {
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	// 頂点情報の作成
	hr = MakeVertex(pDevice);
	if (FAILED(hr)) {
		MessageBoxA(GetMainWnd(), "ブラックホール読み込みエラー", "InitBlackHole", MB_OK);
		return hr;
	}

	// マテリアル
	g_material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_material.Power = 0.0f;
	g_mesh.pMaterial = &g_material;

	// テクスチャ読み込み
	hr = CreateTextureFromFile(pDevice, BlackHoleTexture, &g_mesh.pTexture);
	if (FAILED(hr)) {
		return hr;
	}
	XMStoreFloat4x4(&g_mesh.mtxTexture, XMMatrixIdentity());

	//ワールドマトリクス初期化
	XMStoreFloat4x4(&g_mesh.mtxWorld, XMMatrixIdentity());

	//初期化
	Create(stageNum);

	return hr;
}

//=============================================================================
//終了
//=============================================================================
void BlackHole::Uninit(void) {
	// メッシュの開放
	ReleaseMesh(&g_mesh);
	
	//動的配列の解放
	BlackHoleArray.clear();
}

//=============================================================================
//更新
//=============================================================================
void BlackHole::Update(void) {
	//	XMMATRIX mtxWorld, mtxScl, mtxPos;
	ID3D11Device* pDevice = GetDevice();
	//イテレーター
	std::vector<TBlackHole>::iterator it;
	it = BlackHoleArray.begin();
	while (it != BlackHoleArray.end()) {
		//カウンタ更新
		it->count--;
		if (it->count <= 0) { //フレームが最後まで進んだら
			it->anim = (it->anim + 1) % (BLACKHOLE_COUNT_X * BLACKHOLE_COUNT_Y); // it->anim + 1の値がフレーム総数に達していなかったら (達していた場合、0にリセット)
			it->count = BLACKHOLE_FLAME_COUNT;
		}
		it++;
	}
}

//=============================================================================
//描画
//=============================================================================
void BlackHole::Draw(void)
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	//XMMATRIX mtxWorld, mtxScale, mtxTranslate;

	CLight::Get()->SetDisable();	// 光源無効
	SetBlendState(BS_ALPHABLEND);	// αブレンディング有効

	// ビューマトリックスを取得
	XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();

	//イテレーター
	std::vector<TBlackHole>::iterator it;
	it = BlackHoleArray.begin();
	while (it != BlackHoleArray.end()) {

		// テクスチャ マトリックス更新
		XMMATRIX mtxTex = XMMatrixScaling(1.0f / BLACKHOLE_COUNT_X, 1.0f / BLACKHOLE_COUNT_Y, 1.0f);
		int u = it->anim % BLACKHOLE_COUNT_X;
		int v = it->anim / BLACKHOLE_COUNT_X;
		mtxTex = XMMatrixMultiply(mtxTex, XMMatrixTranslation((float)u / BLACKHOLE_COUNT_X, (float)v / BLACKHOLE_COUNT_Y, 0.0f));
		XMStoreFloat4x4(&g_mesh.mtxTexture, mtxTex);

		//ビュー行列の回転部分の転置行列を設定
		g_mesh.mtxWorld._11 = mView._11 * it->m_Size;
		g_mesh.mtxWorld._12 = mView._21 * it->m_Size;
		g_mesh.mtxWorld._13 = mView._31 * it->m_Size;
		g_mesh.mtxWorld._14 = 0.0f;
		g_mesh.mtxWorld._21 = mView._12 * it->m_Size;
		g_mesh.mtxWorld._22 = mView._22 * it->m_Size;
		g_mesh.mtxWorld._23 = mView._32 * it->m_Size;
		g_mesh.mtxWorld._24 = 0.0f;
		g_mesh.mtxWorld._31 = mView._13;
		g_mesh.mtxWorld._32 = mView._23;
		g_mesh.mtxWorld._33 = mView._33;
		g_mesh.mtxWorld._34 = 0.0f;

		//位置を反映
		g_mesh.mtxWorld._41 = it->m_Pos.x;
		g_mesh.mtxWorld._42 = it->m_Pos.y;
		g_mesh.mtxWorld._43 = it->m_Pos.z;
		g_mesh.mtxWorld._44 = 1.0f;

		// ポリゴンの描画
		DrawMesh(pDC, &g_mesh);
		it++;
	}

	//SetBlendState(BS_NONE);		// αブレンディング無効
	CLight::Get()->SetEnable();	// 光源有効
}

//=============================================================================
//衝突判定 戻り値:衝突したブラックホールの座標
//=============================================================================
XMFLOAT4 BlackHole::Collision(XMFLOAT3 pos, XMFLOAT3 size) {
	//イテレーター
	std::vector<TBlackHole>::iterator it;
	it = BlackHoleArray.begin();
	int count = 0;
	while (it != BlackHoleArray.end()) {
		float m_Size = it->m_Size * 0.7f;
		bool collisilon = CollisionSphere(it->m_Pos, m_Size, pos, size.y); //対象物とブラックホールが接触していたら
		if (collisilon == false) { // 対象物と接触していない場合、次の要素へ進行する
			it++;
			count++;
			continue;
		}

		//接触したブラックホールの座標を返す
		return XMFLOAT4(it->m_Pos.x, it->m_Pos.y, it->m_Pos.z, 1);
	}
	return XMFLOAT4(-1, -1, -1, -1);
}

//=============================================================================
//情報のセット
//=============================================================================
void BlackHole::Create(int StageNum) {
	switch (StageNum) {
	case 1:
		//ステージ5で必要なブラックホールを生成していく
		//                               位置　　　　　　　　　　　大きさ　　　　　色
		BlackHoleArray.push_back({ XMFLOAT3(-354000.0f, -63.0f, 0.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	case 2:
		//ステージ5で必要なブラックホールを生成していく
		//                               位置　　　　　　　　　　　大きさ　　　　　色
		BlackHoleArray.push_back({ XMFLOAT3(-354000.0f, -332.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	case 3:
		//ステージ5で必要なブラックホールを生成していく
		//                               位置　　　　　　　　　　　大きさ　　　　　色
		BlackHoleArray.push_back({ XMFLOAT3(-354000.0f, -332.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	case 4:
		//ステージ5で必要なブラックホールを生成していく
		//                               位置　　　　　　　　　　　大きさ　　　　　色
		BlackHoleArray.push_back({ XMFLOAT3(-354000.0f, -332.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	case 5:
		//ステージ5で必要なブラックホールを生成していく
		//                               位置　　　　　　　　　　　大きさ　　　　　色
		BlackHoleArray.push_back({ XMFLOAT3(-354.0f, -332.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		BlackHoleArray.push_back({ XMFLOAT3(-164.0f,  -64.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		BlackHoleArray.push_back({ XMFLOAT3(150.0f, -130.0f, 10.0f),BLACKHOLE_SIZE, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),0, BLACKHOLE_FLAME_COUNT });
		break;
	default:
		break;
	}
}

//=============================================================================
//頂点情報の作成
//=============================================================================
HRESULT BlackHole::MakeVertex(ID3D11Device* pDevice) {
	//一時的な頂点配列を生成
	g_mesh.nNumVertex = 4;
	VERTEX_3D* pVertexWK = new VERTEX_3D[g_mesh.nNumVertex];

	//頂点配列の中身を埋める
	pVertexWK[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	pVertexWK[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	pVertexWK[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	pVertexWK[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);
	pVertexWK[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[0].tex = XMFLOAT2(0.0f, 0.0f);
	pVertexWK[1].tex = XMFLOAT2(1.0f, 0.0f);
	pVertexWK[2].tex = XMFLOAT2(0.0f, 1.0f);
	pVertexWK[3].tex = XMFLOAT2(1.0f, 1.0f);

	//一時的なインデックス配列を生成
	g_mesh.nNumIndex = 4;
	int* pIndexWK = new int[4];

	//インデックス配列の中身を埋める
	for (int i = 0; i < 4; i++) {
		pIndexWK[i] = i;
	}

	//頂点バッファ/インデックスバッファ生成
	HRESULT hr = MakeMeshVertex(pDevice, &g_mesh, pVertexWK, pIndexWK);

	//一時的な頂点配列/インデックス配列を解放
	delete[] pIndexWK;
	delete[] pVertexWK;
	return hr;
}