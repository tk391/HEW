//=============================================================================
// �Փ˔��� [collision.cpp]
//12/11 �V������
//=============================================================================
#include "collision.h"

//=============================================================================
// �����m�̏Փ˔���
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
// AABB���m�̏Փ˔���
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
		if (W.x<W.y) {//A�̉E��B
			if (Bpos.y >= Apos.y - Asize.y&&Bpos.y <= Apos.y + Asize.y) {
				//B�̒��SY���W��A�̃T�C�YY����͂ݏo���Ă��Ȃ�
				return 1;
			}
			else {
				//B�̒��SY���W��A�̃T�C�YY����͂ݏo���Ă��遨�΂�
				return 5;
			}
		}
		else if (W.y<W.x) {//A�̍���B
			if (Bpos.y >= Apos.y - Asize.y&&Bpos.y <= Apos.y + Asize.y) {
				//B�̒��SY���W��A�̃T�C�YY����͂ݏo���Ă��Ȃ�
				return 2;
			}
			else {
				//B�̒��SY���W��A�̃T�C�YY����͂ݏo���Ă��遨�΂�
				return 5;
			}
		}
		else if (W.x < W.y) {//A�̏��B
			if (Bpos.x >= Apos.x - Asize.x&&Bpos.x <= Apos.x + Asize.x) {
				////B�̒��SX���W��A�̃T�C�YX����͂ݏo���Ă��Ȃ�
				return 3;
			}
			else {
				//B�̒��SX���W��A�̃T�C�YX����͂ݏo���Ă��遨�΂�
				return 5;
			}
		}
		else if (W.y < W.x) {
			if (Bpos.x >= Apos.x - Asize.x&&Bpos.x <= Apos.x + Asize.x) {
				////B�̒��SX���W��A�̃T�C�YX����͂ݏo���Ă��Ȃ�
				return 4;
			}
			else {
				//B�̒��SX���W��A�̃T�C�YX����͂ݏo���Ă��遨�΂�
				return 5;
			}
		}
		else {
			//�ǂ��ɂ��G��Ă��Ȃ�
			return -1;
		}
	}
	else {
		return -1;
	}
}