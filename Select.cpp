//============================
//
// 今使用してるのを表示設定
// Author:hamada ryuuga
//
//============================
#include "Select.h"
#include "input.h"
#include "enemy.h"
#include <stdio.h>
#include "map.h"
#include "PalletEnemy.h"
#define MAP_SIZE (50)
//スタティック変数
static LPDIRECT3DTEXTURE9 s_pTextureSelect = NULL; //テクスチャのポインタ
static LPDIRECT3DVERTEXBUFFER9 s_pVtxBuffSelect = NULL; //頂点バッファの設定

static LPDIRECT3DTEXTURE9 s_pTextureEnemy[ENEMY_TYPE_MAX] = {}; //テクスチャのポインタ
static LPDIRECT3DVERTEXBUFFER9 s_pVtxBuffEnemy = NULL; //頂点バッファの設定

static Select s_aSelect;
static D3DXVECTOR3 s_Move(0.0f, 5.0f, 0.0f);
static D3DXVECTOR3 s_PosOffset;
static float s_fSelectScale;
static int stage = 0;


//================
//初期化処理
//================
void InitSelect(void)
{
	LPDIRECT3DDEVICE9  pDevice;

	//デバイスの取得
	pDevice = GetDevice();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"Data/TEXTURE/mapchip_field2.png",
		&s_pTextureSelect);

	//頂点バッファ
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D, //ここ頂点フォーマット
		D3DPOOL_MANAGED,
		&s_pVtxBuffSelect,
		NULL);

	VERTEX_2D*pVtx; //頂点へのポインタ
					//頂点バッファをアンロック
	s_pVtxBuffSelect->Lock(0, 0, (void**)&pVtx, 0);


	s_aSelect.bUse = true;
	s_aSelect.pos = D3DXVECTOR3(1230.0f, 30.0f, 0.0f);
	s_aSelect.tex = 0;

	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;


	//頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	int X = (s_aSelect.tex % X_MAP);
	int Y = (s_aSelect.tex / X_MAP);

	//テクスチャの座標設定
	Settex2d(pVtx, (1.0f / X_MAP)*(X), (1.0f / X_MAP) * (1.0f + X), (1.0f / Y_MAP)*(Y), (1.0f / Y_MAP) *(Y + 1.0f));

	int count = 0;


	//頂点バッファをアンロック
	s_pVtxBuffSelect->Unlock();

	PalletE* nType = GetPalltE();

	for (int i = 0; i < (int)ENEMY_TYPE_MAX; i++, nType++)
	{
		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
			nType->filename,
			&s_pTextureEnemy[i]);
	}
	//頂点バッファ
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D, //ここ頂点フォーマット
		D3DPOOL_MANAGED,
		&s_pVtxBuffEnemy,
		NULL);

	//頂点バッファをロック
	s_pVtxBuffEnemy->Lock(0, 0, (void**)&pVtx, 0);
	//頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	s_aSelect.nType = 0;

	SetNormalpos2d(pVtx,
		s_aSelect.pos.x - MAP_SIZE,
		s_aSelect.pos.x,
		s_aSelect.pos.y,
		s_aSelect.pos.y + MAP_SIZE);


	//テクスチャの座標設定
	Settex2d(pVtx, 0.0f, 1.0f, 0.0f, 1.0f);


	//頂点バッファをアンロック
	s_pVtxBuffEnemy->Unlock();

	s_fSelectScale = 1.0f;
}

//破棄
void UninitSelect(void)
{
	stage = 0;
	//テクスチャの破棄
	if (s_pTextureSelect != NULL)
	{
		s_pTextureSelect->Release();
		s_pTextureSelect = NULL;
	}

	//頂点バッファの破棄
	if (s_pVtxBuffSelect != NULL)
	{
		s_pVtxBuffSelect->Release();
		s_pVtxBuffSelect = NULL;
	}
	for (int i = 0; i < (int)ENEMY_TYPE_MAX; i++)
	{
		if (s_pTextureEnemy[i] != NULL)
		{
			s_pTextureEnemy[i]->Release();
			s_pTextureEnemy[i] = NULL;
		}
	}
	if (s_pVtxBuffEnemy != NULL)
	{
		s_pVtxBuffEnemy->Release();
		s_pVtxBuffEnemy = NULL;
	}
}

//==================
//更新処理
//==================
void UpdateSelect(int Set, int type)
{

	//頂点へのポインタ
	VERTEX_2D*pVtx;
	//頂点バッファをロック
	s_pVtxBuffSelect->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 pos;

	s_aSelect.tex = Set;

	SetNormalpos2d(pVtx,
		s_aSelect.pos.x,
		s_aSelect.pos.x + MAP_SIZE,
		s_aSelect.pos.y,
		s_aSelect.pos.y + MAP_SIZE);

	int X = (s_aSelect.tex % X_MAP);
	int Y = (s_aSelect.tex / X_MAP);

	//テクスチャの座標設定
	Settex2d(pVtx, (1.0f / X_MAP)*(X), (1.0f / X_MAP) * (1.0f + X), (1.0f / Y_MAP)*(Y), (1.0f / Y_MAP) *(Y + 1.0f));



	//頂点バッファをアンロック
	s_pVtxBuffEnemy->Unlock();


	//頂点バッファをロック
	s_pVtxBuffEnemy->Lock(0, 0, (void**)&pVtx, 0);



	s_aSelect.nType = type;

	SetNormalpos2d(pVtx,
		s_aSelect.pos.x - MAP_SIZE,
		s_aSelect.pos.x,
		s_aSelect.pos.y,
		s_aSelect.pos.y + MAP_SIZE);


	//テクスチャの座標設定
	Settex2d(pVtx, 0.0f, 1.0f, 0.0f, 1.0f);

	//頂点バッファをアンロック
	s_pVtxBuffEnemy->Unlock();
}

//==================
//描画処理
//==================
void DrawSelect(void)
{

	//デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, s_pVtxBuffSelect, 0, sizeof(VERTEX_2D));
	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);
	//テクスチャの設定
	pDevice->SetTexture(0, s_pTextureSelect);

	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,
		0,
		2);

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, s_pVtxBuffEnemy, 0, sizeof(VERTEX_2D));
	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);
	pDevice->SetTexture(0, s_pTextureEnemy[s_aSelect.nType]);

	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,
		0,
		2);
}



