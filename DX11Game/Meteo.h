#pragma once
#include "main.h"
class Meteo
{
public:
	//4大処理
	HRESULT Init(int StageNum);
	void Uninit();
	void Update();
	void Draw();
	XMINT2 Collision(XMFLOAT3 pos, XMFLOAT3 size);//x=あたったモデルの種類 y=その種類の中の番号
	XMFLOAT3 GetPos(XMINT2 colnum);//当たったデブリの位置を取得
	XMFLOAT3 GetSize(XMINT2 num);//当たったデブリの大きさを取得
	XMFLOAT3 GetVector(XMINT2 num);//デブリのベクトルを取得
private:
	void Create(int StageNum);

};

