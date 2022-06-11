#pragma once
#include "main.h"
class Goal
{
public:
	//4大処理
	HRESULT Init(int stageNum);
	void Uninit();
	void Update();
	void Draw();
	bool Collision(XMFLOAT3 pos, XMFLOAT3 size);

	XMFLOAT3& GetPos();//座標取得
	void SetPos(float posX);//座標セット

private:
	XMFLOAT3 m_Pos;
	XMFLOAT3 m_Rot;
	XMFLOAT3 m_Scl;
};

