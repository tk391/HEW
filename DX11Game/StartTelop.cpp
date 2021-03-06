#include"StartTelop.h"
#include"Texture.h"
#include"polygon.h"
#include "debugproc.h"
#include"Light.h"
#include"Scene.h"
#include"Camera.h"
#include"Life.h"
#include"Player.h"
#include"Game.h"
#include "Sound.h"
//デバッグ確認用
#include "input.h"

//=====================================================
//マクロ定義
//=====================================================
#define MAX_MTEXTURE (1)
#define PATH_GAMESTART_TEXTURE  L"data/texture/start.png"
#define TEXTURE_SIZEX  (329.0f * 1.25f) //画像サイズ
#define TEXTURE_SIZEY	(98.0f * 1.25f)

#define CENTER_POS_X	(0.0f)	//中心座標X
#define CENTER_POS_Y	(0.0f)	//中心座標Y

#define ANIM_FIRST (100)					//アニメーション1 カメラ接近
#define ANIM_SECOND	(ANIM_FIRST - 30)		//アニメーション2 テロップくるくる (カメラ接近と同時)
#define ANIM_THIRD  (ANIM_FIRST + 30)		//アニメーション3 テロップ一時停止
#define ANIM_FOURTH  (ANIM_THIRD + 30)		//アニメーション4 テロップフェードアウト

//=====================================================
//グローバル変数
//=====================================================
static ID3D11ShaderResourceView* g_pTexture;

static XMFLOAT2 g_vPos;				//テロップの場所
static XMFLOAT2 g_vSize;			//テロップの大きさ
static float	g_fAngle;			//テロップの回転度

static float g_fAnimProgress;		//アニメーションの進行度(時間)

static XMFLOAT3			g_CameraPos;			//カメラ位置
static XMFLOAT3			g_DeffCameraPos;		//カメラ位置
static XMFLOAT3			g_PlayerCameraPos;		//プレイヤー位置
static XMFLOAT3			g_sabun;				//プレイヤー位置とデフォルトカメラ位置の差分

static bool					g_bAnimFlag;			//現在アニメーション再生中か

Player* Start_Player;
Life* Start_Life;			//Lifeクラス



//=============================================================================
//初期化
//=============================================================================
HRESULT StartTelop::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	//テクスチャ読み込み
	hr = CreateTextureFromFile(pDevice, PATH_GAMESTART_TEXTURE, &g_pTexture);
	if (FAILED(hr))
	{
		return hr;
	}

	g_fAnimProgress = 0;	//アニメーションのフレーム数

	Start_Player = GetPlayer();

	g_vPos = XMFLOAT2(CENTER_POS_X, CENTER_POS_Y);			//テロップの場所
	g_vSize = XMFLOAT2(TEXTURE_SIZEX, TEXTURE_SIZEY);		//テロップの大きさ
	m_Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);				//テロップの色

	g_CameraPos = XMFLOAT3(0.0f, 0.0f, -1000.0f);			//アニメーション開始時のカメラ初期位置
	g_DeffCameraPos = XMFLOAT3(0.0f, 0.0f, -1000.0f);		//アニメーション開始時のカメラ初期位置
	g_PlayerCameraPos = Start_Player->GetPos();				//プレイヤーのカメラ位置
	g_PlayerCameraPos.z -= 350.0f;							//プレイヤー位置にカメラの距離を追加
	// カメラメインポジションとカメラプレイヤーポジションの差分
	g_sabun = XMFLOAT3(g_DeffCameraPos.x - g_PlayerCameraPos.x,
					   g_DeffCameraPos.y - g_PlayerCameraPos.y,
				       g_DeffCameraPos.z - g_PlayerCameraPos.z);
	//Start_Life->GetLife();
	/*diff = XMFLOAT3(g_DeffCameraPos.x - g_PlayerPos.x, g_DeffCameraPos.y - g_PlayerPos.y, g_DeffCameraPos.z - (g_PlayerPos.z - 200.0f));
*/

	//カメラのセット
	CCamera::Get()->SetPos(0.0f, 0.0f, 1000.0f);
