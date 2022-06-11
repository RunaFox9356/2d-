//============================
//
// プレイヤー設定ヘッター
// Author:hamada ryuuga
//
//============================
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "main.h"
#include "motion.h"

class CPlayer
{
public:
	typedef enum
	{
		ANIME_NORMAL = 0,	// ニュートラル
		ANIME_RUN,			// 歩き
		ANIME_ATTACK,		// 攻撃
		ANIME_JUMP,			// ジャンプ
		ANIME_LANDING,		// 着地
		ANIME_MAX
	}ANIME;

	typedef enum
	{
		STATUS_NORMAL = 0,	// ニュートラル
		STATUS_RUN,			// 歩き
		STATUS_ATTACK,		// 攻撃
		STATUS_JUMPUP,		// ジャンプ
		STATUS_JUMPDOWN,	// ジャンプ
		STATUS_LANDING,		// 着地
		STATUS_MAX
	}STATUS;

	typedef enum
	{
		DAMEGE_NORMAL = 0,	// ニュートラル
		DAMEGE_NOU,			// ダメージくらってる
		DAMEGE_MAX
	}DAMEGE;

	typedef enum
	{
		COPY_NORMAL = 0,	// ニュートラル
		COPY_SWORD,			// ソード
		COPY_FIRE,			// ファイア
		COPY_LASER,			// レーザー
		COPY_CUTTER,		// カッター
		COPY_MAX
	}COPY;

	//modelデータの構造体//
	typedef struct
	{
		int key;		// 時間管理
		int nowKey;		// 今のキー
		int loop;		// ループするかどうか[0:ループしない / 1 : ループする]
		int num_key;  	// キー数
		MyKeySet KeySet[MAX_KEY];
	}MODELDATAPLAYER;

public:
	static const float ATTENUATION;		// 減衰係数
	static const float SPEED;			// スピード
	static const float WIDTH;			// モデルの半径
	static const int MAX_PRAYER;		// 最大数
	static const int MAX_MOVE;			// アニメーションの最大数
	static const int INVINCIBLE;		// 無敵時間
	static const int MAX_MODELPARTS = 9;
	static const int MAX_COPY;

public:
	CPlayer();
	~CPlayer();

	void Init(void);	// 初期化
	void Uninit(void);	// 破棄
	void Update(void);	// 更新
	void Draw(void);	// 描画

	void Set(D3DXVECTOR3 pos, D3DXVECTOR3 rot);	// セット引数座標と読み込むファイル名

	void SetCopy(char *pFileName, PartsFile *partsFile, Parts *parts, MyMotion *Motion, int *nMaxParts);

private:
	void Collision(void);	// 当たり判定まとめ
	void Move(void);		// 移動

private:
	D3DXVECTOR3		m_pos;						// 位置
	D3DXVECTOR3		m_posOld;					// 位置過去
	D3DXVECTOR3		m_move;						// ムーブ
	D3DXVECTOR3		m_rot;						// 回転	
	D3DXVECTOR3		m_rotMove;					// 回転ムーブ
	D3DXVECTOR3		m_modelMin;					// サイズ最小
	D3DXVECTOR3		m_modelMax;					// サイズ最大
	D3DXMATRIX		m_mtxWorld;					// マトリックス//ポリゴンの位置や回転行列すべてをつめてるナニカ

	STATUS			m_status;					// 今のステータス
	DAMEGE			m_damege;					// ダメージくらってるかくらってないか
	COPY			m_copy;						// コピー

	Parts			m_parts[MAX_MODELPARTS];		// モデルパーツ
	PartsFile		m_partsFile[MAX_MODELPARTS];	// パーツファイル
	MyMotion		m_motion[ANIME_MAX];			// モーション
	ANIME			m_motionType;					// モーションタイプ(現在)
	ANIME			m_motionTypeOld;				// モーションタイプ(過去)
	int				m_nMaxModelType;				// モデルのタイプ数
	int				m_nMaxModelParts;				// 扱うモデルパーツ数
	int				m_nMaxMotion;					// モーション数

	int				m_type;						// タイプ
	int				m_shadow;					// 影番号
	int				m_invincible;				// 無敵時間
	float			m_consumption;				// 計算用

	bool			m_bMotionBlend;				// モーションブレンド
	bool			m_bMotion;					// モーションを使用状況

	bool			m_isUse;					// 使ってるか使ってないか
	bool			m_isLoop;					// ループするかしないか

	char			m_aFirename[256];//

	int				m_time;		// タイムの最大数
	int				m_nparts;	// パーツの最大数
	int				m_pow;		// ジャンプパワー
	int				m_nMotion;	// 
};
#endif
