//============================
//
//	範囲選択設定
// Author:hamada ryuuga
//
//============================

#include "range.h"
#include "input.h"
#include "comn.h"
#include "camera.h"

//スタティック変数
static LPDIRECT3DVERTEXBUFFER9 s_pVtxBuffRange = NULL; //頂点バッファの設定
static MoveRange s_aRange;
D3DXVECTOR3 MousePoint;

//================
//初期化処理
//================
void InitRange(void)
{
	LPDIRECT3DDEVICE9  pDevice;
	D3DXVECTOR3 MousePoint = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//デバイスの取得
	pDevice = GetDevice();

	//頂点バッファ
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D, //ここ頂点フォーマット
		D3DPOOL_MANAGED,
		&s_pVtxBuffRange,
		NULL);

	VERTEX_2D*pVtx; //頂点へのポインタ
					//頂点バッファをアンロック
	s_pVtxBuffRange->Lock(0, 0, (void**)&pVtx, 0);

	s_aRange.bUse = false;

	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;


	//頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f);

	SetNormalpos2d(pVtx,
		s_aRange.pos.x,
		s_aRange.pos.x,
		s_aRange.pos.y,
		s_aRange.pos.y);

	int count = 0;

	//頂点バッファをアンロック
	s_pVtxBuffRange->Unlock();
}

//破棄
void UninitRange(void)
{
	//頂点バッファの破棄
	if (s_pVtxBuffRange != NULL)
	{
		s_pVtxBuffRange->Release();
		s_pVtxBuffRange = NULL;
	}
}

//==================
//更新処理
//==================
void UpdateRange(void)
{

	//頂点へのポインタ
	VERTEX_2D*pVtx;
	//頂点バッファをロック
	s_pVtxBuffRange->Lock(0, 0, (void**)&pVtx, 0);

	s_aRange.pos = GetMouse();

	SetNormalpos2d(pVtx,
		s_aRange.pos.x,
		s_aRange.pos.x,
		s_aRange.pos.y,
		s_aRange.pos.y);

	if (GetMouseTrigger(MOUSE_INPUT_LEFT))
	{
		s_aRange.posSet = s_aRange.pos;
	}
	if (GetKeyboardPress(DIK_LSHIFT)&& GetMousePress(MOUSE_INPUT_LEFT))
	{
		
		SetNormalpos2d(pVtx,
			s_aRange.posSet.x,
			s_aRange.pos.x,
			s_aRange.posSet.y,
			s_aRange.pos.y);
	}

	//頂点バッファをアンロック
	s_pVtxBuffRange->Unlock();
}

//==================
//描画処理
//==================
void DrawRange(void)
{
	//デバイスのポインタ
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, s_pVtxBuffRange, 0, sizeof(VERTEX_2D));
	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);
	//テクスチャの設定
	pDevice->SetTexture(0, NULL);

	pDevice->DrawPrimitive(
		D3DPT_TRIANGLESTRIP,
		0,
		2);

}

//==================
//範囲選択
//==================
bool CollisionRange(D3DXVECTOR3 pos)
{
	bool Hit = false;//チップとマウスの当たり判定が当たってる時
	D3DXVECTOR3 mapPosStart, mapPosEnd;

	if (GetKeyboardPress(DIK_LSHIFT))
	{
		
		Camera *pCamera = GetCamera();
		mapPosStart = WorldCastScreen(&s_aRange.posSet,								// スクリーン座標
			D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f),			// スクリーンサイズ
			&pCamera->mtxView,										// ビューマトリックス
			&pCamera->mtxProjection);								// プロジェクションマトリックス

		mapPosEnd = WorldCastScreen(&s_aRange.pos,								// スクリーン座標
			D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f),			// スクリーンサイズ
			&pCamera->mtxView,										// ビューマトリックス
			&pCamera->mtxProjection);								// プロジェクションマトリックス

		if (((mapPosStart.x > pos.x) && (mapPosEnd.x < pos.x)) &&
			((mapPosStart.y > pos.y) && (mapPosEnd.y < pos.y)))
		{
			Hit = true;
		}
	}
	return Hit;
}