//=============================================================================
// 衝突判定 [collision.cpp]
//12/11 齋藤成希
//=============================================================================
#include "collision.h"

//=============================================================================
// 球同士の衝突判定
//=============================================================================
bool CollisionSphere(XMFLOAT3 Apos, float Ar, XMFLOAT3 Bpos, float Br)
{
	float dx = Apos.x - Bpos.x;
	float dy = Apos.y - Bpos.y;
	float dz = Apos.z - Bpos.z;
	float dr = Ar + Br;
	return dx * dx + dy * dy + dz * dz <= dr * dr;
}

//=============================================================================
// AABB同士の衝突判定
//=============================================================================
int CollisionAABB(XMFLOAT3 Apos, XMFLOAT3 Asize, XMFLOAT3 Bpos, XMFLOAT3 Bsize)
{
	//B=Player
	//A=Debly
	
	if (Bpos.x - Bsize.x <= Apos.x + Asize.x&&
		Apos.x - Asize.x <= Bpos.x + Bsize.x&&
		Bpos.y - Bsize.y <= Apos.y + Asize.y&&
		Apos.y - Asize.y <= Bpos.y + Bsize.y) {
		float X = Apos.x + Asize.x - Bpos.x - Bsize.x;
		float Y = Bpos.x + Bsize.x - Apos.x - Asize.y;
		XMFLOAT2 H, W;
		W = XMFLOAT2(Apos.x + Asize.x - Bpos.x - Bsize.x, Bpos.x + Bsize.x - Apos.x - Asize.x);
		H = XMFLOAT2(Apos.y + Asize.y - Bpos.y - Bsize.y, Bpos.y + Bsize.y - Apos.y - Asize.y);
		if (W.x<W.y) {//Aの右にB
			if (Bpos.y >= Apos.y - Asize.y&&Bpos.y <= Apos.y + Asize.y) {
				//Bの中心Y座標はAのサイズYからはみ出していない
				return 1;
			}
			else {
				//Bの中心Y座標はAのサイズYからはみ出している→斜め
				return 5;
			}
		}
		else if (W.y<W.x) {//Aの左にB
			if (Bpos.y >= Apos.y - Asize.y&&Bpos.y <= Apos.y + Asize.y) {
				//Bの中心Y座標はAのサイズYからはみ出していない
				return 2;
			}
			else {
				//Bの中心Y座標はAのサイズYからはみ出している→斜め
				return 5;
			}
		}
		else if (W.x < W.y) {//Aの上にB
			if (Bpos.x >= Apos.x - Asize.x&&Bpos.x <= Apos.x + Asize.x) {
				////Bの中心X座標はAのサイズXからはみ出していない
				return 3;
			}
			else {
				//Bの中心X座標はAのサイズXからはみ出している→斜め
				return 5;
			}
		}
		else if (W.y < W.x) {
			if (Bpos.x >= Apos.x - Asize.x&&Bpos.x <= Apos.x + Asize.x) {
				////Bの中心X座標はAのサイズXからはみ出していない
				return 4;
			}
			else {
				//Bの中心X座標はAのサイズXからはみ出している→斜め
				return 5;
			}
		}
		else {
			//どこにも触れていない
			return -1;
		}
	}
	else {
		return -1;
	}
}