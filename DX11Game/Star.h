//=============================================================================
//
// �������� [explosion.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once

#include "main.h"
#include "Texture.h"
#include "mesh.h"
#include "Camera.h"
#include "Light.h"

//�}�N��
#define maxStar	(5)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
class Star {
public:
	HRESULT Init(void);
	void Uninit(void);
	void Update(int StageNum, int animation);
	void Draw(int selectStage);
	int OverFix(int num);
private:
	HRESULT MakeVertex(ID3D11Device* pDevice, int count);

	XMFLOAT3	pos[maxStar];		// �ʒu
	XMFLOAT2	m_size[maxStar];		// �T�C�Y
	XMFLOAT4	color[maxStar];		// �F

	MESH			g_mesh[maxStar];						// ���b�V�����
	MATERIAL		g_material[maxStar];					// �}�e���A��

};
