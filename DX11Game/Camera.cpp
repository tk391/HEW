//=============================================================================
// カメラ クラス [Camera.cpp]
// Author : HIROHIKO HAMAYA
//多少の編集:齋藤成希
//=============================================================================
#include "Camera.h"
#include "input.h"
#include "debugproc.h"
#include "game.h"
#include "Player.h"


//=============================================================================
// グローバル変数
//=============================================================================
namespace {
	//いじることになるかもしれない範囲
	const float CAM_POS_P_X = 0.0f;					// カメラの視点初期位置(X座標)
	const float CAM_POS_P_Y = 0.0f;				// カメラの視点初期位置(Y座標)
	const float CAM_POS_P_Z = -200.0f;				// カメラの視点初期位置(Z座標)
	const float CAM_POS_R_X = 0.0f;					// カメラの注視点初期位置(X座標)
	const float CAM_POS_R_Y = 0.0f;					// カメラの注視点初期位置(Y座標)
	const float CAM_POS_R_Z = 0.0f;					// カメラの注視点初期位置(Z座標)

	//ここからは多分いじらない
	const float VIEW_ANGLE = 45.0f;					// ビュー平面の視野角
	const float VIEW_ASPECT = (float)SCREEN_WIDTH / SCREEN_HEIGHT;	// ビュー平面のアスペクト比
	const float VIEW_NEAR_Z = 10.0f;				// ビュー平面のNearZ値
	const float VIEW_FAR_Z = 1000.0f;				// ビュー平面のFarZ値
	const float VALUE_MOVE_CAMERA = 2.0f;			// カメラの移動量
	const float VALUE_ROTATE_CAMERA = 1.8f;			// カメラの回転量

	const float INTERVAL_CAMERA_R = 12.5f;			// モデルの視線の先までの距離
	const float RATE_CHASE_CAMERA_P = 0.35f;		// カメラの視点への補正係数
	const float RATE_CHASE_CAMERA_R = 0.20f;		// カメラの注視点への補正係数

	const float CHASE_HEIGHT_P = 100.0f;			// 追跡時の視点の高さ
	const float CHASE_HEIGHT_R = 10.0f;				// 追跡時の注視点の高さ

	CCamera g_camera;								// カメラ インスタンス
}

CCamera* CCamera::m_pCamera = &g_camera;			// 現在のカメラ
Player* Camera_Player;	//ターゲットにするプレイヤークラス

//=============================================================================
// コンストラクタ
//=============================================================================
CCamera::CCamera()
{
	Init();
}

//=============================================================================
// 初期化
//=============================================================================
void CCamera::Init()
{
	m_vPos = XMFLOAT3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);	// 視点
	m_vTarget = XMFLOAT3(CAM_POS_R_X, CAM_POS_R_Y, CAM_POS_R_Z);// 注視点
	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);							// 上方ベクトル
	m_vDestPos = m_vPos;
	m_vDestTarget = m_vTarget;
	m_vVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_vAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vDestAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vDestTargetAngle = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_fAspectRatio = VIEW_ASPECT;		// 縦横比
	m_fFovY = VIEW_ANGLE;				// 視野角(単位:Degree)
	m_fNearZ = VIEW_NEAR_Z;				// 前方クリップ距離
	m_fFarZ = VIEW_FAR_Z;				// 後方クリップ距離

	float dx, dz;
	dx = m_vPos.x - m_vTarget.x;
	dz = m_vPos.z - m_vTarget.z;
	m_fLengthInterval = sqrtf(dx * dx + dz * dz);

	CalcWorldMatrix();

	//プレイヤークラスのポインタ取得
	Camera_Player = GetPlayer();
}

