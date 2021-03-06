//=============================================================================
//ゴール（宇宙船）[Goal.cpp]
//12/11 齋藤成希
//=============================================================================
#include "Goal.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "input.h"
#include "collision.h"

#define SPACE_SHIP			"data/model/spaceship2.fbx"

//グローバル変数
static CAssimpModel	g_model;		// モデル

static XMFLOAT4X4	g_mtxWorld;		// ワールドマトリックス

//=============================================================================
//初期化
//=============================================================================
HRESULT Goal::Init(int stageNum) {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	switch (stageNum)
	{
	case 1:
		m_Pos = XMFLOAT3(465.0f, -110.0f, 0.0f);
		break;
	case 2:
		m_Pos = XMFLOAT3(517.0f, -186.0f, 0.0f);
		break;
	case 3:
		m_Pos = XMFLOAT3(400.0f, -180.0f, 0.0f);
		break;
	case 4:
		m_Pos = XMFLOAT3(574.0f, -160.0f, 0.0f);
		break;
	case 5:
		m_Pos = XMFLOAT3(149.0f, -340.0f, 0.0f);
		break;
	}
	//メンバ変数の初期化
	m_Rot = XMFLOAT3(0.0f, -90.0f, 0.0f);
	//m_Scl = XMFLOAT3(66.0f, 30.0f, 23.0f);
	m_Scl = XMFLOAT3(8.0f, 8.0f, 8.0f);

	// モデルデータの読み込み
	if (!g_model.Load(pDevice, pDeviceContext, SPACE_SHIP)) {
		MessageBoxA(GetMainWnd(), "モデルデータ読み込みエラー", "InitModel", MB_OK);
		return E_FAIL;
	}
	return hr;
}
//=============================================================================
//終了
//=============================================================================
void Goal::Uninit() {
	// モデルの解放
	g_model.Release();
}

//=============================================================================
//更新
//=============================================================================
void Goal::Update() {
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScl;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	//スケールを反映
	mtxScl = XMMatrixScaling(m_Scl.x, m_Scl.y, m_Scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);


	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rot.x),
		XMConvertToRadians(m_Rot.y), XMConvertToRadians(m_Rot.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



	// 移動を反映
	mtxTranslate = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	// ワールドマトリックス設定
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);
}

//=============================================================================
//描画
//=============================================================================
void Goal::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// 不透明部分を描画
	g_model.Draw(pDC, g_mtxWorld, eOpacityOnly);

	// 半透明部分を描画
	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
	SetZWrite(false);				// Zバッファ更新しない
	g_model.Draw(pDC, g_mtxWorld, eTransparentOnly);
	SetZWrite(true);				// Zバッファ更新する
	SetBlendState(BS_NONE);			// アルファブレンド無効
}

//=============================================================================
//衝突判定
//=============================================================================
bool Goal::Collision(XMFLOAT3 pos, XMFLOAT3 size) {
	XMFLOAT3 Size = g_model.GetBBox();
	//当たり判定の調整
	size.x *= 0.8f;
	size.y *= 0.8f;
	size.z *= 0.8f;
	Size = XMFLOAT3(size.x*m_Scl.x, size.y*m_Scl.y, size.z*m_Scl.z);

	int isGoal = CollisionAABB(pos, size, m_Pos, Size);
	if (isGoal >= 0) {
		return true;
	}
	else {
		return false;
	}
}

//ゴール座標を取らせる
XMFLOAT3& Goal::GetPos() {
	return m_Pos;
}

//ゴール座標をセット
void Goal::SetPos(float posX) {
	m_Pos.x += posX;
}