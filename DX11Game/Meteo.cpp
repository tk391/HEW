//=============================================================================
//覐�[Meteo.cpp]
//12/11 �V������
//=============================================================================
#include "Meteo.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "collision.h"
#include <vector>
#include"input.h"
#include "Game.h"


//�}�N��
#define MODEL_DEBLY			"data/model/meteorite.fbx"

//���f���̑傫���͎g�p���郂�f���ɂ���ē������Ċm�F���Ȃ���΂Ȃ�Ȃ�
#define DEBLY_SIZE		(16.375f/5.0f)
#define OUTSIDE_AREA_X	(-750.0f)			//�����@�G���A�O���W


//�f�u���̏������\����
struct TMeteo {
	XMFLOAT3 m_Pos;				// �ʒu
	int 		m_RotDir;			//��]����
	XMFLOAT3 m_fRot;			// �p�x

	XMFLOAT4X4	m_mtxWorld;	// ���[���h�}�g���b�N�X
};

struct TMeteoSet {
	int ���e�I�̐�;
	XMFLOAT3 m_Scl;				// �X�P�[��
	float	 m_fAngle;			// �x�N�g�������߂�p�̊p�x
	int m_count;	//�o���������
	bool		m_bVectorDir;		//�I�[�ʒu�Ŏg�p x=true,y=false
	float		m_fOverLine;		//�I�[�ʒu
	XMFLOAT3 m_DefaultPos;		// �����ʒu
	float m_fSpeed;				//�X�s�[�h

	int �����̃J�E���g;
	int ���e�I�̐��̃J�E���g;
	XMFLOAT3 m_Vector;				//�x�N�g��
	std::vector<TMeteo> OneMeteo;
};


//�N���X
Player* Meteo_Player;

//�O���[�o���ϐ�
static CAssimpModel	g_model;				// ���f��
static int g_nMeteoCount;					// �X�e�[�W���Ƃ�覐ΐ�
static int stagenum;

static std::vector<TMeteoSet> meteoArray;		// 覐΂��Ǘ����铮�I�z��

#if _DEBUG
static int g_count;		//�v���p
#endif // _DEBUG


//=============================================================================
//������
//=============================================================================
HRESULT Meteo::Init(int StageNum) {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// ���f���f�[�^�̓ǂݍ���
	if (!g_model.Load(pDevice, pDeviceContext, MODEL_DEBLY)) {
		MessageBoxA(GetMainWnd(), "���f���f�[�^�ǂݍ��݃G���[", "InitEnemy", MB_OK);
		return E_FAIL;
	}
	Create(StageNum);

	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;
	Setit = meteoArray.begin();	//�C�e���[�^�[�𓮓I�z��̍ŏ��Ɏ����Ă���
	while (Setit != meteoArray.end()) {
		Oneit = Setit->OneMeteo.begin();
		while (Oneit != Setit->OneMeteo.end()) {
			Oneit->m_Pos = Setit->m_DefaultPos; //�����ʒu��ۑ�����
			Oneit++;
		}
		Setit->m_Vector = XMFLOAT3(SinDeg(Setit->m_fAngle)*Setit->m_fSpeed, CosDeg(Setit->m_fAngle)*Setit->m_fSpeed, 0.0f);
		Setit->���e�I�̐��̃J�E���g = 1;
		Setit->�����̃J�E���g = 0;
		Setit++;
	}
#if _DEBUG
	g_count = 1;
#endif
	stagenum = StageNum;

	Meteo_Player = GetPlayer();
	return hr;
}

//=============================================================================
//�I��
//=============================================================================
void Meteo::Uninit(void) {
	// ���f���̉��
	g_model.Release();

	//���I�z�������
	std::vector<TMeteoSet>::iterator Setit;
	Setit = meteoArray.begin();	//�C�e���[�^�[�𓮓I�z��̍ŏ��Ɏ����Ă���
	while (Setit != meteoArray.end()) {
		Setit->OneMeteo.clear();
		Setit++;
	}
	meteoArray.clear();
}