//=============================================================================
// 更新
//=============================================================================
void CCamera::Update()
{
	//シーンによって内容を変更（できたらいいな）
	// 追跡カメラ
	//XMFLOAT3& vModelPos = Camera_Player->GetPos();	// モデル座標
	/*m_vDestPos.x = CAM_POS_P_X + vModelPos.x;
	m_vDestPos.y = CAM_POS_P_Y + vModelPos.y;
	m_vDestPos.z = CAM_POS_P_Z + vModelPos.z;
	m_vDestTarget.x = CAM_POS_R_X + vModelPos.x;
	m_vDestTarget.y = CAM_POS_R_Y + vModelPos.y;
	m_vDestTarget.z = CAM_POS_R_Z + vModelPos.z;
	m_vPos.x = m_vPos.x * 0.9f + m_vDestPos.x * 0.1f;
	m_vPos.y = m_vPos.y * 0.9f + m_vDestPos.y * 0.1f;
	m_vPos.z = m_vPos.z * 0.9f + m_vDestPos.z * 0.1f;
	m_vTarget.x = m_vTarget.x * 0.9f + m_vDestTarget.x * 0.1f;
	m_vTarget.y = m_vTarget.y * 0.9f + m_vDestTarget.y * 0.1f;
	m_vTarget.z = m_vTarget.z * 0.9f + m_vDestTarget.z * 0.1f;*/

	/*m_vPos.x = CAM_POS_P_X + vModelPos.x;
	m_vPos.y = CAM_POS_P_Y + vModelPos.y;
	m_vPos.z = CAM_POS_P_Z + vModelPos.z;
	m_vTarget.x = CAM_POS_R_X + vModelPos.x;
	m_vTarget.y = CAM_POS_R_Y + vModelPos.y;
	m_vTarget.z = CAM_POS_R_Z + vModelPos.z;*/

	// マトリックス更新
	UpdateMatrix();

#if _DEBUG
	PrintDebugProc("[ｶﾒﾗ ｲﾁ : (%f, %f, %f)]\n", m_vPos.x, m_vPos.y, m_vPos.z);
	PrintDebugProc("[ﾁｭｳｼﾃﾝ : (%f, %f, %f)]\n", m_vTarget.x, m_vTarget.y, m_vTarget.z);
	PrintDebugProc("\n");
#endif // _DEBUG

}

//=============================================================================
// ビュー/プロジェクション マトリックス更新
//=============================================================================
void CCamera::UpdateMatrix()
{
	XMStoreFloat4x4(&m_mtxView, XMMatrixLookAtLH(
		XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp)));
	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovY), m_fAspectRatio, m_fNearZ, m_fFarZ));
}

//=============================================================================
// ワールド マトリックス設定
//=============================================================================
void CCamera::SetWorldMatrix(XMFLOAT4X4& mtxWorld)
{
	m_mtxWorld = mtxWorld;
	m_vPos = XMFLOAT3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
	m_vTarget = XMFLOAT3(mtxWorld._41 + mtxWorld._31, mtxWorld._42 + mtxWorld._32, mtxWorld._43 + mtxWorld._33);
	m_vUp = XMFLOAT3(mtxWorld._21, mtxWorld._22, mtxWorld._23);
}

//=============================================================================
// 視点/注視点/上方ベクトルからワールド マトリックス算出
//=============================================================================
XMFLOAT4X4& CCamera::CalcWorldMatrix()
{
	XMVECTOR vecZ = XMVectorSet(m_vTarget.x - m_vPos.x, m_vTarget.y - m_vPos.y, m_vTarget.z - m_vPos.z, 0.0f);
	XMFLOAT3 vZ;
	XMStoreFloat3(&vZ, XMVector3Normalize(vecZ));
	XMVECTOR vecY = XMLoadFloat3(&m_vUp);
	XMVECTOR vecX = XMVector3Normalize(XMVector3Cross(vecY, vecZ));
	XMFLOAT3 vX;
	XMStoreFloat3(&vX, vecX);
	vecY = XMVector3Normalize(XMVector3Cross(vecZ, vecX));
	XMFLOAT3 vY;
	XMStoreFloat3(&vY, vecY);

	m_mtxWorld._11 = vX.x;
	m_mtxWorld._12 = vX.y;
	m_mtxWorld._13 = vX.z;
	m_mtxWorld._14 = 0.0f;
	m_mtxWorld._21 = vY.x;
	m_mtxWorld._22 = vY.y;
	m_mtxWorld._23 = vY.z;
	m_mtxWorld._24 = 0.0f;
	m_mtxWorld._31 = vZ.x;
	m_mtxWorld._32 = vZ.y;
	m_mtxWorld._33 = vZ.z;
	m_mtxWorld._34 = 0.0f;
	m_mtxWorld._41 = m_vPos.x;
	m_mtxWorld._42 = m_vPos.y;
	m_mtxWorld._43 = m_vPos.z;
	m_mtxWorld._44 = 1.0f;

	return m_mtxWorld;
}
