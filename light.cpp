//=========================================
// 
// ���C�g�̍쐬
// Author YudaKaito
// 
//=========================================
//------------------------------------
// include
//------------------------------------
#include "main.h"
#include "light.h"

//------------------------------------
// �}�N����`
//------------------------------------
#define LIGTH_MAX	(5)

//------------------------------------
// �ÓI�ϐ�
//------------------------------------
static D3DLIGHT9 s_light[LIGTH_MAX];	//���C�g���

//------------------------------------
// �v���g�^�C�v�錾
//------------------------------------
static void SetLight(D3DLIGHTTYPE type, D3DXCOLOR col, D3DXVECTOR3 vec, int nNum);

//=========================================
// ������
//=========================================
void InitLight(void)
{
	// ���C�g���N���A����
	ZeroMemory(s_light, sizeof(s_light));

	// ���C�g�̐ݒ�
	SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), 0);
	SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1);
	SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), 2);

	// �f�o�C�X�ւ̃|�C���^�̎擾
	GetDevice()->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
}

//=========================================
// �I��
//=========================================
void UninitLight(void)
{
}

//=========================================
// �X�V
//=========================================
void UpdateLight(void)
{
}

//=========================================
// �ݒ�
//=========================================
static void SetLight(D3DLIGHTTYPE type,D3DXCOLOR col, D3DXVECTOR3 vec, int nNum)
{
	// �f�o�C�X�ւ̃|�C���^�̎擾
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXVECTOR3 vecDir;		// ���C�g�̕����x�N�g��

	s_light[nNum].Type = D3DLIGHT_DIRECTIONAL;	// ���C�g�̎�ނ�ݒ� ( ���s���� )
	s_light[nNum].Diffuse = col;	// ���C�g�̊g�U����ݒ� ( ���C�g�̐F )

	vecDir = vec;	// ���C�g�̕�����ݒ�

	// ���K������ ( �傫�� 1 �̃x�N�g���ɂ��� )
	D3DXVec3Normalize(&vecDir, &vecDir);
	s_light[nNum].Direction = vecDir;

	// ���C�g��ݒ肷��
	pDevice->SetLight(nNum, &s_light[nNum]);

	// ���C�g��L���ɂ���
	pDevice->LightEnable(nNum, TRUE);

}