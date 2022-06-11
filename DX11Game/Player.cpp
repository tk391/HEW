//=============================================================================
// �v���C���[�i�F����s�m�j
// 12/11 �V������
//=============================================================================
#include "Player.h"
#include "main.h"
#include "Scene.h"
#include "input.h"
#include "AssimpModel.h"
#include "Game.h"
#include "debugproc.h"
#include "Life.h"
#include "Sound.h"
#include "fade.h"
#include "AlertBg.h"
#include "BlackHole.h"
#include "Select.h"
#include <stdlib.h>	//��Βl�����߂邽�߂̊֐����g�����߂Ɂ@0129	�O�֑�O

// �}�N����`

#define	VALUE_MOVE_PLAYER		(0.20f)		// �ړ����x
#define	RATE_MOVE_PLAYER		(0.20f)		// �ړ������W��
#define	VALUE_ROTATE_PLAYER		(3.5f)		// ��]���x
#define	RATE_ROTATE_PLAYER		(0.20f)		// ��]�����W��
#define VALUE_ROWMOVE_PLAYER	(0.3f)		//�������̈ړ����x

#define PLAYER_SIZE_X		(7.0f)			//�T�C�Y
#define PLAYER_SIZE_Y		(7.0f)			//�T�C�Y

#define MAX_LIFE			(100.0f)		//���C�t�ő吔(���Œ�)

//��Փx�n(��������)==============================================================================================
//================================================================================================================

//���R�R�厖!!�@�Q�[���v���C�̎�G�芴�ɑ傫���e������v�f�B�X�g���X�t���[�����ǂ�������Ƃӂ�ӂ튴����������̂����z
#define INFLUENCE			(0.1f)			//�����e����(���l�������قǈړ����x�����f����₷��)  1/25 �@�� ���l�ύX
#define MAX_MOVE			(1.3f)			//�ړ����x�̏���l					0129	�O�֑�O�ǉ�	



#define DEL_JET				(0.06f)			//���C�t������(�W�F�b�g�g�p��)		  1/25 �@�� ���l�ύX
#define DEL_STAY			(0.02f)			//���C�t������(�W�F�b�g���g�p��)	  1/25 �@�� ���l�ύX
#define DEFF_CAMERA_Z		(350.0f)		//�J�����̃f�t�H���gZ���W (�v���C���[�Ƃ̋���)   1/25 �@�� ���l�ύX

#define BLACKHOLE_POWER		(0.02f)			//�u���b�N�z�[���̋z����
#define REFLECTION			(0.6f)			//�f�u���̔��˗�

// �O���[�o���ϐ�

static CAssimpModel		g_model[2];		// ���f��
static XMFLOAT4X4		g_mtxWorld;			// ���[���h�}�g���b�N�X
static int				g_nShadow;				// �ۉe�ԍ�
static XMFLOAT3			g_Size;				// �����蔻��p�̃T�C�Y
static float			g_RotXPrev;					// 1�t���[���O�̊p�x
static int				g_nCountFrame;		// �A�j���[�V���������Z����t���[��
static double			g_d1FrameAnim;		// 1�t���[��������̃A�j���[�V����
static double			g_dSetAnim;				// ��L2�ϐ�����������->SetAnimTime()�ɓ����
static XMFLOAT3			g_effectPos;		// �G�t�F�N�g�p�ʒu
static XMFLOAT3			g_CameraPos;	// �J�����ʒu
static int				g_nBlackHoleTime;	// �u���b�N�z�[���̎��ԃJ�E���g
static bool				g_bEffectJetFlag;	// �W�F�b�g�����������̃t���O
static bool				g_bOldEffectJetFlag;	// �W�F�b�g�����������̃t���O

static XMFLOAT3			g_OldPos;			// �O�܂Ńv���C���[�������ʒu	01/17	������n
static bool				g_RefFlg;					// ���˕����̐ݒ�				01/18	������n
static bool				g_StartRef;				// ���ˊJ�n						01/18	������n
static XMFLOAT3			g_OldMove;		// �O�܂ł̈ړ���				01/18	������n

static bool				g_bClearGameOverFlag[2];		//�N���A���̓Q�[���I�[�o�[�������𔻒�
static bool				g_goalCollisionFlag;				//�S�[���ƃv���C���[�����S�ɐG��Ă��邩(�N���A���Ɏg�p)
static bool				g_goalCollisionSEFlag;					//�S�[�������u�Ԃ��ǂ���(�N���A����SE�Ɏg�p)
static int				gameOver_stopCount;			//�Q�[���I�[�o�[���Ɉꎞ��~������ׂ̃J�E���g
static bool				g_bAnimTelopFlag;					//�e���b�v�A�j���[�V�������Đ������ǂ����̃t���O // 01/21 �O�֑�O
static bool				g_bOutFlag;							//��ʊO�ɏo����
static bool				g_bRepatriationFlag;				//�������Ғ���
static int				g_nStageNum;						//�X�e�[�W�ԍ�

