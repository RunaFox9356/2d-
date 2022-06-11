//============================
//
// �v���C���[�ݒ�
// Author:hamada ryuuga
//
//============================

#include <stdio.h>
#include "Player.h"
#include "input.h"
#include "camera.h"
#include "motion.h"

//------------------------------------
// static�ϐ�
//------------------------------------
const float CPlayer::ATTENUATION = 0.5f;	// 
const float CPlayer::SPEED = 1.0f;			// �ړ���
const float CPlayer::WIDTH = 10.0f;			// ���f���̔��a
const int CPlayer::MAX_PRAYER = 16;			// �ő吔
const int CPlayer::MAX_MOVE = 9;			// �A�j���[�V�����̍ő吔
const int CPlayer::INVINCIBLE = 300;		// ���G����
const int CPlayer::MAX_COPY = 4;			// �ő�R�s�[��

//------------------------------------
// �R���X�g���N�^
//------------------------------------
CPlayer::CPlayer() : 
	m_pos(D3DXVECTOR3(0.0f,0.0f,0.0f)),
	m_posOld(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_move(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_rot(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_rotMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_modelMin(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_modelMax(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	//m_mtxWorld(),
	m_status(STATUS_NORMAL),
	m_damege(DAMEGE_NORMAL),
	m_copy(COPY_NORMAL),
	//m_parts(),
	//m_partsFile(),
	//m_motion(),
	m_motionType(ANIME_NORMAL),
	m_motionTypeOld(ANIME_NORMAL),
	m_nMaxModelType(0),
	m_nMaxModelParts(0),
	m_nMaxMotion(0),
	m_type(0),
	m_shadow(0),
	m_invincible(0),
	m_consumption(0.0f),
	m_bMotionBlend(false),
	m_bMotion(false),
	m_isUse(false),
	m_isLoop(false),
	//m_aFirename(),
	m_time(0),
	m_nparts(0),
	m_pow(0),
	m_nMotion(0)
{
	//memset(&s_Player, NULL, sizeof(s_Player));

}

//------------------------------------
// �f�X�g���N�^
//------------------------------------
CPlayer::~CPlayer()
{

}

//------------------------------------
// ������
//------------------------------------
void CPlayer::Init(void)
{
	//�J�����̃f�[�^�擾
	Camera *pCamera = GetCamera();
	m_rotMove = D3DXVECTOR3(D3DX_PI + pCamera->rot.y, D3DX_PI * 0.5f + pCamera->rot.y, 0.0f);

	m_modelMin = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_modelMax = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	m_motionTypeOld = m_motionType;

	Set(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

//------------------------------------
// �I��
//------------------------------------
void CPlayer::Uninit(void)
{
	for (int i = 0; i < MAX_MODELPARTS; i++)
	{
		// ���_�o�b�t�@�[�̉��
		if (m_parts[i].pBuffer != NULL)
		{
			m_parts[i].pBuffer->Release();
			m_parts[i].pBuffer = NULL;
		}
		if (m_parts[i].pMesh != NULL)
		{
			m_parts[i].pMesh->Release();
			m_parts[i].pMesh = NULL;
		}
	}
}

//------------------------------------
// �X�V
//------------------------------------
void CPlayer::Update(void)
{
	// ���݂̃��[�V�����ԍ��̕ۊ�
	m_motionTypeOld = m_motionType;

	if (!m_bMotion)
	{// �g�p���Ă郂�[�V�������Ȃ��ꍇ
		m_motionType = ANIME_NORMAL;
		m_isLoop = false;
	}

	Move();	//�����Z�b�g

	Collision();//��

	// //�A�j���[�V�����⑫���̐ݒ�
	if (!m_isLoop)
	{
		m_motionType = ANIME_NORMAL;
	}
	if (GetKeyboardPress(DIK_A) || GetKeyboardPress(DIK_W) || GetKeyboardPress(DIK_D) || GetKeyboardPress(DIK_S) ||
		GetJoypadPress(JOYKEY_UP, 0) || GetJoypadPress(JOYKEY_DOWN, 0) || GetJoypadPress(JOYKEY_LEFT, 0) || GetJoypadPress(JOYKEY_RIGHT, 0))
	{
		m_motionType = ANIME_RUN;//����

	}

	if (GetKeyboardPress(DIK_SPACE) || GetJoypadPress(JOYKEY_A, 0) || GetJoypadPress(JOYKEY_UP, 0))
	{//SPACE�L�[�������ꂽ
		m_pos.y = m_pos.y + 5.0f;
		m_pow++;
		m_motionType = ANIME_JUMP;
	}
	else
	{
		m_pow = 0;
	}
	if (m_pow >= 1 && m_pow <= 10)
	{//�W�����v�V�X�e��
		m_pow++;
		m_move.y = 1.00f* m_pow;
	}


	m_move.y -= 1.0f;
	if (GetKeyboardPress(DIK_B))
	{
		//cLnhale[0].Set(m_pos);
		m_motionType = ANIME_ATTACK;//�U��

		m_bMotion = true;
	}
	if (GetKeyboardPress(DIK_N))
	{
		m_isLoop = true;
	}
	if (GetKeyboardPress(DIK_J))
	{

		m_copy = COPY_SWORD;
		// �t�@�C���̓ǂݍ���
		SetCopy("Data/system/Gon/sword.txt", &m_partsFile[7], &m_parts[7], &m_motion[0], &m_nMaxModelParts);
	}
	if (GetKeyboardPress(DIK_H))
	{
		m_copy = COPY_FIRE;
		// �t�@�C���̓ǂݍ���
		SetCopy("Data/system/Gon/flare.txt", &m_partsFile[7], &m_parts[7], &m_motion[0], &m_nMaxModelParts);
	}
	if (GetKeyboardPress(DIK_F))
	{
		m_copy = COPY_LASER;
		// �t�@�C���̓ǂݍ���	
		SetCopy("Data/system/Gon/Laser.txt", &m_partsFile[7], &m_parts[7], &m_motion[0], &m_nMaxModelParts);
	}
	if (GetKeyboardPress(DIK_G))
	{
		m_copy = COPY_CUTTER;
		// �t�@�C���̓ǂݍ���
		SetCopy("Data/system/Gon/Cutter.txt", &m_partsFile[7], &m_parts[7], &m_motion[0], &m_nMaxModelParts);
	}
	if (GetKeyboardPress(DIK_K))
	{
		m_copy = COPY_NORMAL;
		m_nMaxModelParts = 7;
	}

	if (m_motionTypeOld != m_motionType)
	{// ���[�V�����^�C�v���ύX���ꂽ��
		m_motion[m_motionTypeOld].nCntFrame = 0;
		m_motion[m_motionTypeOld].nCntKeySet = 0;
		m_bMotionBlend = true;
	}

	if (m_bMotionBlend)
	{// ���[�V�����u�����h���g�p���Ă�
		m_bMotionBlend = MotionBlend((int)(m_motionType),		// ���[�V�����̔z��ԍ�
			&m_parts[0],												// �p�[�c���
			m_nMaxModelParts,										// �p�[�c��
			&m_motion[0]);											// ���[�V�������	
	}
	else if (!m_bMotionBlend)
	{// ���[�V�����u�����h���g�p���ĂȂ��ꍇ
		m_bMotion = PlayMotion(m_nMaxModelParts,				// �p�[�c��
			&m_parts[0],												// �p�[�c���
			&m_motion[(int)(m_motionType)]);					// ���[�V�������
	}

}

//------------------------------------
// �`��
//------------------------------------
void CPlayer::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScale, mtxTrans, mtxRot;	// �v�Z�p�}�g���b�N�X
	D3DMATERIAL9 marDef;
	D3DXMATERIAL *pMat = {};
	D3DXVECTOR3 scale(1.8f, 1.8f, 1.8f);

	if (m_isUse)//�g���Ă��o��
	{
		// ���[���h�}�g���b�N�X�̏�����
		// �s�񏉊����֐�(��1�����̍s���P�ʍs��ɏ�����)
		D3DXMatrixIdentity(&m_mtxWorld);

		// �g�k�𔽉f
		// �s��g�k�֐�
		D3DXMatrixScaling(&mtxScale, scale.x, scale.y, scale.z);
		// �s��|���Z�֐�(��2�����~��3��������P�����Ɋi�[)
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

		// �����𔽉f
		// �s���]�֐�(��1�����Ƀ��[(y)�s�b�`(x)���[��(z)�����̉�]�s����쐬)
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
		// �s��|���Z�֐�(��2�����~��3��������P�����Ɋi�[)
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

		// �ʒu�𔽉f
		// �s��ړ��֐�(��P������X,Y,Z�����̈ړ��s����쐬)
		D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
		// �s��|���Z�֐�(��2�����~��3��������P�����Ɋi�[)
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, NULL);

		// �p�[�c�̕`��ݒ�
		SetParts(m_nMaxModelParts,	// �p�[�c��
			&m_parts[0],			// �p�[�c���
			m_mtxWorld,				// ���[���h�}�g���b�N�X
			mtxRot,					// �v�Z�p�}�g���b�N�X
			mtxTrans,				// �v�Z�p�}�g���b�N�X
			&marDef,				// �}�e���A���ۑ��ϐ�
			pMat);					// �}�e���A���f�[�^

		// �s��|���Z�֐�(��2�����~��3��������P�����Ɋi�[)
		pDevice->SetMaterial(&marDef);
	}
}

//------------------------------------
// �ݒ�
//------------------------------------
void CPlayer::Set(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// �v���C���[���̏�����
	m_pos = pos;											// �ʒu�̏�����
	m_posOld = m_pos;								// �ߋ��ʒu�̏�����
	m_posOld = m_pos;								// �ߋ��ʒu�̏�����
	m_rot = rot;											// �����̏�����
	m_modelMin = D3DXVECTOR3(100.0f, 100.0f, 100.0f);	// ���_���W�̍ŏ��l
	m_modelMax = D3DXVECTOR3(-100.0f, -100.0f, -100.0f);	// ���_���W�̍ő�l
	m_mtxWorld = {};										// ���[���h�}�g���b�N�X
															//m_rotDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// �ړI�̌���
	m_motionType = ANIME_NORMAL;							// �j���[�g�������[�V����
	m_motionTypeOld = m_motionType;				// �j���[�g�������[�V����
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ړ���
	m_bMotionBlend = false;								// ���[�V�����u�����h�̎g�p��
	m_isUse = true;										// �g�p��

	// �t�@�C���̓ǂݍ���
	LoodSetMotion("Data/system/Gon/Fox.txt", m_partsFile, m_parts, m_motion, &m_nMaxModelParts);

	for (int i = 0; i < m_nMaxModelParts; i++)
	{
		Parts* pParts = &m_parts[i];

		// �ʒu�ƌ����̏����l��ۑ�
		pParts->posOrigin = pParts->pos;
		pParts->rotOrigin = pParts->rot;

		// �p�[�c���̏�����
		pParts->mtxWorld = {};										// ���[���h�}�g���b�N�X
		pParts->vtxMin = D3DXVECTOR3(1000.0f, 1000.0f, 1000.0f);		// ���_���W�̍ŏ��l
		pParts->vtxMax = D3DXVECTOR3(-1000.0f, -1000.0f, -1000.0f);	// ���_���W�̍ő�l

		// X�t�@�C���̓ǂݍ���
		D3DXLoadMeshFromX(m_partsFile[pParts->nType].aName,
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

		if (pParts->vtxMin.x < m_modelMin.x)
		{// ��r�Ώۂ����݂̒��_���W(X)�̍ŏ��l��菬����
			m_modelMin.x = pParts->vtxMin.x;
		}
		if (pParts->vtxMin.y < m_modelMin.y)
		{// ��r�Ώۂ����݂̒��_���W(Y)�̍ŏ��l��菬����
			m_modelMin.y = pParts->vtxMin.y;
		}
		if (pParts->vtxMin.z < m_modelMin.z)
		{// ��r�Ώۂ����݂̒��_���W(Z)�̍ŏ��l��菬����
			m_modelMin.z = pParts->vtxMin.z;
		}

		if (pParts->vtxMax.x > m_modelMax.x)
		{// ��r�Ώۂ����݂̒��_���W(X)�̍ő�l���傫��
			m_modelMax.x = pParts->vtxMax.x;
		}
		if (pParts->vtxMax.y > m_modelMax.y)
		{// ��r�Ώۂ����݂̒��_���W(Y)�̍ő�l���傫��
			m_modelMax.y = pParts->vtxMax.y;
		}
		if (pParts->vtxMax.z > m_modelMax.z)
		{// ��r�Ώۂ����݂̒��_���W(Z)�̍ő�l���傫��
			m_modelMax.z = pParts->vtxMax.z;
		}

		// ���_�o�b�t�@�̃A�����b�N
		pParts->pMesh->UnlockVertexBuffer();
	}
}

//------------------------------------
// �ړ�����
//------------------------------------
void CPlayer::Move(void)
{
	// �ړ��W���̐錾
	int nDash = 1;
	if (GetKeyboardPress(DIK_LSHIFT) || GetKeyboardPress(DIK_RSHIFT) || GetJoypadPress(JOYKEY_A, 0))
	{//�V�t�g�_�b�V��
		nDash = 3;
	}

	//�Q�[���̎��̈ړ����@
	Camera *pCamera = GetCamera();
	m_consumption = 0.0f;

	if (GetJoypadStick(JOYKEY_LEFT_STICK, 0).x > 0.5f)
	{//�E�ɔ�����葽���|������
		m_move.x += sinf(pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - m_rot.y + pCamera->rot.y;
	}
	else if (GetJoypadStick(JOYKEY_LEFT_STICK, 0).x < -0.5f)
	{//���ɔ�����葽���|������
		m_move.x += sinf(pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - m_rot.y + pCamera->rot.y;
	}
	else if (GetJoypadStick(JOYKEY_LEFT_STICK, 0).y > 0.5f)
	{//��ɔ�����葽���|������
		m_move.x += sinf(pCamera->rot.y - D3DX_PI) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y - D3DX_PI) * SPEED * nDash;

		m_consumption = (m_rotMove.x - D3DX_PI - m_rot.y + pCamera->rot.y);
	}
	else if (GetJoypadStick(JOYKEY_LEFT_STICK, 0).y < -0.5f)
	{//���ɔ�����葽���|������
		m_move.x += sinf(pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - m_rot.y + pCamera->rot.y;
	}
	if (GetKeyboardPress(DIK_W) || GetJoypadPress(JOYKEY_UP, 0))
	{//W�������Ƃ������

		m_move.x += sinf(pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - m_rot.y + pCamera->rot.y;
	}
	if (GetKeyboardPress(DIK_S) || GetJoypadPress(JOYKEY_DOWN, 0))
	{//S�������Ɖ�����

		m_move.x += sinf(pCamera->rot.y - D3DX_PI) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y - D3DX_PI) * SPEED * nDash;

		m_consumption = (m_rotMove.x - D3DX_PI - m_rot.y + pCamera->rot.y);
	}
	if (GetKeyboardPress(DIK_D) || GetJoypadPress(JOYKEY_RIGHT, 0))
	{//D�������Ƃ��E����

		m_move.x += sinf(D3DX_PI * 0.5f + pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(D3DX_PI * 0.5f + pCamera->rot.y) * SPEED * nDash;


		m_consumption = m_rotMove.x + (D3DX_PI*0.5f) - m_rot.y + pCamera->rot.y;
	}

	if (GetKeyboardPress(DIK_A) || GetJoypadPress(JOYKEY_LEFT, 0))
	{//A�������ƍ�����

		m_move.x += sinf(-D3DX_PI * 0.5f + pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(-D3DX_PI * 0.5f + pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - (D3DX_PI*0.5f) - m_rot.y + pCamera->rot.y;

	}
	m_posOld = m_pos;//�ߋ��̈ړ��ʂ�ۑ�

	if (m_invincible <= 0)
	{//���G���Ԃ��[���ɂȂ�����_���[�W���炤�悤�ɂ���
		m_damege = DAMEGE_NORMAL;
	}

	//�i�ړI�̒l-���݂̒l�j�������W��
	m_move.x += (0.0f - m_move.x) * 0.5f;
	m_move.z += (0.0f - m_move.z) * 0.5f;

	m_pos += m_move;	// �ړ������Z

	// ���K��
	if (m_consumption > D3DX_PI)
	{
		m_consumption += D3DX_PI * 2;
	}
	if (m_consumption < -D3DX_PI)
	{
		m_consumption += -D3DX_PI * 2;
	}

	//���Z�ݒ�i�����j
	m_rot.y += m_consumption * ATTENUATION;	//�ړI�̒l-���݂̒l�j�������W��

	//���K��
	if (m_rot.y > D3DX_PI)
	{
		m_rot.y += -D3DX_PI * 2;
	}
	if (m_rot.y <= -D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2;
	}
}

//------------------------------------
// �����蔻��
//------------------------------------
void CPlayer::Collision(void)
{
	if (m_pos.y <= 0.0f)
	{
		m_pos.y = 0.0f;
	}
}

//------------------------------------
//�R�s�[�����Ƃ����[�V���������[�h���鏈��
//------------------------------------
void CPlayer::SetCopy(char * pFileName, PartsFile * partsFile, Parts * parts, MyMotion * Motion, int * nMaxParts)
{
	// �ϐ��錾
	char aString[128] = {};			// �������r�p�̕ϐ�
	char g_aEqual[128] = {};		// ���ǂݍ��ݗp�ϐ�
	int	nCntKeySet = 0;				// KeySet�J�E���g
	int	nCntKey = 0;				// Key�J�E���g

	//-------------------------------
	//�R�s�[������
	//-------------------------------
	if (m_nMaxModelParts >= 7)
	{
		m_nMaxModelParts = 7;
	}

	// �t�@�C���|�C���^�̐錾
	FILE* pFile = fopen(pFileName, "r");

	if (pFile != NULL)
	{
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
				fscanf(pFile, "%s", &g_aEqual[0]);
				fscanf(pFile, "%s", &partsFile->aName[0]);
			}

			if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{// �L�����N�^�[�̓ǂݍ���
				while (strcmp(&aString[0], "END_CHARACTERSET") != 0)
				{
					fscanf(pFile, "%s", &aString[0]);

					if (strncmp(&aString[0], "#", 1) == 0)
					{// ���ǂݍ���
						fgets(&aString[0], sizeof(aString), pFile);
					}
					if (strcmp(&aString[0], "PARTSSET") == 0)
					{// �p�[�c�̓ǂݍ���
						while (strcmp(&aString[0], "END_PARTSSET") != 0)
						{
							fscanf(pFile, "%s", &aString[0]);

							if (strncmp(&aString[0], "#", 1) == 0)
							{// ���ǂݍ���
								fgets(&aString[0], sizeof(aString), pFile);
							}

							if (strcmp(&aString[0], "INDEX") == 0)
							{// �^�C�v�̓ǂݍ���
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%d", &parts->nType);
							}
							if (strcmp(&aString[0], "PARENT") == 0)
							{// �e�̓ǂݍ���
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%d", &parts->nIdxModelParent);
							}
							if (strcmp(&aString[0], "POS") == 0)
							{// �ʒu�̓ǂݍ���
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%f", &parts->pos.x);
								fscanf(pFile, "%f", &parts->pos.y);
								fscanf(pFile, "%f", &parts->pos.z);
							}
							if (strcmp(&aString[0], "ROT") == 0)
							{// �����̓ǂݍ���
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%f", &parts->rot.x);
								fscanf(pFile, "%f", &parts->rot.y);
								fscanf(pFile, "%f", &parts->rot.z);
							}
						}
					}
				}
			}
			if (strcmp(&aString[0], "MOTIONSET") == 0)
			{// ���[�V�����̓ǂݍ���
				while (strcmp(&aString[0], "END_MOTIONSET") != 0)
				{
					fscanf(pFile, "%s", &aString[0]);

					if (strncmp(&aString[0], "#", 1) == 0)
					{// ���ǂݍ���
						fgets(&aString[0], sizeof(aString), pFile);
					}

					if (strcmp(&aString[0], "LOOP") == 0)
					{// ���[�v�L���̓ǂݍ���
						fscanf(pFile, "%s", &g_aEqual[0]);
						fscanf(pFile, "%d", (int*)(&Motion->bLoop));
					}
					if (strcmp(&aString[0], "NUM_KEY") == 0)
					{// �L�[���̓ǂݍ���
						fscanf(pFile, "%s", &g_aEqual[0]);
						fscanf(pFile, "%d", &Motion->nNumKey);
					}
					if (strcmp(&aString[0], "KEYSET") == 0)
					{// �L�[�Z�b�g�̓ǂݍ���
						while (strcmp(&aString[0], "END_KEYSET") != 0)
						{
							fscanf(pFile, "%s", &aString[0]);

							if (strncmp(&aString[0], "#", 1) == 0)
							{// ���ǂݍ���
								fgets(&aString[0], sizeof(aString), pFile);
							}

							if (strcmp(&aString[0], "FRAME") == 0)
							{// �t���[�����̓ǂݍ���
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%d", &Motion->keySet[nCntKeySet].nFrame);
							}
							if (strcmp(&aString[0], "KEY") == 0)
							{// �L�[�̓ǂݍ���
								while (strcmp(&aString[0], "END_KEY") != 0)
								{
									fscanf(pFile, "%s", &aString[0]);

									if (strncmp(&aString[0], "#", 1) == 0)
									{// ���ǂݍ���
										fgets(&aString[0], sizeof(aString), pFile);
									}

									if (strcmp(&aString[0], "POS") == 0)
									{// �ʒu�̓ǂݍ���
										fscanf(pFile, "%s", &g_aEqual[0]);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].pos.x);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].pos.y);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].pos.z);
									}
									if (strcmp(&aString[0], "ROT") == 0)
									{// �����̓ǂݍ���
										fscanf(pFile, "%s", &g_aEqual[0]);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].rot.x);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].rot.y);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].rot.z);
									}
								}

								// �L�[�J�E���g�̃C���N�������g
								nCntKey++;
							}
						}

						// �L�[�J�E���g�̏�����
						nCntKey = 0;

						// �L�[�Z�b�g�J�E���g�̃C���N�������g
						nCntKeySet++;
					}
				}
				// �L�[�Z�b�g�J�E���g�̏�����
				nCntKeySet = 0;

				// �p�[�c���̃C���N�������g
				Motion++;
			}

		}

		//�t�@�C�������
		fclose(pFile);
	}
	else
	{//�t�@�C�����J���Ȃ��ꍇ
		printf("\n * * * �t�@�C�����J���܂��� * * * \n");
	}

	// �ʒu�ƌ����̏����l��ۑ�
	m_parts[m_nMaxModelParts].posOrigin = m_parts[m_nMaxModelParts].pos;
	m_parts[m_nMaxModelParts].rotOrigin = m_parts[m_nMaxModelParts].rot;

	// X�t�@�C���̓ǂݍ���
	D3DXLoadMeshFromX(&m_partsFile[m_parts[m_nMaxModelParts].nType].aName[0],
		D3DXMESH_SYSTEMMEM,
		GetDevice(),
		NULL,
		&m_parts[m_nMaxModelParts].pBuffer,
		NULL,
		&m_parts[m_nMaxModelParts].nNumMat,
		&m_parts[m_nMaxModelParts].pMesh);

	m_nMaxModelParts++;
}