//==================================================
//
// �v���C���[�ݒ�
// Author:hamada ryuuga
//
//==================================================
//--------------------------------------------------
// include
//--------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include "enemy.h"
#include "input.h"
#include "camera.h"
#include "utility.h"
#include "motion.h"
#include "map.h"
#include "comn.h"
#include "range.h"

//--------------------------------------------------
// �}�N����`
#define MAX_ENEMY	(100)	// �ő�G�l�~�[��
#define SIZE_ENEMY (D3DXVECTOR3(50.0f,50.0f,0.0f))
//#define ATTENUATION	(0.5f)		//�����W��
//#define SPEED	(1.0f)			//�X�s�[�h
#define WIDTH (10.0f)			//���f���̔��a
#define MAX_PRAYER (16)			//�ő吔
#define MAX_MOVE (9)			//�A�j���[�V�����̍ő吔
#define INVINCIBLE (300)		//���G����

//--------------------------------------------------
// �ÓI�ϐ�
//--------------------------------------------------
static Enemy s_Enemy[MAX_ENEMY];		// �G�l�~�[�̍\����
static Enemy s_EnemyType[ENEMY_TYPE_MAX];	// �G�l�~�[��ʂ̍\����
static CPlayer::MODELDATAPLAYER s_ModelData[MAX_MOVE];

static float s_fMapScale;
static float s_fLog;
static D3DXVECTOR3 s_Move(5.0f, 0.0f, 0.0f);

static int s_parts;	// �p�[�c�̍ő吔
static int s_pow;	// �W�����v�p���[
static int s_nMotion; //

//--------------------------------------------------
// �v���g�^�C�v�錾
//--------------------------------------------------
static void Loadmotion(CPlayer::MODELDATAPLAYER* set, int Setnumber);	// ���[�V�����̓Ǎ�
static void AnimationSet(int animation);						// �A�j���[�V�����̌v�Z
static void MoveSet(void);										// ���[�u�Z�b�g
static void Collision(void);									// �����蔻��܂Ƃ�
static void SetCopy(void);

//==================================================
// ����������
//==================================================
void InitEnemy(void)
{
	// �J�����̃f�[�^�擾
	Camera *pCamera = GetCamera();

	s_pow = 0;
	
	// ����������
	memset(&s_Enemy, NULL, sizeof(s_Enemy));

	// �Ǎ�
	LoadEnemy();

	s_fLog = 0.0f;
	s_fMapScale = 1.0f;
}

//=========================================
// �I������
//=========================================
void UninitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		Enemy* pEnemy = &s_Enemy[i];

		for (int i = 0; i <= s_parts; i++)
		{
			// ���_�o�b�t�@�[�̉��
			if (pEnemy->parts[i].pBuffer != NULL)
			{
				pEnemy->parts[i].pBuffer->Release();
				pEnemy->parts[i].pBuffer = NULL;
			}

			if (pEnemy->parts[i].pMesh != NULL)
			{
				pEnemy->parts[i].pMesh->Release();
				pEnemy->parts[i].pMesh = NULL;
			}
		}
	}
}

