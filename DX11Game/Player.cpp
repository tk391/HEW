//=============================================================================
// プレイヤー（宇宙飛行士）
// 12/11 齋藤成希
//=============================================================================
#include "Player.h"
#include "main.h"
#include "Scene.h"
#include "input.h"
#include "AssimpModel.h"
#include "Game.h"
#include "debugproc.h"
#include "Life.h"
#include "Sound.h"
#include "fade.h"
#include "AlertBg.h"
#include "BlackHole.h"
#include "Select.h"
#include <stdlib.h>	//絶対値を求めるための関数を使うために　0129	三輪卓弘

// マクロ定義

#define	VALUE_MOVE_PLAYER		(0.20f)		// 移動速度
#define	RATE_MOVE_PLAYER		(0.20f)		// 移動慣性係数
#define	VALUE_ROTATE_PLAYER		(3.5f)		// 回転速度
#define	RATE_ROTATE_PLAYER		(0.20f)		// 回転慣性係数
#define VALUE_ROWMOVE_PLAYER	(0.3f)		//減速時の移動速度

#define PLAYER_SIZE_X		(7.0f)			//サイズ
#define PLAYER_SIZE_Y		(7.0f)			//サイズ

#define MAX_LIFE			(100.0f)		//ライフ最大数(※固定)

//難易度系(調整求む)==============================================================================================
//================================================================================================================

//※ココ大事!!　ゲームプレイの手触り感に大きく影響する要素。ストレスフリーだけどしっかりとふわふわ感を感じられるのが理想
#define INFLUENCE			(0.1f)			//慣性影響力(数値が高いほど移動速度が反映されやすい)  1/25 宗像 数値変更
#define MAX_MOVE			(1.3f)			//移動速度の上限値					0129	三輪卓弘追加	



#define DEL_JET				(0.06f)			//ライフ減少率(ジェット使用中)		  1/25 宗像 数値変更
#define DEL_STAY			(0.02f)			//ライフ減少率(ジェット未使用中)	  1/25 宗像 数値変更
#define DEFF_CAMERA_Z		(350.0f)		//カメラのデフォルトZ座標 (プレイヤーとの距離)   1/25 宗像 数値変更

#define BLACKHOLE_POWER		(0.02f)			//ブラックホールの吸引力
#define REFLECTION			(0.6f)			//デブリの反射力

// グローバル変数

static CAssimpModel		g_model[2];		// モデル
static XMFLOAT4X4		g_mtxWorld;			// ワールドマトリックス
static int				g_nShadow;				// 丸影番号
static XMFLOAT3			g_Size;				// 当たり判定用のサイズ
static float			g_RotXPrev;					// 1フレーム前の角度
static int				g_nCountFrame;		// アニメーションを加算するフレーム
static double			g_d1FrameAnim;		// 1フレーム当たりのアニメーション
static double			g_dSetAnim;				// 上記2変数をかけた数->SetAnimTime()に入れる
static XMFLOAT3			g_effectPos;		// エフェクト用位置
static XMFLOAT3			g_CameraPos;	// カメラ位置
static int				g_nBlackHoleTime;	// ブラックホールの時間カウント
static bool				g_bEffectJetFlag;	// ジェットを押したかのフラグ
static bool				g_bOldEffectJetFlag;	// ジェットを押したかのフラグ

static XMFLOAT3			g_OldPos;			// 前までプレイヤーが居た位置	01/17	佐藤大地
static bool				g_RefFlg;					// 反射方向の設定				01/18	佐藤大地
static bool				g_StartRef;				// 反射開始						01/18	佐藤大地
static XMFLOAT3			g_OldMove;		// 前までの移動量				01/18	佐藤大地

static bool				g_bClearGameOverFlag[2];		//クリア又はゲームオーバーしたかを判定
static bool				g_goalCollisionFlag;				//ゴールとプレイヤーが完全に触れているか(クリア時に使用)
static bool				g_goalCollisionSEFlag;					//ゴールした瞬間かどうか(クリア時のSEに使用)
static int				gameOver_stopCount;			//ゲームオーバー時に一時停止させる為のカウント
static bool				g_bAnimTelopFlag;					//テロップアニメーションが再生中かどうかのフラグ // 01/21 三輪卓弘
static bool				g_bOutFlag;							//画面外に出たか
static bool				g_bRepatriationFlag;				//強制送還中か
static int				g_nStageNum;						//ステージ番号

//定数
const float AnimationTime = 120.0f;					//アニメーションの再生時間（フレーム数）
const float AleattelopPos = 70.0f;						//酸素が減る位置から何もない位置までの距離
const float AleatRepatriationPos = 100.0f;			//酸素が減る位置から強制送還される位置までの距離

