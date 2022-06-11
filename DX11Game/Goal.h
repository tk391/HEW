#pragma once
#include "main.h"
class Goal
{
public:
	//4�又��
	HRESULT Init(int stageNum);
	void Uninit();
	void Update();
	void Draw();
	bool Collision(XMFLOAT3 pos, XMFLOAT3 size);

	XMFLOAT3& GetPos();//���W�擾
	void SetPos(float posX);//���W�Z�b�g

private:
	XMFLOAT3 m_Pos;
	XMFLOAT3 m_Rot;
	XMFLOAT3 m_Scl;
};

