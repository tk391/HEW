//ステージに配置されるデブリ（キューブ）
//2021.12.08 齋藤成希
#pragma once
#include "main.h"

#define MAX_DEBLY			(10)	

class Debly
{
public:
	//4大処理
	HRESULT Init(int StageNum);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	int Collision(XMFLOAT3 pos, XMFLOAT3 size);
	XMFLOAT3 GetPos(int num);//当たったデブリの位置を取得
	XMFLOAT3 GetSize(int num);//当たったデブリの大きさを取得

private:
	void Create(int StageNum);


};