//=============================================================================
//�X�V
//=============================================================================
void Meteo::Update(void) {
	//�X�e�[�W�R�A�S�ȊO�͉������Ȃ�
	if (!(stagenum == 3 || stagenum == 4)) {
		return;
	}
	XMMATRIX mtxWorld, mtxRot, mtxScl, mtxPos;
	//�C�e���[�^�[
	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;
	Setit = meteoArray.begin();	//�C�e���[�^�[�𓮓I�z��̍ŏ��Ɏ����Ă���
	while (Setit != meteoArray.end()) {
		Oneit = Setit->OneMeteo.begin();
		Setit->�����̃J�E���g++;
		if (Setit->�����̃J�E���g >= Setit->m_count&&Setit->���e�I�̐�>Setit->���e�I�̐��̃J�E���g) {
			Setit->���e�I�̐��̃J�E���g++;
			Setit->�����̃J�E���g = 0;
		}
		for(int i=0;i<Setit->���e�I�̐��̃J�E���g;++i) {
			//��]
			switch (Oneit->m_RotDir) {
			case 0:
				Oneit->m_fRot.x = Oneit->m_fRot.x + 0.3f;
				break;
			case 1:
				Oneit->m_fRot.y = Oneit->m_fRot.y + 0.3f;
				break;
			case 2:
				Oneit->m_fRot.z = Oneit->m_fRot.z + 0.3f;
				break;
			}
			//�ʒu�Ƀx�N�g�������Z
			Oneit->m_Pos.x += Setit->m_Vector.x;
			Oneit->m_Pos.y += Setit->m_Vector.y;

			//������X��Y��
			if (Setit->m_bVectorDir) {
				//���E�͉E������
				if (Setit->m_fOverLine >= 0) {
					//���E���z���Ă��邩
					if (Oneit->m_Pos.x > Setit->m_fOverLine) {
						//覐΂̍��W���f�t�H���g�ʒu�ɏC��
						Oneit->m_Pos = Setit->m_DefaultPos;
					}
				}
				else {
					if (Oneit->m_Pos.y < Setit->m_fOverLine) {
						//覐΂̍��W���f�t�H���g�ʒu�ɏC��
						Oneit->m_Pos = Setit->m_DefaultPos;
					}
				}
			}
			else {
				//���E�͏ォ����
				if (Setit->m_fOverLine >= 0) {
					//���E���z���Ă��邩
					if (Oneit->m_Pos.x > Setit->m_fOverLine) {
						//覐΂̍��W���f�t�H���g�ʒu�ɏC��
						Oneit->m_Pos = Setit->m_DefaultPos;
					}
				}
				else {
					if (Oneit->m_Pos.y < Setit->m_fOverLine) {
						//覐΂̍��W���f�t�H���g�ʒu�ɏC��
						Oneit->m_Pos = Setit->m_DefaultPos;
					}
				}
			}


			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			//�f�u����]�̔��f
			mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Oneit->m_fRot.x), XMConvertToRadians(Oneit->m_fRot.y), XMConvertToRadians(Oneit->m_fRot.z));
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//�X�P�[���𔽉f
			mtxScl = XMMatrixScaling(Setit->m_Scl.x, Setit->m_Scl.y, Setit->m_Scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ʒu�𔽉f
			mtxPos = XMMatrixTranslation(Oneit->m_Pos.x, Oneit->m_Pos.y, Oneit->m_Pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxPos);

			// ���[���h�}�g���b�N�X�ݒ�
			XMStoreFloat4x4(&Oneit->m_mtxWorld, mtxWorld);
			Oneit++;
		}
		Setit++;
	}

}

//=============================================================================
//�`��
//=============================================================================
void Meteo::Draw(void) {
	//�X�e�[�W�R�A�S�ȊO�͉������Ȃ�
	if (!(stagenum == 3 || stagenum == 4)) {
		return;
	}
	ID3D11DeviceContext* pDC = GetDeviceContext();

	//�C�e���[�^�[
	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;
	Setit = meteoArray.begin();

	while (Setit != meteoArray.end()) {
		Oneit = Setit->OneMeteo.begin();
		while (Oneit != Setit->OneMeteo.end()) {
			// �s����������`��
			g_model.Draw(pDC, Oneit->m_mtxWorld, eOpacityOnly);

			// ������������`��
			SetBlendState(BS_ALPHABLEND);	// �A���t�@�u�����h�L��
			SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�
			g_model.Draw(pDC, Oneit->m_mtxWorld, eTransparentOnly);
			SetZWrite(true);				// Z�o�b�t�@�X�V����
			SetBlendState(BS_NONE);			// �A���t�@�u�����h����

			Oneit++;
		}
		Setit++;
	}
}

