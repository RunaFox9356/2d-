//============================
//
// マップチップ設定
// Author:hamada ryuuga
//
//============================
#include "Pallet.h"
#include "input.h"
#include "enemy.h"
#include <stdio.h>
#include "map.h"

#include <crtdbg.h>
#ifdef _DEBUG
//#define DEBUG_PRINT(...) _RPT_BASE(_CRT_WARN, __FILE__, __LINE__, NULL, __VA_ARGS__)
#define DEBUG_PRINT(...) _RPT_BASE(_CRT_WARN, nullptr, 0, NULL, __VA_ARGS__)
#else
#define DEBUG_PRINT(...) ((void)0)
#endif

// マクロ定義

#define NUM_PALLET (X_MAP*Y_MAP)

//スタティック変数
static LPDIRECT3DTEXTURE9 s_pTexturePallet = NULL; //テクスチャのポインタ
static LPDIRECT3DVERTEXBUFFER9 s_pVtxBuffPallet = NULL; //頂点バッファの設定
static Pallet s_aPallet[NUM_PALLET];
static D3DXVECTOR3 s_Move(0.0f, 5.0f, 0.0f);
static D3DXVECTOR3 s_PosOffset;
static float s_fPalletScale;
static int stage = 0;


//================
//初期化処理
//================
void InitPallet(void)
{
	LPDIRECT3DDEVICE9  pDevice;

	//デバイスの取得
	pDevice = GetDevice();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"Data/TEXTURE/mapchip_field2.png",
		&s_pTexturePallet);

	//頂点バッファ
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4 * NUM_PALLET,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D, //ここ頂点フォーマット
		D3DPOOL_MANAGED,
		&s_pVtxBuffPallet,
		NULL);

	VERTEX_2D*pVtx; //頂点へのポインタ
					//頂点バッファをアンロック
	s_pVtxBuffPallet->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < NUM_PALLET; i++, pVtx += 4)
	{
		s_aPallet[i].bUse = false;

		s_aPallet[i].tex = 0;

		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		
		//頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	}
	int count = 0;
	//頂点バッファをアンロック
	s_pVtxBuffPallet->Unlock();
	for (int nCntY = 0; nCntY < Y_MAP-4; nCntY++)
	{
		for (int nCntX = 0; nCntX < X_MAP; nCntX++)
		{//Palletの書き込み
			
			SetPalletMap(D3DXVECTOR3(BLOCKSIZEX*nCntX, ((BLOCKSIZEY*nCntY)), 0.0f), 1, count);
			
			count++;
		}
	}
	s_PosOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	s_fPalletScale = 1.0f;
}

//破棄
void UninitPallet(void)
{
	stage = 0;
	//テクスチャの破棄
	if (s_pTexturePallet != NULL)
	{
		s_pTexturePallet->Release();
		s_pTexturePallet = NULL;
	}

	//頂点バッファの破棄
	if (s_pVtxBuffPallet != NULL)
	{
		s_pVtxBuffPallet->Release();
		s_pVtxBuffPallet = NULL;
	}
}

//==================
//更新処理
//==================
void UpdatePallet(void)
{

	//頂点へのポインタ
	VERTEX_2D*pVtx;
	//頂点バッファをロック
	s_pVtxBuffPallet->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < NUM_PALLET; i++, pVtx += 4)
	{
		if (!s_aPallet[i].bUse)
		{
			continue;
		}

		if (GetKeyboardPress(DIK_LCONTROL))
		{

			//頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		}
		else
		{

			//頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);


		}

		SetNormalpos2d(pVtx,
			s_aPallet[i].pos.x,
			s_aPallet[i].pos.x + BLOCKSIZEX,
			s_aPallet[i].pos.y,
			s_aPallet[i].pos.y + BLOCKSIZEX);
	
	}

	//頂点バッファをアンロック
	s_pVtxBuffPallet->Unlock();
}

//==================
//描画処理
//==================
void DrawPallet(void)
{
	for (int i = 0; i < NUM_PALLET; i++)
	{
		if (!s_aPallet[i].bUse)
		{
			continue;
		}

		//デバイスのポインタ
		LPDIRECT3DDEVICE9 pDevice = GetDevice();
		//頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, s_pVtxBuffPallet, 0, sizeof(VERTEX_2D));
		//頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);
		//テクスチャの設定
		pDevice->SetTexture(0, s_pTexturePallet);

		pDevice->DrawPrimitive(
			D3DPT_TRIANGLESTRIP,
			4 * i,
			2);
	}
}

//==================
//マップデータの設定
//==================
void SetPalletMap(D3DXVECTOR3 pos, int nType, int tex)
{
	VERTEX_2D *pVtx; //頂点へのポインタ
	s_pVtxBuffPallet->Lock(0, 0, (void**)&pVtx, 0);

	Pallet* pPallet = s_aPallet;
	for (int i = 0; i < NUM_PALLET; i++, pVtx += 4, pPallet++)
	{
		if (pPallet->bUse)
		{
			continue;
		}

		pPallet->bUse = true;

		//マップが使用されてない場合
		pPallet->pos = pos;
		pPallet->nType = nType;
		pPallet->tex = tex;
		
		int X = (pPallet->tex % X_MAP);
		int Y = (pPallet->tex / X_MAP);
		float XSIZE = (1.0f / X_MAP);
		float YSIZE = (1.0f / Y_MAP);
		//テクスチャの座標設定
		Settex2d(pVtx, XSIZE * X, XSIZE * (X + 1), YSIZE * Y, YSIZE * (Y + 1));

		DEBUG_PRINT("l:%f, r:%f, t:%f, b:%f\n", XSIZE * X, XSIZE * (X + 1), YSIZE * Y, YSIZE * (Y + 1));

		break;
	}
	//頂点バッファをアンロック
	s_pVtxBuffPallet->Unlock();
}

//==================
//MAPスポイト
//==================
int CollisionPallet(D3DXVECTOR3 pos)
{
	int Hit = 0;//チップとマウスの当たり判定が当たってる時
	D3DXVECTOR3 mapPos;

	for (int i = 0; i < NUM_PALLET; i++)
	{
		if (!s_aPallet[i].bUse)
		{
			continue;
		}


		if (((s_aPallet[i].pos.x < pos.x) && (s_aPallet[i].pos.x + BLOCKSIZEX > pos.x)) &&
			((s_aPallet[i].pos.y < pos.y) && (s_aPallet[i].pos.y + BLOCKSIZEX > pos.y)))
		{

			Hit = s_aPallet[i].tex;
			
		}
	}
	return Hit;
}

void PalletMoveMap(bool Set)
{
	//頂点へのポインタ
	VERTEX_2D*pVtx;
	//頂点バッファをロック
	s_pVtxBuffPallet->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < NUM_PALLET; i++, pVtx += 4)
	{
		if (!s_aPallet[i].bUse)
		{
			continue;
		}

		if (GetKeyboardPress(DIK_LCONTROL))
		{

			//頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		}
		else
		{

			//頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);


		}

		SetNormalpos2d(pVtx,
			s_aPallet[i].pos.x,
			s_aPallet[i].pos.x + BLOCKSIZEX,
			s_aPallet[i].pos.y,
			s_aPallet[i].pos.y + BLOCKSIZEX);

	}

	//頂点バッファをアンロック
	s_pVtxBuffPallet->Unlock();
}