//�萔
const float AnimationTime = 120.0f;					//�A�j���[�V�����̍Đ����ԁi�t���[�����j
const float AleattelopPos = 70.0f;						//�_�f������ʒu���牽���Ȃ��ʒu�܂ł̋���
const float AleatRepatriationPos = 100.0f;			//�_�f������ʒu���狭�����҂����ʒu�܂ł̋���

const float warningArea[5][4]{//�e�X�e�[�W�͈̔͊O�̋��E��������𒴂���ƃA���[�g����
	//��			��			��			�E
	{100.0f,-350.0f,-730.0f,700.0f},	//	�X�e�[�W1
	{100.0f,-350.0f,-730.0f,700.0f},	//	�X�e�[�W2
	{100.0f,-350.0f,-730.0f,700.0f},	//	�X�e�[�W3
	{100.0f,-350.0f,-730.0f,700.0f},	//	�X�e�[�W4
	{100.0f,-350.0f,-730.0f,700.0f}	//	�X�e�[�W5
	//�e�X�e�[�W�Œ��������肢���܂�
};
enum Area {//�㉺���E��warningArea�Ŏg�p
	UP = 0, DOWN, LEFT, RIGHT
};
enum anim {//�A�j���[�V����
	amimAll = 0, stay, hold, release, jet
};

//���f��
const std::string ModelData[]{
	"data/model/Complete.fbx",
	"data/model/aaaa.fbx"
};

//�N���X��`
Life* Player_Life;			//Life�N���X
Debly* Player_Debly;		//Debly�N���X
Rayer* Player_Rayer;		//Rayer�N���X
Meteo* Player_Meteo;
Goal* Player_Goal;
BlackHole* Player_BlackHole;
//============================================
// ����������
//============================================
HRESULT Player::Init(void)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// �����o�ϐ��i�O���[�o���ϐ��j�̏�����
	m_Pos = XMFLOAT3(-526.0f, -63.0f, 0.0f);
	m_Move = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Rot = XMFLOAT3(90.0f, -90.0f, 0.0f);
	m_RotDest = XMFLOAT3(0.0f, -75.0f, 0.0f);
	m_Scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_fLife = MAX_LIFE;
	m_bFlag = false;
	m_bPull[0] = false;
	m_bPull[1] = false;
	m_bHold = false;
	g_Size = g_model[0].GetBBox();
	g_RotXPrev = 0.0f;
	g_effectPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_CameraPos = XMFLOAT3(m_Pos.x, m_Pos.y, -DEFF_CAMERA_Z);
	g_bAnimTelopFlag = true;
	g_bOutFlag = false;
	g_bRepatriationFlag = false;

	//���˂Ɋւ��֐��̏�����
	//01/18	������n
	g_OldPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_RefFlg = true;
	g_StartRef = false;
	g_OldMove = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 2; i++) {
		g_bClearGameOverFlag[i] = false;
	}
	g_goalCollisionFlag = false;
	gameOver_stopCount = 30;
	m_Accel = XMFLOAT3(0.0f, 0.0f, 0.0f);//12/24 X
	m_OldRot = XMFLOAT3(0.0f, 0.0f, 0.0f); //12 / 26 X
	g_nStageNum = GetStageNum();
	// ���f���f�[�^�̓ǂݍ���
	for (int i = 0; i < 2; i++) {
		if (!g_model[i].Load(pDevice, pDeviceContext, ModelData[i].c_str())) {
			MessageBoxA(GetMainWnd(), "���f���f�[�^�ǂݍ��݃G���[", "InitModel", MB_OK);
			return E_FAIL;
		}
	}

	//�A�j���[�V�����Ǘ�(�����ݒ�)
	m_nAnimNum = stay;
	g_model[0].SetAnimIndex(m_nAnimNum);
	g_d1FrameAnim = g_model[0].GetAnimDuration(stay) / AnimationTime;
	g_bEffectJetFlag = false;
	g_bOldEffectJetFlag = false;

	//�J�����̃Z�b�g
	CCamera::Get()->SetPos(m_Pos.x, m_Pos.y, m_Pos.z - DEFF_CAMERA_Z);
	CCamera::Get()->SetTarget(m_Pos);

	//���̃N���X�Ŏg�p���鑼�̃N���X�̏����擾
	Player_Life = GetLife();
	Player_Debly = GetDebly();
	Player_Rayer = GetRayer();
	Player_Meteo = GetMeteo();
	Player_BlackHole = GetBlackHole();
	Player_Goal = GetGoal();

	return hr;
}

//============================================
// �I������
//============================================
void Player::Uninit(void)
{
	CSound::Stop(SE_GOSHIP);
	// ���f���̉��
	for (int i = 0; i < 2; i++) {
		g_model[0].Release();
	}
}

