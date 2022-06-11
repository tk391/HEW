#pragma once
#include "main.h"
#include"UI.h"

class ControlUI :public UI
{
public:
	HRESULT Init(void);
	void    Uninit(void);
	void    Update(void);
	void    Draw(void);
private:
	XMFLOAT2		m_Pos;			// à íu
	XMFLOAT2		m_Size;			// ëÂÇ´Ç≥
	XMFLOAT4		m_Color;		// êF

};
