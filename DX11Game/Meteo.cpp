//=============================================================================
//隕石[Meteo.cpp]
//12/11 齋藤成希
//=============================================================================
#include "Meteo.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "collision.h"
#include <vector>
#include"input.h"
#include "Game.h"


//マクロ
#define MODEL_DEBLY			"data/model/meteorite.fbx"

//モデルの大きさは使用するモデルによって動かして確認しなければならない
#define DEBLY_SIZE		(16.375f/5.0f)
#define OUTSIDE_AREA_X	(-750.0f)			//◆仮　エリア外座標


//デブリの情報を持つ構造体
struct TMeteo {
	XMFLOAT3 m_Pos;				// 位置
	int 		m_RotDir;			//回転方向
	XMFLOAT3 m_fRot;			// 角度

	XMFLOAT4X4	m_mtxWorld;	// ワールドマトリックス
};

struct TMeteoSet {
	int メテオの数;
	XMFLOAT3 m_Scl;				// スケール
	float	 m_fAngle;			// ベクトルを求める用の角度
	int m_count;	//出現する周期
	bool		m_bVectorDir;		//終端位置で使用 x=true,y=false
	float		m_fOverLine;		//終端位置
	XMFLOAT3 m_DefaultPos;		// 初期位置
	float m_fSpeed;				//スピード

	int 周期のカウント;
	int メテオの数のカウント;
	XMFLOAT3 m_Vector;				//ベクトル
	std::vector<TMeteo> OneMeteo;
};


//クラス
Player* Meteo_Player;

//グローバル変数
static CAssimpModel	g_model;				// モデル
static int g_nMeteoCount;					// ステージごとの隕石数
static int stagenum;

static std::vector<TMeteoSet> meteoArray;		// 隕石を管理する動的配列

#if _DEBUG
static int g_count;		//計測用
#endif // _DEBUG


//=============================================================================
//初期化
//=============================================================================
HRESULT Meteo::Init(int StageNum) {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// モデルデータの読み込み
	if (!g_model.Load(pDevice, pDeviceContext, MODEL_DEBLY)) {
		MessageBoxA(GetMainWnd(), "モデルデータ読み込みエラー", "InitEnemy", MB_OK);
		return E_FAIL;
	}
	Create(StageNum);

	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;
	Setit = meteoArray.begin();	//イテレーターを動的配列の最初に持ってくる
	while (Setit != meteoArray.end()) {
		Oneit = Setit->OneMeteo.begin();
		while (Oneit != Setit->OneMeteo.end()) {
			Oneit->m_Pos = Setit->m_DefaultPos; //初期位置を保存する
			Oneit++;
		}
		Setit->m_Vector = XMFLOAT3(SinDeg(Setit->m_fAngle)*Setit->m_fSpeed, CosDeg(Setit->m_fAngle)*Setit->m_fSpeed, 0.0f);
		Setit->メテオの数のカウント = 1;
		Setit->周期のカウント = 0;
		Setit++;
	}
#if _DEBUG
	g_count = 1;
#endif
	stagenum = StageNum;

	Meteo_Player = GetPlayer();
	return hr;
}

//=============================================================================
//終了
//=============================================================================
void Meteo::Uninit(void) {
	// モデルの解放
	g_model.Release();

	//動的配列を消去
	std::vector<TMeteoSet>::iterator Setit;
	Setit = meteoArray.begin();	//イテレーターを動的配列の最初に持ってくる
	while (Setit != meteoArray.end()) {
		Setit->OneMeteo.clear();
		Setit++;
	}
	meteoArray.clear();
}

