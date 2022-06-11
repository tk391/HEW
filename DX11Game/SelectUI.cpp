//===========
//01/16 �O�֑�O �����o�ϐ��̔z��� �񋓑� ESelectUI �̒��g�ɍ����ւ�

#include "SelectUI.h"
#include"Texture.h"
#include"polygon.h"
#include "debugproc.h"
#include"Light.h"
#include "Scene.h"

//�f�o�b�O�m�F�p
#include "input.h"

//=============================================================================
// �}�N����`
//=============================================================================
#define STAR_FIRST		-550.0f				//��UI ��ԁ��̈ʒu(y)				01/16 �O�֑�O
#define STAR_POS_Y		290.0f				//��UI Y���W						01/16 �O�֑�O
#define STAR_INTERVAL	105.0f				//��UI �摜���Ƃ̊Ԋu(x)			01/16 �O�֑�O
#define STAR_SIZE		50.0f				//��UI �\���T�C�Y(x,y)				01/16 �O�֑�O
#define STAR_SIZE_SAT_X	STAR_SIZE * 1.52f	//��UI �\���T�C�Y(x,y)				01/16 �O�֑�O

#define STAR_FLAMESIZEX	701.0f				//���g �T�C�Y(x)					01/16 �O�֑�O
#define STAR_FLAMESIZEY	101.0f				//���g �T�C�Y(y)					01/16 �O�֑�O

#define ASTRONAUT_X		950					//�F����s�m�̌��摜�T�C�YX			01/18 �O�֑�O
#define ASTRONAUT_Y		650					//�F����s�m�̌��摜�T�C�YY			01/18 �O�֑�O
#define ASTRONAUT_MOVE	0.4f				//�F����s�m�̂ӂ�ӂ����l		01/18 �O�֑�O
#define ASTRONAUT_SPEED	0.01f				//�F����s�m�̂ӂ�ӂ푬�x			01/18 �O�֑�O

#define SHUTTER_SIZE_X	617.0f				//�V���b�^�[�̌��摜�T�C�YX		01/18 �O�֑�O
#define SHUTTER_SIZE_Y	435.0f				//�V���b�^�[�̌��摜�T�C�YY		01/18 �O�֑�O

#define SHINING_ANIM_TIME	60.0f				//�X�e�[�W���̔�������			01/25 �O�֑�O
#define SHINING_ANIM_SPEED	1.0f			//�X�e�[�W���̔������x			01/25 �O�֑�O

#define NOPLAY_ANIM_TIME	2				//"�܂��V�ׂȂ���"���h��鎞��	01/23 �O�֑�O
#define NOPLAY_ANIM_SPEED	10.0f			//"�܂��V�ׂȂ���"���h��鑬�x	01/23 �O�֑�O

#define STAGE_ANIM			10.0f			//�X�e�[�W�����h��鑬�x		01/23 �O�֑�O
#define STAGENAME_POS		XMFLOAT2(0.0f, -250.0f)	//���T�C�Y�̉摜�����������̂Ńf�t�@�C����	01/25 �O�֑�O

//�摜���
static std::string fileName_SelectUI[]{
	"data/texture/defoMoonShrink.png",
	"data/texture/defoMarseShrink.png",
	"data/texture/defoJupiterShrink.png",
	"data/texture/defoSturnShrink.png",
	"data/texture/defoAsteroisShrink.png",

	"data/texture/astronaut.png",
	"data/texture/arrowSprites.png",
	"data/texture/arrow2Sprites.png",
	"data/texture/noplay.png",
	"data/texture/StarFlame2.png",
	"data/texture/shutter.png",

	"data/texture/stage1.png",
	"data/texture/stage2.png",
	"data/texture/stage3.png",
	"data/texture/stage4.png",
	"data/texture/stage5.png",

	"data/texture/stage1Shining.png",
	"data/texture/stage2Shining.png",
	"data/texture/stage3Shining.png",
	"data/texture/stage4Shining.png",
	"data/texture/stage5Shining.png"
};

