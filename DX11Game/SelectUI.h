#pragma once
#include "UI.h"
#include "main.h"

#define MAX_SELECTUI_TEXUTURE			21
/*
星5個
宇宙飛行士
ステージ名
矢印*/

// 列挙型:画像情報		01/16 三輪卓弘
enum ESelectUI {
	MOON_1 = 0,		// 画像:ステージ1
	MARSE_2,		// 画像:ステージ2
	JUPITER_3,		// 画像:ステージ3
	VINUS_4,		// 画像:ステージ4
	ASTEROIDS_5,	// 画像:ステージ5

	ASTRONAUT,		// 文字:宇宙飛行士
	ARROW_RIGHT,	// UI  :矢印→
	ARROW_LEFT,		// UI  :矢印←
	NOPLAY,			// 文字:まだ遊べないよ
	STARFLAME,		// 星枠
	SHUTTER,		// シャッター

	STAGENAME1,		// 文字:ステージ名
	STAGENAME2,		// 文字:ステージ名
	STAGENAME3,		// 文字:ステージ名
	STAGENAME4,		// 文字:ステージ名
	STAGENAME5,		// 文字:ステージ名
	NAMESHINING1,	// 文字発光
	NAMESHINING2,	// 文字発光
	NAMESHINING3,	// 文字発光
	NAMESHINING4,	// 文字発光
	NAMESHINING5,	// 文字発光

	MAX_SELECTUI	// セレクト画面UI画像総数
};

class SelectUI :
	public UI
{
public:
	HRESULT Init();
	void Uninit();
	void Update(int StageNum);
	void Draw();
	void AstroFloating(int num, int animation, int stageNum);		//画面上の宇宙飛行士の位置を選択中のステージと同期する関数	01/16 三輪卓弘
	void ArrowAnim(int LR, int animation);		//Select.cppの内容と同期して、矢印キーをちょっと動かす関数	01/16 三輪卓弘
	void NoPlayAnimSet();	//"まだ遊べないよ"の揺れるフラグを設定	01/23三輪卓弘
	bool NoPlayAnimGet();	//"まだ遊べないよ"の揺れるフラグを取得	01/23三輪卓弘
	void StageAnimSet();	//ステージ名の揺れるフラグを設定		01/23三輪卓弘
	void ShiningAnimSet();	//ステージ名の発光フラグを設定			01/23三輪卓弘
private:
	XMFLOAT2 m_vPos[MAX_SELECTUI_TEXUTURE];
	XMFLOAT2 m_vSize[MAX_SELECTUI_TEXUTURE];
	XMFLOAT4 m_vColor[MAX_SELECTUI_TEXUTURE];
	ID3D11ShaderResourceView* m_pTexture[MAX_SELECTUI_TEXUTURE];
};