//=============================================================================
//更新
//=============================================================================
void Meteo::Update(void) {
	//ステージ３、４以外は何もしない
	if (!(stagenum == 3 || stagenum == 4)) {
		return;
	}
	XMMATRIX mtxWorld, mtxRot, mtxScl, mtxPos;
	//イテレーター
	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;
	Setit = meteoArray.begin();	//イテレーターを動的配列の最初に持ってくる
	while (Setit != meteoArray.end()) {
		Oneit = Setit->OneMeteo.begin();
		Setit->周期のカウント++;
		if (Setit->周期のカウント >= Setit->m_count&&Setit->メテオの数>Setit->メテオの数のカウント) {
			Setit->メテオの数のカウント++;
			Setit->周期のカウント = 0;
		}
		for(int i=0;i<Setit->メテオの数のカウント;++i) {
			//回転
			switch (Oneit->m_RotDir) {
			case 0:
				Oneit->m_fRot.x = Oneit->m_fRot.x + 0.3f;
				break;
			case 1:
				Oneit->m_fRot.y = Oneit->m_fRot.y + 0.3f;
				break;
			case 2:
				Oneit->m_fRot.z = Oneit->m_fRot.z + 0.3f;
				break;
			}
			//位置にベクトルを加算
			Oneit->m_Pos.x += Setit->m_Vector.x;
			Oneit->m_Pos.y += Setit->m_Vector.y;

			//方向がXかYか
			if (Setit->m_bVectorDir) {
				//境界は右か左か
				if (Setit->m_fOverLine >= 0) {
					//境界を越えているか
					if (Oneit->m_Pos.x > Setit->m_fOverLine) {
						//隕石の座標をデフォルト位置に修正
						Oneit->m_Pos = Setit->m_DefaultPos;
					}
				}
				else {
					if (Oneit->m_Pos.y < Setit->m_fOverLine) {
						//隕石の座標をデフォルト位置に修正
						Oneit->m_Pos = Setit->m_DefaultPos;
					}
				}
			}
			else {
				//境界は上か下か
				if (Setit->m_fOverLine >= 0) {
					//境界を越えているか
					if (Oneit->m_Pos.x > Setit->m_fOverLine) {
						//隕石の座標をデフォルト位置に修正
						Oneit->m_Pos = Setit->m_DefaultPos;
					}
				}
				else {
					if (Oneit->m_Pos.y < Setit->m_fOverLine) {
						//隕石の座標をデフォルト位置に修正
						Oneit->m_Pos = Setit->m_DefaultPos;
					}
				}
			}


			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			//デブリ回転の反映
			mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Oneit->m_fRot.x), XMConvertToRadians(Oneit->m_fRot.y), XMConvertToRadians(Oneit->m_fRot.z));
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//スケールを反映
			mtxScl = XMMatrixScaling(Setit->m_Scl.x, Setit->m_Scl.y, Setit->m_Scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 位置を反映
			mtxPos = XMMatrixTranslation(Oneit->m_Pos.x, Oneit->m_Pos.y, Oneit->m_Pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxPos);

			// ワールドマトリックス設定
			XMStoreFloat4x4(&Oneit->m_mtxWorld, mtxWorld);
			Oneit++;
		}
		Setit++;
	}

}

//=============================================================================
//描画
//=============================================================================
void Meteo::Draw(void) {
	//ステージ３、４以外は何もしない
	if (!(stagenum == 3 || stagenum == 4)) {
		return;
	}
	ID3D11DeviceContext* pDC = GetDeviceContext();

	//イテレーター
	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;
	Setit = meteoArray.begin();

	while (Setit != meteoArray.end()) {
		Oneit = Setit->OneMeteo.begin();
		while (Oneit != Setit->OneMeteo.end()) {
			// 不透明部分を描画
			g_model.Draw(pDC, Oneit->m_mtxWorld, eOpacityOnly);

			// 半透明部分を描画
			SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
			SetZWrite(false);				// Zバッファ更新しない
			g_model.Draw(pDC, Oneit->m_mtxWorld, eTransparentOnly);
			SetZWrite(true);				// Zバッファ更新する
			SetBlendState(BS_NONE);			// アルファブレンド無効

			Oneit++;
		}
		Setit++;
	}
}