//=========================================
// �X�V����
//=========================================
void UpdateEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		Enemy* pEnemy = &s_Enemy[i];

		if (!pEnemy->isUse)
		{
			continue;
		}

		D3DXVECTOR3 Mouse = GetMouse();
		if (s_Enemy[i].bRange)
		{
			pEnemy->scale = D3DXVECTOR3(2.0f, 2.0f, 2.0f);

			s_Enemy[i].pos = s_Enemy[i].posOld + Mouse;

		}
		else
		{
			s_Enemy[i].posOld = s_Enemy[i].pos - Mouse;
			pEnemy->scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			//s_Move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}


		if (GetKeyboardTrigger(DIK_V))
		{//V�����Əꏊ���ς��
			s_Enemy[i].bRange = false;
			s_Enemy[i].fLog = s_fLog;
		}


		bool Hit = CollisionRange(s_Enemy[i].pos);
		if (Hit)
		{
			if (GetKeyboardTrigger(DIK_V))
			{
				s_Enemy[i].bRange = true;

			}
		}

		// ���݂̃��[�V�����ԍ��̕ۊ�
		pEnemy->motionTypeOld = pEnemy->motionType;

		if (!pEnemy->bMotion)
		{// �g�p���Ă郂�[�V�������Ȃ��ꍇ
			pEnemy->motionType = CPlayer::ANIME_NORMAL;
		}

		// �}�b�v�̃X�N���[��
		if (!GetKeyboardPress(DIK_LCONTROL) && !GetKeyboardPress(DIK_LSHIFT))
		{
			int wheel = GetMouseWheel();
			if (wheel > 0)
			{
				pEnemy->pos += s_Move * MAPMOVE / s_fMapScale;				
			}
			else if (wheel < 0)
			{
				pEnemy->pos -= s_Move * MAPMOVE / s_fMapScale;
			}
		}

		//Collision();	// ��

		// �A�j���[�V�����⑫���̐ݒ�
		if (!pEnemy->notLoop)
		{
			pEnemy->motionType = CPlayer::ANIME_NORMAL;
		}

		if (s_pow >= 1 && s_pow <= 10)
		{// �W�����v�V�X�e��
			s_pow++;
			pEnemy->move.y = 1.00f * s_pow;
		}

		pEnemy->move.y -= 1.0f;
		if (GetKeyboardPress(DIK_B))
		{
			pEnemy->motionType = CPlayer::ANIME_ATTACK;//�U��

			pEnemy->notLoop = true;
		}
		if (GetKeyboardPress(DIK_N))
		{
			pEnemy->notLoop = true;
		}

		if (pEnemy->motionTypeOld != pEnemy->motionType)
		{// ���[�V�����^�C�v���ύX���ꂽ��
			pEnemy->motion[pEnemy->motionTypeOld].nCntFrame = 0;
			pEnemy->motion[pEnemy->motionTypeOld].nCntKeySet = 0;
			pEnemy->bMotionBlend = true;
		}

		if (pEnemy->bMotionBlend)
		{// ���[�V�����u�����h���g�p���Ă�
			pEnemy->bMotionBlend = MotionBlend((int)(pEnemy->motionType),	// ���[�V�����̔z��ԍ�
				&pEnemy->parts[0],											// �p�[�c���
				pEnemy->nMaxModelParts,										// �p�[�c��
				&pEnemy->motion[0]);										// ���[�V�������	
		}
		else if (!pEnemy->bMotionBlend)
		{// ���[�V�����u�����h���g�p���ĂȂ��ꍇ
			pEnemy->bMotion = PlayMotion(pEnemy->nMaxModelParts,			// �p�[�c��
				&pEnemy->parts[0],											// �p�[�c���
				&pEnemy->motion[(int)(pEnemy->motionType)]);				// ���[�V�������
		}
	}
	//�ړ��L�^��ۑ�
	if (!GetKeyboardPress(DIK_LCONTROL) && !GetKeyboardPress(DIK_LSHIFT))
	{
		int wheel = GetMouseWheel();
		if (wheel > 0)
		{	
			s_fLog += s_Move.x * MAPMOVE / s_fMapScale;

		}
		else if (wheel < 0)
		{
			s_fLog -= s_Move.x * MAPMOVE / s_fMapScale;
		}
	}
	
}

//=========================================
// �`�揈��
//=========================================
void DrawEnemy(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScale, mtxTrans, mtxRot;	// �v�Z�p�}�g���b�N�X
	D3DMATERIAL9 marDef;
	D3DXMATERIAL *pMat = {};


	for (int i = 0; i < MAX_ENEMY; i++)
	{
		Enemy* pEnemy = &s_Enemy[i];

		D3DXVECTOR3 scale(pEnemy->scale.x, pEnemy->scale.y, pEnemy->scale.z);

		if (!pEnemy->isUse)//�g���Ă��o��
		{
			continue;
		}

		// ���[���h�}�g���b�N�X�̏�����
		// �s�񏉊����֐�(��1�����̍s���P�ʍs��ɏ�����)
		D3DXMatrixIdentity(&pEnemy->mtxWorld);

		// �g�k�𔽉f
		// �s��g�k�֐�
		D3DXMatrixScaling(&mtxScale, scale.x, scale.y, scale.z);
		// �s��|���Z�֐�(��2�����~��3��������P�����Ɋi�[)
		D3DXMatrixMultiply(&pEnemy->mtxWorld, &pEnemy->mtxWorld, &mtxScale);

		// �����𔽉f
		// �s���]�֐�(��1�����Ƀ��[(y)�s�b�`(x)���[��(z)�����̉�]�s����쐬)
		D3DXMatrixRotationYawPitchRoll(&mtxRot, pEnemy->rot.y, pEnemy->rot.x, pEnemy->rot.z);
		// �s��|���Z�֐�(��2�����~��3��������P�����Ɋi�[)
		D3DXMatrixMultiply(&pEnemy->mtxWorld, &pEnemy->mtxWorld, &mtxRot);

		// �ʒu�𔽉f
		// �s��ړ��֐�(��P������X,Y,Z�����̈ړ��s����쐬)
		D3DXMatrixTranslation(&mtxTrans, pEnemy->pos.x, pEnemy->pos.y, pEnemy->pos.z);
		// �s��|���Z�֐�(��2�����~��3��������P�����Ɋi�[)
		D3DXMatrixMultiply(&pEnemy->mtxWorld, &pEnemy->mtxWorld, &mtxTrans);

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, NULL);
		// �p�[�c�̕`��ݒ�
		SetParts(pEnemy->nMaxModelParts,	// �p�[�c��
			&pEnemy->parts[0],				// �p�[�c���
			pEnemy->mtxWorld,				// ���[���h�}�g���b�N�X
			mtxRot,							// �v�Z�p�}�g���b�N�X
			mtxTrans,						// �v�Z�p�}�g���b�N�X
			&marDef,						// �}�e���A���ۑ��ϐ�
			pMat);							// �}�e���A���f�[�^

		//���݂̃}�e���A�������ɖ߂�
		GetDevice()->SetMaterial(&marDef);
	}
}

