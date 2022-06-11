//=============================================================================
// エフェクト管理クラス[EffectManager.cpp]
// 12/19 齋藤成希
//=============================================================================
#include "EffectManager.h"
#include "Texture.h"
#include "mesh.h"
#include "Camera.h"
#include "Light.h"
#include "polygon.h"
#include"debugproc.h"

//マクロ
#define MAX_ONEEFFECT		(100)	//一つのエフェクトで出せル最大数
#define M_DIFFUSE					XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR					XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER						(1.0f)
#define M_AMBIENT					XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE					XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

#define EXPLOSION_FRAME_X		4//横フレーム数
#define EXPLOSION_FRAME_Y		4//縦フレーム数
#define EXPLOSION_ANIM_COUNT	4//1コマ当たりの表示回数

//シングルトン
EffectManager* EffectManager::m_pInstance = nullptr;


//各エフェクト（一つ一つ）の要素
struct TEffect {
	XMFLOAT3 pos;				//位置
	XMFLOAT2 size;				//速度
	float			angle;				//角度
	bool			use;				//使用しているか
	bool			one;				//1つしか出ないエフェクトか
	int				anim;			//アニメーション再生位置
	int				count;			//アニメーションフレーム数
	int StartAnim;
	int EndAnim;
};

//各エフェクト（エフェクト全体）の要素
struct TEffectSet {
	TEffect oneEffect[MAX_ONEEFFECT];//各エフェクトの情報
	Dimension dimension;//2D座標に生成するか、3D座標に生成するか
	Loop loopJuge;			//その場でループするのか
	//3Dエフェクトのみ使用
	MESH	m_mesh;
	MATERIAL m_material;
	//テクスチャのみ使用
	ID3D11ShaderResourceView*	m_pTexture;
};

//=============================================================================
//プロトタイプ宣言
//=============================================================================
static HRESULT MakeVertexEffect(ID3D11Device* pDevice, std::string effectName);

//マップ
std::map<std::string, TEffectSet*> mapEffects;
std::map<std::string, TEffectSet*>::iterator it;//mapEffectsのイテレーター

EffectManager::EffectManager() {
}

HRESULT EffectManager::Init() {
	//シングルトン
	m_pInstance = new EffectManager;

	HRESULT hr = S_OK;
	//エフェクトで使用する画像は適宜ここで追加してください
	//ジェットエフェクト
	mapEffects.insert(std::make_pair("jet", new TEffectSet));//エフェクト追加
	mapEffects["jet"]->dimension = Two;//2Dか3Dか
	mapEffects["jet"]->oneEffect[0].size = XMFLOAT2(100.0f, 100.0f);//大きさを先に指定
	mapEffects["jet"]->loopJuge = Loop_ON;
	mapEffects["jet"]->oneEffect[0].StartAnim = 7;
	mapEffects["jet"]->oneEffect[0].EndAnim = mapEffects["jet"]->oneEffect[0].StartAnim + 3;
	hr = m_pInstance->Set("jet", "data/texture/hunsya_T_Sprit.png");
	if (FAILED(hr)) {
		MessageBoxA(GetMainWnd(), "読み込みエラーjet", "EffectManager::Init", MB_OK);
	}

	//衝突エフェクト
	mapEffects.insert(std::make_pair("collision", new TEffectSet));//エフェクト追加
	mapEffects["collision"]->dimension = Three;//2Dか3Dか
	mapEffects["collision"]->oneEffect[0].size = XMFLOAT2(30.0f, 30.0f);//大きさを先に指定
	mapEffects["collision"]->loopJuge = Loop_OFF;
	hr = m_pInstance->Set("collision", "data/texture/Hit_arfaSprit.png");
	if (FAILED(hr)) {
		MessageBoxA(GetMainWnd(), "読み込みエラーcollision", "EffectManager::Init", MB_OK);
	}

	return hr;
}

void EffectManager::Uninit() {
	it = mapEffects.begin();
	while (it != mapEffects.end()) {
		if (it->second->dimension == Two) {
			// テクスチャ解放
			SAFE_RELEASE(it->second->m_pTexture);
		}
		else {
			//メッシュ解放
			ReleaseMesh(&it->second->m_mesh);
		}
		it++;
	}
	//マップ削除
	mapEffects.clear();

	//解放
	delete m_pInstance;
}