//�O���[�o���ϐ�
static float g_fStageNameY;
XMFLOAT2 g_DefArrowRight;		//�����̃f�t�H�ʒu						01/16 �O�֑�O
XMFLOAT2 g_DefArrowLeft;		//�����̃f�t�H�ʒu						01/16 �O�֑�O
static float g_fMoveY;			//�F����s�m�̂ӂ�ӂ�͂��Ǘ�����ϐ�		01/18 �O�֑�O
static bool g_bMoveUP;			//���݂̉F����s�m�̂ӂ�ӂ���� ��or��			01/18 �O�֑�O

static bool g_bShiningAnimFlag;			// �X�e�[�W�̕����̔����t���O			01/25 �O�֑�O
static bool g_bOldShiningAnimFlag;		// ���O�̃X�e�[�W�̕����̔����t���O		01/25 �O�֑�O
static int  g_nShiningAnimTime;			// �X�e�[�W�̕����̔�������				01/25 �O�֑�O

static bool g_bNoPlayAnimFlag;			// "�܂��V�ׂȂ���"�̕������h���t���O	01/23 �O�֑�O
static int  g_nNoPlayAnimTime;			// "�܂��V�ׂȂ���"�̕������h��鎞��	01/23 �O�֑�O

static bool g_bStageAnimFlag;			// �X�e�[�W�̕������h���t���O			01/23 �O�֑�O
static int  g_nStageAnimTime;			// �X�e�[�W�̕������h��鎞��			01/23 �O�֑�O
static float g_fDeffStageNameY;			// �X�e�[�W�̕����̏���Y���W			01/23 �O�֑�O