//=========================================
// �ݒ�
//=========================================
void SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, ENEMY_TYPE type)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		Enemy* pEnemy = &s_Enemy[i];

		if (pEnemy->isUse)
		{
			continue;
		}

		LoadEnemy();

		*pEnemy = s_EnemyType[type];
		// �v���C���[���̏�����
		pEnemy->pos = pos;									// �ʒu�̏�����
		pEnemy->posOld = pEnemy->pos;						// �ߋ��ʒu�̏�����
		pEnemy->rot = rot;									// �����̏�����
		pEnemy->fLog = s_fLog;
		pEnemy->mtxWorld = {};								// ���[���h�}�g���b�N�X
		pEnemy->motionType = CPlayer::ANIME_NORMAL;					// �j���[�g�������[�V����
		pEnemy->motionTypeOld = pEnemy->motionType;			// �O��̃��[�V����
		pEnemy->move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ړ���
		pEnemy->bMotionBlend = false;						// ���[�V�����u�����h�̎g�p��
		pEnemy->isUse = true;								// �g�p��

		break;
	}
}

//--------------------------
//�����蔻��̃T�C�Y�����Ă�
//--------------------------
void SizeSet(void)
{

}

//------------------------------
//�A�j���[�V�����Z�b�g
//-------------------------------
void AnimationSet(int animation)
{

}

//------------------------------
//�����Z�b�g
//-------------------------------
void MoveSet(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		Enemy* pEnemy = &s_Enemy[i];

		if (!pEnemy->isUse)
		{
			continue;
		}
	}
}

//-------------------------------
//�����蔻��܂Ƃ�
//-------------------------------
void Collision(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		Enemy* pEnemy = &s_Enemy[i];

		if (!pEnemy->isUse)
		{
			continue;
		}

		if (pEnemy->pos.y <= 0.0f)
		{
			pEnemy->pos.y = 0.0f;
		}
	}
}

//-------------------------------
//���[�V���������[�h���鏈��
//-------------------------------
void  Loadmotion(CPlayer::MODELDATAPLAYER* set, int Setnumber)
{
	s_ModelData[s_nMotion] = *set;
	s_nMotion++;
}

//----------------------
// �Q�b�g(�\����)
//----------------------
Enemy *GetEnemy(void)
{
	return s_Enemy;
}

