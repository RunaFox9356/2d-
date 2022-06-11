//============================
//
// �v���C���[�ݒ�w�b�^�[
// Author:hamada ryuuga
//
//============================
#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "main.h"
#include "motion.h"
#include "player.h"

//------------------------------------
// �}�N��
//------------------------------------
#define MAX_MODELPARTS (9)

//------------------------------------
// ��ʂ̗񋓌^
//------------------------------------
enum ENEMY_TYPE
{
	ENEMY_TYPE_HUMANSOUL = 0,	// �l��
	ENEMY_TYPE_SKELETON,		// ��������
	ENEMY_TYPE_UNGAIKYO,		// �_�O��
	ENEMY_TYPE_KAMAITACHI,		// ���܂�����
	ENEMY_TYPE_BUDDHA,			// �u�c�[
	ENEMY_TYPE_NULL1,		// ��������
	ENEMY_TYPE_NULL2,		// �_�O��
	ENEMY_TYPE_NULL3,		// ���܂�����
	ENEMY_TYPE_NULL4,			// �u�c�[
	ENEMY_TYPE_NULL5,
	ENEMY_TYPE_NULL6,
	ENEMY_TYPE_NULL7,
	ENEMY_TYPE_NULL8,
	ENEMY_TYPE_NULL9,
	ENEMY_TYPE_NULL10,
	ENEMY_TYPE_NULL11,
	ENEMY_TYPE_NULL12,
	ENEMY_TYPE_NULL13,
	ENEMY_TYPE_NULL14,
	ENEMY_TYPE_MAX
};

typedef struct
{
	D3DXVECTOR3	pos;						// �ʒu
	D3DXVECTOR3	posOld;						// �ʒu�ߋ�
	D3DXVECTOR3	move;						// ���[�u
	D3DXVECTOR3	rot;						// ��]	
	D3DXVECTOR3	rotMove;					// ��]���[�u
	D3DXVECTOR3	modelMin;					// �T�C�Y�ŏ�
	D3DXVECTOR3	modelMax;					// �T�C�Y�ő�

	D3DXVECTOR3 scale;
	D3DXMATRIX	mtxWorld;					// �}�g���b�N�X//�|���S���̈ʒu���]�s�񂷂ׂĂ��߂Ă�i�j�J

	CPlayer::STATUS		status;						// ���̃X�e�[�^�X
	CPlayer::DAMEGE		damege;						// �_���[�W������Ă邩������ĂȂ���
	CPlayer::COPY		copy;						// �R�s�[
	ENEMY_TYPE	type;						// �G�l�~�[�̃^�C�v

	Parts		parts[MAX_MODELPARTS];		// ���f���p�[�c
	PartsFile	partsFile[MAX_MODELPARTS];	// �p�[�c�t�@�C��
	MyMotion	motion[CPlayer::ANIME_MAX];			// ���[�V����
	CPlayer::ANIME		motionType;					// ���[�V�����^�C�v(����)
	CPlayer::ANIME		motionTypeOld;				// ���[�V�����^�C�v(�ߋ�)
	int			nMaxModelType;				// ���f���̃^�C�v��
	int			nMaxModelParts;				// �������f���p�[�c��
	int			nMaxMotion;					// ���[�V������

	int			nInvincible;				// ���G����
	float		fConsumption;				// �v�Z�p
	float		fLog;
	
	bool		bMotionBlend;				// ���[�V�����u�����h
	bool		bMotion;					// ���[�V�������g�p��
	bool		bRange;
	bool		bSelect;
	bool		isUse;						// �g���Ă邩�g���ĂȂ���
	bool		notLoop;					// ���[�v���邩���Ȃ���
}Enemy;

//�v���g�^�C�v�錾
void InitEnemy(void);	// ������
void UninitEnemy(void);	// �j��
void UpdateEnemy(void);	// �X�V
void DrawEnemy(void);	// �`��

void SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, ENEMY_TYPE type);//�Z�b�g�������W�Ɠǂݍ��ރt�@�C����
void SizeSet(void);//�����蔻��擾
Enemy *GetEnemy(void);//�Q�b�g
void LoadEnemy(void);	// �Ǎ�
void LoadSetFile(char *Filename);
void OutputEnemy(char *Filename);

void SelectDes(D3DXVECTOR3 pos);
//void falseSetEnemy(void);
//void selectDes(D3DXVECTOR3 Mouse, D3DXVECTOR3 Size);
#endif