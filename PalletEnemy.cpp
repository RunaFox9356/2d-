
//============================
//
// マップチップ設定
// Author:hamada ryuuga
//
//============================
#include "PalletEnemy.h"
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

//スタティック変数
static LPDIRECT3DTEXTURE9 s_pTexturePalletE[ENEMY_TYPE_MAX] = {}; //テクスチャのポインタ
static LPDIRECT3DVERTEXBUFFER9 s_pVtxBuffPalletE = NULL; //頂点バッファの設定
static PalletE s_aPalletE[ENEMY_TYPE_MAX];
static D3DXVECTOR3 s_Move(0.0f, 5.0f, 0.0f);
static D3DXVECTOR3 s_PosOffset;
static float s_fPalletEScale;
static int stage = 0;


//================
//初期化処理
//================
void InitPalletE(void)
{
	LPDIRECT3DDEVICE9  pDevice;

	//デバイスの取得
	pDevice = GetDevice();



	//頂点バッファ
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4 * ENEMY_TYPE_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D, //ここ頂点フォーマット
		D3DPOOL_MANAGED,
		&s_pVtxBuffPalletE,
		NULL);

	VERTEX_2D*pVtx; //頂点へのポインタ
					//頂点バッファをアンロック
	s_pVtxBuffPalletE->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < ENEMY_TYPE_MAX; i++, pVtx += 4)
	{
		s_aPalletE[i].bUse = false;

		s_aPalletE[i].tex = 0;

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
	s_pVtxBuffPalletE->Unlock();

	for (int nCntX = 0; nCntX < ENEMY_TYPE_MAX; nCntX++)
	{//PalletEの書き込み

		SetPalletE(D3DXVECTOR3(BLOCKSIZEX*nCntX, (150), 0.0f), 1, count);

		count++;
	}

	s_PosOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	s_fPalletEScale = 1.0f;
	//------------------------
	//テクスチャ設定
	//------------------------
	PasSetTex("Data/txt/enemy.txt");
}

//================
//破棄処理
//================
void UninitPalletE(void)
{
	stage = 0;
	//テクスチャの破棄
	for (int i = 0; i < (int)ENEMY_TYPE_MAX; i++)
	{
		if (s_pTexturePalletE[i] != NULL)
		{
			s_pTexturePalletE[i]->Release();
			s_pTexturePalletE[i] = NULL;
		}
	}

	//頂点バッファの破棄
	if (s_pVtxBuffPalletE != NULL)
	{
		s_pVtxBuffPalletE->Release();
		s_pVtxBuffPalletE = NULL;
	}
}

//==================
//更新処理
//==================
void UpdatePalletE(void)
{

	//頂点へのポインタ
	VERTEX_2D*pVtx;
	//頂点バッファをロック
	s_pVtxBuffPalletE->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < ENEMY_TYPE_MAX; i++, pVtx += 4)
	{
		if (!s_aPalletE[i].bUse)
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
			s_aPalletE[i].pos.x,
			s_aPalletE[i].pos.x + BLOCKSIZEX,
			s_aPalletE[i].pos.y,
			s_aPalletE[i].pos.y + BLOCKSIZEX);

	}

	//頂点バッファをアンロック
	s_pVtxBuffPalletE->Unlock();
}

//==================
//描画処理
//==================
void DrawPalletE(void)
{
	for (int i = 0; i < ENEMY_TYPE_MAX; i++)
	{
		if (!s_aPalletE[i].bUse)
		{
			continue;
		}

		//デバイスのポインタ
		LPDIRECT3DDEVICE9 pDevice = GetDevice();
		//頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, s_pVtxBuffPalletE, 0, sizeof(VERTEX_2D));
		//頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);
		//テクスチャの設定
		pDevice->SetTexture(0, s_pTexturePalletE[s_aPalletE[i].tex]);

		pDevice->DrawPrimitive(
			D3DPT_TRIANGLESTRIP,
			4 * i,
			2);
	}
}

