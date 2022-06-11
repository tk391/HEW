#include"StartTelop.h"
#include"Texture.h"
#include"polygon.h"
#include "debugproc.h"
#include"Light.h"
#include"Scene.h"
#include"Camera.h"
#include"Life.h"
#include"Player.h"
#include"Game.h"
#include "Sound.h"
//�f�o�b�O�m�F�p
#include "input.h"

//=====================================================
//�}�N����`
//=====================================================
#define MAX_MTEXTURE (1)
#define PATH_GAMESTART_TEXTURE  L"data/texture/start.png"
#define TEXTURE_SIZEX  (329.0f * 1.25f) //�摜�T�C�Y
#define TEXTURE_SIZEY	(98.0f * 1.25f)

#define CENTER_POS_X	(0.0f)	//���S���WX
#define CENTER_POS_Y	(0.0f)	//���S���WY

#define ANIM_FIRST (100)					//�A�j���[�V����1 �J�����ڋ�
#define ANIM_SECOND	(ANIM_FIRST - 30)		//�A�j���[�V����2 �e���b�v���邭�� (�J�����ڋ߂Ɠ���)
#define ANIM_THIRD  (ANIM_FIRST + 30)		//�A�j���[�V����3 �e���b�v�ꎞ��~
#define ANIM_FOURTH  (ANIM_THIRD + 30)		//�A�j���[�V����4 �e���b�v�t�F�[�h�A�E�g

//=====================================================
//�O���[�o���ϐ�
//=====================================================
static ID3D11ShaderResourceView* g_pTexture;

static XMFLOAT2 g_vPos;				//�e���b�v�̏ꏊ
static XMFLOAT2 g_vSize;			//�e���b�v�̑傫��
static float	g_fAngle;			//�e���b�v�̉�]�x

static float g_fAnimProgress;		//�A�j���[�V�����̐i�s�x(����)

static XMFLOAT3			g_CameraPos;			//�J�����ʒu
static XMFLOAT3			g_DeffCameraPos;		//�J�����ʒu
static XMFLOAT3			g_PlayerCameraPos;		//�v���C���[�ʒu
static XMFLOAT3			g_sabun;				//�v���C���[�ʒu�ƃf�t�H���g�J�����ʒu�̍���

static bool					g_bAnimFlag;			//���݃A�j���[�V�����Đ�����

Player* Start_Player;
Life* Start_Life;			//Life�N���X



//=============================================================================
//������
//=============================================================================
HRESULT StartTelop::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	//�e�N�X�`���ǂݍ���
	hr = CreateTextureFromFile(pDevice, PATH_GAMESTART_TEXTURE, &g_pTexture);
	if (FAILED(hr))
	{
		return hr;
	}

	g_fAnimProgress = 0;	//�A�j���[�V�����̃t���[����

	Start_Player = GetPlayer();

	g_vPos = XMFLOAT2(CENTER_POS_X, CENTER_POS_Y);			//�e���b�v�̏ꏊ
	g_vSize = XMFLOAT2(TEXTURE_SIZEX, TEXTURE_SIZEY);		//�e���b�v�̑傫��
	m_Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);				//�e���b�v�̐F

	g_CameraPos = XMFLOAT3(0.0f, 0.0f, -1000.0f);			//�A�j���[�V�����J�n���̃J���������ʒu
	g_DeffCameraPos = XMFLOAT3(0.0f, 0.0f, -1000.0f);		//�A�j���[�V�����J�n���̃J���������ʒu
	g_PlayerCameraPos = Start_Player->GetPos();				//�v���C���[�̃J�����ʒu
	g_PlayerCameraPos.z -= 350.0f;							//�v���C���[�ʒu�ɃJ�����̋�����ǉ�
	// �J�������C���|�W�V�����ƃJ�����v���C���[�|�W�V�����̍���
	g_sabun = XMFLOAT3(g_DeffCameraPos.x - g_PlayerCameraPos.x,
					   g_DeffCameraPos.y - g_PlayerCameraPos.y,
				       g_DeffCameraPos.z - g_PlayerCameraPos.z);
	//Start_Life->GetLife();
	/*diff = XMFLOAT3(g_DeffCameraPos.x - g_PlayerPos.x, g_DeffCameraPos.y - g_PlayerPos.y, g_DeffCameraPos.z - (g_PlayerPos.z - 200.0f));
*/

	//�J�����̃Z�b�g
	CCamera::Get()->SetPos(0.0f, 0.0f, 1000.0f);