//============================================
// �X�V����
//============================================
void Player::Update(void)
{
	if (!g_bAnimTelopFlag)
	{
		//�����蔻��
		static int nTouchDebly, nTouchDeblyPrev;
		static XMINT2 nTouchMeteo, nTouchMeteoPrev;
		XMFLOAT4 fTouchBlackHole = Player_BlackHole->Collision(m_Pos, g_Size);
		nTouchDebly = Player_Debly->Collision(m_Pos, g_Size);
		nTouchMeteo = Player_Meteo->Collision(m_Pos, g_Size);

		//01/17 ������n
		g_Size = g_model[0].GetBBox();
		g_OldPos = m_Pos;
		//��O�̊p�x���i�[
		g_RotXPrev = m_RotDest.x;

		// �J�����̌����擾
		XMFLOAT3 rotCamera = CCamera::Get()->GetAngle();

		//�p�b�h����
		XMFLOAT2  JOY = XMFLOAT2((float)GetJoyX(0), (float)GetJoyY(0));//���X�e�B�b�N�̍��W���p�x�┭�˕����Ɏg��
		float JOYZ = (float)GetJoyZ(0);//ZL,ZR�̓��͓x���������o�i0������ZL���������Ƃ�,0�ȏとZR���������Ƃ��j
		//�^����^��ɂ�������悤�ɂ���(���܂�ɏ��������͎͂󂯕t���Ȃ�)
		if (JOY.x <= 2000.0f&&JOY.x >= -2000.0f) {
			JOY.x = 0.0f;
		}
		if (JOY.y <= 2000.0f&&JOY.y >= -2000.0f) {
			JOY.y = 0.0f;
		}
		if (JOYZ <= 5000.0f&&JOYZ >= -5000.0f) {
			JOYZ = 0.0f;
		}

		//�N���A���̓Q�[���I�[�o�[�̃t���O�������Ă�����
		if (g_bClearGameOverFlag[0]) {
			//�N���A���o
			GameClearAnimation();

		}
		else if (g_bClearGameOverFlag[1]) {
			//�Q�[���I�[�o�[���o
			GameOverAnimation();
		}
		//�Q�[�����̉��o
		else {

			//�ŏ��͓����Ȃ�
			//�ŏ��̃X�y�[�X�L�[�œ����n�߂�
			if (m_bFlag) {
				m_Move.x -= SinDeg(m_Rot.x) * VALUE_MOVE_PLAYER;
				m_Move.y += CosDeg(m_Rot.x) * VALUE_MOVE_PLAYER;
			}

			//�p�x�����o���Ċi�[
			if (JOY.x == 0.0f&&JOY.y == 0.0f) {//�W���C�X�e�B�b�N�̓��͂��Ȃ������ꍇ
				m_RotDest.x = g_RotXPrev;
			}
			else {
				m_RotDest.x = RotZ(JOY);
			}

#if _DEBUG
			PrintDebugProc("rot=%.2f,%.2f,%.2f\n", m_Rot.x, m_Rot.y, m_Rot.z);
			PrintDebugProc("rotDest=%.2f,%.2f,%.2f\n", m_RotDest.x, m_RotDest.y, m_RotDest.z);
#endif
			//���E�����Ă�������Ŋp�x�ɍ����o�遨����
			if (JOY.x < 0.0f) {
				//m_RotDest.y = 90.0f;
				m_RotDest.y = 75.0f;
				m_RotDest.x *= -1;
			}
			else {
				//m_RotDest.y = -90.0f;
				m_RotDest.y = -75.0f;
			}

			//XBOX��B�{�^���������Ɗp�x�ύX�i�C���\��j
			//if(GetJoyButton(0, XBOXA))
			if (GetJoyButton(0, XBOXA))
			{
				//���E�����Ă�������Ŋp�x�𒲐�
				if (JOY.x < 0.0f) {
					m_Rot.x = -(m_RotDest.x) + 90.0f;
				}
				else {
					m_Rot.x = m_RotDest.x - 90.0f;
				}
				m_Rot.y = m_RotDest.y;
				m_Rot.z = m_RotDest.z;
				//�W�F�b�g����
				m_bFlag = true;
				m_fLife -= DEL_JET;
				m_nAnimNum = jet;
			}
			else {
				//��Ƀ��C�t�͌������Ă���
				m_fLife -= DEL_STAY;
				m_bFlag = false;
			}

			// �ʒu�ړ�
			m_Pos.x += m_Move.x;
			m_Pos.y += m_Move.y;
			m_Pos.z += m_Move.z;

			static XMFLOAT3 Targetpos, RepatirationVector;//�͈͊O�ɏo��u�Ԃ̈ʒu�Ƌ������҂̃x�N�g��
			//****************************
			//�͈͊O�ɍs���ƐԂ��Ȃ�
			//****************************
			int x = g_nStageNum - 1;
			//�e���b�v���Ԃ��Ȃ鏈�����ɏo��
			if (m_Pos.y > warningArea[x][UP] - AleattelopPos || m_Pos.y < warningArea[x][DOWN] + AleattelopPos ||
				warningArea[x][LEFT] + AleattelopPos > m_Pos.x || warningArea[x][RIGHT] - AleattelopPos < m_Pos.x) {
				if (!g_bOutFlag) {
					Targetpos = m_Pos;
				}
				g_bOutFlag = true;
				if (m_Pos.y > warningArea[x][UP] || m_Pos.y < warningArea[x][DOWN] || warningArea[x][LEFT] > m_Pos.x || warningArea[x][RIGHT] < m_Pos.x) {
					//�G���A�O�ɍs������_�f�ւ��
					m_fLife -= 0.15f;
					//����ɍs���Ƌ�������
					if (m_Pos.y > warningArea[x][UP] + AleatRepatriationPos || m_Pos.y < warningArea[x][DOWN] - AleatRepatriationPos ||
						warningArea[x][LEFT] - AleatRepatriationPos > m_Pos.x || warningArea[x][RIGHT] + AleatRepatriationPos < m_Pos.x) {
						m_Move = XMFLOAT3(0.0f, 0.0f, 0.0f);
						g_bRepatriationFlag = true;
						RepatirationVector = XMFLOAT3(Targetpos.x - m_Pos.x, Targetpos.y - m_Pos.y, Targetpos.z - m_Pos.z);
						XMStoreFloat3(&RepatirationVector, XMVector3Normalize(XMLoadFloat3(&RepatirationVector)));//�x�N�g���̐��K��
					}
				}
			}
			else {
				g_bOutFlag = false;
			}
			AlertWarning(g_bOutFlag);

			//��������
			if (g_bRepatriationFlag) {
				Repatriation(RepatirationVector);
			}

			//****************************
			//�͂�
			//****************************
			if (GetJoyButton(0, XBOXY)) {
				//�͂ރt���O��������
				m_bHold = true;
				m_nAnimNum = hold;
			}
			//�A�j���[�V�������r����������
			else if (m_bHold) {
				//�͂ރt���O�����낷
				m_bHold = false;
				//m_nAnimNum = release;
			}

			Animator(m_nAnimNum);

			//���C���[�̈ʒu���Z�b�g
			Player_Rayer->Set(m_Move.x, m_Move.y, false);

			//���C�t���؂ꂽ�烊�U���g
			if (m_fLife <= 0.0f) {
				//StartFadeOut(SCENE_RESULT);
				EffectManager::Get()->End("all");
				m_RotDest = XMFLOAT3(0.0f, 0.0f, 0.0f);
				g_bClearGameOverFlag[1] = true;
			}

			// �ړ��ʂɊ�����������
			//m_Move.x += (0.0f - m_Move.x) * RATE_MOVE_PLAYER;
			//m_Move.y += (0.0f - m_Move.y) * RATE_MOVE_PLAYER;
			//m_Move.z += (0.0f - m_Move.z) * RATE_MOVE_PLAYER;
			// �v���C���[�̌��݂̃t���[���̈ړ���/�p�x��1�t���[���O�̈ړ���/�p�x���e�������� 12 / 26 X kokokara
			m_Move.x = ((m_Move.x * INFLUENCE) + (m_Accel.x * (1.0f - INFLUENCE)));
			m_Move.y = ((m_Move.y * INFLUENCE) + (m_Accel.y * (1.0f - INFLUENCE)));
			m_Move.z = ((m_Move.z * INFLUENCE) + (m_Accel.z * (1.0f - INFLUENCE)));

			m_Rot.x = ((m_OldRot.x * INFLUENCE) + (m_Rot.x * (1.0f - INFLUENCE)));
			//		m_Rot.y  = ((m_OldRot.y * INFLUENCE) + (m_Rot.y * (1.0f - INFLUENCE)));
			m_Rot.z = ((m_OldRot.z * INFLUENCE) + (m_Rot.z * (1.0f - INFLUENCE))); //12 / 26 X kokomade

			//�����蔻��i���j
			//����
			//01/18		������n

			//�f�u���ɐG�ꂽ��
			if (nTouchDebly >= 0) {
				XMFLOAT3 Col_Stone_Pos = Player_Debly->GetPos(nTouchDebly);//���������Ώە��̈ʒu
				XMFLOAT3 Col_Stone_Vec = Col_Stone_Pos;
				XMFLOAT3 Col_Stone_Size = Player_Debly->GetSize(nTouchDebly);//���������Ώە��̑傫��
				Col_Stone_Vec.x = m_Pos.x - Col_Stone_Vec.x;//�v���C���[����f�u���̃x�N�g���擾.x
				Col_Stone_Vec.y = m_Pos.y - Col_Stone_Vec.y;//�v���C���[����f�u���̃x�N�g���擾.y
				Col_Stone_Pos.x += Col_Stone_Vec.x;
				Col_Stone_Pos.y += Col_Stone_Vec.y;
				Col_Stone_Pos.z += Col_Stone_Vec.z;
				XMStoreFloat3(&Col_Stone_Vec, XMVector3Normalize(XMLoadFloat3(&Col_Stone_Vec)));//�x�N�g���̐��K��
				XMStoreFloat3(&Col_Stone_Size, XMVector3Normalize(XMLoadFloat3(&Col_Stone_Size)));//�x�N�g���̐��K��

				//01/18	������n
				if (Col_Stone_Size.x < Col_Stone_Vec.x) {
					m_Move.x = Col_Stone_Vec.x * REFLECTION;
					m_Pos.x += Col_Stone_Vec.x * REFLECTION;
				}
				if (Col_Stone_Size.x > Col_Stone_Vec.x) {
					m_Move.x = Col_Stone_Vec.x * REFLECTION;
					m_Pos.x += Col_Stone_Vec.x * REFLECTION;
				}
				if (Col_Stone_Size.y < Col_Stone_Vec.y) {
					m_Move.y = Col_Stone_Vec.y * REFLECTION;
					m_Pos.y += Col_Stone_Vec.y * REFLECTION;
				}
				if (Col_Stone_Size.y > Col_Stone_Vec.y) {
					m_Move.y = Col_Stone_Vec.y * REFLECTION;
					m_Pos.y += Col_Stone_Vec.y * REFLECTION;
				}

				//�G�t�F�N�g����
				if (nTouchDebly != nTouchDeblyPrev) {
					EffectManager::Get()->Start("collision", Col_Stone_Pos);
				}
				m_bFlag = false;
				CSound::Stop(SE_COLLISION);
				CSound::Play(SE_COLLISION);
			}
			//覐΂ɐG�ꂽ��
			//����
			PrintDebugProc("ntouchMeteo={%d,%d}\n", nTouchMeteo.x, nTouchMeteo.y);
			//01/18	������n
			if ((nTouchMeteo.x >= 0) && (nTouchMeteo.y >= 0)) {
				XMFLOAT3 Col_Stone_Pos = Player_Meteo->GetPos(nTouchMeteo);//���������Ώە��̈ʒu
				XMFLOAT3 Col_Stone_Size = Player_Meteo->GetSize(nTouchMeteo);//���������Ώە��̑傫��
				XMFLOAT3 Col_Stone_Vec = Col_Stone_Pos;
				Col_Stone_Vec.x = m_Pos.x - Col_Stone_Vec.x;//�v���C���[����f�u���̃x�N�g���擾.x
				Col_Stone_Vec.y = m_Pos.y - Col_Stone_Vec.y;//�v���C���[����f�u���̃x�N�g���擾.y
				Col_Stone_Pos.x += Col_Stone_Vec.x;
				Col_Stone_Pos.y += Col_Stone_Vec.y;
				Col_Stone_Pos.z += Col_Stone_Vec.z;
				XMStoreFloat3(&Col_Stone_Vec, XMVector3Normalize(XMLoadFloat3(&Col_Stone_Vec)));//�x�N�g���̐��K��
				//XMStoreFloat3(&Col_Stone_Size, XMVector3Normalize(XMLoadFloat3(&Col_Stone_Size)));//�x�N�g���̐��K��
				if (m_bHold) {
					m_Move = Player_Meteo->GetVector(nTouchMeteo);
				}
				else {
					//01/18	������n
					if (Col_Stone_Size.x < Col_Stone_Vec.x) {
						m_Move.x = Col_Stone_Vec.x * REFLECTION;
						m_Pos.x += Col_Stone_Vec.x * REFLECTION*5;
					}
					if (Col_Stone_Size.x > Col_Stone_Vec.x) {
						m_Move.x = Col_Stone_Vec.x * REFLECTION;
						m_Pos.x += Col_Stone_Vec.x * REFLECTION*5;
					}
					if (Col_Stone_Size.y < Col_Stone_Vec.y) {
						m_Move.y = Col_Stone_Vec.y * REFLECTION;
						m_Pos.y += Col_Stone_Vec.y * REFLECTION*5;
					}
					if (Col_Stone_Size.y > Col_Stone_Vec.y) {
						m_Move.y = Col_Stone_Vec.y * REFLECTION;
						m_Pos.y += Col_Stone_Vec.y * REFLECTION*5;
					}

					//�G�t�F�N�g����
					if ((nTouchMeteo.x != nTouchMeteoPrev.x) && (nTouchMeteo.y != nTouchMeteoPrev.y)) {
						EffectManager::Get()->Start("collision", Col_Stone_Pos);
					}
					CSound::Stop(SE_COLLISION);
					CSound::Play(SE_COLLISION);
				}
				m_bFlag = false;
			}
			//�u���b�N�z�[���ɐG�ꂽ��
			if (fTouchBlackHole.w >= 1) {
#if _DEBUG
				PrintDebugProc("Hit BlackHole\n");
#endif
				XMFLOAT3 BlackHoleVector;
				BlackHoleVector.x = m_Pos.x - fTouchBlackHole.x;
				BlackHoleVector.y = m_Pos.y - fTouchBlackHole.y;
				BlackHoleVector.z = 0.0f;
				//���K��
				XMStoreFloat3(&BlackHoleVector, XMVector3Normalize(XMLoadFloat3(&BlackHoleVector)));//�x�N�g���̐��K��
				//������������
				m_Move.x -= BlackHoleVector.x * 0.02f;
				m_Move.y -= BlackHoleVector.y * 0.02f;
				//CSound::SetVolume(SE_BLACKHOLE, 100.0f);
				//CSound::Stop(SE_BLACKHOLE);
				//CSound::Play(SE_BLACKHOLE);

				//�u���b�N�z�[���̉�
				g_nBlackHoleTime++;
				if (g_nBlackHoleTime >= 60) {
					g_nBlackHoleTime = 0;
					CSound::Play(SE_BLACKHOLE);
					PrintDebugProc("60");
				}
				PrintDebugProc("\n");
			}
			if (MAX_MOVE < abs(m_Move.x) + abs(m_Move.y)) //X������Y�����̐�Βl�̍��v������l�ȏ�
			{
				if (abs(m_Accel.x) + abs(m_Accel.y) < abs(m_Move.x) + abs(m_Move.y))	//1�t���[���O��move�̍��v��Βl�𒴂��Ă�����
				{
					m_Move.x *= 0.93f;
					m_Move.y *= 0.93f;
					//m_Move = m_Accel; //���O�̈ړ��ʂɃ��Z�b�g
				}
			}
			m_Accel = m_Move; // 12/24 X ���݂̃t���[���̈ړ����x/�p�x�̏���ۑ�����
			m_OldRot = m_Rot; // 12/26 X

			float angle;//�W�F�b�g�G�t�F�N�g�Ŏg�p����p�x
			angle = m_RotDest.x;
#if _DEBUG
			PrintDebugProc("����:%.2f\n", angle);
#endif

			if (JOY.x < 0.0f) {
				angle += 180.0f;
				angle *= -1;
			}
			g_Size = g_model[0].GetBBox();
#if _DEBUG
			PrintDebugProc("x,y,z={%.2f,%.2f,%.2f}\n", g_Size.x, g_Size.y, g_Size.z);
			PrintDebugProc("move.x,move.y,move.z={%.2f,%.2f,%.2f}\n", m_Move.x, m_Move.y, m_Move.z);
			PrintDebugProc("m_Rot.x,m_Rot.y,m_Rot.z={%.2f,%.2f,%.2f}\n", m_Rot.x, m_Rot.y, m_Rot.z);
			PrintDebugProc("m_RotDest.x,m_RotDest.y,m_RotDest.z={%.2f,%.2f,%.2f}\n", m_RotDest.x, m_RotDest.y, m_RotDest.z);
			PrintDebugProc("RotZ={%.2f}\n", RotZ(JOY));
#endif

			float rot = m_Rot.x + 90.0f;
			XMFLOAT3 t = XMFLOAT3(-(CosDeg(angle)*50.0f), -(SinDeg(angle)*50.0f), 0.0f);

			bool flag = false;
			//�G�t�F�N�g�i���j
			if (GetJoyTrigger(0, XBOXA)) {
				CSound::Play(SE_JET);
			}
			if (GetJoyButton(0, XBOXA)) {
				EffectManager::Get()->Start("jet", XMFLOAT3(t.x, t.y, 0.0f), angle);
				flag = true;
			}

			//�S�[���ɓ������Ă�H
			if (Player_Goal->Collision(m_Pos, g_Size)) {
				if (!g_bClearGameOverFlag[0]) {

					CSound::Play(SE_GOSHIP);
				}
				g_bClearGameOverFlag[0] = true;
			}
			//���C�t�ɂ���ăQ�[�W���ϓ�
			Player_Life->Set(m_fLife);

			//���C�t�ɂ���ĐU�����ϓ�
			//01/10  ������n
			//Player_Life->Vib(m_fLife);

			//���C�t�ɂ���Čۓ����ϓ�
			//01/10  ������n
			Player_Life->Beat(m_fLife);

			//�G�t�F�N�g�X�V
			EffectManager::Get()->Update("jet", XMFLOAT3(t.x, t.y, 0.0f), angle, flag);
			EffectManager::Get()->Update("collision", false);

			XMFLOAT3 sabun = XMFLOAT3(m_Pos.x / 20.0f, m_Pos.y / 20.0f, (900 - DEFF_CAMERA_Z) / 20.0f);
			static int count;

			//���ꂼ�ꉟ����������t���O�𔭓�
			if (GetJoyButton(0, PS4L) ) {
				m_bPull[0] = true;
			}
			else if (GetJoyTrigger(0, XBOXA)) {
				m_bPull[1] = true;
				count = 0;
			}
			//���ꂼ��t���O�͓����ɔ������Ȃ�
			if (m_bPull[0]) {
				m_bPull[1] = false;
			}
			else if (m_bPull[1]) {
				m_bPull[0] = false;
			}

			//space�������ꂽ��J����������
			if (m_bPull[0]) {
				EffectManager::Get()->End("jet");
				if (g_CameraPos.z > -900.0f) {
					g_CameraPos.z -= sabun.z;
				}
				if (m_Pos.x >= 0.0f) {
					if (g_CameraPos.x > 0.0f) {
						g_CameraPos.x -= sabun.x;
					}
					else {
						//�s���߂������̕␳
						g_CameraPos.x = 0.0f;
					}
				}
				else {
					if (g_CameraPos.x < 0.0f) {
						g_CameraPos.x -= sabun.x;
					}
					else {
						//�s���߂������̕␳
						g_CameraPos.x = 0.0f;
					}
				}
				if (m_Pos.y >= 0.0f) {
					if (g_CameraPos.y > 0.0f) {
						g_CameraPos.y -= sabun.y;
					}
					else {
						//�s���߂������̕␳
						g_CameraPos.y = 0.0f;
					}
				}
				else {
					if (g_CameraPos.y < 0.0f) {
						g_CameraPos.y -= sabun.y;
					}
					else {
						//�s���߂������̕␳
						g_CameraPos.y = 0.0f;
					}
				}
			}
			//����������ĂȂ��Ƃ��͈�����Ă��邩�m�F
			else if (!m_bPull[0]) {
				//�J�����������Ă�����
				if (g_CameraPos.z < -DEFF_CAMERA_Z) {
					g_CameraPos.z += sabun.z;
					if (m_Pos.x >= 0.0f) {
						if (g_CameraPos.x < m_Pos.x) {
							g_CameraPos.x += sabun.x;
						}
						else {
							g_CameraPos.x = m_Pos.x;//�s���߂������̕␳
						}
					}
					else {
						if (g_CameraPos.x > m_Pos.x) {
							g_CameraPos.x += sabun.x;
						}
						else {
							g_CameraPos.x = m_Pos.x;//�s���߂������̕␳
						}
					}
					if (m_Pos.y >= 0.0f) {
						if (g_CameraPos.y < m_Pos.y) {
							g_CameraPos.y += sabun.y;
						}
						else {
							g_CameraPos.y = m_Pos.y;//�s���߂������̕␳
						}
					}
					else {
						if (g_CameraPos.y > m_Pos.y) {
							g_CameraPos.y += sabun.y;
						}
						else {
							g_CameraPos.y = m_Pos.y;//�s���߂������̕␳
						}
					}
				}
				//�J�����������Ă��Ȃ������炻�̂܂܈ʒu���J�����ʒu�ɒu��������
				else {
					g_CameraPos = XMFLOAT3(m_Pos.x, m_Pos.y, -DEFF_CAMERA_Z);
				}
			}

			CCamera::Get()->SetPos(g_CameraPos);
			CCamera::Get()->SetTarget(g_CameraPos.x, g_CameraPos.y, 0.0f);
		}
		//1�t���[���O�Ɣ�r���邽�߂ɒl���i�[
		nTouchDeblyPrev = nTouchDebly;
		nTouchMeteoPrev = nTouchMeteo;
	}

	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScl;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	//�X�P�[���𔽉f
	mtxScl = XMMatrixScaling(m_Scl.x, m_Scl.y, m_Scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_RotDest.x), XMConvertToRadians(m_RotDest.y), XMConvertToRadians(m_RotDest.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�ݒ�
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);
}