//----------------------
// �Ǎ�
//----------------------
void LoadSetFile(char *Filename)
{
	FILE *pFile = fopen(Filename, "r");
	if (pFile == NULL)
	{//�t�@�C�����J�����ꍇ
		assert(false);
		return;
	}
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ړ���
	D3DXVECTOR3 size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �T�C�Y
	D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���b�g
	char aString[128] = {};			// �������r�p�̕ϐ�
	char aEqual[128] = {};		// ���ǂݍ��ݗp�ϐ�
	int nType = 0;
	fscanf(pFile, "%s", &aString);

	while (strncmp(&aString[0], "SCRIPT", 6) != 0)
	{// �����񂪈�v�����ꍇ
		aString[0] = {};
		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aString[0]);
	}
	while (strncmp(&aString[0], "END_SCRIPT", 10) != 0)
	{
		fscanf(pFile, "%s", &aString[0]);

		if (strncmp(&aString[0], "#", 1) == 0)
		{// ���ǂݍ��ރR�����g
			fgets(&aString[0], sizeof(aString), pFile);
		}
		if (strcmp(&aString[0], "SET_ENEMY") == 0)
		{
			while (1)
			{
				fscanf(pFile, "%s", &aString[0]);

				if (strncmp(&aString[0], "#", 1) == 0)
				{// ���ǂݍ��ރR�����g
					fgets(&aString[0], sizeof(aString), pFile);
				}
				if (strcmp(&aString[0], "POS") == 0)
				{// POS�̓ǂݍ���
					fscanf(pFile, "%s", &aEqual[0]);

					fscanf(pFile, "%f", &pos.x);
					fscanf(pFile, "%f", &pos.y);
					fscanf(pFile, "%f", &pos.z);
				}
				if (strcmp(&aString[0], "SIZE") == 0)
				{// SIZE�̓ǂݍ���
					fscanf(pFile, "%s", &aEqual[0]);

					fscanf(pFile, "%f", &size.x);
					fscanf(pFile, "%f", &size.y);
					fscanf(pFile, "%f", &size.z);
				}
				if (strcmp(&aString[0], "ROT") == 0)
				{// ROT�̓ǂݍ���
					fscanf(pFile, "%s", &aEqual[0]);

					fscanf(pFile, "%f", &rot.x);
					fscanf(pFile, "%f", &rot.y);
					fscanf(pFile, "%f", &rot.z);
				}
				if (strcmp(&aString[0], "TYPE") == 0)
				{
					fscanf(pFile, "%s", &aEqual[0]);

					fscanf(pFile, "%d", &nType);
				}
				if (strcmp(&aString[0], "END_SET") == 0)
				{
					SetEnemy(pos, rot, (ENEMY_TYPE)nType);
					break;
				}
			}
		}
	}
}

