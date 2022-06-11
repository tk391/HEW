//=============================================================================
// �G�t�F�N�g�Ǘ��N���X[EffectManager.cpp]
// 12/19 �V������
//=============================================================================
#include "EffectManager.h"
#include "Texture.h"
#include "mesh.h"
#include "Camera.h"
#include "Light.h"
#include "polygon.h"
#include"debugproc.h"

//�}�N��
#define MAX_ONEEFFECT		(100)	//��̃G�t�F�N�g�ŏo�����ő吔
#define M_DIFFUSE					XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR					XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_POWER						(1.0f)
#define M_AMBIENT					XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE					XMFLOAT4(0.0f,0.0f,0.0f,1.0f)

#define EXPLOSION_FRAME_X		4//���t���[����
#define EXPLOSION_FRAME_Y		4//�c�t���[����
#define EXPLOSION_ANIM_COUNT	4//1�R�}������̕\����

//�V���O���g��
EffectManager* EffectManager::m_pInstance = nullptr;


//�e�G�t�F�N�g�i���j�̗v�f
struct TEffect {
	XMFLOAT3 pos;				//�ʒu
	XMFLOAT2 size;				//���x
	float			angle;				//�p�x
	bool			use;				//�g�p���Ă��邩
	bool			one;				//1�����o�Ȃ��G�t�F�N�g��
	int				anim;			//�A�j���[�V�����Đ��ʒu
	int				count;			//�A�j���[�V�����t���[����
	int StartAnim;
	int EndAnim;
};

//�e�G�t�F�N�g�i�G�t�F�N�g�S�́j�̗v�f
struct TEffectSet {
	TEffect oneEffect[MAX_ONEEFFECT];//�e�G�t�F�N�g�̏��
	Dimension dimension;//2D���W�ɐ������邩�A3D���W�ɐ������邩
	Loop loopJuge;			//���̏�Ń��[�v����̂�
	//3D�G�t�F�N�g�̂ݎg�p
	MESH	m_mesh;
	MATERIAL m_material;
	//�e�N�X�`���̂ݎg�p
	ID3D11ShaderResourceView*	m_pTexture;
};

//=============================================================================
//�v���g�^�C�v�錾
//=============================================================================
static HRESULT MakeVertexEffect(ID3D11Device* pDevice, std::string effectName);

//�}�b�v
std::map<std::string, TEffectSet*> mapEffects;
std::map<std::string, TEffectSet*>::iterator it;//mapEffects�̃C�e���[�^�[

EffectManager::EffectManager() {
}

HRESULT EffectManager::Init() {
	//�V���O���g��
	m_pInstance = new EffectManager;

	HRESULT hr = S_OK;
	//�G�t�F�N�g�Ŏg�p����摜�͓K�X�����Œǉ����Ă�������
	//�W�F�b�g�G�t�F�N�g
	mapEffects.insert(std::make_pair("jet", new TEffectSet));//�G�t�F�N�g�ǉ�
	mapEffects["jet"]->dimension = Two;//2D��3D��
	mapEffects["jet"]->oneEffect[0].size = XMFLOAT2(100.0f, 100.0f);//�傫�����Ɏw��
	mapEffects["jet"]->loopJuge = Loop_ON;
	mapEffects["jet"]->oneEffect[0].StartAnim = 7;
	mapEffects["jet"]->oneEffect[0].EndAnim = mapEffects["jet"]->oneEffect[0].StartAnim + 3;
	hr = m_pInstance->Set("jet", "data/texture/hunsya_T_Sprit.png");
	if (FAILED(hr)) {
		MessageBoxA(GetMainWnd(), "�ǂݍ��݃G���[jet", "EffectManager::Init", MB_OK);
	}

	//�Փ˃G�t�F�N�g
	mapEffects.insert(std::make_pair("collision", new TEffectSet));//�G�t�F�N�g�ǉ�
	mapEffects["collision"]->dimension = Three;//2D��3D��
	mapEffects["collision"]->oneEffect[0].size = XMFLOAT2(30.0f, 30.0f);//�傫�����Ɏw��
	mapEffects["collision"]->loopJuge = Loop_OFF;
	hr = m_pInstance->Set("collision", "data/texture/Hit_arfaSprit.png");
	if (FAILED(hr)) {
		MessageBoxA(GetMainWnd(), "�ǂݍ��݃G���[collision", "EffectManager::Init", MB_OK);
	}

	return hr;
}

void EffectManager::Uninit() {
	it = mapEffects.begin();
	while (it != mapEffects.end()) {
		if (it->second->dimension == Two) {
			// �e�N�X�`�����
			SAFE_RELEASE(it->second->m_pTexture);
		}
		else {
			//���b�V�����
			ReleaseMesh(&it->second->m_mesh);
		}
		it++;
	}
	//�}�b�v�폜
	mapEffects.clear();

	//���
	delete m_pInstance;
}

