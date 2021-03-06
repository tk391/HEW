//=============================================================================
// 背景 [bg.cpp]
//12/11 齋藤成希
//=============================================================================
#include "bg.h"
#include "Texture.h"
#include "polygon.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define BG_POS_X		0.0f
#define BG_POS_Y		0.0f
#define BG_WIDTH		SCREEN_WIDTH
#define BG_HEIGHT		SCREEN_HEIGHT

//=============================================================================
// グローバル変数
//=============================================================================
std::string BGFileName[] = {
	"data/texture/titleBackground.png",
	"data/texture/5.jpg",
	"data/texture/動かない背景.png",
	"data/texture/prottype/option.png",
	"data/texture/prottype/result.png",
};
//マップ
std::map<std::string, ID3D11ShaderResourceView*> mapBGTexture;
std::map<std::string, ID3D11ShaderResourceView*>::iterator it;//mapBGTextureのイテレーター

//=============================================================================
// 初期化
//=============================================================================
HRESULT InitBG()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	//画像追加
	mapBGTexture.insert(std::make_pair("title", nullptr));//タイトル
	hr = CreateTextureFromFile(pDevice, L"data/texture/titleBackground.png", &mapBGTexture["title"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "読み込みエラーtitle", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("select", nullptr));//選択
	hr = CreateTextureFromFile(pDevice, L"data/texture/5.jpg", &mapBGTexture["select"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "読み込みエラーselect", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("game", nullptr));//ゲーム
	hr = CreateTextureFromFile(pDevice, L"data/texture/動かない背景.png", &mapBGTexture["game"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "読み込みエラーgame", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("option", nullptr));//オプション
	hr = CreateTextureFromFile(pDevice, L"data/texture/prottype/option.png", &mapBGTexture["option"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "読み込みエラーoption", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("result", nullptr));//リザルト
	hr = CreateTextureFromFile(pDevice, L"data/texture/prottype/result.png", &mapBGTexture["result"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "読み込みエラーresult", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("clear", nullptr));//クリア
	hr = CreateTextureFromFile(pDevice, L"data/texture/stageclear_back3.png", &mapBGTexture["clear"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "読み込みエラーclear", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("gameover", nullptr));//ゲームオーバー
	hr = CreateTextureFromFile(pDevice, L"data/texture/gameover_back.png", &mapBGTexture["gameover"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "読み込みエラーgameover", "EffectManager::Init", MB_OK); }

	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBG()
{
	it = mapBGTexture.begin();
	while (it != mapBGTexture.end()) {
		// テクスチャ解放
		SAFE_RELEASE(it->second);
		it++;
	}
	//マップ削除
	mapBGTexture.clear();
}

//=============================================================================
// 描画
//=============================================================================
void DrawBG(std::string bgName)
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	SetPolygonSize(BG_WIDTH, BG_HEIGHT);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonTexture(mapBGTexture[bgName]);
	SetPolygonUV(0.0f, 0.0f);
	DrawPolygon(pDC);
}