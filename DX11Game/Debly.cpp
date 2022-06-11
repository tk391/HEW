//=============================================================================
//�f�u��[debly.cpp]
//12/11 �V������
//=============================================================================
#include "Debly.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "collision.h"
#include <vector>
#include"input.h"


//�}�N��
#define MODEL_DEBLY		"data/model/deburi1.fbx"

//���f���̑傫���͎g�p���郂�f���ɂ���ē������Ċm�F���Ȃ���΂Ȃ�Ȃ�
#define DEBLY_SIZE		(16.375f/5.0f)

//�f�u���̏������\����
struct TDebly {
	XMFLOAT3	m_Pos;		// �ʒu
	XMFLOAT3 m_Scl;		//�X�P�[��
	int		m_RotDir;
	XMFLOAT3 m_Rot; //��]

	XMFLOAT4X4	m_mtxWorld;	// ���[���h�}�g���b�N�X
};


//�O���[�o���ϐ�
static CAssimpModel	g_model;				// ���f��
static int g_nDeblyCount;						//�X�e�[�W���Ƃ̃L���[�u��

static std::vector<TDebly> deblyArray;	//�L���[�u���Ǘ����铮�I�z��

#if _DEBUG
static int g_count;		//�v���p
#endif // _DEBUG


//=============================================================================
//������
//=============================================================================
HRESULT Debly::Init(int stageNum) {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// ���f���f�[�^�̓ǂݍ���
	if (!g_model.Load(pDevice, pDeviceContext, MODEL_DEBLY)) {
		MessageBoxA(GetMainWnd(), "���f���f�[�^�ǂݍ��݃G���[", "InitEnemy", MB_OK);
		return E_FAIL;
	}
	Create(stageNum);

#if _DEBUG
	g_count = 1;
#endif

	return hr;
}

//=============================================================================
//�I��
//=============================================================================
void Debly::Uninit(void) {
	// ���f���̉��
	g_model.Release();

	//���I�z�������
	deblyArray.clear();
}

//=============================================================================
//�X�V
//=============================================================================
void Debly::Update(void) {
	XMMATRIX mtxWorld, mtxRot, mtxScl, mtxPos;

	//�C�e���[�^�[
	std::vector<TDebly>::iterator it;
	it = deblyArray.begin();	//�C�e���[�^�[�𓮓I�z��̍ŏ��Ɏ����Ă���
	while (it != deblyArray.end()) {
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		//�łԂ�̉�]
		switch (it->m_RotDir)
		{
		case 0:
			it->m_Rot.x = it->m_Rot.x + 0.3f;
			break;
		case 1:
			it->m_Rot.y = it->m_Rot.y + 0.3f;
			break;
		case 2:
			it->m_Rot.z = it->m_Rot.z + 0.3f;
			break;
		}
		//it->m_Rot.z = it->m_Rot.z + 0.3f;
		//int num = rand() % 4


		//�f�u����]�̔��f
		mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(it->m_Rot.x), XMConvertToRadians(it->m_Rot.y), XMConvertToRadians(it->m_Rot.z));
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		//�X�P�[���𔽉f
		mtxScl = XMMatrixScaling(it->m_Scl.x, it->m_Scl.y, it->m_Scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// �ʒu�𔽉f
		mtxPos = XMMatrixTranslation(it->m_Pos.x, it->m_Pos.y, it->m_Pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxPos);

		// ���[���h�}�g���b�N�X�ݒ�
		XMStoreFloat4x4(&it->m_mtxWorld, mtxWorld);
		it++;
	}
}

//=============================================================================
//�`��
//=============================================================================
void Debly::Draw(void) {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	//�C�e���[�^�[
	std::vector<TDebly>::iterator it;
	it = deblyArray.begin();
	while (it != deblyArray.end()) {

		// �s����������`��
		g_model.Draw(pDC, it->m_mtxWorld, eOpacityOnly);

		// ������������`��
		SetBlendState(BS_ALPHABLEND);	// �A���t�@�u�����h�L��
		SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�
		g_model.Draw(pDC, it->m_mtxWorld, eTransparentOnly);
		SetZWrite(true);				// Z�o�b�t�@�X�V����
		SetBlendState(BS_NONE);			// �A���t�@�u�����h����

		it++;
	}
}

//=============================================================================
//�Փ˔���
//=============================================================================
int Debly::Collision(XMFLOAT3 pos, XMFLOAT3 size) {
	//�C�e���[�^�[
	std::vector<TDebly>::iterator it;
	it = deblyArray.begin();
	int count = 0;
	while (it != deblyArray.end()) {
		XMFLOAT3 m_Size = g_model.GetBBox();
		m_Size = XMFLOAT3(m_Size.x*it->m_Scl.x * 0.9f, m_Size.y*it->m_Scl.y * 0.9f, m_Size.z*it->m_Scl.z * 0.9f);
		bool collisilon = CollisionSphere(it->m_Pos, m_Size.x, pos, size.y);
		//������Ă����āA�������ĂȂ������玟�̔ԍ���
		if (collisilon == false) {
			it++;
			count++;
			continue;
		}
		//�������Ă��炻�̃��f���̔ԍ���Ԃ�
		return count;
	}
	//�����������Ă��Ȃ�������-1
	return -1;
}