const float warningArea[5][4]{//各ステージの範囲外の境界線→これを超えるとアラートが鳴る
	//上			下			左			右
	{100.0f,-350.0f,-730.0f,700.0f},	//	ステージ1
	{100.0f,-350.0f,-730.0f,700.0f},	//	ステージ2
	{100.0f,-350.0f,-730.0f,700.0f},	//	ステージ3
	{100.0f,-350.0f,-730.0f,700.0f},	//	ステージ4
	{100.0f,-350.0f,-730.0f,700.0f}	//	ステージ5
	//各ステージで調整をお願いします
};
enum Area {//上下左右→warningAreaで使用
	UP = 0, DOWN, LEFT, RIGHT
};
enum anim {//アニメーション
	amimAll = 0, stay, hold, release, jet
};

//モデル
const std::string ModelData[]{
	"data/model/Complete.fbx",
	"data/model/aaaa.fbx"
};

//クラス定義
Life* Player_Life;			//Lifeクラス
Debly* Player_Debly;		//Deblyクラス
Rayer* Player_Rayer;		//Rayerクラス
Meteo* Player_Meteo;
Goal* Player_Goal;
BlackHole* Player_BlackHole;
//============================================
// 初期化処理
//============================================
HRESULT Player::Init(void)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// メンバ変数（グローバル変数）の初期化
	m_Pos = XMFLOAT3(-526.0f, -63.0f, 0.0f);
	m_Move = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Rot = XMFLOAT3(90.0f, -90.0f, 0.0f);
	m_RotDest = XMFLOAT3(0.0f, -75.0f, 0.0f);
	m_Scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_fLife = MAX_LIFE;
	m_bFlag = false;
	m_bPull[0] = false;
	m_bPull[1] = false;
	m_bHold = false;
	g_Size = g_model[0].GetBBox();
	g_RotXPrev = 0.0f;
	g_effectPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_CameraPos = XMFLOAT3(m_Pos.x, m_Pos.y, -DEFF_CAMERA_Z);
	g_bAnimTelopFlag = true;
	g_bOutFlag = false;
	g_bRepatriationFlag = false;

	//反射に関わる関数の初期化
	//01/18	佐藤大地
	g_OldPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_RefFlg = true;
	g_StartRef = false;
	g_OldMove = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 2; i++) {
		g_bClearGameOverFlag[i] = false;
	}
	g_goalCollisionFlag = false;
	gameOver_stopCount = 30;
	m_Accel = XMFLOAT3(0.0f, 0.0f, 0.0f);//12/24 X
	m_OldRot = XMFLOAT3(0.0f, 0.0f, 0.0f); //12 / 26 X
	g_nStageNum = GetStageNum();
	// モデルデータの読み込み
	for (int i = 0; i < 2; i++) {
		if (!g_model[i].Load(pDevice, pDeviceContext, ModelData[i].c_str())) {
			MessageBoxA(GetMainWnd(), "モデルデータ読み込みエラー", "InitModel", MB_OK);
			return E_FAIL;
		}
	}

	//アニメーション管理(初期設定)
	m_nAnimNum = stay;
	g_model[0].SetAnimIndex(m_nAnimNum);
	g_d1FrameAnim = g_model[0].GetAnimDuration(stay) / AnimationTime;
	g_bEffectJetFlag = false;
	g_bOldEffectJetFlag = false;

	//カメラのセット
	CCamera::Get()->SetPos(m_Pos.x, m_Pos.y, m_Pos.z - DEFF_CAMERA_Z);
	CCamera::Get()->SetTarget(m_Pos);

	//このクラスで使用する他のクラスの情報を取得
	Player_Life = GetLife();
	Player_Debly = GetDebly();
	Player_Rayer = GetRayer();
	Player_Meteo = GetMeteo();
	Player_BlackHole = GetBlackHole();
	Player_Goal = GetGoal();

	return hr;
}

//============================================
// 終了処理
//============================================
void Player::Uninit(void)
{
	CSound::Stop(SE_GOSHIP);
	// モデルの解放
	for (int i = 0; i < 2; i++) {
		g_model[0].Release();
	}
}

