#pragma once
#include "main.h"
class Result_Rocket
{
public:
	//4‘åˆ—
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	XMFLOAT3 GetPos();

private:
	XMFLOAT3 m_Pos;
	XMFLOAT3 m_Rot;
	XMFLOAT3 m_Scl;
};