//----------------------
// �Ǎ�
//----------------------
void LoadEnemy(void)
{
	FILE *pFile = fopen("Data/system/enemy/enemy.txt", "r");

	if (pFile == NULL)
	{//�t�@�C�����J�����ꍇ
		assert(false);
		return;
	}

	char aString[128] = {};			// �������r�p�̕ϐ�
	char aEqual[128] = {};		// ���ǂݍ��ݗp�ϐ�
	char fileName[ENEMY_TYPE_MAX][128];
	int fileCnt = 0;

	fscanf(pFile, "%s", &aString);

	while (strncmp(&aString[0], "SCRIPT", 6) != 0)
	{// �����񂪈�v�����ꍇ
		aString[0] = {};
		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aString[0]);
	}
	while (strncmp(&aString[0], "END_SCRIPT", 10) != 0)
	{
		fscanf(pFile, "%s", &aString[0]);

		if (strncmp(&aString[0], "#", 1) == 0)
		{// ���ǂݍ���
			fgets(&aString[0], sizeof(aString), pFile);
		}

		if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
		{// �t�@�C�����̓ǂݍ���
			fscanf(pFile, "%s", &aEqual[0]);
			fscanf(pFile, "%s", fileName[fileCnt]);

			Enemy* pEnemy = &s_EnemyType[fileCnt];
			if (pEnemy->isUse)
			{
				continue;
			}

			// �v���C���[���̏�����
			pEnemy->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu�̏�����
			pEnemy->posOld = pEnemy->pos;					// �ߋ��ʒu�̏�����
			pEnemy->fLog = s_fLog;
			pEnemy->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);;	// �����̏�����
			pEnemy->modelMin = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);		// ���_���W�̍ŏ��l
			pEnemy->modelMax = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);	// ���_���W�̍ő�l
			pEnemy->mtxWorld = {};								// ���[���h�}�g���b�N�X
			pEnemy->motionType = CPlayer::ANIME_NORMAL;					// �j���[�g�������[�V����
			pEnemy->motionTypeOld = pEnemy->motionType;			// �j���[�g�������[�V����
			pEnemy->move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ړ���
			pEnemy->bMotionBlend = false;						// ���[�V�����u�����h�̎g�p��
			pEnemy->isUse = true;								// �g�p��
			pEnemy->bMotionBlend = false;						// �v���C���[���f�B�X�N�������Ă��Ȃ�

			// �t�@�C���̓ǂݍ���
			LoodSetMotion(fileName[fileCnt], pEnemy->partsFile, pEnemy->parts, pEnemy->motion, &pEnemy->nMaxModelParts);

			for (int i = 0; i < pEnemy->nMaxModelParts; i++)
			{
				Parts* pParts = &pEnemy->parts[i];

				// �ʒu�ƌ����̏����l��ۑ�
				pParts->posOrigin = pParts->pos;
				pParts->rotOrigin = pParts->rot;

				// �p�[�c���̏�����
				pParts->mtxWorld = {};	// ���[���h�}�g���b�N�X
				pParts->vtxMin = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);	// ���_���W�̍ŏ��l
				pParts->vtxMax = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);	// ���_���W�̍ő�l

				// X�t�@�C���̓ǂݍ���
				D3DXLoadMeshFromX(pEnemy->partsFile[pParts->nType].aName,
					D3DXMESH_SYSTEMMEM,
					GetDevice(),
					NULL,
					&pParts->pBuffer,
					NULL,
					&pParts->nNumMat,
					&pParts->pMesh);

				// ���_���W�̍ŏ��l�E�ő�l�̎Z�o
				int nNumVtx;	// ���_��
				DWORD sizeFVF;	// ���_�t�H�[�}�b�g�̃T�C�Y
				BYTE *pVtxBuff;	// ���_�o�b�t�@�ւ̃|�C���^

				// ���_���̎擾
				nNumVtx = pParts->pMesh->GetNumVertices();

				// ���_�t�H�[�}�b�g�̃T�C�Y�̎擾
				sizeFVF = D3DXGetFVFVertexSize(pParts->pMesh->GetFVF());

				// ���_�o�b�t�@�̃��b�N
				pParts->pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

				for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
				{
					// ���_���W�̑��
					D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

					if (vtx.x < pParts->vtxMin.x)
					{// ��r�Ώۂ����݂̒��_���W(X)�̍ŏ��l��菬����
						pParts->vtxMin.x = vtx.x;
					}
					if (vtx.y < pParts->vtxMin.y)
					{// ��r�Ώۂ����݂̒��_���W(Y)�̍ŏ��l��菬����
						pParts->vtxMin.y = vtx.y;
					}
					if (vtx.z < pParts->vtxMin.z)
					{// ��r�Ώۂ����݂̒��_���W(Z)�̍ŏ��l��菬����
						pParts->vtxMin.z = vtx.z;
					}

					if (vtx.x > pParts->vtxMax.x)
					{// ��r�Ώۂ����݂̒��_���W(X)�̍ő�l���傫��
						pParts->vtxMax.x = vtx.x;
					}
					if (vtx.y > pParts->vtxMax.y)
					{// ��r�Ώۂ����݂̒��_���W(Y)�̍ő�l���傫��
						pParts->vtxMax.y = vtx.y;
					}
					if (vtx.z > pParts->vtxMax.z)
					{// ��r�Ώۂ����݂̒��_���W(Z)�̍ő�l���傫��
						pParts->vtxMax.z = vtx.z;
					}

					// ���_�t�H�[�}�b�g�̃T�C�Y���|�C���^��i�߂�
					pVtxBuff += sizeFVF;
				}

				if (pParts->vtxMin.x < pEnemy->modelMin.x)
				{// ��r�Ώۂ����݂̒��_���W(X)�̍ŏ��l��菬����
					pEnemy->modelMin.x = pParts->vtxMin.x;
				}
				if (pParts->vtxMin.y < pEnemy->modelMin.y)
				{// ��r�Ώۂ����݂̒��_���W(Y)�̍ŏ��l��菬����
					pEnemy->modelMin.y = pParts->vtxMin.y;
				}
				if (pParts->vtxMin.z < pEnemy->modelMin.z)
				{// ��r�Ώۂ����݂̒��_���W(Z)�̍ŏ��l��菬����
					pEnemy->modelMin.z = pParts->vtxMin.z;
				}

				if (pParts->vtxMax.x > pEnemy->modelMax.x)
				{// ��r�Ώۂ����݂̒��_���W(X)�̍ő�l���傫��
					pEnemy->modelMax.x = pParts->vtxMax.x;
				}
				if (pParts->vtxMax.y > pEnemy->modelMax.y)
				{// ��r�Ώۂ����݂̒��_���W(Y)�̍ő�l���傫��
					pEnemy->modelMax.y = pParts->vtxMax.y;
				}
				if (pParts->vtxMax.z > pEnemy->modelMax.z)
				{// ��r�Ώۂ����݂̒��_���W(Z)�̍ő�l���傫��
					pEnemy->modelMax.z = pParts->vtxMax.z;
				}

				// ���_�o�b�t�@�̃A�����b�N
				pParts->pMesh->UnlockVertexBuffer();
			}
			fileCnt++;
		}
	}
}
//----------------------------
//�t�@�C���̓��̓}�b�v���
//----------------------------
void OutputEnemy(char *Filename)
{

	D3DXVECTOR3 pos;
	//�t�@�C���J��
	FILE *pFile = fopen(Filename, "w");
	if (pFile == NULL)
	{
		assert(false);
	}
	fprintf(pFile, "#-----------------------\n");
	fprintf(pFile, "#�G�l�~�[�̐ݒ�X�N���v�g\n");
	fprintf(pFile, "#Author:hamada ryuuga\n");
	fprintf(pFile, "#-----------------------\n");

	fprintf(pFile, "SCRIPT #���������Ȃ�\n\n");

	fprintf(pFile, "\n# ��������G�̍��W�Ȃǂ̐ݒ�\n\n");

	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{
		
		if (s_Enemy[nCntEnemy].isUse)
		{
			//s_Enemy[nCntEnemy].pos.y += Log;
			fprintf(pFile, "SET_ENEMY\n");
			fprintf(pFile, "TYPE = %d\n", s_Enemy[nCntEnemy].type);
			fprintf(pFile, "POS = %.4f %.4f %.4f\n", s_Enemy[nCntEnemy].pos.x - s_fLog, s_Enemy[nCntEnemy].pos.y, s_Enemy[nCntEnemy].pos.z);
			fprintf(pFile, "SIZE = %.4f %.4f %.4f\n", s_Enemy[nCntEnemy].scale.x, s_Enemy[nCntEnemy].scale.y, s_Enemy[nCntEnemy].scale.z);
			fprintf(pFile, "ROT = %.4f %.4f %.4f\n", s_Enemy[nCntEnemy].rot.x, s_Enemy[nCntEnemy].rot.y, s_Enemy[nCntEnemy].rot.z);
			fprintf(pFile, "END_SET\n");
			fprintf(pFile, "\n");
		}
	}

	fprintf(pFile, "END_SCRIPT #���������Ȃ�");
	fclose(pFile);
}
//----------------------------
//�G�l�~-�̃T�C�Y�ƍ��W�ƃ^�C�v��ύX����
//----------------------------
void SelectDes(D3DXVECTOR3 pos)
{
	Camera *pCamera = GetCamera();
	pos = WorldCastScreen(&pos,								// �X�N���[�����W
		D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f),			// �X�N���[���T�C�Y
		&pCamera->mtxView,										// �r���[�}�g���b�N�X
		&pCamera->mtxProjection);								// �v���W�F�N�V�����}�g���b�N�X

	D3DXVECTOR3 enemyPos;
	for (int nCntEnemy = 0; nCntEnemy < MAX_ENEMY; nCntEnemy++)
	{
		if (s_Enemy[nCntEnemy].isUse)
		{
			if (s_Enemy[nCntEnemy].bSelect)
			{
				s_Enemy[nCntEnemy].pos = D3DXVECTOR3(pos.x, pos.y, 0.0f);
			}
			
			D3DXVECTOR3 Size = s_Enemy[nCntEnemy].modelMax - s_Enemy[nCntEnemy].modelMin;

			if (((s_Enemy[nCntEnemy].pos.x - Size.x* s_fMapScale < pos.x) && (s_Enemy[nCntEnemy].pos.x + Size.x* s_fMapScale > pos.x)) &&
				((s_Enemy[nCntEnemy].pos.y - Size.y*2* s_fMapScale < pos.y) && (s_Enemy[nCntEnemy].pos.y + Size.y * 2 * s_fMapScale > pos.y)))
			{
				//�}�E�X�J�[�\�����킳���Ă���T�C�Y�ύX��������
				s_Enemy[nCntEnemy].scale = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
				if (GetKeyboardPress(DIK_LCONTROL))
				{//CONTROL�����Ȃ���
					
				}
				if (GetKeyboardTrigger(DIK_Z))
				{
					s_Enemy[nCntEnemy].bSelect = !s_Enemy[nCntEnemy].bSelect;
				}
				if (GetKeyboardTrigger(DIK_D))
				{
					s_Enemy[nCntEnemy].isUse = false;
					s_Enemy[nCntEnemy].pos = D3DXVECTOR3(NULL, NULL, NULL);
				}
			}
			else
			{
				s_Enemy[nCntEnemy].scale = D3DXVECTOR3(1.8f, 1.8f, 1.8f);
			}

		}
	}
}