//==================
//マップデータの設定
//==================
void SetPalletE(D3DXVECTOR3 pos, int nType, int tex)
{
	VERTEX_2D *pVtx; //頂点へのポインタ
	s_pVtxBuffPalletE->Lock(0, 0, (void**)&pVtx, 0);

	PalletE* pPalletE = s_aPalletE;
	for (int i = 0; i < ENEMY_TYPE_MAX; i++, pVtx += 4, pPalletE++)
	{
		if (pPalletE->bUse)
		{
			continue;
		}

		pPalletE->bUse = true;

		//マップが使用されてない場合
		pPalletE->pos = pos;
		pPalletE->nType = nType;
		pPalletE->tex = tex;

		//テクスチャの座標設定
		Settex2d(pVtx, 0.0f, 1.0f, 0.0f, 1.0f);

		break;
	}
	//頂点バッファをアンロック
	s_pVtxBuffPalletE->Unlock();
}

//==================
//MAPスポイト
//==================
int CollisionPalletE(D3DXVECTOR3 pos)
{
	int Hit = 0;//チップとマウスの当たり判定が当たってる時
	D3DXVECTOR3 mapPos;

	for (int i = 0; i < ENEMY_TYPE_MAX; i++)
	{
		if (!s_aPalletE[i].bUse)
		{
			continue;
		}


		if (((s_aPalletE[i].pos.x < pos.x) && (s_aPalletE[i].pos.x + BLOCKSIZEX > pos.x)) &&
			((s_aPalletE[i].pos.y < pos.y) && (s_aPalletE[i].pos.y + BLOCKSIZEX > pos.y)))
		{

			Hit = s_aPalletE[i].tex;

		}
	}
	return Hit;
}

//================
//なんだっけこれ処理
//================
void PalletMove(bool Set)
{
	//頂点へのポインタ
	VERTEX_2D*pVtx;
	//頂点バッファをロック
	s_pVtxBuffPalletE->Lock(0, 0, (void**)&pVtx, 0);

	for (int i = 0; i < ENEMY_TYPE_MAX; i++, pVtx += 4)
	{
		if (!s_aPalletE[i].bUse)
		{
			continue;
		}

		if (Set)
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
			s_aPalletE[i].pos.x,
			s_aPalletE[i].pos.x + BLOCKSIZEX,
			s_aPalletE[i].pos.y,
			s_aPalletE[i].pos.y + BLOCKSIZEX);

	}
	//頂点バッファをアンロック
	s_pVtxBuffPalletE->Unlock();
}

//==================
//fileパス読み込み
//==================
void PasSetTex(char *Filename)
{
	//ファイルを開く
	Enemy *Enemy = GetEnemy();
	FILE *pFile = fopen(&Filename[0], "r");
	char	s_aString[256];//ファイルの文字入れる
	if (pFile != NULL)
	{//ファイルが開いた場合
		fscanf(pFile, "%s", &s_aString);

		while (strncmp(&s_aString[0], "SCRIPT", 6) != 0)
		{//スタート来るまで空白読み込む
			s_aString[0] = {};
			fscanf(pFile, "%s", &s_aString[0]);
		}
		int nNumber = 0;
		int nEnemt = 0;
		while (strncmp(&s_aString[0], "END_SCRIPT", 10) != 0)
		{// 文字列の初期化と読み込み// 文字列の初期化と読み込み
			fscanf(pFile, "%s", &s_aString[0]);
			if (strcmp(&s_aString[0], "TEXTURE_FILENAME") == 0)
			{
				LPDIRECT3DDEVICE9  pDevice;
				//デバイスの取得
				pDevice = GetDevice();
				fscanf(pFile, "%s", &s_aString[0]);//＝読み込むやつ
				fscanf(pFile, "%s", &s_aPalletE[nEnemt].filename[0]);
				//テクスチャの読み込み
				D3DXCreateTextureFromFile(pDevice,
					&s_aPalletE[nEnemt].filename[0],
					&s_pTexturePalletE[nEnemt]);
				nEnemt++;
			}
		}
		//ファイルを閉じる
		fclose(pFile);
	}

}

//==================
//ゲット
//==================
PalletE *GetPalltE(void)
{
	return &s_aPalletE[0];
}