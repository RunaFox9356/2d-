//=============================================================================
//
// 入力処理 [input.cpp]
// Author1 : KOZUNA HIROHITO
// Author2 : ISOE JUKIA
// Author3 : YUDA KAITO
//
//=============================================================================

//-----------------------------------------------------------------------------
//インクルードファイル
//-----------------------------------------------------------------------------
#include "input.h"

//-----------------------------------------------------------------------------
// マクロ定義
//-----------------------------------------------------------------------------
#define NUM_KEY_MAX		(256)	// キーの最大数（キーボード）
#define MOUSE_KEY_MAX	(8)		// キーの最大数（マウス）
#define PLAYER_MAX		(4)		// プレイヤーの最大人数

//-----------------------------------------------------------------------------
// グローバル変数
//-----------------------------------------------------------------------------
// キーボード
static LPDIRECTINPUT8 s_pInput = NULL;					// DirectInputオブジェクトへのポインタ
static LPDIRECTINPUTDEVICE8 s_pDevKeyboard = NULL;		// 入力デバイス（キーボード（コントローラー用は別に作る））へのポインタ
static BYTE s_aKeyState[NUM_KEY_MAX];					// キーボードのプレス情報
static BYTE s_aKeyStateTrigger[NUM_KEY_MAX];			// キーボードのトリガー情報
static BYTE s_aKeyStateRelease[NUM_KEY_MAX];			// キーボードのリリース情報

// ジョイパッド
static XINPUT_STATE s_JoyKeyState[PLAYER_MAX];			// ジョイパットのプレス情報
static XINPUT_STATE s_JoyKeyStateTrigger[PLAYER_MAX];	// ジョイパットのトリガー情報
static XINPUT_STATE s_JoyKeyStateRelease[PLAYER_MAX];	// ジョイパットのリリース情報
static D3DXVECTOR3 s_JoyStickPos[PLAYER_MAX];			// ジョイスティックの傾き
static XINPUT_VIBRATION s_JoyMoter[PLAYER_MAX];			// ジョイパッドのモーター
static int s_nTime[PLAYER_MAX];							// 振動持続時間
static WORD s_nStrength[PLAYER_MAX];					// 振動の強さ (0 - 65535)

// マウス
static LPDIRECTINPUT8 s_pMouseInput = NULL;				// Directinutオブジェクトへのポインタ
static LPDIRECTINPUTDEVICE8 s_pDevMouse = NULL;			// 入力でパスへのポインタ
static DIMOUSESTATE2 s_aKeyStateMouse;					// マウスのプレス処理
static DIMOUSESTATE2 s_aKeyStateMouseTrigger;			// マウスのトリガー処理
static DIMOUSESTATE2 s_aKeyStateMouseRelease;			// マウスのリリース処理
static POINT s_MousePos;								// マウスのカーソル用
static HWND s_hMouseWnd;								// ウィンドウハンドル

//-----------------------------------------------------------------------------
// プロトタイプ宣言
//-----------------------------------------------------------------------------
// キーボード
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd);	// 初期化
void UninitKeyboard(void);								// 終了処理
void UpdateKeyboard(void);								// 更新処理

// ジョイパッド
HRESULT InitJoypad(void);								// 初期化
void UninitJoypad(void);								// 終了処理
void UpdateJoypad(void);								// 更新処理

// マウス
HRESULT InitMouse(HINSTANCE hlnstance, HWND hWnd);		// 初期化
void UninitMouse(void);									// 終了処理
void UpdateMouse(void);									// 更新処理

//*************************************************************************************
// 入力処理全体
//*************************************************************************************