//============================================
// �`�揈��
//============================================
void Player::Draw(void)
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	XMFLOAT3 goalPos = Player_Goal->GetPos();
	if (g_bClearGameOverFlag[1]) {
		// �s����������`��
		g_model[1].Draw(pDC, g_mtxWorld, eOpacityOnly);
	}
	else {
		g_model[0].Draw(pDC, g_mtxWorld, eOpacityOnly);
	}

	// ������������`��
	SetBlendState(BS_ALPHABLEND);	// �A���t�@�u�����h�L��
	SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�
	if (g_bClearGameOverFlag[1]) {
		g_model[1].Draw(pDC, g_mtxWorld, eTransparentOnly);
	}
	else {
		g_model[0].Draw(pDC, g_mtxWorld, eTransparentOnly);
	}
	SetZWrite(true);				// Z�o�b�t�@�X�V����
	SetBlendState(BS_NONE);			// �A���t�@�u�����h����

	//�G�t�F�N�g�`��
	EffectManager::Get()->Draw("jet");
	EffectManager::Get()->Draw("collision");
}

//============================================
// �ʒu�擾
//============================================
XMFLOAT3& Player::GetPos()
{
	return m_Pos;
}

//============================================
//�p�x���擾
//============================================
float Player::RotZ(XMFLOAT2 JOY) {
	JOY.y *= -1;
	float z = atan(JOY.y / JOY.x);	//�A�[�N�^���W�F���g���擾�����W�A���l
	float x = z * 180.0f / PI;			//�擾�������W�A���l���p�x�ɒ����ĕԂ�
	return x;
}

