//�X�e�[�W�ɔz�u�����f�u���i�L���[�u�j
//2021.12.08 �V������
#pragma once
#include "main.h"

#define MAX_DEBLY			(10)	

class Debly
{
public:
	//4�又��
	HRESULT Init(int StageNum);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	int Collision(XMFLOAT3 pos, XMFLOAT3 size);
	XMFLOAT3 GetPos(int num);//���������f�u���̈ʒu���擾
	XMFLOAT3 GetSize(int num);//���������f�u���̑傫�����擾

private:
	void Create(int StageNum);


};

