#pragma once
#include "main.h"
class Meteo
{
public:
	//4�又��
	HRESULT Init(int StageNum);
	void Uninit();
	void Update();
	void Draw();
	XMINT2 Collision(XMFLOAT3 pos, XMFLOAT3 size);//x=�����������f���̎�� y=���̎�ނ̒��̔ԍ�
	XMFLOAT3 GetPos(XMINT2 colnum);//���������f�u���̈ʒu���擾
	XMFLOAT3 GetSize(XMINT2 num);//���������f�u���̑傫�����擾
	XMFLOAT3 GetVector(XMINT2 num);//�f�u���̃x�N�g�����擾
private:
	void Create(int StageNum);

};

