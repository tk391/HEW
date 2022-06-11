//�|�[�Y�@�\�̎���
//12/21  ������n
#pragma once
#ifndef __PAUSE_H__
#define __PAUSE_H__

#include "main.h"

// �|�[�Y���j���[
typedef enum {
	PAUSE_MENU_CONTINUE = 0,	// �R���e�B�j���[
	PAUSE_MENU_RETRY,			// ���g���C
	PAUSE_MENU_QUIT,			// �N�C�b�g
	PAUSE_MENU_MAX
} PAUSE_MENU;

// �v���g�^�C�v�錾
HRESULT InitPause(void);
void UninitPause(void);
void UpdatePause(void);
void DrawPause(void);

void SetPauseMenu(void);
PAUSE_MENU GetPauseMenu(void);
void ResetPauseMenu(void);

#endif // !__PAUSE_H__
//�����܂