//CCamera::Get()->SetTarget(m_Pos);


	g_fAngle = 100;
	return hr;
}
//=============================================================================
//�I��
//=============================================================================
void StartTelop::Uninit()
{
	for (int i = 0; i < MAX_MTEXTURE; ++i)
	{
		SAFE_RELEASE(g_pTexture);
	}
}
//=============================================================================
//�X�V
//=============================================================================
void StartTelop::Update()
{
	//3�߂̃A�j���[�V�������I������u��SE���Ȃ�
	if(g_fAnimProgress == ANIM_THIRD)
	{
		CSound::Stop(SE_START);
		CSound::Play(SE_START);
	}
	if (g_fAnimProgress <= ANIM_FIRST)
	{
		g_CameraPos.x = g_DeffCameraPos.x - (g_sabun.x * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST));
		g_CameraPos.y = g_DeffCameraPos.y - (g_sabun.y * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST));
		g_CameraPos.z = g_DeffCameraPos.z - (g_sabun.z * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST) * (g_fAnimProgress / ANIM_FIRST));
		CCamera::Get()->SetTarget(g_CameraPos.x, g_CameraPos.y, 0.0f);
		if (ANIM_SECOND <= g_fAnimProgress && g_fAnimProgress < ANIM_FIRST)	//�e���b�v�����邮�邵�Ȃ���t�F�[�h�C������
		{
			g_vSize = XMFLOAT2(TEXTURE_SIZEX * (ANIM_FIRST) / g_fAnimProgress, TEXTURE_SIZEY * (ANIM_FIRST) / g_fAnimProgress);
			g_fAngle = (ANIM_FIRST) * 10 - g_fAnimProgress * 10 - 10;
			m_Color.w = (g_fAnimProgress - ANIM_SECOND) / (ANIM_FIRST - ANIM_SECOND);

		}
	}
	if (ANIM_FIRST <= g_fAnimProgress && g_fAnimProgress < ANIM_THIRD)
	{
		m_Color.w = 1;
		g_vSize = XMFLOAT2(TEXTURE_SIZEX, TEXTURE_SIZEY);
	}
	if (ANIM_THIRD <= g_fAnimProgress && g_fAnimProgress <= ANIM_FOURTH)
	{//�|���S���̊g�又��
		g_vSize.x += TEXTURE_SIZEX * 0.03f;
		g_vSize.y += TEXTURE_SIZEY * 0.03f;

		m_Color.w = 1 - (g_fAnimProgress - ANIM_THIRD) / (ANIM_FOURTH - ANIM_THIRD);		//�|���S���̕s�����x��1��0
	}

	//�Q�[�����J�n�����痬���
	//���o������Ă���Ԃ̓��C�t������Ȃ��悤�ɒ����A�e���b�v���I��������Ƀ��C�t������悤��
	if (g_fAnimProgress < ANIM_FOURTH)	//�A�j���[�V�����̍Ō�̃t���[�����z���Ă��Ȃ����
	{
		g_fAnimProgress++;						//�A�j���[�V�����𑝂₷
		CCamera::Get()->SetPos(g_CameraPos);	//�J�������X�V
		Start_Player->SetAnimTelopFlag(true);	//�A�j���[�V�����t���OON
	}
	else //�����łȂ��ꍇ�A�e���b�v�Đ��t���O��false��
	{
		Start_Player->SetAnimTelopFlag(false);
	}
		
}
//=============================================================================
//�`��
//=============================================================================
void StartTelop::Draw()
{
		ID3D11DeviceContext* pDC = GetDeviceContext();
		//�X�^�[�g�e���b�v
		SetPolygonSize(g_vSize.x, g_vSize.y);
		SetPolygonPos(g_vPos.x, g_vPos.y);
		SetPolygonAlpha(m_Color.w);
		SetPolygonAngle(g_fAngle);
		SetPolygonTexture(g_pTexture);
		SetPolygonUV(0.0f, 0.0f);
		DrawPolygon(pDC);

		//���ɖ߂�
		SetPolygonFrameSize(1.0f, 1.0f);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonAngle(0.0f);
}