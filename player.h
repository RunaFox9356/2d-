//============================
//
// �v���C���[�ݒ�w�b�^�[
// Author:hamada ryuuga
//
//============================
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "main.h"
#include "motion.h"

class CPlayer
{
public:
	typedef enum
	{
		ANIME_NORMAL = 0,	// �j���[�g����
		ANIME_RUN,			// ����
		ANIME_ATTACK,		// �U��
		ANIME_JUMP,			// �W�����v
		ANIME_LANDING,		// ���n
		ANIME_MAX
	}ANIME;

	typedef enum
	{
		STATUS_NORMAL = 0,	// �j���[�g����
		STATUS_RUN,			// ����
		STATUS_ATTACK,		// �U��
		STATUS_JUMPUP,		// �W�����v
		STATUS_JUMPDOWN,	// �W�����v
		STATUS_LANDING,		// ���n
		STATUS_MAX
	}STATUS;

	typedef enum
	{
		DAMEGE_NORMAL = 0,	// �j���[�g����
		DAMEGE_NOU,			// �_���[�W������Ă�
		DAMEGE_MAX
	}DAMEGE;

	typedef enum
	{
		COPY_NORMAL = 0,	// �j���[�g����
		COPY_SWORD,			// �\�[�h
		COPY_FIRE,			// �t�@�C�A
		COPY_LASER,			// ���[�U�[
		COPY_CUTTER,		// �J�b�^�[
		COPY_MAX
	}COPY;

	//model�f�[�^�̍\����//
	typedef struct
	{
		int key;		// ���ԊǗ�
		int nowKey;		// ���̃L�[
		int loop;		// ���[�v���邩�ǂ���[0:���[�v���Ȃ� / 1 : ���[�v����]
		int num_key;  	// �L�[��
		MyKeySet KeySet[MAX_KEY];
	}MODELDATAPLAYER;

public:
	static const float ATTENUATION;		// �����W��
	static const float SPEED;			// �X�s�[�h
	static const float WIDTH;			// ���f���̔��a
	static const int MAX_PRAYER;		// �ő吔
	static const int MAX_MOVE;			// �A�j���[�V�����̍ő吔
	static const int INVINCIBLE;		// ���G����
	static const int MAX_MODELPARTS = 9;
	static const int MAX_COPY;

public:
	CPlayer();
	~CPlayer();

	void Init(void);	// ������
	void Uninit(void);	// �j��
	void Update(void);	// �X�V
	void Draw(void);	// �`��

	void Set(D3DXVECTOR3 pos, D3DXVECTOR3 rot);	// �Z�b�g�������W�Ɠǂݍ��ރt�@�C����

	void SetCopy(char *pFileName, PartsFile *partsFile, Parts *parts, MyMotion *Motion, int *nMaxParts);

private:
	void Collision(void);	// �����蔻��܂Ƃ�
	void Move(void);		// �ړ�

private:
	D3DXVECTOR3		m_pos;						// �ʒu
	D3DXVECTOR3		m_posOld;					// �ʒu�ߋ�
	D3DXVECTOR3		m_move;						// ���[�u
	D3DXVECTOR3		m_rot;						// ��]	
	D3DXVECTOR3		m_rotMove;					// ��]���[�u
	D3DXVECTOR3		m_modelMin;					// �T�C�Y�ŏ�
	D3DXVECTOR3		m_modelMax;					// �T�C�Y�ő�
	D3DXMATRIX		m_mtxWorld;					// �}�g���b�N�X//�|���S���̈ʒu���]�s�񂷂ׂĂ��߂Ă�i�j�J

	STATUS			m_status;					// ���̃X�e�[�^�X
	DAMEGE			m_damege;					// �_���[�W������Ă邩������ĂȂ���
	COPY			m_copy;						// �R�s�[

	Parts			m_parts[MAX_MODELPARTS];		// ���f���p�[�c
	PartsFile		m_partsFile[MAX_MODELPARTS];	// �p�[�c�t�@�C��
	MyMotion		m_motion[ANIME_MAX];			// ���[�V����
	ANIME			m_motionType;					// ���[�V�����^�C�v(����)
	ANIME			m_motionTypeOld;				// ���[�V�����^�C�v(�ߋ�)
	int				m_nMaxModelType;				// ���f���̃^�C�v��
	int				m_nMaxModelParts;				// �������f���p�[�c��
	int				m_nMaxMotion;					// ���[�V������

	int				m_type;						// �^�C�v
	int				m_shadow;					// �e�ԍ�
	int				m_invincible;				// ���G����
	float			m_consumption;				// �v�Z�p

	bool			m_bMotionBlend;				// ���[�V�����u�����h
	bool			m_bMotion;					// ���[�V�������g�p��

	bool			m_isUse;					// �g���Ă邩�g���ĂȂ���
	bool			m_isLoop;					// ���[�v���邩���Ȃ���

	char			m_aFirename[256];//

	int				m_time;		// �^�C���̍ő吔
	int				m_nparts;	// �p�[�c�̍ő吔
	int				m_pow;		// �W�����v�p���[
	int				m_nMotion;	// 
};
#endif