//=============================================================================
//������
//=============================================================================
HRESULT SelectUI::Init() {

	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`���ǂݍ���
	for (int i = 0; i < MAX_SELECTUI_TEXUTURE; ++i) {
		hr = CreateTextureFromFile(pDevice, fileName_SelectUI[i].c_str(), &m_pTexture[i]);
		if (FAILED(hr))
			return hr;
	}

	//�����o�ϐ��̏�����
	m_vPos [MOON_1]		= XMFLOAT2(STAR_FIRST, STAR_POS_Y);
	m_vSize[MOON_1]		= XMFLOAT2(STAR_SIZE, STAR_SIZE);
	m_vPos[MARSE_2]		= XMFLOAT2(m_vPos[MOON_1].x    + STAR_INTERVAL, STAR_POS_Y);
	m_vSize[MARSE_2]	= XMFLOAT2(STAR_SIZE, STAR_SIZE);
	m_vPos[JUPITER_3]	= XMFLOAT2(m_vPos[MARSE_2].x   + STAR_INTERVAL, STAR_POS_Y);
	m_vSize[JUPITER_3]	= XMFLOAT2(STAR_SIZE, STAR_SIZE);
	m_vPos[VINUS_4]		= XMFLOAT2(m_vPos[JUPITER_3].x + STAR_INTERVAL, STAR_POS_Y);
	m_vSize[VINUS_4]	= XMFLOAT2(STAR_SIZE_SAT_X, STAR_SIZE);
	m_vPos[ASTEROIDS_5] = XMFLOAT2(m_vPos[VINUS_4].x   + STAR_INTERVAL, STAR_POS_Y);
	m_vSize[ASTEROIDS_5]= XMFLOAT2(STAR_SIZE, STAR_SIZE);

	m_vPos[ASTRONAUT]	= XMFLOAT2(STAR_FIRST, STAR_POS_Y + 10.0f);
	m_vSize[ASTRONAUT]	= XMFLOAT2(ASTRONAUT_X / 16, ASTRONAUT_Y / 16);
	m_vPos[ARROW_RIGHT] = XMFLOAT2(300.0f, -250.0f);
	m_vSize[ARROW_RIGHT]= XMFLOAT2(100.0f, 100.0f);
	m_vPos[ARROW_LEFT]	= XMFLOAT2(-300.0f, -250.0f);
	m_vSize[ARROW_LEFT] = XMFLOAT2(100.0f, 100.0f);
	m_vPos[NOPLAY] = STAGENAME_POS;
	m_vSize[NOPLAY] = XMFLOAT2(0.0f, 0.0f);
	m_vPos[SHUTTER] = STAGENAME_POS;
	m_vSize[SHUTTER] = XMFLOAT2(SHUTTER_SIZE_X / 1.8f, SHUTTER_SIZE_Y / 1.8f);
	m_vPos[STARFLAME] = m_vPos[JUPITER_3];
	m_vSize[STARFLAME] = XMFLOAT2(STAR_FLAMESIZEX * 0.7f, STAR_FLAMESIZEY * 0.7f);
	
	for (int i = STAGENAME1; i <= NAMESHINING5; ++i) {
		m_vPos[i] = STAGENAME_POS;
		m_vSize[i] = XMFLOAT2(300.0f, 150.0f);
	}

	//�F������
	for (int i = 0; i < MAX_SELECTUI_TEXUTURE; ++i) {
		m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		if (i >= STAGENAME1)
		{
			m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
		}
	}
	//���̏����ʒu��ۑ�		01/16 �O�֑�O
	g_DefArrowRight = m_vPos[ARROW_RIGHT];
	g_DefArrowLeft  = m_vPos[ARROW_LEFT];
	g_fStageNameY = 0.0f;

	g_fDeffStageNameY = m_vPos[STAGENAME1].y;	//�X�e�[�W�̕����̏���X���W			01/23 �O�֑�O
	g_fMoveY = 0.0f;			//�F����s�m�̂ӂ�ӂ�͂��Ǘ�����ϐ�		01/18 �O�֑�O
	g_bMoveUP = 1;				//���݂̉F����s�m�̂ӂ�ӂ���� ��or��		01/18 �O�֑�O
	g_bShiningAnimFlag = 0;		// �X�e�[�W�̕����̔����t���O				01/25 �O�֑�O
	g_bOldShiningAnimFlag = 0;	// ���O�̃X�e�[�W�̕����̔����t���O			01/25 �O�֑�O
	g_nShiningAnimTime = 0;		// �X�e�[�W�̕����̔�������					01/25 �O�֑�O

	g_bNoPlayAnimFlag = 0;		// "�܂��V�ׂȂ���"�̕������h���t���O		01/23 �O�֑�O
	g_nNoPlayAnimTime = 0;		// "�܂��V�ׂȂ���"�̕������h��鎞��		01/23 �O�֑�O

	g_bStageAnimFlag = 0;		// �X�e�[�W�̕������h���t���O				01/23 �O�֑�O
	g_nStageAnimTime = 0;		// �X�e�[�W�̕������h��鎞��				01/23 �O�֑�O
	return hr;
}

//=============================================================================
//�I��
//=============================================================================
void SelectUI::Uninit() {
	for (int i = 0; i < MAX_SELECTUI_TEXUTURE; ++i) {
		//�w�i�e�N�X�`�����
		SAFE_RELEASE(m_pTexture[i]);
	}
}

