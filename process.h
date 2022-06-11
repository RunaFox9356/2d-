//=========================================
// 
// デバッグ時のみ発生の処理(ヘッダーファイル)
// Author YudaKaito
// 
//=========================================
#ifndef _PROCESS_H_
#define _PROCESS_H_

//=========================================
// include
//=========================================

//=========================================
// プロトタイプ宣言
//=========================================
void InitProcess();
void UninitProcess();
void UpdateProcess();
void DrawProcess();
bool GetSetMode();		// 設置モードの取得
void ChangeSetMode(bool hasChange);	// 設置モードの切り替え
void setEnemyNumber(int inDebugNumber);
void setDebugNumber(int inDebugNumber);
bool IsDebug(void);				// 取得
void ChangeDebug(bool inDebug);				// 取得
bool GetEnemyAlignment();
int GetDebugNumberEnemy();			//デバックナンバー（Enemy）
int GetDebugNumber();				//デバックナンバー（プレイヤー）

#endif // !_DEBUG_H_