//============================================
// 更新処理
//============================================
void Player::Update(void)
{
	if (!g_bAnimTelopFlag)
	{
		//当たり判定
		static int nTouchDebly, nTouchDeblyPrev;
		static XMINT2 nTouchMeteo, nTouchMeteoPrev;
		XMFLOAT4 fTouchBlackHole = Player_BlackHole->Collision(m_Pos, g_Size);
		nTouchDebly = Player_Debly->Collision(m_Pos, g_Size);
		nTouchMeteo = Player_Meteo->Collision(m_Pos, g_Size);

		//01/17 佐藤大地
		g_Size = g_model[0].GetBBox();
		g_OldPos = m_Pos;
		//一つ前の角度を格納
		g_RotXPrev = m_RotDest.x;

		// カメラの向き取得
		XMFLOAT3 rotCamera = CCamera::Get()->GetAngle();

		//パッド操作
		XMFLOAT2  JOY = XMFLOAT2((float)GetJoyX(0), (float)GetJoyY(0));//左スティックの座標→角度や発射方向に使う
		float JOYZ = (float)GetJoyZ(0);//ZL,ZRの入力度合いを検出（0未満→ZLを押したとき,0以上→ZRを押したとき）
		//真横や真上にも向けるようにする(あまりに小さい入力は受け付けない)
		if (JOY.x <= 2000.0f&&JOY.x >= -2000.0f) {
			JOY.x = 0.0f;
		}
		if (JOY.y <= 2000.0f&&JOY.y >= -2000.0f) {
			JOY.y = 0.0f;
		}
		if (JOYZ <= 5000.0f&&JOYZ >= -5000.0f) {
			JOYZ = 0.0f;
		}

		//クリア又はゲームオーバーのフラグが立っていたら
		if (g_bClearGameOverFlag[0]) {
			//クリア演出
			GameClearAnimation();

		}
		else if (g_bClearGameOverFlag[1]) {
			//ゲームオーバー演出
			GameOverAnimation();
		}
		//ゲーム中の演出
		else {

			//最初は動かない
			//最初のスペースキーで動き始める
			if (m_bFlag) {
				m_Move.x -= SinDeg(m_Rot.x) * VALUE_MOVE_PLAYER;
				m_Move.y += CosDeg(m_Rot.x) * VALUE_MOVE_PLAYER;
			}

			//角度を検出して格納
			if (JOY.x == 0.0f&&JOY.y == 0.0f) {//ジョイスティックの入力がなかった場合
				m_RotDest.x = g_RotXPrev;
			}
			else {
				m_RotDest.x = RotZ(JOY);
			}

#if _DEBUG
			PrintDebugProc("rot=%.2f,%.2f,%.2f\n", m_Rot.x, m_Rot.y, m_Rot.z);
			PrintDebugProc("rotDest=%.2f,%.2f,%.2f\n", m_RotDest.x, m_RotDest.y, m_RotDest.z);
#endif
			//左右向いている方向で角度に差が出る→調整
			if (JOY.x < 0.0f) {
				//m_RotDest.y = 90.0f;
				m_RotDest.y = 75.0f;
				m_RotDest.x *= -1;
			}
			else {
				//m_RotDest.y = -90.0f;
				m_RotDest.y = -75.0f;
			}

			//XBOXのBボタンを押すと角度変更（修正予定）
			//if(GetJoyButton(0, XBOXA))
			if (GetJoyButton(0, XBOXA))
			{
				//左右向いている方向で角度を調整
				if (JOY.x < 0.0f) {
					m_Rot.x = -(m_RotDest.x) + 90.0f;
				}
				else {
					m_Rot.x = m_RotDest.x - 90.0f;
				}
				m_Rot.y = m_RotDest.y;
				m_Rot.z = m_RotDest.z;
				//ジェット噴射
				m_bFlag = true;
				m_fLife -= DEL_JET;
				m_nAnimNum = jet;
			}
			else {
				//常にライフは減少している
				m_fLife -= DEL_STAY;
				m_bFlag = false;
			}

			// 位置移動
			m_Pos.x += m_Move.x;
			m_Pos.y += m_Move.y;
			m_Pos.z += m_Move.z;

			static XMFLOAT3 Targetpos, RepatirationVector;//範囲外に出る瞬間の位置と強制送還のベクトル
			//****************************
			//範囲外に行くと赤くなる
			//****************************
			int x = g_nStageNum - 1;
			//テロップ＆赤くなる処理を先に出す
			if (m_Pos.y > warningArea[x][UP] - AleattelopPos || m_Pos.y < warningArea[x][DOWN] + AleattelopPos ||
				warningArea[x][LEFT] + AleattelopPos > m_Pos.x || warningArea[x][RIGHT] - AleattelopPos < m_Pos.x) {
				if (!g_bOutFlag) {
					Targetpos = m_Pos;
				}
				g_bOutFlag = true;
				if (m_Pos.y > warningArea[x][UP] || m_Pos.y < warningArea[x][DOWN] || warningArea[x][LEFT] > m_Pos.x || warningArea[x][RIGHT] < m_Pos.x) {
					//エリア外に行ったら酸素へるよ
					m_fLife -= 0.15f;
					//さらに行くと強制送還
					if (m_Pos.y > warningArea[x][UP] + AleatRepatriationPos || m_Pos.y < warningArea[x][DOWN] - AleatRepatriationPos ||
						warningArea[x][LEFT] - AleatRepatriationPos > m_Pos.x || warningArea[x][RIGHT] + AleatRepatriationPos < m_Pos.x) {
						m_Move = XMFLOAT3(0.0f, 0.0f, 0.0f);
						g_bRepatriationFlag = true;
						RepatirationVector = XMFLOAT3(Targetpos.x - m_Pos.x, Targetpos.y - m_Pos.y, Targetpos.z - m_Pos.z);
						XMStoreFloat3(&RepatirationVector, XMVector3Normalize(XMLoadFloat3(&RepatirationVector)));//ベクトルの正規化
					}
				}
			}
			else {
				g_bOutFlag = false;
			}
			AlertWarning(g_bOutFlag);

			//強制送還
			if (g_bRepatriationFlag) {
				Repatriation(RepatirationVector);
			}

			//****************************
			//掴む
			//****************************
			if (GetJoyButton(0, XBOXY)) {
				//掴むフラグ立たせる
				m_bHold = true;
				m_nAnimNum = hold;
			}
			//アニメーションが途中だったら
			else if (m_bHold) {
				//掴むフラグをおろす
				m_bHold = false;
				//m_nAnimNum = release;
			}

			Animator(m_nAnimNum);

			//レイヤーの位置をセット
			Player_Rayer->Set(m_Move.x, m_Move.y, false);

			//ライフが切れたらリザルト
			if (m_fLife <= 0.0f) {
				//StartFadeOut(SCENE_RESULT);
				EffectManager::Get()->End("all");
				m_RotDest = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_bClearGameOverFlag[1] = true;
			}

			// 移動量に慣性をかける
			//m_Move.x += (0.0f - m_Move.x) * RATE_MOVE_PLAYER;
			//m_Move.y += (0.0f - m_Move.y) * RATE_MOVE_PLAYER;
			//m_Move.z += (0.0f - m_Move.z) * RATE_MOVE_PLAYER;
			// プレイヤーの現在のフレームの移動量/角度に1フレーム前の移動量/角度を影響させる 12 / 26 X kokokara
			m_Move.x = ((m_Move.x * INFLUENCE) + (m_Accel.x * (1.0f - INFLUENCE)));
			m_Move.y = ((m_Move.y * INFLUENCE) + (m_Accel.y * (1.0f - INFLUENCE)));
			m_Move.z = ((m_Move.z * INFLUENCE) + (m_Accel.z * (1.0f - INFLUENCE)));

			m_Rot.x = ((m_OldRot.x * INFLUENCE) + (m_Rot.x * (1.0f - INFLUENCE)));
			//		m_Rot.y  = ((m_OldRot.y * INFLUENCE) + (m_Rot.y * (1.0f - INFLUENCE)));
			m_Rot.z = ((m_OldRot.z * INFLUENCE) + (m_Rot.z * (1.0f - INFLUENCE))); //12 / 26 X kokomade

			//当たり判定（仮）
			//反射
			//01/18		佐藤大地

			//デブリに触れたら
			if (nTouchDebly >= 0) {
				XMFLOAT3 Col_Stone_Pos = Player_Debly->GetPos(nTouchDebly);//当たった対象物の位置
				XMFLOAT3 Col_Stone_Vec = Col_Stone_Pos;
				XMFLOAT3 Col_Stone_Size = Player_Debly->GetSize(nTouchDebly);//当たった対象物の大きさ
				Col_Stone_Vec.x = m_Pos.x - Col_Stone_Vec.x;//プレイヤーからデブリのベクトル取得.x
				Col_Stone_Vec.y = m_Pos.y - Col_Stone_Vec.y;//プレイヤーからデブリのベクトル取得.y
				Col_Stone_Pos.x += Col_Stone_Vec.x;
				Col_Stone_Pos.y += Col_Stone_Vec.y;
				Col_Stone_Pos.z += Col_Stone_Vec.z;
				XMStoreFloat3(&Col_Stone_Vec, XMVector3Normalize(XMLoadFloat3(&Col_Stone_Vec)));//ベクトルの正規化
				XMStoreFloat3(&Col_Stone_Size, XMVector3Normalize(XMLoadFloat3(&Col_Stone_Size)));//ベクトルの正規化

				//01/18	佐藤大地
				if (Col_Stone_Size.x < Col_Stone_Vec.x) {
					m_Move.x = Col_Stone_Vec.x * REFLECTION;
					m_Pos.x += Col_Stone_Vec.x * REFLECTION;
				}
				if (Col_Stone_Size.x > Col_Stone_Vec.x) {
					m_Move.x = Col_Stone_Vec.x * REFLECTION;
					m_Pos.x += Col_Stone_Vec.x * REFLECTION;
				}
				if (Col_Stone_Size.y < Col_Stone_Vec.y) {
					m_Move.y = Col_Stone_Vec.y * REFLECTION;
					m_Pos.y += Col_Stone_Vec.y * REFLECTION;
				}
				if (Col_Stone_Size.y > Col_Stone_Vec.y) {
					m_Move.y = Col_Stone_Vec.y * REFLECTION;
					m_Pos.y += Col_Stone_Vec.y * REFLECTION;
				}

				//エフェクト発動
				if (nTouchDebly != nTouchDeblyPrev) {
					EffectManager::Get()->Start("collision", Col_Stone_Pos);
				}
				m_bFlag = false;
				CSound::Stop(SE_COLLISION);
				CSound::Play(SE_COLLISION);
			}
			//隕石に触れたら
			//反射
			PrintDebugProc("ntouchMeteo={%d,%d}\n", nTouchMeteo.x, nTouchMeteo.y);
			//01/18	佐藤大地
			if ((nTouchMeteo.x >= 0) && (nTouchMeteo.y >= 0)) {
				XMFLOAT3 Col_Stone_Pos = Player_Meteo->GetPos(nTouchMeteo);//当たった対象物の位置
				XMFLOAT3 Col_Stone_Size = Player_Meteo->GetSize(nTouchMeteo);//当たった対象物の大きさ
				XMFLOAT3 Col_Stone_Vec = Col_Stone_Pos;
				Col_Stone_Vec.x = m_Pos.x - Col_Stone_Vec.x;//プレイヤーからデブリのベクトル取得.x
				Col_Stone_Vec.y = m_Pos.y - Col_Stone_Vec.y;//プレイヤーからデブリのベクトル取得.y
				Col_Stone_Pos.x += Col_Stone_Vec.x;
				Col_Stone_Pos.y += Col_Stone_Vec.y;
				Col_Stone_Pos.z += Col_Stone_Vec.z;
				XMStoreFloat3(&Col_Stone_Vec, XMVector3Normalize(XMLoadFloat3(&Col_Stone_Vec)));//ベクトルの正規化
				//XMStoreFloat3(&Col_Stone_Size, XMVector3Normalize(XMLoadFloat3(&Col_Stone_Size)));//ベクトルの正規化
				if (m_bHold) {
					m_Move = Player_Meteo->GetVector(nTouchMeteo);
				}
				else {
					//01/18	佐藤大地
					if (Col_Stone_Size.x < Col_Stone_Vec.x) {
						m_Move.x = Col_Stone_Vec.x * REFLECTION;
						m_Pos.x += Col_Stone_Vec.x * REFLECTION*5;
					}
					if (Col_Stone_Size.x > Col_Stone_Vec.x) {
						m_Move.x = Col_Stone_Vec.x * REFLECTION;
						m_Pos.x += Col_Stone_Vec.x * REFLECTION*5;
					}
					if (Col_Stone_Size.y < Col_Stone_Vec.y) {
						m_Move.y = Col_Stone_Vec.y * REFLECTION;
						m_Pos.y += Col_Stone_Vec.y * REFLECTION*5;
					}
					if (Col_Stone_Size.y > Col_Stone_Vec.y) {
						m_Move.y = Col_Stone_Vec.y * REFLECTION;
						m_Pos.y += Col_Stone_Vec.y * REFLECTION*5;
					}

					//エフェクト発動
					if ((nTouchMeteo.x != nTouchMeteoPrev.x) && (nTouchMeteo.y != nTouchMeteoPrev.y)) {
						EffectManager::Get()->Start("collision", Col_Stone_Pos);
					}
					CSound::Stop(SE_COLLISION);
					CSound::Play(SE_COLLISION);
				}
				m_bFlag = false;
			}
			//ブラックホールに触れたら
			if (fTouchBlackHole.w >= 1) {
#if _DEBUG
				PrintDebugProc("Hit BlackHole\n");
#endif
				XMFLOAT3 BlackHoleVector;
				BlackHoleVector.x = m_Pos.x - fTouchBlackHole.x;
				BlackHoleVector.y = m_Pos.y - fTouchBlackHole.y;
				BlackHoleVector.z = 0.0f;
				//正規化
				XMStoreFloat3(&BlackHoleVector, XMVector3Normalize(XMLoadFloat3(&BlackHoleVector)));//ベクトルの正規化
				//強さをかける
				m_Move.x -= BlackHoleVector.x * 0.02f;
				m_Move.y -= BlackHoleVector.y * 0.02f;
				//CSound::SetVolume(SE_BLACKHOLE, 100.0f);
				//CSound::Stop(SE_BLACKHOLE);
				//CSound::Play(SE_BLACKHOLE);

				//ブラックホールの音
				g_nBlackHoleTime++;
				if (g_nBlackHoleTime >= 60) {
					g_nBlackHoleTime = 0;
					CSound::Play(SE_BLACKHOLE);
					PrintDebugProc("60");
				}
				PrintDebugProc("\n");
			}
			if (MAX_MOVE < abs(m_Move.x) + abs(m_Move.y)) //X方向とY方向の絶対値の合計が既定値以上
			{
				if (abs(m_Accel.x) + abs(m_Accel.y) < abs(m_Move.x) + abs(m_Move.y))	//1フレーム前のmoveの合計絶対値を超えていたら
				{
					m_Move.x *= 0.93f;
					m_Move.y *= 0.93f;
					//m_Move = m_Accel; //直前の移動量にリセット
				}
			}
			m_Accel = m_Move; // 12/24 X 現在のフレームの移動速度/角度の情報を保存する
			m_OldRot = m_Rot; // 12/26 X

			float angle;//ジェットエフェクトで使用する角度
			angle = m_RotDest.x;
#if _DEBUG
			PrintDebugProc("ｶｸﾄﾞ:%.2f\n", angle);
#endif

			if (JOY.x < 0.0f) {
				angle += 180.0f;
				angle *= -1;
			}
			g_Size = g_model[0].GetBBox();
#if _DEBUG
			PrintDebugProc("x,y,z={%.2f,%.2f,%.2f}\n", g_Size.x, g_Size.y, g_Size.z);
			PrintDebugProc("move.x,move.y,move.z={%.2f,%.2f,%.2f}\n", m_Move.x, m_Move.y, m_Move.z);
			PrintDebugProc("m_Rot.x,m_Rot.y,m_Rot.z={%.2f,%.2f,%.2f}\n", m_Rot.x, m_Rot.y, m_Rot.z);
			PrintDebugProc("m_RotDest.x,m_RotDest.y,m_RotDest.z={%.2f,%.2f,%.2f}\n", m_RotDest.x, m_RotDest.y, m_RotDest.z);
			PrintDebugProc("RotZ={%.2f}\n", RotZ(JOY));
#endif

			float rot = m_Rot.x + 90.0f;
			XMFLOAT3 t = XMFLOAT3(-(CosDeg(angle)*50.0f), -(SinDeg(angle)*50.0f), 0.0f);

			bool flag = false;
			//エフェクト（仮）
			if (GetJoyTrigger(0, XBOXA)) {
				CSound::Play(SE_JET);
			}
			if (GetJoyButton(0, XBOXA)) {
				EffectManager::Get()->Start("jet", XMFLOAT3(t.x, t.y, 0.0f), angle);
				flag = true;
			}

			//ゴールに当たってる？
			if (Player_Goal->Collision(m_Pos, g_Size)) {
				if (!g_bClearGameOverFlag[0]) {

					CSound::Play(SE_GOSHIP);
				}
				g_bClearGameOverFlag[0] = true;
			}
			//ライフによってゲージが変動
			Player_Life->Set(m_fLife);

			//ライフによって振動が変動
			//01/10  佐藤大地
			//Player_Life->Vib(m_fLife);

			//ライフによって鼓動が変動
			//01/10  佐藤大地
			Player_Life->Beat(m_fLife);

			//エフェクト更新
			EffectManager::Get()->Update("jet", XMFLOAT3(t.x, t.y, 0.0f), angle, flag);
			EffectManager::Get()->Update("collision", false);

			XMFLOAT3 sabun = XMFLOAT3(m_Pos.x / 20.0f, m_Pos.y / 20.0f, (900 - DEFF_CAMERA_Z) / 20.0f);
			static int count;

			//それぞれ押したら引くフラグを発動
			if (GetJoyButton(0, PS4L) ) {
				m_bPull[0] = true;
			}
			else if (GetJoyTrigger(0, XBOXA)) {
				m_bPull[1] = true;
				count = 0;
			}
			//それぞれフラグは同時に発動しない
			if (m_bPull[0]) {
				m_bPull[1] = false;
			}
			else if (m_bPull[1]) {
				m_bPull[0] = false;
			}

			//spaceを押されたらカメラを引く
			if (m_bPull[0]) {
				EffectManager::Get()->End("jet");
				if (g_CameraPos.z > -900.0f) {
					g_CameraPos.z -= sabun.z;
				}
				if (m_Pos.x >= 0.0f) {
					if (g_CameraPos.x > 0.0f) {
						g_CameraPos.x -= sabun.x;
					}
					else {
						//行き過ぎた時の補正
						g_CameraPos.x = 0.0f;
					}
				}
				else {
					if (g_CameraPos.x < 0.0f) {
						g_CameraPos.x -= sabun.x;
					}
					else {
						//行き過ぎた時の補正
						g_CameraPos.x = 0.0f;
					}
				}
				if (m_Pos.y >= 0.0f) {
					if (g_CameraPos.y > 0.0f) {
						g_CameraPos.y -= sabun.y;
					}
					else {
						//行き過ぎた時の補正
						g_CameraPos.y = 0.0f;
					}
				}
				else {
					if (g_CameraPos.y < 0.0f) {
						g_CameraPos.y -= sabun.y;
					}
					else {
						//行き過ぎた時の補正
						g_CameraPos.y = 0.0f;
					}
				}
			}
			//何も押されてないときは引かれているか確認
			else if (!m_bPull[0]) {
				//カメラが引いていたら
				if (g_CameraPos.z < -DEFF_CAMERA_Z) {
					g_CameraPos.z += sabun.z;
					if (m_Pos.x >= 0.0f) {
						if (g_CameraPos.x < m_Pos.x) {
							g_CameraPos.x += sabun.x;
						}
						else {
							g_CameraPos.x = m_Pos.x;//行き過ぎた時の補正
						}
					}
					else {
						if (g_CameraPos.x > m_Pos.x) {
							g_CameraPos.x += sabun.x;
						}
						else {
							g_CameraPos.x = m_Pos.x;//行き過ぎた時の補正
						}
					}
					if (m_Pos.y >= 0.0f) {
						if (g_CameraPos.y < m_Pos.y) {
							g_CameraPos.y += sabun.y;
						}
						else {
							g_CameraPos.y = m_Pos.y;//行き過ぎた時の補正
						}
					}
					else {
						if (g_CameraPos.y > m_Pos.y) {
							g_CameraPos.y += sabun.y;
						}
						else {
							g_CameraPos.y = m_Pos.y;//行き過ぎた時の補正
						}
					}
				}
				//カメラが引いていなかったらそのまま位置をカメラ位置に置き換える
				else {
					g_CameraPos = XMFLOAT3(m_Pos.x, m_Pos.y, -DEFF_CAMERA_Z);
				}
			}

			CCamera::Get()->SetPos(g_CameraPos);
			CCamera::Get()->SetTarget(g_CameraPos.x, g_CameraPos.y, 0.0f);
		}
		//1フレーム前と比較するために値を格納
		nTouchDeblyPrev = nTouchDebly;
		nTouchMeteoPrev = nTouchMeteo;
	}

	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScl;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	//スケールを反映
	mtxScl = XMMatrixScaling(m_Scl.x, m_Scl.y, m_Scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_RotDest.x), XMConvertToRadians(m_RotDest.y), XMConvertToRadians(m_RotDest.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックス設定
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);
}

