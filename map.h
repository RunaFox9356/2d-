//============================
//
// マップチップ設定ヘッター
// Author:hamada ryuuga
//
//============================
#ifndef _MAP_H_
#define _MAP_H_

#include"main.h"

// ファイル名
#define MAP_FILE0	"Data/txt/map.txt"
#define MAP_FILE1	"Data/txt/map1.txt"
#define MAP_FILE2	"Data/txt/map2.txt"
#define MAP_FILE3	"Data/txt/map3.txt"
#define MAP_FILE4	"Data/txt/map4.txt"
#define MAP_FILE5	"Data/txt/map5.txt"
#define MAP_FILE6	"Data/txt/map6.txt"

// マクロ定義
#define NUM_MAP	    (2048)		//背景枚数
#define X_MAP		(8)		//マップチップサイズX
#define Y_MAP		(8)			//マップチップサイズY
#define MAX_MAP		(X_MAP*Y_MAP)
#define MAP_SIZEX	(100)		//マップサイズX
#define MAP_SIZEY	(10)		//マップサイズY
#define MAPYLENGTH	(10)		//マップの長さ
#define BLOCKSIZEX	(BLOCKSIZEY)
#define BLOCKSIZEY	((float)SCREEN_HEIGHT/MAP_SIZEY)//
#define MAPMOVE (5.0f)

//-------------
//構造体
//-------------
typedef struct
{
	D3DXVECTOR3 pos;	//位置
	D3DXCOLOR col;		//カラー
	int nType;			//テクスチャ
	int tex;			//テクスチャタイプ
	bool bUse;			//
	char filename[256];
}Map;

//-----------------
//プロトタイプ宣言
//-----------------
void InitMap(void);
void UninitMap(void);
void UpdateMap(void);
void DrawMap(void);
void SetMap(D3DXVECTOR3 pos, int nType, int tex);
void InitMapSet(char *Filename);					//読み込み
void ConversionMap (D3DXVECTOR3 pos,int tex);
void OutputMap(char *Filename);
void SizeMap(float fSize);
Map *GetMap(void);
bool CollisionMap(D3DXVECTOR3 pos);//当たり判定
void ConteSet(int nStage);			//コンテ
void PasSetMap(char *Filename);//マップパスをセットする
int SopiteMap(D3DXVECTOR3 pos);
void FalseSetMap(void);								//マップ削除
D3DXVECTOR3 EnemyMap(D3DXVECTOR3 pos);				//エネミをマップの上に置く処理
#endif // !_Map_H_
