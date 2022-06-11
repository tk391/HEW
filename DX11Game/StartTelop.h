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
	XMFLOAT3		m_Pos;			// ���݂̈ʒu
	XMFLOAT3		m_Rot;			// ���݂̌���

	XMFLOAT4       m_Color;			// �F

};