//============================================
//�_�f�ʂ��擾
//============================================
float Player::GetO2(void) {
	return m_fLife;
}

//============================================
//�e���b�v�A�j���[�V�����Đ������ǂ����̃t���O�Z�b�g	// 01/22 �O�֑�O
//============================================
void  Player::SetAnimTelopFlag(bool bAnimTelopFlag)
{
	g_bAnimTelopFlag = bAnimTelopFlag;
}

//============================================
//�Q�[���N���A���̃A�j���[�V����
//============================================
void Player::GameClearAnimation() {
	//�G�t�F�N�g�̋����I��
	EffectManager::Get()->End("all");

	//�ϐ��錾
	XMFLOAT3 goalPos = Player_Goal->GetPos();
	XMFLOAT3 playerVector;//�v���C���[����S�[���̃x�N�g��

	//�S�[���ƃv���C���[���߂Â���
	if ((abs(m_Pos.x - goalPos.x) >= 10.0f || abs(m_Pos.y - goalPos.y) >= 10.0f || abs(m_Pos.z - goalPos.z) >= 10.0f) && !g_goalCollisionFlag) {
		CCamera::Get()->SetPos(m_Pos.x, m_Pos.y, m_Pos.z - DEFF_CAMERA_Z);
		CCamera::Get()->SetTarget(m_Pos);
		//�x�N�g������
		playerVector.x = goalPos.x - m_Pos.x;
		playerVector.y = goalPos.y - m_Pos.y;
		playerVector.z = goalPos.z - m_Pos.z;
		XMStoreFloat3(&playerVector, XMVector3Normalize(XMLoadFloat3(&playerVector)));//�x�N�g���̐��K��

		//�ړ�
		m_Pos.x += playerVector.x*2.0f;
		m_Pos.y += playerVector.y*2.0f;
		m_Pos.z += playerVector.z*2.0f;
		if (m_Scl.x > 0.0f) {
			m_Scl.x -= 0.01f;
			m_Scl.y -= 0.01f;
			m_Scl.z -= 0.01f;
		}
	}
	//�v���C���[���S�[���Ɋ��S�ɐG�ꂽ��
	else {
		m_Scl = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_goalCollisionFlag = true;
		CCamera::Get()->SetTarget(goalPos);//�J�������S�[���Ɍ�����
		Player_Goal->SetPos(5.0f);		//�S�[���𓮂���
		Player_Rayer->Set(20.0f, 0.0f, true);
		//���̍��W��ʉ߂�����V�[���J��
		if (goalPos.x > 1300.0f) {
			StartFadeOut(SCENE_RESULT);
		}
	}
}

