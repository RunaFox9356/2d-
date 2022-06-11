//============================
//
// メイン設定のヘッター
// Author:hamada ryuuga
//
//============================
#ifndef _MAIN_H_
#define _MAIN_H_

#include <windows.h>
#include "d3dx9.h"
#define DIRECTINPUT_VERSION (0x0800)
#include "dinput.h"
#include "xaudio2.h"
#include "xinput.h"//ジョイパット入力に必要

//ライブラリのリンク
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"xinput.lib")//ジョイパット入力に必要

//マクロ定義
#define SCREEN_WIDTH	(1280)			// 幅サイズ
#define SCREEN_HEIGHT	(720)			// 高さサイズ
#define FVF_VERTEX_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define FVF_VERTEX_3D (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define CLASS_NAME		"WindowClass"	// クラスの名前
#define WINDOW_NAME		"平行投影におけるマップエディタ"		// 名前決めるとこ
#define ID_BUTTON_FINISH (101)			// 終了ボタンのID
#define ID_EDIT_INPUT1 (111)			// 入力ウインドウ1ID
#define ID_MODE	()
#define WORLDSIZ (100.0f)
#define ZERO_VECTOR		(D3DXVECTOR3(0.0f, 0.0f, 0.0f))		// ゼロベクトル
#define ZERO_QUATERNION	(D3DXQUATERNION(0.0f,0.0f,0.0f,1.0f))

//=========================================
//頂点情報「２ｄ」の構造体を定義
//=========================================
typedef struct
{
	D3DXVECTOR3 pos;
	float rhw;
	D3DCOLOR col;
	D3DXVECTOR2 tex;

}VERTEX_2D;

//=========================================
//頂点情報「3ｄ」の構造体を定義
//=========================================
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標
	D3DXVECTOR3 nor;	// ベクトル
	D3DCOLOR col;		// カラー
	D3DXVECTOR2 tex;	// テクスチャ
}VERTEX_3D;

//=========================================
//モードの種類
//=========================================
typedef enum
{
	MODE_TITLE = 0,
	MODE_TUTORIAL,
	MODE_GAME,
	MODE_RESULT,
	MODE_GAMEOVER,
	MODE_RANKING,
	MODE_MAX
}MODE;

//=========================================
// プロトタイプ宣言
//=========================================
LPDIRECT3DDEVICE9 GetDevice(void);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	// ウィンドウプロシージャ
HRESULT Init(HINSTANCE hlnstance, HWND hWnd, BOOL bWindow);	// 初期化
void Uninit(void);				// 破棄
void Update(HWND hWnd);			// 更新
void Draw(void);				// 描画
int GetFPS();					// FPSの取得

//=========================================
//セットの設定まとめ
//=========================================
void Settex(VERTEX_3D *pVtx, float left, float right, float top, float down);
void Settex2d(VERTEX_2D *pVtx, float left, float right, float top, float down);
void SetNormalpos2d(VERTEX_2D *pVtx, float XUP, float XDW, float YUP, float YDW);
void SetNormalpos(VERTEX_3D *pVtx, float XUP, float XDW, float YUP, float YDW, float ZUP, float ZDW);
void SetUp(VERTEX_2D *pVtx, float posx, float posy, float rotPlayer, float fAngleAttack, float fLengthAttack);
void Allfalse(void);
#endif