void EffectManager::Update(std::string effectName, XMFLOAT3 Pos, float Angle, bool push) {
	TEffect* pEffect = mapEffects[effectName]->oneEffect;
	int count = 0;
	for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
		//未使用ならスキップ
		if (!pEffect->use) {
			if (mapEffects[effectName]->loopJuge == Loop_ON) {
				break;
			}
			else {
				continue;
			}
		}
		count++;
		//カウンタ更新
		pEffect->count--;
		//位置更新
		pEffect->pos = Pos;
		//角度更新
		pEffect->angle = Angle;
		if (pEffect->count <= 0) {
			++pEffect->anim;
			//ボタンを押してる(ループ再生状態になっている)間無限ループ
			if (push&&pEffect->anim >= pEffect->EndAnim) {
				pEffect->anim = pEffect->StartAnim;
			}
			else {
				//ボタンを押していなければアニメーションの終端で消滅
				if (pEffect->anim >= EXPLOSION_FRAME_X * EXPLOSION_FRAME_Y) {
					pEffect->use = false;	//消滅
					continue;
				}
			}
			//カウンタ初期化
			pEffect->count = EXPLOSION_ANIM_COUNT;
		}
	}
	//PrintDebugProc("pEffect->anim=%d\n", mapEffects["jet"]->oneEffect->anim);
	PrintDebugProc("count=%d\n", count);
}
void EffectManager::Update(std::string EffectName, bool push) {
	TEffect* pEffect = mapEffects[EffectName]->oneEffect;
	for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
		//未使用ならスキップ
		if (!pEffect->use) {
			if (mapEffects[EffectName]->loopJuge == Loop_ON) {
				return;
			}
			else {
				continue;
			}
		}
		//カウンタ更新
		pEffect->count--;
		if (pEffect->count <= 0) {
			++pEffect->anim;
			//ボタンを押してる(ループ再生状態になっている)間無限ループ
			if (push&&pEffect->anim >= pEffect->EndAnim) {
				pEffect->anim = pEffect->StartAnim;
			}
			else {
				//ボタンを押していなければアニメーションの終端で消滅
				if (pEffect->anim >= EXPLOSION_FRAME_X * EXPLOSION_FRAME_Y) {
					pEffect->use = false;	//消滅
					continue;
				}
			}
			//カウンタ初期化
			pEffect->count = EXPLOSION_ANIM_COUNT;
		}
	}
}

