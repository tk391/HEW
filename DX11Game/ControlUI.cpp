#pragma once
#include "ControlUI.h"
#include "Texture.h"
#include "polygon.h"
#include "debugproc.h"
#include "Scene.h"
#include "Player.h"
#include "Game.h"
#include "Sound.h"
//�f�o�b�O�m�F�p
#include "input.h"

//=====================================================
//�}�N����`
//=====================================================
#define PATH_CONTROLUI_TEXTURE  L"data/texture/operationXbox.png"
#define TEXTURE_SIZEX  (1724.0f * 0.28f) //�摜�T�C�Y
#define TEXTURE_SIZEY	(1288.0f * 0.28f)

#define CENTER_POS_X	(-400.0f)	//���S���WX
#define CENTER_POS_Y	(170.0f)	//���S���WY

//=====================================================
//�O���[�o���ϐ�
//=====================================================
static ID3D11ShaderResourceView* g_pTexture;

//=============================================================================
//������
//=============================================================================
HRESULT ControlUI::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	//�e�N�X�`���ǂݍ���
	hr = CreateTextureFromFile(pDevice, PATH_CONTROLUI_TEXTURE, &g_pTexture);
	if (FAILED(hr))
	{
		return hr;
	}

	m_Pos = XMFLOAT2(CENTER_POS_X, CENTER_POS_Y);			//�ꏊ
	m_Size = XMFLOAT2(TEXTURE_SIZEX, TEXTURE_SIZEY);		//�傫��
	m_Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);				//�F

	//���ʂ̃Z�b�g	0130�O�֑�O
	CSound::SetVolume(SE_CONTROLUIPOP, 80.0f);

	return hr;
}
//=============================================================================
//�I��
//=============================================================================
void ControlUI::Uninit()
{
		SAFE_RELEASE(g_pTexture);
}
//=============================================================================
//�X�V
//=============================================================================
void ControlUI::Update()
{
	int i = 0;

	if (GetJoyTrigger(0, XBOXSCREENSHOT)) //�{�^������͂����u��
	{
		//SE�Đ�
		CSound::Stop(SE_CONTROLUIPOP);
		CSound::Play(SE_CONTROLUIPOP);
		if (m_Color.w == 1.0f)	//������@UI���s�����Ȃ��
		{
			m_Color.w = 0.0f;	//������(�\��)
		}
		else if (m_Color.w == 0.0f)	//������@UI�������Ȃ��
		{
			m_Color.w = 1.0f;	//�s������(��\��)
		}
	}

#if _DEBUG
	PrintDebugProc("m_Color.w={%.2f}\n", m_Color.w);
#endif
}
//=============================================================================
//�`��
//=============================================================================
void ControlUI::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	//�X�^�[�g�e���b�v
	SetPolygonSize(m_Size.x, m_Size.y);
	SetPolygonPos(m_Pos.x, m_Pos.y);
	SetPolygonAlpha(m_Color.w);
	SetPolygonTexture(g_pTexture);
	SetPolygonUV(0.0f, 0.0f);
	DrawPolygon(pDC);
}