//=============================================================================
//�X�V
//=============================================================================
void SelectUI::Update(int stageNum) {
	stageNum--;
	g_fStageNameY = 1.0f / 5.0f * stageNum;
	for (int i = MOON_1; i <= ASTEROIDS_5; i++) {
		//�i�s�x�ɒB���Ă�����̂͒ʏ�\����
		if (i < GetProgress()-1) {
			m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else if(i == GetProgress() - 1) {	// �v���C�\�����N���A�̐��A�C�R���͂�������\��
			m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
		}
		else { //�i�s�x�ɒB���Ă��Ȃ����A�C�R���͊D�F��
			m_vColor[i] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.2f);
		}
	}

	//���ݑI�𒆂̃X�e�[�W���i�s�x�ɒB���Ă��邩
	if (stageNum < GetProgress()) {
		m_vColor[ASTRONAUT] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_vColor[NOPLAY].w = 0.0f;
		m_vColor[SHUTTER].w = 0.0f;
	}
	else {
		m_vSize[NOPLAY] = m_vSize[STAGENAME1];
		m_vColor[ASTRONAUT] = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.4f);
		m_vColor[NOPLAY].w = 1.0f;
		m_vColor[SHUTTER].w = 1.0f;
	}

	for (int i = STAGENAME1; i <= STAGENAME5; i++)
	{
		m_vColor[i].w = 0.0f;
	}
	m_vColor[STAGENAME1 + stageNum].w = 1.0f;

	//���ݑI�𒆂́��X�e�[�W���i�s�x�ɒB���Ă��Ȃ���΁A���𔖂�����	01/23 �O�֑�O
	if (GetProgress() < stageNum + 2 && stageNum != 4) {
		m_vColor[ARROW_RIGHT].w = 0.6f;
	}
	else 
	{
		m_vColor[ARROW_RIGHT].w = 1.0f;
	}
	//���ݑI�𒆂́��X�e�[�W���i�s�x�ɒB���Ă��Ȃ���΁A���𔖂�����	01/23 �O�֑�O
	if (GetProgress() < stageNum || (stageNum == 0 && GetProgress() < 5)) {
		m_vColor[ARROW_LEFT].w = 0.6f;
	}
	else
	{
		m_vColor[ARROW_LEFT].w = 1.0f;
	}

	//�~�j�F����s�m�ӂ�ӂ폈��(�c�ړ���g_fMoveY���F����s�m�ɔ��f)	01/18 �O�֑�O
	m_vPos[ASTRONAUT].y += g_fMoveY;
	if (g_fMoveY >= ASTRONAUT_MOVE) //�F����s�m�̏�ړ�������l�ɒB������
	{
		g_bMoveUP = false; //���ړ���Ԃ�
	}
	else if (g_fMoveY <= -ASTRONAUT_MOVE) //�F����s�m�̉��ړ��������l�ɒB������
	{
		g_bMoveUP = true; //��ړ���Ԃ�
	}
	if (g_bMoveUP == true)
	{
		g_fMoveY += ASTRONAUT_SPEED;
	}
	else
	{
		g_fMoveY -= ASTRONAUT_SPEED;
	}
	//"�܂��V�ׂȂ���"�̕������h���t���O��true��������		 01/23	�O�֑�O
	if (g_bNoPlayAnimFlag == true)
	{
		//Time�ϐ����g����"�܂��V�ׂȂ���"�̕�����h�炷����		 01/23	�O�֑�O
		g_nNoPlayAnimTime++;
		if (g_nNoPlayAnimTime <= NOPLAY_ANIM_TIME)
		{
			m_vPos[NOPLAY].x += NOPLAY_ANIM_SPEED;
		}
		if (NOPLAY_ANIM_TIME < g_nNoPlayAnimTime && g_nNoPlayAnimTime <= NOPLAY_ANIM_TIME * 3)
		{
			m_vPos[NOPLAY].x -= NOPLAY_ANIM_SPEED;
		}
		if (NOPLAY_ANIM_TIME * 3 < g_nNoPlayAnimTime && g_nNoPlayAnimTime <= NOPLAY_ANIM_TIME * 4)
		{
			m_vPos[NOPLAY].x += NOPLAY_ANIM_SPEED;
		}
		if (g_nNoPlayAnimTime >= NOPLAY_ANIM_TIME * 4)
		{
			g_bNoPlayAnimFlag = false;
			m_vPos[NOPLAY].x = 0.0f;
		}
	}
	//"�܂��V�ׂȂ���"�̕������h���t���O��true��������		 01/23	�O�֑�O
	if (g_bStageAnimFlag == true)
	{
		//Time�ϐ����g�����X�e�[�W�̕�����h�炷����		 01/23	�O�֑�O
		if (g_nStageAnimTime <= STAGE_ANIM)
		{
			//�f�t�H���g���W�ɃA�j���[�V�����̒l�������ĕ␳				 01/23	�O�֑�O
			m_vPos[STAGENAME1 + stageNum].y = g_fDeffStageNameY + (g_nStageAnimTime / STAGE_ANIM) * 5;
			g_nStageAnimTime++;
		}
		else
		{
			g_bStageAnimFlag = false;		//�A�j���[�V�����t���O��false��		 01/23	�O�֑�O
		}
	}

	//�����t���O��true��������		 01/25	�O�֑�O
	if (g_bShiningAnimFlag == true)
	{
		if (g_bShiningAnimFlag != g_bOldShiningAnimFlag)
		{
			//�Y���X�e�[�W�̔��������̕s�����x��1��		 01/25	�O�֑�O
			m_vColor[NAMESHINING1 + stageNum].w = 1.0f;
		}
		//Time�ϐ����g������������		 01/25	�O�֑�O
		if (g_nShiningAnimTime <= SHINING_ANIM_TIME)
		{
			//�|���S���̊g�又��
			m_vSize[NAMESHINING1 + stageNum].x += SHUTTER_SIZE_X * 0.008f;
			m_vSize[NAMESHINING1 + stageNum].y += SHUTTER_SIZE_Y * 0.008f;
			//�Y���X�e�[�W�̔��������̕s�����x���A�A�j���[�V�����̐i�݋�ɍ��킹�ĕs�����x��������	 01/23	�O�֑�O
			m_vColor[NAMESHINING1 + stageNum].w = 1.0f - (g_nShiningAnimTime / SHINING_ANIM_TIME);
			g_nShiningAnimTime++;
		}
		else
		{
			g_bShiningAnimFlag = false;		//�A�j���[�V�����t���O��false��		 01/23	�O�֑�O
		}
		g_bOldShiningAnimFlag = g_bShiningAnimFlag;
	}
}

