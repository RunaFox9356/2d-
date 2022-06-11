//=========================================
// 
// ゲーム内処理
// Author YudaKaito
// 
//=========================================
//-----------------------------------------
// include
//-----------------------------------------
#include "main.h"
#include "debug.h"
#include "process.h"
#include <stdio.h>

//-----------------------------------------
// マクロ宣言
//-----------------------------------------
#define DEBUG_NUM	(7)

//-----------------------------------------
// 静的変数宣言
//-----------------------------------------
static LPD3DXFONT s_pFont = NULL;	// フォントへのポインタ

//-----------------------------------------
// プロトタイプ宣言
//-----------------------------------------

//=========================================
// FPS表示の初期化
//=========================================
void InitDebug(void)
{
	// デバッグ表示用フォントの生成
	D3DXCreateFont(GetDevice(), 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "HG創英角ｺﾞｼｯｸUB", &s_pFont);
}

//=========================================
// FPS表示の終了処理
//=========================================
void UninitDebug(void)
{
	// デバッグ表示用フォントの破棄
	if (s_pFont != NULL)
	{
		s_pFont->Release();
		s_pFont = NULL;
	}
}

//=========================================
// FPSの表示
//=========================================
void DrawDebug(void)
{
	char aStr[DEBUG_NUM][512];	// 表示文字

	//文字列に代入
	wsprintf(&aStr[0][0], "FPS%d", GetFPS());

	wsprintf(&aStr[1][0], "ステージ番号%d", GetDebugNumber());

	wsprintf(&aStr[2][0], "エネミータイプ%d", GetDebugNumberEnemy());

	if (IsDebug())
	{
		wsprintf(&aStr[3][0], "debug[ON]");

		if (GetSetMode())
		{
			wsprintf(&aStr[4][0], "EnemyMode");
			if (GetEnemyAlignment())
			{
				wsprintf(&aStr[5][0], "敵をブロックごとに配置します");
			}
			else
			{
				wsprintf(&aStr[5][0], "敵を自由にに配置します");
			}
			wsprintf(&aStr[6][0], "操作説明\nデバック切り替えYキー\n左敵生成\n右変更CTRL押しながら右クリックでぱれっと\nCTRL+Fで設置されてるタイプ変更\nVキー移動再度押して確定\nホイールでマップ移動\nマウスカーソル合わせたときは選択\nDキー削除\nAキーMAP配置とEnemy配置の切り替え\nL全部削除\nS自由配置とBLOCKごとに配置の切り替え\nシフト+マウスホイールサイズ変更\n8キーマップ保存\n9キーエネミー保存\nM&Nキーでマップ切り替え\n");
		}
		else
		{
			wsprintf(&aStr[4][0], "MapMode");
			wsprintf(&aStr[5][0], "操作説明\nデバック切り替えYキー\n左クリックタイル生成\n右クリックタイル変更\nCTRL押しながら右クリックでパレットからスポイトできる\n8キーマップ保存\n9キーエネミー保存\nAキーMAP配置とEnemy配置の切り替え");
			wsprintf(&aStr[6][0], "");
		}


	}
	else
	{
		wsprintf(&aStr[3][0], "debug[OFF]");
		wsprintf(&aStr[4][0], "操作説明\nデバック切り替えYキー\n");
		wsprintf(&aStr[5][0], "");
		wsprintf(&aStr[6][0], "");
	}

	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	for (int i = 0; i < DEBUG_NUM; i++)
	{
		rect = { 0,i * 30,SCREEN_WIDTH,SCREEN_HEIGHT };
		//テキストの描画
		s_pFont->DrawText(NULL, &aStr[i][0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
	}
}