//============================================
// 描画処理
//============================================
void Player::Draw(void)
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	XMFLOAT3 goalPos = Player_Goal->GetPos();
	if (g_bClearGameOverFlag[1]) {
		// 不透明部分を描画
		g_model[1].Draw(pDC, g_mtxWorld, eOpacityOnly);
	}
	else {
		g_model[0].Draw(pDC, g_mtxWorld, eOpacityOnly);
	}

	// 半透明部分を描画
	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
	SetZWrite(false);				// Zバッファ更新しない
	if (g_bClearGameOverFlag[1]) {
		g_model[1].Draw(pDC, g_mtxWorld, eTransparentOnly);
	}
	else {
		g_model[0].Draw(pDC, g_mtxWorld, eTransparentOnly);
	}
	SetZWrite(true);				// Zバッファ更新する
	SetBlendState(BS_NONE);			// アルファブレンド無効

	//エフェクト描画
	EffectManager::Get()->Draw("jet");
	EffectManager::Get()->Draw("collision");
}

//============================================
// 位置取得
//============================================
XMFLOAT3& Player::GetPos()
{
	return m_Pos;
}

//============================================
//角度を取得
//============================================
float Player::RotZ(XMFLOAT2 JOY) {
	JOY.y *= -1;
	float z = atan(JOY.y / JOY.x);	//アークタンジェントを取得→ラジアン値
	float x = z * 180.0f / PI;			//取得したラジアン値を角度に直して返す
	return x;
}