//=============================================================================
//�Փ˔���
//=============================================================================
XMINT2 Meteo::Collision(XMFLOAT3 pos, XMFLOAT3 size) {
	//�X�e�[�W�R�A�S�ȊO�͉������Ȃ�
	if (!(stagenum == 3 || stagenum == 4)) {
		return XMINT2(-1,-1);
	}
	const float abc = 0.18f;//�����蔻��̔�����
	//�C�e���[�^�[
	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;
	Setit = meteoArray.begin();
	XMINT2 count = XMINT2(0,0);
	while (Setit != meteoArray.end()) {
		XMFLOAT3 m_Size = g_model.GetBBox();
		m_Size = XMFLOAT3(m_Size.x*Setit->m_Scl.x * abc, m_Size.y*Setit->m_Scl.y * abc, m_Size.z*Setit->m_Scl.z * abc);
		Oneit = Setit->OneMeteo.begin();
		while (Oneit != Setit->OneMeteo.end()) {
			int collisilon = CollisionSphere(Oneit->m_Pos, m_Size.x, pos, size.y);
			//������Ă����āA�������ĂȂ������玟�̔ԍ���
			if (collisilon == true) {
				return count;
			}
			Oneit++;
			count.y++;
		}
		
		count.x++;
		count.y = 0;
		Setit++;
	}
	//�����������Ă��Ȃ�������-1
	return XMINT2( -1,-1);
}

//=============================================================================
//�v���C���[�ƏՓ˂����f�u���̈ʒu���擾
//=============================================================================
XMFLOAT3 Meteo::GetPos(XMINT2 num) {
	return meteoArray[num.x].OneMeteo[num.y].m_Pos;
}

//=============================================================================
//�v���C���[�ƏՓ˂����f�u���̑傫�����擾
//=============================================================================
XMFLOAT3 Meteo::GetSize(XMINT2 num) {
	XMFLOAT3 size = g_model.GetBBox();
	size = XMFLOAT3(meteoArray[num.x].m_Scl.x*size.x, meteoArray[num.x].m_Scl.y*size.x, meteoArray[num.x].m_Scl.z*size.x);

	return size;
}

//=============================================================================
//�v���C���[�ƏՓ˂����f�u���̂������x�N�g�����擾
//=============================================================================
XMFLOAT3 Meteo::GetVector(XMINT2 num) {
	return meteoArray[num.x].m_Vector;
}

//=============================================================================
//���̃Z�b�g
//=============================================================================
void Meteo::Create(int StageNum) {
	switch (StageNum) {
		//									���e�I�̐�		�傫���@�@					�p�x�@ �����@		�����t���O�@�I�[�ʒu�@				�����ʒu�@		�X�s�[�h
	case 3:
		meteoArray.push_back({ 7, XMFLOAT3(20.5f, 20.5f, 20.5f), 135.0f,300,false,-SCREEN_HEIGHT,  XMFLOAT3(-750.0f,260.0f,0.0f),1.0f });
		meteoArray.push_back({ 2, XMFLOAT3(30.5f,30.5f,30.5f),150.0f,1000,false, -SCREEN_HEIGHT, XMFLOAT3(-395.0f,470.0f,0.0f),0.7f });
		meteoArray.push_back({ 7,XMFLOAT3(20.5f,20.5f,20.5f),290.0f,300,true,-SCREEN_WIDTH ,XMFLOAT3(750.0f,-50.0f,0.0f),1.0f });
		break;
	case 4:
		meteoArray.push_back({ 7,XMFLOAT3(20.0f,20.0f,20.0f),200.0f,300,false,-SCREEN_HEIGHT,XMFLOAT3(10.0f,470.0f,0.0f),1.0f });
		meteoArray.push_back({ 7,XMFLOAT3(26.5f,26.5f,26.5f),180.0f,300,false,-SCREEN_HEIGHT,XMFLOAT3(260.0f,470.0f,0.0f),1.0f });
		meteoArray.push_back({ 7,XMFLOAT3(13.5f,13.5f,13.5f),220.0f,300,false,-SCREEN_HEIGHT,XMFLOAT3(570.0f,470.0f,0.0f),1.0f });
		meteoArray.push_back({ 7,XMFLOAT3(16.0f,16.0f,16.0f),255.0f,300,false,-SCREEN_HEIGHT,XMFLOAT3(750.0f,-195.0f,0.0f) ,1.0f});
		break;
	default:
		break;
	}
	//�C�e���[�^�[
	std::vector<TMeteoSet>::iterator Setit;
	std::vector<TMeteo>::iterator Oneit;

	Setit = meteoArray.begin();	//�C�e���[�^�[�𓮓I�z��̍ŏ��Ɏ����Ă���
	while (Setit != meteoArray.end())	{
		Setit->OneMeteo.resize(Setit->���e�I�̐�);
		Oneit = Setit->OneMeteo.begin();
		while (Oneit != Setit->OneMeteo.end()) {
			Oneit->m_RotDir = rand() % 3;
			Oneit++;
		}
		Setit++;
	}
}