//=============================================================================
//�`��
//=============================================================================
void SelectUI::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	//�F����s�m	01/18 �O�֑�O
	SetPolygonSize(m_vSize[STARFLAME].x, m_vSize[STARFLAME].y);
	SetPolygonPos(m_vPos[STARFLAME].x, m_vPos[STARFLAME].y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonTexture(m_pTexture[STARFLAME]);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

	//���̕`��
	for (int i = MOON_1; i <= ASTEROIDS_5; i++) {
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonColor(m_vColor[i].x, m_vColor[i].y, m_vColor[i].z);
		SetPolygonAlpha(m_vColor[i].w);	//���`��̕s�����x�́A�X�e�[�W�i�s�x�ɂ���ĕύX�����
		DrawPolygon(pDC);
	}

	SetPolygonAlpha(1.0f);

	//�F����s�m	01/16 �O�֑�O
	SetPolygonSize(m_vSize[ASTRONAUT].x, m_vSize[ASTRONAUT].y);
	SetPolygonPos(m_vPos[ASTRONAUT].x, m_vPos[ASTRONAUT].y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonTexture(m_pTexture[ASTRONAUT]);
	SetPolygonAlpha(m_vColor[ASTRONAUT].w);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

	//�X�e�[�W��
	for (int i = STAGENAME1; i < NAMESHINING5; ++i) 
	{
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonFrameSize(1.0f, 1.0f);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonColor(m_vColor[i].x, m_vColor[i].y, m_vColor[i].z);
		SetPolygonAlpha(m_vColor[i].w);
		DrawPolygon(pDC);
	}
/*	if (ANIM_THIRD <= g_fAnimProgress && g_fAnimProgress < ANIM_FOURTH)
	{//�|���S���̊g�又��
		g_vSize.x += TEXTURE_SIZEX * 0.03f;
		g_vSize.y += TEXTURE_SIZEY * 0.03f;

		m_Color.w = 1 - (g_fAnimProgress - ANIM_THIRD) / (ANIM_FOURTH - ANIM_THIRD);		//�|���S���̕s�����x��1��0
	}
	*/
	//�V���b�^�[
	SetPolygonSize(m_vSize[SHUTTER].x, m_vSize[SHUTTER].y);
	SetPolygonPos(m_vPos[SHUTTER].x, m_vPos[SHUTTER].y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonTexture(m_pTexture[SHUTTER]);
	SetPolygonAlpha(m_vColor[SHUTTER].w);
	SetPolygonColor(m_vColor[SHUTTER].x, m_vColor[SHUTTER].y, m_vColor[SHUTTER].z);
	DrawPolygon(pDC);

	//���
	for (int i = ARROW_RIGHT; i <= ARROW_LEFT; i++) {
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonFrameSize(0.25f, 0.25f);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonColor(m_vColor[i].x, m_vColor[i].y, m_vColor[i].z);
		SetPolygonAlpha(m_vColor[i].w);
		DrawPolygon(pDC);
	}

	//�s�����x��MAX��
	SetPolygonAlpha(1.0f);

	//�I�ׂȂ�(�܂�����)
	SetPolygonSize(m_vSize[NOPLAY].x, m_vSize[NOPLAY].y);
	SetPolygonPos(m_vPos[NOPLAY].x, m_vPos[NOPLAY].y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonTexture(m_pTexture[NOPLAY]);
	SetPolygonColor(m_vColor[NOPLAY].x, m_vColor[NOPLAY].y, m_vColor[NOPLAY].z);
	SetPolygonAlpha(m_vColor[NOPLAY].w);
	DrawPolygon(pDC);

	//���ɖ߂�
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
}

//=============================================================================
//��ʏ�̉F����s�m�̈ʒu��I�𒆂̃X�e�[�W�Ɠ���	01/16�O�֑�O
//=============================================================================
void SelectUI::AstroFloating(int num, int animation, int stageNum) {
	//�F����s�m�̍��W �� �I�𒆃X�e�[�W�̍��WX �{ �A�j���[�V�����ł̈ړ���
	m_vPos[ASTRONAUT].x = m_vPos[stageNum].x + (float)animation * 6;
}
//=============================================================================
//Select.cpp�̓��e�Ɠ������āA���L�[��������Ɠ������֐�	01/16�O�֑�O
//=============================================================================
void SelectUI::ArrowAnim(int LR, int animation) {
	//�F����s�m�̍��W �� �I�𒆃X�e�[�W�̍��WX �{ �A�j���[�V�����ł̈ړ���
	if (!LR) // LR�̒l���E�̏ꍇ
	{
		if (animation > 0 && 8 > animation) // �A�j���[�V������1�ȏ�̎�
		{	//���L�[��x���W		�����L�[�̏���x���W+ �A�j���[�V�����̐i�s�x * 5
			m_vPos[ARROW_RIGHT].x = g_DefArrowRight.x + animation * 5.0f;
		}
		else
		{	//�ܕԂ����_����A�i�񂾖������Ƃɖ߂��Ă���
			m_vPos[ARROW_RIGHT].x -= 5.0f;
		}
	}
	else // LR�̒l�����̏ꍇ
	{
		if (animation < 0 && -8 < animation) // �A�j���[�V������1�ȏ�̎�
		{	//���L�[��x���W		�����L�[�̏���x���W+ �A�j���[�V�����̐i�s�x * 5
			m_vPos[ARROW_LEFT].x = g_DefArrowLeft.x + animation * 5.0f;
		}
		else
		{	//�ܕԂ����_����A�i�񂾖������Ƃɖ߂��Ă���
			m_vPos[ARROW_LEFT].x += 5.0f;
		}
	}
}

//=============================================================================
//Select.cpp����A"�܂��V�ׂȂ���"�̗h���t���O��ݒ�	01/23�O�֑�O
//=============================================================================
void SelectUI::NoPlayAnimSet() {
	g_bNoPlayAnimFlag = true;
	g_nNoPlayAnimTime = 0;
}

//=============================================================================
//Select.cpp����A"�܂��V�ׂȂ���"�̗h���t���O���擾	01/23�O�֑�O
//=============================================================================
bool SelectUI::NoPlayAnimGet() {
	return g_bNoPlayAnimFlag;
}

//=============================================================================
//Select.cpp����A�X�e�[�W���̗h���t���O��ݒ�	01/23�O�֑�O
//=============================================================================
void SelectUI::StageAnimSet() {
	g_bStageAnimFlag = true;
	g_nStageAnimTime = 0;
}

//=============================================================================
//Select.cpp����A�X�e�[�W���̔����t���O��ݒ�	01/23�O�֑�O
//=============================================================================
void SelectUI::ShiningAnimSet() {
	g_bShiningAnimFlag = true;
	g_nShiningAnimTime = 0;
}