void EffectManager::Update(std::string effectName, XMFLOAT3 Pos, float Angle, bool push) {
	TEffect* pEffect = mapEffects[effectName]->oneEffect;
	int count = 0;
	for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
		//���g�p�Ȃ�X�L�b�v
		if (!pEffect->use) {
			if (mapEffects[effectName]->loopJuge == Loop_ON) {
				break;
			}
			else {
				continue;
			}
		}
		count++;
		//�J�E���^�X�V
		pEffect->count--;
		//�ʒu�X�V
		pEffect->pos = Pos;
		//�p�x�X�V
		pEffect->angle = Angle;
		if (pEffect->count <= 0) {
			++pEffect->anim;
			//�{�^���������Ă�(���[�v�Đ���ԂɂȂ��Ă���)�Ԗ������[�v
			if (push&&pEffect->anim >= pEffect->EndAnim) {
				pEffect->anim = pEffect->StartAnim;
			}
			else {
				//�{�^���������Ă��Ȃ���΃A�j���[�V�����̏I�[�ŏ���
				if (pEffect->anim >= EXPLOSION_FRAME_X * EXPLOSION_FRAME_Y) {
					pEffect->use = false;	//����
					continue;
				}
			}
			//�J�E���^������
			pEffect->count = EXPLOSION_ANIM_COUNT;
		}
	}
	//PrintDebugProc("pEffect->anim=%d\n", mapEffects["jet"]->oneEffect->anim);
	PrintDebugProc("count=%d\n", count);
}
void EffectManager::Update(std::string EffectName, bool push) {
	TEffect* pEffect = mapEffects[EffectName]->oneEffect;
	for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
		//���g�p�Ȃ�X�L�b�v
		if (!pEffect->use) {
			if (mapEffects[EffectName]->loopJuge == Loop_ON) {
				return;
			}
			else {
				continue;
			}
		}
		//�J�E���^�X�V
		pEffect->count--;
		if (pEffect->count <= 0) {
			++pEffect->anim;
			//�{�^���������Ă�(���[�v�Đ���ԂɂȂ��Ă���)�Ԗ������[�v
			if (push&&pEffect->anim >= pEffect->EndAnim) {
				pEffect->anim = pEffect->StartAnim;
			}
			else {
				//�{�^���������Ă��Ȃ���΃A�j���[�V�����̏I�[�ŏ���
				if (pEffect->anim >= EXPLOSION_FRAME_X * EXPLOSION_FRAME_Y) {
					pEffect->use = false;	//����
					continue;
				}
			}
			//�J�E���^������
			pEffect->count = EXPLOSION_ANIM_COUNT;
		}
	}
}