//--------------------------------------------------
// 入力処理全部の初期化
//--------------------------------------------------
HRESULT InitInput(HINSTANCE hInstance, HWND hWnd)
{
	// キーボードの初期化処理
	if (FAILED(InitKeyboard(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// マウスの初期化処理
	if (FAILED(InitMouse(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// ジョイパッド初期化
	InitJoypad();

	return S_OK;
}

//--------------------------------------------------
// 入力処理全部の終了処理
//--------------------------------------------------
void UninitInput(void)
{
	// 終了処理
	UninitKeyboard();	// キーボード
	UninitMouse();		// マウス
	UninitJoypad();		// ジョイパッド

}

//--------------------------------------------------
// 入力処理全部の更新処理
//--------------------------------------------------
void UpdateInput(void)
{
	// 更新処理
	UpdateKeyboard();	// キーボード
	UpdateMouse();		// マウス
	UpdateJoypad();		// ジョイパッド
}

//*****************************************************************************
// キーボード入力処理
//*****************************************************************************

//--------------------------------------------------
// キーボード初期化処理
//--------------------------------------------------
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	// DirectInputオブジェクトの生成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&s_pInput, NULL)))
	{
		return E_FAIL;
	}

	// 入力デバイス（キーボード）の生成
	if (FAILED(s_pInput->CreateDevice(GUID_SysKeyboard, &s_pDevKeyboard, NULL)))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(s_pDevKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	if (FAILED(s_pDevKeyboard->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// キーボードへのアクセス権を獲得
	s_pDevKeyboard->Acquire();

	return S_OK;
}

//--------------------------------------------------
// キーボードの終了処理
//--------------------------------------------------
void UninitKeyboard(void)
{
	// 入力デバイス（キーボード）の放棄
	if (s_pDevKeyboard != NULL)
	{
		s_pDevKeyboard->Unacquire();	// キーボードへのアクセス権を放棄
		s_pDevKeyboard->Release();
		s_pDevKeyboard = NULL;
	}

	// DirectInputオブジェクトの破壊
	if (s_pInput != NULL)
	{
		s_pInput->Release();
		s_pInput = NULL;
	}
}

//--------------------------------------------------
// キーボードの更新処理
//--------------------------------------------------
void UpdateKeyboard(void)
{
	BYTE aKeyState[NUM_KEY_MAX];	// キーボードの入力情報

	if (SUCCEEDED(s_pDevKeyboard->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{// 入力デバイスからデータを取得
		for (int i = 0; i < NUM_KEY_MAX; i++)
		{
			s_aKeyStateTrigger[i] = ~s_aKeyState[i] & aKeyState[i];	// キーボードのトリガー情報を保存
			s_aKeyStateRelease[i] = s_aKeyState[i] & ~aKeyState[i];	// キーボードのリリース情報を保存
			s_aKeyState[i] = aKeyState[i];	// キーボードのプレス情報を保存
		}
	}
	else
	{
		s_pDevKeyboard->Acquire();	// キーボードへのアクセス権を獲得
	}
}

//--------------------------------------------------
// キーボードのプレス情報を取得
//--------------------------------------------------
bool GetKeyboardPress(int nKey)
{
	return (s_aKeyState[nKey] & 0x80) ? true : false;
}

//--------------------------------------------------
// キーボードのトリガー情報を取得
//--------------------------------------------------
bool GetKeyboardTrigger(int nKey)
{
	return (s_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//--------------------------------------------------
// キーボードのリリース情報を取得
//--------------------------------------------------
bool GetKeyboardRelease(int nKey)
{
	return (s_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//--------------------------------------------------
// キーボードの全キープレス情報を取得
//--------------------------------------------------
bool GetKeyboardAllPress(void)
{
	for (int i = 0; i < NUM_KEY_MAX; i++)
	{
		if (GetKeyboardPress(i))
		{
			return true;
		}
	}
	return false;
}

//--------------------------------------------------
// キーボードの全キートリガー情報を取得
//--------------------------------------------------
bool GetKeyboardAllTrigger(void)
{
	for (int i = 0; i < NUM_KEY_MAX; i++)
	{
		if (GetKeyboardTrigger(i))
		{
			return true;
		}
	}
	return false;
}

//--------------------------------------------------
// キーボードの全キーリリース情報を取得
//--------------------------------------------------
bool GetKeyboardAllRelease(void)
{
	for (int i = 0; i < NUM_KEY_MAX; i++)
	{
		if (GetKeyboardRelease(i))
		{
			return true;
		}
	}
	return false;
}

//*************************************************************************************
//ジョイパッド入力処理
//*************************************************************************************

//--------------------------------------------------
// ジョイパッドの初期化
//--------------------------------------------------
HRESULT InitJoypad(void)
{
	//XInputのステートを設定（有効にする）
	XInputEnable(true);

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//メモリーのクリア
		memset(&s_JoyKeyState[i], 0, sizeof(XINPUT_STATE));
		memset(&s_JoyKeyStateTrigger[i], 0, sizeof(XINPUT_STATE));

		//ジョイパッドの振動制御の０クリア
		ZeroMemory(&s_JoyMoter[i], sizeof(XINPUT_VIBRATION));

		//振動制御用の初期化
		s_nStrength[i] = 0;
		s_nTime[i] = 0;
	}
	return S_OK;
}

//--------------------------------------------------
// ジョイパッドの終了
//--------------------------------------------------
void UninitJoypad(void)
{
	//XInputのステートを設定（無効にする）
	XInputEnable(false);
}

//--------------------------------------------------
// ジョイパッドの更新
//--------------------------------------------------
void UpdateJoypad(void)
{
	XINPUT_STATE JoyKeyState[PLAYER_MAX];		//ジョイパッド入力情報

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//ジョイパッドの状態を取得
		if (XInputGetState(i, &JoyKeyState[i]) == ERROR_SUCCESS)
		{
			WORD wButtons = s_JoyKeyState[i].Gamepad.wButtons;
			WORD wOldButtons = s_JoyKeyState[i].Gamepad.wButtons;
			s_JoyKeyStateTrigger[i].Gamepad.wButtons = ~wButtons & wOldButtons; //トリガー情報を保存
			s_JoyKeyStateRelease[i].Gamepad.wButtons = wButtons & ~wOldButtons; //リリース情報を保存
			s_JoyKeyState[i] = JoyKeyState[i];  //プレス処理
		}

		//ジョイパッドの振動
		s_JoyMoter[i].wLeftMotorSpeed = s_nStrength[i];
		s_JoyMoter[i].wRightMotorSpeed = s_nStrength[i];
		XInputSetState(i, &s_JoyMoter[i]);

		if (s_nTime[i] > 0)
		{
			s_nTime[i]--;
		}
		else
		{
			s_nStrength[i] = 0;
			s_nTime[i] = 0;
		}
	}
}

//--------------------------------------------------
// ジョイパッドのプレス処理
//--------------------------------------------------
bool GetJoypadPress(JOYKEY Key, int nPlayer)
{
	return (s_JoyKeyState[nPlayer].Gamepad.wButtons & (0x01 << Key)) ? true : false;
}

//--------------------------------------------------
// ジョイパッドのトリガー処理
//--------------------------------------------------
bool GetJoypadTrigger(JOYKEY Key, int nPlayer)
{
	return (s_JoyKeyStateTrigger[nPlayer].Gamepad.wButtons & (0x01 << Key)) ? true : false;
}

//--------------------------------------------------
// ジョイパット（スティックプレス）処理
//--------------------------------------------------
D3DXVECTOR3 GetJoypadStick(JOYKEY Key, int nPlayer)
{
	switch (Key)
	{
	case JOYKEY_LEFT_STICK:
		s_JoyStickPos[nPlayer] = D3DXVECTOR3(s_JoyKeyState[nPlayer].Gamepad.sThumbLX / 32767.0f, -s_JoyKeyState[nPlayer].Gamepad.sThumbLY / 32767.0f, 0.0f);
		break;
	case JOYKEY_RIGHT_STICK:
		s_JoyStickPos[nPlayer] = D3DXVECTOR3(s_JoyKeyState[nPlayer].Gamepad.sThumbRX / 32767.0f, -s_JoyKeyState[nPlayer].Gamepad.sThumbRY / 32767.0f, 0.0f);
		break;
	}

	return s_JoyStickPos[nPlayer];
}

//--------------------------------------------------
// ジョイパット（トリガーペダル）処理
//--------------------------------------------------
int GetJoypadTriggerPedal(JOYKEY Key, int nPlayer)
{
	int nJoypadTriggerPedal = 0;
	switch (Key)
	{
	case JOYKEY_LEFT_TRIGGER:
		nJoypadTriggerPedal = s_JoyKeyState[nPlayer].Gamepad.bLeftTrigger;
		break;
	case JOYKEY_RIGHT_TRIGGER:
		nJoypadTriggerPedal = s_JoyKeyState[nPlayer].Gamepad.bRightTrigger;
		break;
	}

	return nJoypadTriggerPedal;
}

//--------------------------------------------------
// コントローラーの振動制御
//--------------------------------------------------
void JoypadVibration(int nTime, WORD nStrength, int nPlayer)
{
	s_nTime[nPlayer] = nTime;			//振動持続時間
	s_nStrength[nPlayer] = nStrength;	//振動の強さ
}

//*************************************************************************************
//マウス入力処理
//*************************************************************************************

//--------------------------------------------------
// マウスの初期化
//--------------------------------------------------
HRESULT InitMouse(HINSTANCE hInstance, HWND hWnd)
{
	//DirectInputオブジェクトの生成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&s_pMouseInput, NULL)))
	{
		return E_FAIL;
	}

	//入力デバイス（マウス）の生成
	if (FAILED(s_pMouseInput->CreateDevice(GUID_SysMouse, &s_pDevMouse, NULL)))
	{
		return E_FAIL;
	}

	//データフォーマットを設定
	if (FAILED(s_pDevMouse->SetDataFormat(&c_dfDIMouse2)))
	{
		return E_FAIL;
	}

	//協調モードを設定
	if (FAILED(s_pDevMouse->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//ウィンドウハンドルの保管
	s_hMouseWnd = hWnd;

	//キーボードへのアクセス権を獲得
	s_pDevMouse->Acquire();

	return S_OK;
}

//--------------------------------------------------
// マウスの終了処理
//--------------------------------------------------
void UninitMouse(void)
{
	if (s_pDevMouse != NULL)
	{
		s_pDevMouse->Unacquire();
		s_pDevMouse = NULL;
	}
}

//--------------------------------------------------
//マウスの更新処理
//--------------------------------------------------
void UpdateMouse(void)
{
	DIMOUSESTATE2 aKeyState;	// 入力情報

	if (SUCCEEDED(s_pDevMouse->GetDeviceState(sizeof(aKeyState), &aKeyState)))
	{// 入力デバイスからデータを取得
		for (int nCntKey = 0; nCntKey < MOUSE_KEY_MAX; nCntKey++)
		{
			WORD wButtons = s_aKeyStateMouse.rgbButtons[nCntKey];
			WORD wOldButtons = aKeyState.rgbButtons[nCntKey];
			s_aKeyStateMouseTrigger.rgbButtons[nCntKey] = ~wButtons & wOldButtons; // トリガー情報を保存
			s_aKeyStateMouseRelease.rgbButtons[nCntKey] = wButtons & ~wOldButtons; // リリース情報を保存
		}

		s_aKeyStateMouse = aKeyState;		// プレス情報を保存
	}
	else
	{
		s_pDevMouse->Acquire();			// アクセス権を獲得
	}
}

//--------------------------------------------------
// マウスのプレス処理
//--------------------------------------------------
bool GetMousePress(MOUSE mouse)
{
	return (s_aKeyStateMouse.rgbButtons[mouse] & 0x80) ? true : false;
}

//--------------------------------------------------
// マウスのトリガー処理
//--------------------------------------------------
bool GetMouseTrigger(MOUSE mouse)
{
	return (s_aKeyStateMouseTrigger.rgbButtons[mouse] & 0x80) ? true : false;
}

//--------------------------------------------------
// マウスのリリース処理
//--------------------------------------------------
bool GetMouseRelease(MOUSE mouse)
{
	return (s_aKeyStateMouseRelease.rgbButtons[mouse] & 0x80) ? true : false;
}

//--------------------------------------------------
//　マウスポインターの位置
//--------------------------------------------------
D3DXVECTOR3 GetMouse(void)
{
	//画面上のマウスポインターの位置
	GetCursorPos(&s_MousePos);
	//ウィンドウ上のマウスポインターの位置
	ScreenToClient(s_hMouseWnd, &s_MousePos);

	return D3DXVECTOR3((float)s_MousePos.x, (float)s_MousePos.y, 0.0f);
}

//--------------------------------------------------
// マウスのホイールの動き感知
//--------------------------------------------------
int GetMouseWheel(void)
{
	return (int)s_aKeyStateMouse.lZ;
}