//=============================================================================
//�v���C���[�ƏՓ˂����f�u���̈ʒu���擾
//=============================================================================
XMFLOAT3 Debly::GetPos(int num) {
	return deblyArray[num].m_Pos;
}

//=============================================================================
//�v���C���[�ƏՓ˂����f�u���̑傫�����擾
//=============================================================================
XMFLOAT3 Debly::GetSize(int num) {
	XMFLOAT3 size = XMFLOAT3(deblyArray[num].m_Scl.x*DEBLY_SIZE,
		deblyArray[num].m_Scl.y*DEBLY_SIZE,
		deblyArray[num].m_Scl.z*DEBLY_SIZE);
	return size;
}


//=============================================================================
//���̃Z�b�g
//=============================================================================
void Debly::Create(int StageNum) {
	switch (StageNum) {
	case 1:
		//�X�e�[�W1�ŕK�v�ȃf�u���𐶐����Ă���
		//							�ʒu							�傫��
		deblyArray.push_back({ XMFLOAT3(-520.0f - 100.0f,   0.8f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�������獶�̃f�u��[0]
		deblyArray.push_back({ XMFLOAT3(-462.0f - 100.0f,  10.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-402.0f - 100.0f,   0.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-334.0f - 100.0f, -14.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-522.0f - 100.0f,-134.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-458.0f - 100.0f,-126.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-390.0f - 100.0f,-142.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-512.0f - 100.0f,-190.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-204.0f - 100.0f, -92.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-145.0f - 100.0f, -82.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-202.0f - 100.0f,-148.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-146.0f - 100.0f,-158.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-202.0f - 100.0f,-234.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-148.0f - 100.0f,-210.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-162.0f - 100.0f,-288.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-184.0f - 100.0f,-342.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�����܂ł����̃f�u��[15]
		deblyArray.push_back({ XMFLOAT3(14.0f - 100.0f,-186.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//��������^�񒆂̃f�u��[16]
		deblyArray.push_back({ XMFLOAT3(76.0f - 100.0f,-178.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(132.0f - 100.0f,-178.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(96.0f - 100.0f,-120.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(112.0f - 100.0f, -66.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(80.0f - 100.0f, -18.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(98.0f - 100.0f,  38.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(64.0f - 100.0f,  90.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(44.0f - 100.0f,  150.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(50.0f - 100.0f,  200.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(60.0f - 100.0f,  250.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(00.0f - 100.0f,  245.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-70.0f - 100.0f,  255.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-150.0f - 100.0f,  320.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-80.0f - 100.0f,  310.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(20.0f - 100.0f,  330.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�����܂ł��^�񒆂̃f�u��[31]


		deblyArray.push_back({ XMFLOAT3(644.0f - 100.0f, -40.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//��������E���̃f�u��[32]
		deblyArray.push_back({ XMFLOAT3(584.0f - 100.0f, -36.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(520.0f - 100.0f, -36.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(460.0f - 100.0f, -14.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(410.0f - 100.0f, -58.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(432.0f - 100.0f,-114.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(472.0f - 100.0f,-156.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(516.0f - 100.0f,-198.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(580.0f - 100.0f,-210.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(632.0f - 100.0f,-236.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(688.0f - 100.0f,-264.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�����܂ł��E���̃f�u��[42]

		break;
	case 2:
		deblyArray.push_back({ XMFLOAT3(-422.0f,  -61.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�������獶�̃f�u��[0]
		deblyArray.push_back({ XMFLOAT3(-412.0f, -124.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-448.0f, -174.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-508.0f,-204.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-568.0f,-222.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-628.0f,-220.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-596.0f,-358.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�����܂ł����̃f�u��[6]
		deblyArray.push_back({ XMFLOAT3(-248.0f,  -10.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//��������^�񒆂̃f�u��[7]
		deblyArray.push_back({ XMFLOAT3(-198.0f, 46.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-140.0f, 149.0f - 85.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-76.0f, 157.0f - 85.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(18.0f, 175.0f - 85.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(76.0f, 191.0f - 85.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(132.0f, 225.0f - 85.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(192.0f, 185.0f - 85.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(260.0f, 139.0f - 85.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });

		deblyArray.push_back({ XMFLOAT3(380.0f, 0.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(440.0f, -50.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(340.0f, 65.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(410.0f, -115.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(400.0f, -185.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(440.0f, -255.0f, 0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });


		deblyArray.push_back({ XMFLOAT3(38.0f,-241.0f - 85.0f, 0.0f),XMFLOAT3(55.0f,55.0f,55.0f) });//�����܂ł��^��[16]
		break;
	case 3:
		deblyArray.push_back({ XMFLOAT3(-600.0f,-167.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�������獶�̃f�u��[0]
		deblyArray.push_back({ XMFLOAT3(-538.0f,-167.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-476.0f,-189.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-428.0f,-155.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-564.0f,-227.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-506.0f,-245.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-434.0f,-247.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-472.0f,-309.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-412.0f,-307.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-344.0f,-315.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-400.0f,32.f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-350.0f,-17.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-306.0f,-63.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-268.0f,-113.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-218.0f,-151.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-150.0f,-163.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-90.0f,-195.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-80.0f,115.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[17]
		deblyArray.push_back({ XMFLOAT3(0.0f,170.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[18]
		deblyArray.push_back({ XMFLOAT3(-70.0f,185.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[19]
		deblyArray.push_back({ XMFLOAT3(-18.0f,113.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//��������^�񒆂̃f�u��[20]
		deblyArray.push_back({ XMFLOAT3(-14.0f,-19.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(26.0f,-65.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(72.0f,-19.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(108.0f,23.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(62.0f,-117.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(112.0f,-65.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(156.0f,-13.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(216.0f,-11.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(276.0f,-9.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(280.0f,-73.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(216.0f,-77.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(154.0f,-109.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(108.0f,-157.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(158.0f,-211.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(150.0f,-270.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(200.0f,-150.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(225.0f,-245.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(200.0f,-320.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[38]

		break;
	case 4:
		deblyArray.push_back({ XMFLOAT3(-544.0f,13.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�������獶�̃f�u��[0]
		deblyArray.push_back({ XMFLOAT3(-486.0f,27.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-432.0f,57.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-372.0f,63.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-314.0f,69.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-482.0f,-103.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-430.0f,-79.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-372.0f,-55.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-308.0f,-73.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-274.0f,-127.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-326.0f,-171.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-382.0f,-199.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-438.0f,-192.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-490.0f,-163.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-534.0f,-309.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-476.0f,-337.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-408.0f,-335.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-350.0f,-347.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[17]
		deblyArray.push_back({ XMFLOAT3(-140.0f,-195.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//��������^�񒆂̃f�u��[18]
		deblyArray.push_back({ XMFLOAT3(-104.0f,-139.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-46.0f,-127.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-78.0f,-77.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-20.0f,-75.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(38.0f,-75.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-42.0f,100.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(20.0f,115.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(86.0f,120.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-42.0f,-20.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(20.0f,-15.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(86.0f,-25.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(58.0f,-237.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(110.0f,-197.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(144.0f,-147.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(142.0f,-243.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(180.0f,-193.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[31]
		deblyArray.push_back({ XMFLOAT3(334.0f,-141.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[32]migi
		deblyArray.push_back({ XMFLOAT3(346.0f,-205.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(348.0f,-327.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(350.0f,5.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(352.0f,67.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(350.0f,123.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(412.0f,15.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(434.0f,69.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(464.0f,127.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(478.0f,19.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(538.0f,11.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[42]
		break;
	case 5:
		deblyArray.push_back({ XMFLOAT3(-470.0f,70.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�������獶�̃f�u��[0]
		deblyArray.push_back({ XMFLOAT3(-545.0f,50.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�������獶�̃f�u��[0]
		deblyArray.push_back({ XMFLOAT3(-625.0f,55.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�������獶�̃f�u��[0]
		deblyArray.push_back({ XMFLOAT3(-395.0f,45.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�������獶�̃f�u��[0]

		deblyArray.push_back({ XMFLOAT3(-432.0f,3.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//�������獶�̃f�u��[0]
		deblyArray.push_back({ XMFLOAT3(-394.0f,-43.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-436.0f,-83.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-476.0f,-123.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-426.0f,-155.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-366.0f,-157.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-306.0f,-161.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-246.0f,-147.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-614.0f,-271.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-572.0f,-315.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-506.0f,-313.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-454.0f,-335.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-260.0f,-335.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-204.0f,-313.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[13]
		deblyArray.push_back({ XMFLOAT3(-150.0f,-281.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//��������^�񒆂̃f�u��[14]
		deblyArray.push_back({ XMFLOAT3(-86.0f,-247.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-32.0f,-229.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(-4.0f,-171.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(22.0f,-111.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(400.0f,171.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(458.0f,121.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(512.0f,161.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(538.0f,89.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });
		deblyArray.push_back({ XMFLOAT3(538.0f,21.0f,0.0f),XMFLOAT3(5.0f,5.0f,5.0f) });//[23]

		//�u���b�N�z�[���̍��W�����
		/*deblyArray.push_back({ XMFLOAT3(-354.0f,-332.0f,0.0f),XMFLOAT3(7.0f,7.0f,7.0f) });
		deblyArray.push_back({ XMFLOAT3(-164.0f,-64.0f,0.0f),XMFLOAT3(7.0f,7.0f,7.0f) });
		deblyArray.push_back({ XMFLOAT3(150.0f,-130.0f,0.0f),XMFLOAT3(7.0f,7.0f,7.0f) });*/
	default:
		break;
	}
	//�C�e���[�^�[
	std::vector<TDebly>::iterator it;
	it = deblyArray.begin();	//�C�e���[�^�[�𓮓I�z��̍ŏ��Ɏ����Ă���
	while (it != deblyArray.end())
	{
		it->m_RotDir = rand() % 3;
		it++;
	}
}