void EffectManager::Draw(std::string effectName) {
	ID3D11DeviceContext* pDC = GetDeviceContext();
	TEffect* pEffect = mapEffects[effectName]->oneEffect;

	SetBlendState(BS_ALPHABLEND);//���u�����f�B���O�L��

	if (mapEffects[effectName]->dimension == Two) {//�QD�G�t�F�N�g��������
		// Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
		SetZBuffer(false);

		for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
			//���g�p�Ȃ�X�L�b�v
			if (!pEffect->use) {
				if (mapEffects[effectName]->loopJuge == Loop_ON) {
					break;
				}
				else {
					continue;
				}
			}
			int u = pEffect->anim%EXPLOSION_FRAME_X;
			int v = pEffect->anim / EXPLOSION_FRAME_X;
			//�|���S���Z�b�g
			SetPolygonSize(pEffect->size.x, pEffect->size.y);
			SetPolygonTexture(mapEffects[effectName]->m_pTexture);
			SetPolygonFrameSize(1.0f / EXPLOSION_FRAME_X, 1.0f / EXPLOSION_FRAME_Y);
			SetPolygonUV((float)u / EXPLOSION_FRAME_X, (float)v / EXPLOSION_FRAME_Y);
			SetPolygonAngle(pEffect->angle);
			SetPolygonPos(pEffect->pos.x, pEffect->pos.y);
			DrawPolygon(pDC);
		}
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonFrameSize(1.0f, 1.0f);
		SetPolygonAngle(0.0f);

		// Z�o�b�t�@�L��(Z�`�F�b�N�L&Z�X�V�L)
		SetZBuffer(true);
	}
	else {//3D�G�t�F�N�g��������
		CLight::Get()->SetDisable();	//��������
		XMFLOAT4X4& mView = CCamera::Get()->GetViewMatrix();
		for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
			//���g�p�Ȃ�X�L�b�v
			if (!pEffect->use) {
				if (mapEffects[effectName]->loopJuge == Loop_ON) {
					break;
				}
				else {
					continue;
				}
			}
			//�e�N�X�`���}�g���N�X�X�V
			XMMATRIX mtxTex = XMMatrixScaling(1.0f / EXPLOSION_FRAME_X, 1.0f / EXPLOSION_FRAME_Y, 1.0f);
			int u = pEffect->anim%EXPLOSION_FRAME_X;
			int v = pEffect->anim / EXPLOSION_FRAME_X;
			mtxTex = XMMatrixMultiply(mtxTex, XMMatrixTranslation((float)u / EXPLOSION_FRAME_X, (float)v / EXPLOSION_FRAME_Y, 0.0f));
			XMStoreFloat4x4(&mapEffects[effectName]->m_mesh.mtxTexture, mtxTex);

			//�r���[�s��̉�]�����̓]�u�s���ݒ�
			mapEffects[effectName]->m_mesh.mtxWorld._11 = mView._11*pEffect->size.x;
			mapEffects[effectName]->m_mesh.mtxWorld._12 = mView._21*pEffect->size.x;
			mapEffects[effectName]->m_mesh.mtxWorld._13 = mView._31*pEffect->size.x;
			mapEffects[effectName]->m_mesh.mtxWorld._14 = 0.0f;
			mapEffects[effectName]->m_mesh.mtxWorld._21 = mView._12*pEffect->size.y;
			mapEffects[effectName]->m_mesh.mtxWorld._22 = mView._22*pEffect->size.y;
			mapEffects[effectName]->m_mesh.mtxWorld._23 = mView._32*pEffect->size.y;
			mapEffects[effectName]->m_mesh.mtxWorld._24 = 0.0f;
			mapEffects[effectName]->m_mesh.mtxWorld._31 = mView._13;
			mapEffects[effectName]->m_mesh.mtxWorld._32 = mView._23;
			mapEffects[effectName]->m_mesh.mtxWorld._33 = mView._33;
			mapEffects[effectName]->m_mesh.mtxWorld._34 = 0.0f;

			//�ʒu�𔽉f
			mapEffects[effectName]->m_mesh.mtxWorld._41 = pEffect->pos.x;
			mapEffects[effectName]->m_mesh.mtxWorld._42 = pEffect->pos.y;
			mapEffects[effectName]->m_mesh.mtxWorld._43 = pEffect->pos.z;
			mapEffects[effectName]->m_mesh.mtxWorld._44 = 1.0f;

			//�`��
			DrawMesh(pDC, &mapEffects[effectName]->m_mesh);
		}
		CLight::Get()->SetEnable();	//�����L��
	}
	SetBlendState(BS_NONE);		//���u�����f�B���O����
}

//======================================================================
//�G�t�F�N�g�X�^�[�g
//======================================================================
int EffectManager::Start(std::string effectName, XMFLOAT3 Pos, float Angle) {
	TEffect* pEffect = mapEffects[effectName]->oneEffect;
	for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
		//�g�p���Ȃ�X�L�b�v
		if (pEffect->use) {
			if (mapEffects[effectName]->loopJuge == Loop_ON) {
				return -1;
			}
			else {
				continue;
			}
		}
		pEffect->pos = Pos;
		pEffect->anim = 0;
		pEffect->angle = Angle;
		pEffect->count = EXPLOSION_ANIM_COUNT;
		pEffect->use = true;
		return i;		//���˂ł����i�O�`�e�ԍ��j
	}
	return -1;		//���˂ł��Ȃ�����

}
int EffectManager::Start(std::string effectName, XMFLOAT3 Pos) {
	TEffect* pEffect = mapEffects[effectName]->oneEffect;
	for (int i = 0; i < MAX_ONEEFFECT; ++i, ++pEffect) {
		//�g�p���Ȃ�X�L�b�v
		if (pEffect->use) {
			if (mapEffects[effectName]->loopJuge == Loop_ON) {
				return -1;
			}
			else {
				continue;
			}
		}
		pEffect->pos = Pos;
		pEffect->anim = 0;
		pEffect->count = EXPLOSION_ANIM_COUNT;
		pEffect->use = true;
		return i;		//���˂ł����i�O�`�e�ԍ��j
	}
	return -1;		//���˂ł��Ȃ�����
}

