//============================
//
// はいけい設定
// Author:hamada ryuuga
//
//============================
#include"bg.h"
#include "input.h"


#define NUM_BG (1)//背景枚数

//スタティック変数
static LPDIRECT3DTEXTURE9 s_pTextureBG[NUM_BG] = {}; //テクスチャのポインタ
static LPDIRECT3DVERTEXBUFFER9 s_pVtxBuffBG = NULL; //頂点バッファの設定
static D3DXVECTOR3 s_moveBG;	//移動量
static D3DXVECTOR3 s_posBG[NUM_BG];	//座標
static Bg s_nBgnCount;
static float txeX,txeY;
//================
//初期化処理
//================
void InitBG(void)
{
	LPDIRECT3DDEVICE9  pDevice;

	//デバイスの取得
	pDevice = GetDevice();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"Data/TEXTURE/map2.png",
		&s_pTextureBG[0]);
	//頂点バッファ
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,//ここ頂点フォーマット
		&s_pVtxBuffBG,
		NULL);

	VERTEX_2D*pVtx; //頂点へのポインタ
	//頂点バッファをアンロック
	s_pVtxBuffBG->Lock(0, 0, (void**)&pVtx, 0);
	
	s_posBG[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	SetNormalpos2d(pVtx,
		0.0f,
		SCREEN_WIDTH,
		0.0f,
		SCREEN_HEIGHT);
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	txeX = 0.0f;
	txeY = 0.0f;
	//頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//テクスチャの座標設定
	Settex2d(pVtx, 0.0f, 0.2f, 0.0f, 1.0f);
	//頂点バッファをアンロック
	s_pVtxBuffBG->Unlock();

}

//破棄
void UninitBG(void)
{

	for (int nCnt = 0; nCnt < NUM_BG; nCnt++)
	{
		//テクスチャの破棄
		if (s_pTextureBG[nCnt] != NULL)
		{
			s_pTextureBG[nCnt]->Release();
			s_pTextureBG[nCnt] = NULL;
		}
	}
	//頂点バッファの破棄
	if (s_pVtxBuffBG != NULL)
	{
		s_pVtxBuffBG->Release();
		s_pVtxBuffBG = NULL;
	}

}
//==================
//更新処理
//==================
void UpdateBG(void)
{
	VERTEX_2D*pVtx; //頂点へのポインタ
					//頂点バッファをアンロック
	s_pVtxBuffBG->Lock(0, 0, (void**)&pVtx, 0);
	//テクスチャの座標設定
	//s_posBG[0] -= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	SetNormalpos2d(pVtx,
		0.0f,
		SCREEN_WIDTH,
		0.0f + s_posBG[0].y,
		SCREEN_HEIGHT * 10+ s_posBG[0].y);
	txeY -= 0.0005f;
	if (txeY <= -1.0f)
	{
		txeY = 0.0f;
		txeX += 0.2f;
	}
	if (txeX >= 1.0f)
	{	
		txeX = 0.0f;
	}
	//テクスチャの座標設定
	Settex2d(pVtx, 0.0f + txeX, 0.2f + txeX, 0.0f + txeY, 0.2f+txeY);
	//頂点バッファをアンロック
	s_pVtxBuffBG->Unlock();
}
//==================
//描画処理
//==================
void DrawBG(void)
{
	LPDIRECT3DDEVICE9 pDevice;//デバイスのポインタ
//	int nCntEnemy;

	pDevice = GetDevice();		//代入
	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, s_pVtxBuffBG, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャの設定
	pDevice->SetTexture(0, s_pTextureBG[0]);

	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,
		0,
		2);
}

Bg *GetBg(void)
{
	return &s_nBgnCount;
}