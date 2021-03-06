#pragma once
#include "main.h"

//前方宣言
class Life;

class Player
{
public:
	//4大処理
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//4大処理意外のpublicメンバ関数
	XMFLOAT3& GetPos();				//位置取得
	float GetO2();					//酸素量取得（リザルトで使用）
	void  SetAnimTelopFlag(bool);	//テロップアニメーションのフラグセット	// 01/22 三輪卓弘

private:
	//privateメンバ関数
	float RotZ(XMFLOAT2);							//角度取得
	void Animator(int animNum);					//アニメーションを一括管理
	void GameOverAnimation();					//酸素が切れた時のアニメーション
	void GameClearAnimation();					//クリアした時のアニメーション
	void Repatriation(XMFLOAT3 Vector);		//範囲外の強制送還

	//メンバ変数
	XMFLOAT3		m_Pos;			// 現在の位置
	XMFLOAT3		m_Rot;			// 現在の向き
	XMFLOAT3		m_RotDest;	// 目的の向き
	XMFLOAT3		m_Move;		// 移動量
	XMFLOAT3		m_Scl;			//大きさ
	float m_fLife;						//ライフゲージ
	bool m_bFlag;					//最初から動くのを防ぐフラグ
	bool m_bCollision;				//ぶつかっているフラグ
	bool m_bPull[2];				//カメラを引くフラグ（0:ジェット時　1:全体）
	bool m_bHold;						//隕石をつかんでいる判定

	int m_nAnimNum;				//現在のアニメーションの番号
	XMFLOAT3		m_Accel;	//加速度 12/24 X

	XMFLOAT3        m_OldRot;  // 1フレーム前の角度　12/26 X
};

