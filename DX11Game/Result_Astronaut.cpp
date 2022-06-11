//=============================================================================
//ゴール（宇宙船）[Result_Astronaut.cpp]
//12/11 齋藤成希
//=============================================================================
#include "Result_Astronaut.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "input.h"
#include "collision.h"

#define SPACE_SHIP			"data/model/sitdown.fbx"

//グローバル変数
static CAssimpModel	g_model;		// モデル

static XMFLOAT4X4		g_mtxWorld;		// ワールドマトリックス

//=============================================================================
//初期化
//=============================================================================
HRESULT Result_Astronaut::Init() {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	//メンバ変数の初期化
	m_Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Rot = XMFLOAT3(-5.0f, -30.0f, 0.0f);
	m_Scl = XMFLOAT3(2.0f, 2.0f, 2.0f);

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
void Result_Astronaut::Uninit() {
	// モデルの解放
	g_model.Release();
}

//=============================================================================
//更新
//=============================================================================
void Result_Astronaut::Update() {

#if _DEBUG
	PrintDebugProc("pos.z=%.2f\n", m_Pos.z);
#endif

	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScl;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	//スケールを反映
	mtxScl = XMMatrixScaling(m_Scl.x, m_Scl.y, m_Scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rot.x), XMConvertToRadians(m_Rot.y), XMConvertToRadians(m_Rot.z));
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
void Result_Astronaut::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// 不透明部分を描画
	g_model.Draw(pDC, g_mtxWorld, eOpacityOnly);

	// 半透明部分を描画
	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
	SetZWrite(false);							// Zバッファ更新しない
	g_model.Draw(pDC, g_mtxWorld, eTransparentOnly);
	SetZWrite(true);							// Zバッファ更新する
	SetBlendState(BS_NONE);			// アルファブレンド無効
}

XMFLOAT3 Result_Astronaut::GetPos() {
	return m_Pos;
}