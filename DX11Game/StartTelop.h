#pragma once
#include "main.h"
#include"UI.h"

class StartTelop :public UI
{
public:
	HRESULT Init(void);
	void    Uninit(void);
	void    Update(void);
	void    Draw(void);

	bool	Timeflag;

	float m_Interval;


private:
	XMFLOAT3		m_Pos;			// 現在の位置
	XMFLOAT3		m_Rot;			// 現在の向き

	XMFLOAT4       m_Color;			// 色

};