//=============================================================================
//���̏�����
//=============================================================================
HRESULT EffectManager::Set(std::string effectName, std::string fileName) {
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	if (mapEffects[effectName]->dimension == Two) {//�QD�G�t�F�N�g��������
		// �e�N�X�`���Ǎ�
		hr = CreateTextureFromFile(pDevice, fileName.c_str(), &mapEffects[effectName]->m_pTexture);
		if (FAILED(hr)) {
			return hr;
		}
	}
	else {//3D�G�t�F�N�g��������
		//���_���̍쐬
		hr = MakeVertexEffect(pDevice, effectName);
		if (FAILED(hr)) {
			return hr;
		}
		//�}�e���A���̐ݒ�
		mapEffects[effectName]->m_material.Diffuse = M_DIFFUSE;
		mapEffects[effectName]->m_material.Ambient = M_AMBIENT;
		mapEffects[effectName]->m_material.Specular = M_SPECULAR;
		mapEffects[effectName]->m_material.Power = M_POWER;
		mapEffects[effectName]->m_material.Emissive = M_EMISSIVE;
		mapEffects[effectName]->m_mesh.pMaterial = &mapEffects[effectName]->m_material;

		//�e�N�X�`���̓ǂݍ���
		hr = CreateTextureFromFile(pDevice, fileName.c_str(), &mapEffects[effectName]->m_mesh.pTexture);
		if (FAILED(hr)) {
			return hr;
		}
		XMStoreFloat4x4(&mapEffects[effectName]->m_mesh.mtxTexture, XMMatrixIdentity());

		//���[���h�}�g���N�X������
		XMStoreFloat4x4(&mapEffects[effectName]->m_mesh.mtxWorld, XMMatrixIdentity());
	}
	// ������
	for (int i = 0; i < MAX_ONEEFFECT; ++i) {
		mapEffects[effectName]->oneEffect[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		mapEffects[effectName]->oneEffect[i].size = mapEffects[effectName]->oneEffect[0].size;
		mapEffects[effectName]->oneEffect[i].angle = 0.0f;
		mapEffects[effectName]->oneEffect[i].use = false;
		mapEffects[effectName]->oneEffect[i].anim = 0;
		mapEffects[effectName]->oneEffect[i].count = EXPLOSION_ANIM_COUNT;
	}
	return hr;
}

//======================================================================
//���_���̃Z�b�g
//======================================================================
HRESULT MakeVertexEffect(ID3D11Device* pDevice, std::string effectName) {
	//�ꎞ�I�Ȓ��_�z��𐶐�
	mapEffects[effectName]->m_mesh.nNumVertex = 4;
	mapEffects[effectName]->m_mesh.primitiveType = PT_TRIANGLESTRIP;
	VERTEX_3D* pVertexWK = new VERTEX_3D[mapEffects[effectName]->m_mesh.nNumVertex];

	//���_�z��̒��g�𖄂߂�
	pVertexWK[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	pVertexWK[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	pVertexWK[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	pVertexWK[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);
	pVertexWK[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVertexWK[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVertexWK[0].tex = XMFLOAT2(0.0f, 0.0f);
	pVertexWK[1].tex = XMFLOAT2(1.0f, 0.0f);
	pVertexWK[2].tex = XMFLOAT2(0.0f, 1.0f);
	pVertexWK[3].tex = XMFLOAT2(1.0f, 1.0f);

	//�ꎞ�I�ȃC���f�b�N�X�z��𐶐�
	mapEffects[effectName]->m_mesh.nNumIndex = 4;
	int* pIndexWK = new int[4];

	//�C���f�b�N�X�z��̒��g�𖄂߂�
	for (int i = 0; i < 4; i++) {
		pIndexWK[i] = i;
	}

	//���_�o�b�t�@/�C���f�b�N�X�o�b�t�@����
	HRESULT hr = MakeMeshVertex(pDevice, &mapEffects[effectName]->m_mesh, pVertexWK, pIndexWK);

	//�ꎞ�I�Ȓ��_�z��/�C���f�b�N�X�z������
	delete[] pIndexWK;
	delete[] pVertexWK;
	return hr;
}

//========================================
//�����I��
//========================================
void EffectManager::End(std::string effectName) {
	//"all"=�S�ẴG�t�F�N�g
	//������"all"��������S�ẴG�t�F�N�g�������I��
	if (effectName == "all") {
		it = mapEffects.begin();
		while (it != mapEffects.end()) {
			for (int i = 0; i < MAX_ONEEFFECT; ++i) {
				it->second->oneEffect[i].use = false;
			}
			it++;
		}
	}
	else {
		for (int i = 0; i < MAX_ONEEFFECT; ++i) {
			mapEffects[effectName]->oneEffect[i].use = false;
		}
	}
}

//========================================
//�A�h���X�Ăяo��
//========================================
EffectManager* EffectManager::Get() {
	return m_pInstance;
}