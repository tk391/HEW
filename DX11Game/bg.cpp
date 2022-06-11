//=============================================================================
// �w�i [bg.cpp]
//12/11 �V������
//=============================================================================
#include "bg.h"
#include "Texture.h"
#include "polygon.h"

//=============================================================================
// �}�N����`
//=============================================================================
#define BG_POS_X		0.0f
#define BG_POS_Y		0.0f
#define BG_WIDTH		SCREEN_WIDTH
#define BG_HEIGHT		SCREEN_HEIGHT

//=============================================================================
// �O���[�o���ϐ�
//=============================================================================
std::string BGFileName[] = {
	"data/texture/titleBackground.png",
	"data/texture/5.jpg",
	"data/texture/�����Ȃ��w�i.png",
	"data/texture/prottype/option.png",
	"data/texture/prottype/result.png",
};
//�}�b�v
std::map<std::string, ID3D11ShaderResourceView*> mapBGTexture;
std::map<std::string, ID3D11ShaderResourceView*>::iterator it;//mapBGTexture�̃C�e���[�^�[

//=============================================================================
// ������
//=============================================================================
HRESULT InitBG()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	//�摜�ǉ�
	mapBGTexture.insert(std::make_pair("title", nullptr));//�^�C�g��
	hr = CreateTextureFromFile(pDevice, L"data/texture/titleBackground.png", &mapBGTexture["title"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "�ǂݍ��݃G���[title", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("select", nullptr));//�I��
	hr = CreateTextureFromFile(pDevice, L"data/texture/5.jpg", &mapBGTexture["select"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "�ǂݍ��݃G���[select", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("game", nullptr));//�Q�[��
	hr = CreateTextureFromFile(pDevice, L"data/texture/�����Ȃ��w�i.png", &mapBGTexture["game"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "�ǂݍ��݃G���[game", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("option", nullptr));//�I�v�V����
	hr = CreateTextureFromFile(pDevice, L"data/texture/prottype/option.png", &mapBGTexture["option"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "�ǂݍ��݃G���[option", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("result", nullptr));//���U���g
	hr = CreateTextureFromFile(pDevice, L"data/texture/prottype/result.png", &mapBGTexture["result"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "�ǂݍ��݃G���[result", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("clear", nullptr));//�N���A
	hr = CreateTextureFromFile(pDevice, L"data/texture/stageclear_back3.png", &mapBGTexture["clear"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "�ǂݍ��݃G���[clear", "EffectManager::Init", MB_OK); }
	mapBGTexture.insert(std::make_pair("gameover", nullptr));//�Q�[���I�[�o�[
	hr = CreateTextureFromFile(pDevice, L"data/texture/gameover_back.png", &mapBGTexture["gameover"]);
	if (FAILED(hr)) { MessageBoxA(GetMainWnd(), "�ǂݍ��݃G���[gameover", "EffectManager::Init", MB_OK); }

	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG()
{
	it = mapBGTexture.begin();
	while (it != mapBGTexture.end()) {
		// �e�N�X�`�����
		SAFE_RELEASE(it->second);
		it++;
	}
	//�}�b�v�폜
	mapBGTexture.clear();
}

//=============================================================================
// �`��
//=============================================================================
void DrawBG(std::string bgName)
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	SetPolygonSize(BG_WIDTH, BG_HEIGHT);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonTexture(mapBGTexture[bgName]);
	SetPolygonUV(0.0f, 0.0f);
	DrawPolygon(pDC);
}