
//============================
//
// マップチップ設定ヘッター
// Author:hamada ryuuga
//
//============================

#ifndef _PALLETENEMY_H_
#define _PALLETENEMY_H_

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
}PalletE;

//-----------------
//プロトタイプ宣言
//-----------------
void InitPalletE(void);
void UninitPalletE(void);
void UpdatePalletE(void);
void DrawPalletE(void);
void SetPalletE(D3DXVECTOR3 pos, int nType, int tex);//引数座標　タイプ　テクスチャの番号
int CollisionPalletE(D3DXVECTOR3 pos);	//引数座標
void PalletMove(bool Set);				//引数？？
void PasSetTex(char *Filename);			//引数ファイルパス
PalletE *GetPalltE(void);
#endif // !_PalletE_H_

