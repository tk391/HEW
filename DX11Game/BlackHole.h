/////////////////////////////////////////////////
//�u���b�N�z�[��
//1/3 �O�֑�O
////////////////////////////////////////////////
#pragma once
#include "main.h"
#include "mesh.h"

#define MAX_BLACKHOLE			(10)	

class BlackHole
{
public:
	//4�又��
	HRESULT Init(int stageNum);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	HRESULT MakeVertex(ID3D11Device* pDevice);

	XMFLOAT4 Collision(XMFLOAT3 pos, XMFLOAT3 size);

	MESH			g_mesh;						// ���b�V�����
	MATERIAL		g_material;					// �}�e���A��

private:
	void Create(int StageNum);


};