//============================================
//酸素量を取得
//============================================
float Player::GetO2(void) {
	return m_fLife;
}

//============================================
//テロップアニメーション再生中かどうかのフラグセット	// 01/22 三輪卓弘
//============================================
void  Player::SetAnimTelopFlag(bool bAnimTelopFlag)
{
	g_bAnimTelopFlag = bAnimTelopFlag;
}

//============================================
//ゲームクリア時のアニメーション
//============================================
void Player::GameClearAnimation() {
	//エフェクトの強制終了
	EffectManager::Get()->End("all");

	//変数宣言
	XMFLOAT3 goalPos = Player_Goal->GetPos();
	XMFLOAT3 playerVector;//プレイヤーからゴールのベクトル

	//ゴールとプレイヤーを近づける
	if ((abs(m_Pos.x - goalPos.x) >= 10.0f || abs(m_Pos.y - goalPos.y) >= 10.0f || abs(m_Pos.z - goalPos.z) >= 10.0f) && !g_goalCollisionFlag) {
		CCamera::Get()->SetPos(m_Pos.x, m_Pos.y, m_Pos.z - DEFF_CAMERA_Z);
		CCamera::Get()->SetTarget(m_Pos);
		//ベクトル生成
		playerVector.x = goalPos.x - m_Pos.x;
		playerVector.y = goalPos.y - m_Pos.y;
		playerVector.z = goalPos.z - m_Pos.z;
		XMStoreFloat3(&playerVector, XMVector3Normalize(XMLoadFloat3(&playerVector)));//ベクトルの正規化

		//移動
		m_Pos.x += playerVector.x*2.0f;
		m_Pos.y += playerVector.y*2.0f;
		m_Pos.z += playerVector.z*2.0f;
		if (m_Scl.x > 0.0f) {
			m_Scl.x -= 0.01f;
			m_Scl.y -= 0.01f;
			m_Scl.z -= 0.01f;
		}
	}
	//プレイヤーがゴールに完全に触れた後
	else {
		m_Scl = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_goalCollisionFlag = true;
		CCamera::Get()->SetTarget(goalPos);//カメラをゴールに向ける
		Player_Goal->SetPos(5.0f);		//ゴールを動かす
		Player_Rayer->Set(20.0f, 0.0f, true);
		//一定の座標を通過したらシーン遷移
		if (goalPos.x > 1300.0f) {
			StartFadeOut(SCENE_RESULT);
		}
	}
}

