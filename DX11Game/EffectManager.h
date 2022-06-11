//=============================================================================
// �G�t�F�N�g�Ǘ��N���X[EffectManager.h]
// 12/19 �V������
//1/30 �V���O���g����
//=============================================================================
#pragma once
#include "main.h"

enum Dimension
{
	Two=0,
	Three
};

enum Loop {
	Loop_ON = 0,
	Loop_OFF
};
class EffectManager{
private:
	EffectManager();//�R���X�g���N�^
public:
	static HRESULT Init();
	static void Uninit();
	static EffectManager* Get();

	void Update(std::string effectName, XMFLOAT3 Pos, float Angle,bool push);	//2D�p
	void Update(std::string effectName,bool push);											//3D�p
	void Draw(std::string effectName);
	int Start(std::string effectName,XMFLOAT3 Pos,float Angle);	//2D�p
	int Start(std::string effectName, XMFLOAT3 Pos);					//3D�p
	void End(std::string effectName);										//�G�t�F�N�g�̋����I��

private:
	HRESULT Set(std::string effectName, std::string fileName);
	static EffectManager* m_pInstance;
};

