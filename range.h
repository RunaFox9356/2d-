//============================
//
// マップチップ設定ヘッター
// Author:hamada ryuuga
//
//============================

#ifndef _RANGE_H_
#define _RANGE_H_

#include"main.h"

//-------------
//構造体
//-------------
typedef struct
{
	D3DXVECTOR3 pos;	//位置
	D3DXVECTOR3 posSet;	//位置
	D3DXCOLOR col; //カラー
	bool bUse;
}MoveRange;

//-----------------
//プロトタイプ宣言
//-----------------
void InitRange(void);
void UninitRange(void);
void UpdateRange(void);
void DrawRange(void);
bool CollisionRange(D3DXVECTOR3 pos);

#endif 


