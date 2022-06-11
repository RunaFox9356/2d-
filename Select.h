#pragma once
//============================
//
// マップチップ設定ヘッター
// Author:hamada ryuuga
//
//============================

#ifndef _SELECT_H_
#define _SELECT_H_

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
}Select;

//-----------------
//プロトタイプ宣言
//-----------------
void InitSelect(void);
void UninitSelect(void);
void UpdateSelect(int Set, int type);
void DrawSelect(void);

#endif // !_Select_H_

#pragma once
