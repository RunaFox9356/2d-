//============================
//
// マップチップ設定ヘッター
// Author:hamada ryuuga
//
//============================

#ifndef _PALLET_H_
#define _PALLET_H_

#include"main.h"

//-------------
//構造体
//-------------
typedef struct
{
	D3DXVECTOR3 pos;	//位置
	D3DXCOLOR col; //カラー
	int nType;//テクスチャ
	int tex;
	bool bUse;
	char filename[256];
}Pallet;

//-----------------
//プロトタイプ宣言
//-----------------
void InitPallet(void);
void UninitPallet(void);
void UpdatePallet(void);
void DrawPallet(void);
void SetPalletMap(D3DXVECTOR3 pos, int nType, int tex);
int CollisionPallet(D3DXVECTOR3 pos);
void PalletMoveMap(bool Set);

#endif // !_Pallet_H_

