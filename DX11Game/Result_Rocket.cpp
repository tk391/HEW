//=============================================================================
//ゴール（宇宙船）[Result_Rocket.cpp]
//12/11 齋藤成希
//=============================================================================
#include "Result_Rocket.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "input.h"
#include "collision.h"
#include "Camera.h"

#define SPACE_SHIP			"data/model/spaceship2.fbx"

//グローバル変数
static CAssimpModel	g_model;		// モデル

static bool oasobi;

static XMFLOAT4X4	g_mtxWorld;		// ワールドマトリックス

//=============================================================================
//初期化
//=============================================================================
HRESULT Result_Rocket::Init() {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	//メンバ変数の初期化
	m_Pos = XMFLOAT3(0.0f, 0.0f, 500.0f);
	m_Rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scl = XMFLOAT3(8.0f, 8.0f, 8.0f);
	// モデルデータの読み込み
	if (!g_model.Load(pDevice, pDeviceContext, SPACE_SHIP)) {
		MessageBoxA(GetMainWnd(), "モデルデータ読み込みエラー", "InitModel", MB_OK);
		return E_FAIL;
	}

	oasobi = false;

	return hr;
}
//=============================================================================
//終了
//=============================================================================
void Result_Rocket::Uninit() {
	// モデルの解放
	g_model.Release();
}

//=============================================================================
//更新
//=============================================================================
void Result_Rocket::Update() {
	if (!oasobi) {
		m_Pos.z -= 5.0f;
	}
	if (m_Pos.z < -500.0f&&GetJoyTrigger(0, XBOXY)) {
		CCamera::Get()->SetPos(0.0f, 0.0f, -200.0f);
		CCamera::Get()->SetTarget(0.0f, 0.0f, 0.0f);
		m_Pos = XMFLOAT3(1000.0f, -80.0f, 0.0f);
		m_Scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
		m_Rot.y = 90.0f;
		oasobi = true;
	}
	if (oasobi) {
		m_Pos.x -= 5.0f;
	}

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
void Result_Rocket::Draw() {
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

XMFLOAT3 Result_Rocket::GetPos() {
	return m_Pos;
}