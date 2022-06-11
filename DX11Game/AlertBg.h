#pragma once
//警告時画面赤化
#include "main.h"

class Alert {
public:
	HRESULT Init();	//初期化
	void Uninit();		//更新
	void Update();
	void Draw();		//描画

private:
	XMFLOAT2 m_vPos[2];	//位置
	XMFLOAT2 m_vSize[2];	//大きさ

	ID3D11ShaderResourceView* m_pTexture[2];	//テクスチャ
};

//これを呼び出して処理を開始する
void AlertWarning(bool Flg);
//ここまで