//CCamera::Get()->SetTarget(m_Pos);


	g_fAngle = 100;
	return hr;
}
//=============================================================================
//終了
//=============================================================================
void StartTelop::Uninit()
{
	for (int i = 0; i < MAX_MTEXTURE; ++i)
	{
		SAFE_RELEASE(g_pTexture);
	}
}
//=============================================================================
//更新
//=============================================================================
void StartTelop::Update()
{
	//3つめのアニメーションが終わった瞬間SEがなる
	if(g_fAnimProgress == ANIM_THIRD)
	{
		CSound::Stop(SE_START);
		CSound::Play(SE_START);
	}
	if (g_fAnimProgress <= ANIM_FIRST)
	{
		g_CameraPos.x = g_DeffCameraPos.x - (g_sabun.x * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST));
		g_CameraPos.y = g_DeffCameraPos.y - (g_sabun.y * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST));
		g_CameraPos.z = g_DeffCameraPos.z - (g_sabun.z * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST));
		CCamera::Get()->SetTarget(g_CameraPos.x, g_CameraPos.y, 0.0f);
		if (ANIM_SECOND <= g_fAnimProgress && g_fAnimProgress < ANIM_FIRST)	//テロップがぐるぐるしながらフェードインする
		{
			g_vSize = XMFLOAT2(TEXTURE_SIZEX * (ANIM_FIRST) / g_fAnimProgress, TEXTURE_SIZEY * (ANIM_FIRST) / g_fAnimProgress);
			g_fAngle = (ANIM_FIRST) * 10 - g_fAnimProgress * 10 - 10;
			m_Color.w = (g_fAnimProgress - ANIM_SECOND) / (ANIM_FIRST - ANIM_SECOND);

		}
	}
	if (ANIM_FIRST <= g_fAnimProgress && g_fAnimProgress < ANIM_THIRD)
	{
		m_Color.w = 1;
		g_vSize = XMFLOAT2(TEXTURE_SIZEX, TEXTURE_SIZEY);
	}
	if (ANIM_THIRD <= g_fAnimProgress && g_fAnimProgress <= ANIM_FOURTH)
	{//ポリゴンの拡大処理
		g_vSize.x += TEXTURE_SIZEX * 0.03f;
		g_vSize.y += TEXTURE_SIZEY * 0.03f;

		m_Color.w = 1 - (g_fAnimProgress - ANIM_THIRD) / (ANIM_FOURTH - ANIM_THIRD);		//ポリゴンの不透明度が1⇒0
	}

	//ゲームが開始したら流れる
	//演出が流れている間はライフが減らないように調整、テロップが終了した後にライフが減るように
	if (g_fAnimProgress < ANIM_FOURTH)	//アニメーションの最後のフレームを越していなければ
	{
		g_fAnimProgress++;						//アニメーションを増やす
		CCamera::Get()->SetPos(g_CameraPos);	//カメラを更新
		Start_Player->SetAnimTelopFlag(true);	//アニメーションフラグON
	}
	else //そうでない場合、テロップ再生フラグをfalseに
	{
		Start_Player->SetAnimTelopFlag(false);
	}
		
}
//=============================================================================
//描画
//=============================================================================
void StartTelop::Draw()
{
		ID3D11DeviceContext* pDC = GetDeviceContext();
		//スタートテロップ
		SetPolygonSize(g_vSize.x, g_vSize.y);
		SetPolygonPos(g_vPos.x, g_vPos.y);
		SetPolygonAlpha(m_Color.w);
		SetPolygonAngle(g_fAngle);
		SetPolygonTexture(g_pTexture);
		SetPolygonUV(0.0f, 0.0f);
		DrawPolygon(pDC);

		//元に戻す
		SetPolygonFrameSize(1.0f, 1.0f);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonAngle(0.0f);
}
