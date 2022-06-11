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
	XMFLOAT2		m_Pos;			// �ʒu
	XMFLOAT2		m_Size;			// �傫��
	XMFLOAT4		m_Color;		// �F

};