//=============================================================================
//衝突判定
//=============================================================================
XMINT2 Meteo::Collision(XMFLOAT3 pos, XMFLOAT3 size) {
	//ステージ３、４以外は何もしない
	if (!(stagenum == 3 || stagenum == 4)) {
		return XMINT2(-1,-1);
	}
	const float abc = 0.18f;//当たり判定の微調整
	//イテレーター
	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;
	Setit = meteoArray.begin();
	XMINT2 count = XMINT2(0,0);
	while (Setit != meteoArray.end()) {
		XMFLOAT3 m_Size = g_model.GetBBox();
		m_Size = XMFLOAT3(m_Size.x*Setit->m_Scl.x * abc, m_Size.y*Setit->m_Scl.y * abc, m_Size.z*Setit->m_Scl.z * abc);
		Oneit = Setit->OneMeteo.begin();
		while (Oneit != Setit->OneMeteo.end()) {
			int collisilon = CollisionSphere(Oneit->m_Pos, m_Size.x, pos, size.y);
			//一つずつ見ていって、あたってなかったら次の番号へ
			if (collisilon == true) {
				return count;
			}
			Oneit++;
			count.y++;
		}
		
		count.x++;
		count.y = 0;
		Setit++;
	}
	//何もあたっていなかったら-1
	return XMINT2( -1,-1);
}

//=============================================================================
//プレイヤーと衝突したデブリの位置を取得
//=============================================================================
XMFLOAT3 Meteo::GetPos(XMINT2 num) {
	return meteoArray[num.x].OneMeteo[num.y].m_Pos;
}

//=============================================================================
//プレイヤーと衝突したデブリの大きさを取得
//=============================================================================
XMFLOAT3 Meteo::GetSize(XMINT2 num) {
	XMFLOAT3 size = g_model.GetBBox();
	size = XMFLOAT3(meteoArray[num.x].m_Scl.x*size.x, meteoArray[num.x].m_Scl.y*size.x, meteoArray[num.x].m_Scl.z*size.x);

	return size;
}

//=============================================================================
//プレイヤーと衝突したデブリのうごくベクトルを取得
//=============================================================================
XMFLOAT3 Meteo::GetVector(XMINT2 num) {
	return meteoArray[num.x].m_Vector;
}

//=============================================================================
//情報のセット
//=============================================================================
void Meteo::Create(int StageNum) {
	switch (StageNum) {
		//									メテオの数		大きさ　　					角度　 周期　		方向フラグ　終端位置　				初期位置　		スピード
	case 3:
		meteoArray.push_back({ 7, XMFLOAT3(20.5f, 20.5f, 20.5f), 135.0f,300,false,-SCREEN_HEIGHT,  XMFLOAT3(-750.0f,260.0f,0.0f),1.0f });
		meteoArray.push_back({ 2, XMFLOAT3(30.5f,30.5f,30.5f),150.0f,1000,false, -SCREEN_HEIGHT, XMFLOAT3(-395.0f,470.0f,0.0f),0.7f });
		meteoArray.push_back({ 7,XMFLOAT3(20.5f,20.5f,20.5f),290.0f,300,true,-SCREEN_WIDTH ,XMFLOAT3(750.0f,-50.0f,0.0f),1.0f });
		break;
	case 4:
		meteoArray.push_back({ 7,XMFLOAT3(20.0f,20.0f,20.0f),200.0f,300,false,-SCREEN_HEIGHT,XMFLOAT3(10.0f,470.0f,0.0f),1.0f });
		meteoArray.push_back({ 7,XMFLOAT3(26.5f,26.5f,26.5f),180.0f,300,false,-SCREEN_HEIGHT,XMFLOAT3(260.0f,470.0f,0.0f),1.0f });
		meteoArray.push_back({ 7,XMFLOAT3(13.5f,13.5f,13.5f),220.0f,300,false,-SCREEN_HEIGHT,XMFLOAT3(570.0f,470.0f,0.0f),1.0f });
		meteoArray.push_back({ 7,XMFLOAT3(16.0f,16.0f,16.0f),255.0f,300,false,-SCREEN_HEIGHT,XMFLOAT3(750.0f,-195.0f,0.0f) ,1.0f});
		break;
	default:
		break;
	}
	//イテレーター
	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;

	Setit = meteoArray.begin();	//イテレーターを動的配列の最初に持ってくる
	while (Setit != meteoArray.end())	{
		Setit->OneMeteo.resize(Setit->メテオの数);
		Oneit = Setit->OneMeteo.begin();
		while (Oneit != Setit->OneMeteo.end()) {
			Oneit->m_RotDir = rand() % 3;
			Oneit++;
		}
		Setit++;
	}
}