void EffectManager::Draw(std::string effectName) {
	ID3D11DeviceContext* pDC = GetDeviceContext();
	TEffect* pEffect = mapEffects[effectName]->oneEffect;

	SetBlendState(BS_ALPHABLEND);//αブレンディング有効

	if (mapEffects[effectName]->dimension == Two) {//２Dエフェクトだったら
		// Zバッファ無効(Zチェック無&Z更新無)
		SetZBuffer(false);

		for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
			//未使用ならスキップ
			if (!pEffect->use) {
				if (mapEffects[effectName]->loopJuge == Loop_ON) {
					break;
				}
				else {
					continue;
				}
			}
			int u = pEffect->anim%EXPLOSION_FRAME_X;
			int v = pEffect->anim / EXPLOSION_FRAME_X;
			//ポリゴンセット
			SetPolygonSize(pEffect->size.x, pEffect->size.y);
			SetPolygonTexture(mapEffects[effectName]->m_pTexture);
			SetPolygonFrameSize(1.0f / EXPLOSION_FRAME_X, 1.0f / EXPLOSION_FRAME_Y);
			SetPolygonUV((float)u / EXPLOSION_FRAME_X, (float)v / EXPLOSION_FRAME_Y);
			SetPolygonAngle(pEffect->angle);
			SetPolygonPos(pEffect->pos.x, pEffect->pos.y);
			DrawPolygon(pDC);
		}
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonFrameSize(1.0f, 1.0f);
		SetPolygonAngle(0.0f);

		// Zバッファ有効(Zチェック有&Z更新有)
		SetZBuffer(true);
	}
	else {//3Dエフェクトだったら
		CLight::Get()->SetDisable();	//光源無効
		XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();
		for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
			//未使用ならスキップ
			if (!pEffect->use) {
				if (mapEffects[effectName]->loopJuge == Loop_ON) {
					break;
				}
				else {
					continue;
				}
			}
			//テクスチャマトリクス更新
			XMMATRIX mtxTex = XMMatrixScaling(1.0f / EXPLOSION_FRAME_X, 1.0f / EXPLOSION_FRAME_Y, 1.0f);
			int u = pEffect->anim%EXPLOSION_FRAME_X;
			int v = pEffect->anim / EXPLOSION_FRAME_X;
			mtxTex = XMMatrixMultiply(mtxTex, XMMatrixTranslation((float)u / EXPLOSION_FRAME_X, (float)v / EXPLOSION_FRAME_Y, 0.0f));
			XMStoreFloat4x4(&mapEffects[effectName]->m_mesh.mtxTexture, mtxTex);

			//ビュー行列の回転部分の転置行列を設定
			mapEffects[effectName]->m_mesh.mtxWorld._11 = mView._11*pEffect->size.x;
			mapEffects[effectName]->m_mesh.mtxWorld._12 = mView._21*pEffect->size.x;
			mapEffects[effectName]->m_mesh.mtxWorld._13 = mView._31*pEffect->size.x;
			mapEffects[effectName]->m_mesh.mtxWorld._14 = 0.0f;
			mapEffects[effectName]->m_mesh.mtxWorld._21 = mView._12*pEffect->size.y;
			mapEffects[effectName]->m_mesh.mtxWorld._22 = mView._22*pEffect->size.y;
			mapEffects[effectName]->m_mesh.mtxWorld._23 = mView._32*pEffect->size.y;
			mapEffects[effectName]->m_mesh.mtxWorld._24 = 0.0f;
			mapEffects[effectName]->m_mesh.mtxWorld._31 = mView._13;
			mapEffects[effectName]->m_mesh.mtxWorld._32 = mView._23;
			mapEffects[effectName]->m_mesh.mtxWorld._33 = mView._33;
			mapEffects[effectName]->m_mesh.mtxWorld._34 = 0.0f;

			//位置を反映
			mapEffects[effectName]->m_mesh.mtxWorld._41 = pEffect->pos.x;
			mapEffects[effectName]->m_mesh.mtxWorld._42 = pEffect->pos.y;
			mapEffects[effectName]->m_mesh.mtxWorld._43 = pEffect->pos.z;
			mapEffects[effectName]->m_mesh.mtxWorld._44 = 1.0f;

			//描画
			DrawMesh(pDC, &mapEffects[effectName]->m_mesh);
		}
		CLight::Get()->SetEnable();	//光源有効
	}
	SetBlendState(BS_NONE);		//αブレンディング無効
}

//======================================================================
//エフェクトスタート
//======================================================================
int EffectManager::Start(std::string effectName, XMFLOAT3 Pos, float Angle) {
	TEffect* pEffect = mapEffects[effectName]->oneEffect;
	for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
		//使用中ならスキップ
		if (pEffect->use) {
			if (mapEffects[effectName]->loopJuge == Loop_ON) {
				return -1;
			}
			else {
				continue;
			}
		}
		pEffect->pos = Pos;
		pEffect->anim = 0;
		pEffect->angle = Angle;
		pEffect->count = EXPLOSION_ANIM_COUNT;
		pEffect->use = true;
		return i;		//発射できた（０～弾番号）
	}
	return -1;		//発射できなかった

}
int EffectManager::Start(std::string effectName, XMFLOAT3 Pos) {
	TEffect* pEffect = mapEffects[effectName]->oneEffect;
	for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
		//使用中ならスキップ
		if (pEffect->use) {
			if (mapEffects[effectName]->loopJuge == Loop_ON) {
				return -1;
			}
			else {
				continue;
			}
		}
		pEffect->pos = Pos;
		pEffect->anim = 0;
		pEffect->count = EXPLOSION_ANIM_COUNT;
		pEffect->use = true;
		return i;		//発射できた（０～弾番号）
	}
	return -1;		//発射できなかった
}

