//=============================================================================
// エフェクト管理クラス[EffectManager.h]
// 12/19 齋藤成希
//1/30 シングルトン化
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
	EffectManager();//コンストラクタ
public:
	static HRESULT Init();
	static void Uninit();
	static EffectManager* Get();

	void Update(std::string effectName, XMFLOAT3 Pos, float Angle,bool push);	//2D用
	void Update(std::string effectName,bool push);											//3D用
	void Draw(std::string effectName);
	int Start(std::string effectName,XMFLOAT3 Pos,float Angle);	//2D用
	int Start(std::string effectName, XMFLOAT3 Pos);					//3D用
	void End(std::string effectName);										//エフェクトの強制終了

private:
	HRESULT Set(std::string effectName, std::string fileName);
	static EffectManager* m_pInstance;
};