//============================================
//�Q�[���I�[�o�[���̃A�j���[�V����
//============================================
void Player::GameOverAnimation() {
	//SE�̒�~
	CSound::Stop(SE_WARNING);
	CSound::Stop(SE_COLLISION);
	CSound::Stop(SE_JET);
	gameOver_stopCount--;
	//��U��~������
	//��]���ĉ����ɍs��
	if (gameOver_stopCount < 0) {
		m_RotDest.z -= 6.0f;
		m_Pos.z += 2.0f;
	}
	//���̍��W��ʉ߂�����V�[���J��
	if (m_Pos.z > 300.0f) {
		StartFadeOut(SCENE_RESULT);
	}

}

//�A�j���[�V�������ꊇ�Ǘ�
void Player::Animator(int animNum) {
	static int animPrev;

	//�Đ����Ԃ̃��Z�b�g
	if (animNum != animPrev) {
		g_nCountFrame = 0;
	}
	static bool upFlag = true;
	switch (animNum) {
	case hold:
		if (GetJoyButton(0, XBOXY)) {
			if (g_nCountFrame >= AnimationTime - 3.0f) {
				upFlag = false;
			}
			else if (g_nCountFrame < 60) {
				upFlag = true;
			}
			if (upFlag) {
				g_nCountFrame++;
			}
			else {
				g_nCountFrame--;
			}
		}
		else {
			if (g_nCountFrame > 60) {
				g_nCountFrame = 60;
			}
			g_nCountFrame--;
			if (g_nCountFrame <= 0) {
				m_nAnimNum = stay;
			}
		}
		break;
	case release:
		g_nCountFrame++;
		if (g_nCountFrame >= AnimationTime - 3.0f) {
			m_nAnimNum = stay;
		}
		break;
	case jet:
		g_nCountFrame++;
		if (g_nCountFrame >= AnimationTime - 3.0f) {
			m_nAnimNum = stay;
		}
		break;
	default:
		g_nCountFrame++;
		break;
	}

	//�ʏ�̃��f���A�j���[�V����
	g_dSetAnim = g_nCountFrame * g_d1FrameAnim;//�A�j���[�V�����Đ��ӏ����X�V
	//�A�j���[�V�����Z�b�g
	g_model[0].SetAnimTime(g_dSetAnim);
	g_model[0].SetAnimIndex(animNum);

	animPrev = animNum;
}

//�͈͊O�ɏo�����̋�������
void Player::Repatriation(XMFLOAT3 Vector) {
	//�ʒu���x�N�g���ɑ����ĕύX
	m_Move.x = Vector.x*0.9f;
	m_Move.y = Vector.y*0.9f;
	//�G���A���ɓ�������t���O�����낷
	int x = g_nStageNum - 1;
	if (m_Pos.y < warningArea[x][UP] - AleattelopPos && m_Pos.y > warningArea[x][DOWN] + AleattelopPos &&
		warningArea[x][LEFT] + AleattelopPos < m_Pos.x && warningArea[x][RIGHT] - AleattelopPos > m_Pos.x) {
		g_bRepatriationFlag = false;
		m_Move = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}