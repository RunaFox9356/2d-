//============================
//
// はいけい設定ヘッター
// Author:hamada ryuuga
//
//============================
#ifndef _BG_H_
#define _BG_H_

#include"main.h"

//構造体
typedef struct
{
	D3DXVECTOR3 pos;	//位置
	D3DXVECTOR3 move;	//移動量
	D3DXCOLOR col; //カラー
	int nType;//テクスチャ
}Bg;


//プロトタイプ宣言
void InitBG(void);
void UninitBG(void);
void UpdateBG(void);
void DrawBG(void);
Bg *GetBg(void);

#endif // !_BG_H_