//=============================================================================
//情報の初期化
//=============================================================================
HRESULT EffectManager::Set(std::string effectName, std::string fileName) {
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	if (mapEffects[effectName]->dimension == Two) {//２Dエフェクトだったら
		// テクスチャ読込
		hr = CreateTextureFromFile(pDevice, fileName.c_str(), &mapEffects[effectName]->m_pTexture);
		if (FAILED(hr)) {
			return hr;
		}
	}
	else {//3Dエフェクトだったら
		//頂点情報の作成
		hr = MakeVertexEffect(pDevice, effectName);
		if (FAILED(hr)) {
			return hr;
		}
		//マテリアルの設定
		mapEffects[effectName]->m_material.Diffuse = M_DIFFUSE;
		mapEffects[effectName]->m_material.Ambient = M_AMBIENT;
		mapEffects[effectName]->m_material.Specular = M_SPECULAR;
		mapEffects[effectName]->m_material.Power = M_POWER;
		mapEffects[effectName]->m_material.Emissive = M_EMISSIVE;
		mapEffects[effectName]->m_mesh.pMaterial = &mapEffects[effectName]->m_material;

		//テクスチャの読み込み
		hr = CreateTextureFromFile(pDevice, fileName.c_str(), &mapEffects[effectName]->m_mesh.pTexture);
		if (FAILED(hr)) {
			return hr;
		}
		XMStoreFloat4x4(&mapEffects[effectName]->m_mesh.mtxTexture, XMMatrixIdentity());

		//ワールドマトリクス初期化
		XMStoreFloat4x4(&mapEffects[effectName]->m_mesh.mtxWorld, XMMatrixIdentity());
	}
	// 初期化
	for (int i = 0; i < MAX_ONEEFFECT; ++i) {
		mapEffects[effectName]->oneEffect[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		mapEffects[effectName]->oneEffect[i].size = mapEffects[effectName]->oneEffect[0].size;
		mapEffects[effectName]->oneEffect[i].angle = 0.0f;
		mapEffects[effectName]->oneEffect[i].use = false;
		mapEffects[effectName]->oneEffect[i].anim = 0;
		mapEffects[effectName]->oneEffect[i].count = EXPLOSION_ANIM_COUNT;
	}
	return hr;
}

//======================================================================
//頂点情報のセット
//======================================================================
HRESULT MakeVertexEffect(ID3D11Device* pDevice, std::string effectName) {
	//一時的な頂点配列を生成
	mapEffects[effectName]->m_mesh.nNumVertex = 4;
	mapEffects[effectName]->m_mesh.primitiveType = PT_TRIANGLESTRIP;
	VERTEX_3D* pVertexWK = new VERTEX_3D[mapEffects[effectName]->m_mesh.nNumVertex];

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
	mapEffects[effectName]->m_mesh.nNumIndex = 4;
	int* pIndexWK = new int[4];

	//インデックス配列の中身を埋める
	for (int i = 0; i < 4; i++) {
		pIndexWK[i] = i;
	}

	//頂点バッファ/インデックスバッファ生成
	HRESULT hr = MakeMeshVertex(pDevice, &mapEffects[effectName]->m_mesh, pVertexWK, pIndexWK);

	//一時的な頂点配列/インデックス配列を解放
	delete[] pIndexWK;
	delete[] pVertexWK;
	return hr;
}

//========================================
//強制終了
//========================================
void EffectManager::End(std::string effectName) {
	//"all"=全てのエフェクト
	//引数が"all"だったら全てのエフェクトを強制終了
	if (effectName == "all") {
		it = mapEffects.begin();
		while (it != mapEffects.end()) {
			for (int i = 0; i < MAX_ONEEFFECT; ++i) {
				it->second->oneEffect[i].use = false;
			}
			it++;
		}
	}
	else {
		for (int i = 0; i < MAX_ONEEFFECT; ++i) {
			mapEffects[effectName]->oneEffect[i].use = false;
		}
	}
}

//========================================
//アドレス呼び出し
//========================================
EffectManager* EffectManager::Get() {
	return m_pInstance;
}