//============================================
//ゲームオーバー時のアニメーション
//============================================
void Player::GameOverAnimation() {
	//SEの停止
	CSound::Stop(SE_WARNING);
	CSound::Stop(SE_COLLISION);
	CSound::Stop(SE_JET);
	gameOver_stopCount--;
	//一旦停止した後
	//回転して遠くに行く
	if (gameOver_stopCount < 0) {
		m_RotDest.z -= 6.0f;
		m_Pos.z += 2.0f;
	}
	//一定の座標を通過したらシーン遷移
	if (m_Pos.z > 300.0f) {
		StartFadeOut(SCENE_RESULT);
	}

}

//アニメーションを一括管理
void Player::Animator(int animNum) {
	static int animPrev;

	//再生時間のリセット
	if (animNum != animPrev) {
		g_nCountFrame = 0;
	}
	static bool upFlag = true;
	switch (animNum) {
	case hold:
		if (GetJoyButton(0, XBOXY)) {
			if (g_nCountFrame >= AnimationTime - 3.0f) {
				upFlag = false;
			}
			else if (g_nCountFrame < 60) {
				upFlag = true;
			}
			if (upFlag) {
				g_nCountFrame++;
			}
			else {
				g_nCountFrame--;
			}
		}
		else {
			if (g_nCountFrame > 60) {
				g_nCountFrame = 60;
			}
			g_nCountFrame--;
			if (g_nCountFrame <= 0) {
				m_nAnimNum = stay;
			}
		}
		break;
	case release:
		g_nCountFrame++;
		if (g_nCountFrame >= AnimationTime - 3.0f) {
			m_nAnimNum = stay;
		}
		break;
	case jet:
		g_nCountFrame++;
		if (g_nCountFrame >= AnimationTime - 3.0f) {
			m_nAnimNum = stay;
		}
		break;
	default:
		g_nCountFrame++;
		break;
	}

	//通常のモデルアニメーション
	g_dSetAnim = g_nCountFrame * g_d1FrameAnim;//アニメーション再生箇所を更新
	//アニメーションセット
	g_model[0].SetAnimTime(g_dSetAnim);
	g_model[0].SetAnimIndex(animNum);

	animPrev = animNum;
}

//範囲外に出た時の強制送還
void Player::Repatriation(XMFLOAT3 Vector) {
	//位置をベクトルに則って変更
	m_Move.x = Vector.x*0.9f;
	m_Move.y = Vector.y*0.9f;
	//エリア内に入ったらフラグをおろす
	int x = g_nStageNum - 1;
	if (m_Pos.y < warningArea[x][UP] - AleattelopPos && m_Pos.y > warningArea[x][DOWN] + AleattelopPos &&
		warningArea[x][LEFT] + AleattelopPos < m_Pos.x && warningArea[x][RIGHT] - AleattelopPos > m_Pos.x) {
		g_bRepatriationFlag = false;
		m_Move = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}