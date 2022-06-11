//============================
//
// プレイヤー設定
// Author:hamada ryuuga
//
//============================

#include <stdio.h>
#include "Player.h"
#include "input.h"
#include "camera.h"
#include "motion.h"

//------------------------------------
// static変数
//------------------------------------
const float CPlayer::ATTENUATION = 0.5f;	// 
const float CPlayer::SPEED = 1.0f;			// 移動量
const float CPlayer::WIDTH = 10.0f;			// モデルの半径
const int CPlayer::MAX_PRAYER = 16;			// 最大数
const int CPlayer::MAX_MOVE = 9;			// アニメーションの最大数
const int CPlayer::INVINCIBLE = 300;		// 無敵時間
const int CPlayer::MAX_COPY = 4;			// 最大コピー数

//------------------------------------
// コンストラクタ
//------------------------------------
CPlayer::CPlayer() : 
	m_pos(D3DXVECTOR3(0.0f,0.0f,0.0f)),
	m_posOld(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_move(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_rot(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_rotMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_modelMin(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	m_modelMax(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
	//m_mtxWorld(),
	m_status(STATUS_NORMAL),
	m_damege(DAMEGE_NORMAL),
	m_copy(COPY_NORMAL),
	//m_parts(),
	//m_partsFile(),
	//m_motion(),
	m_motionType(ANIME_NORMAL),
	m_motionTypeOld(ANIME_NORMAL),
	m_nMaxModelType(0),
	m_nMaxModelParts(0),
	m_nMaxMotion(0),
	m_type(0),
	m_shadow(0),
	m_invincible(0),
	m_consumption(0.0f),
	m_bMotionBlend(false),
	m_bMotion(false),
	m_isUse(false),
	m_isLoop(false),
	//m_aFirename(),
	m_time(0),
	m_nparts(0),
	m_pow(0),
	m_nMotion(0)
{
	//memset(&s_Player, NULL, sizeof(s_Player));

}

//------------------------------------
// デストラクタ
//------------------------------------
CPlayer::~CPlayer()
{

}

//------------------------------------
// 初期化
//------------------------------------
void CPlayer::Init(void)
{
	//カメラのデータ取得
	Camera *pCamera = GetCamera();
	m_rotMove = D3DXVECTOR3(D3DX_PI + pCamera->rot.y, D3DX_PI * 0.5f + pCamera->rot.y, 0.0f);

	m_modelMin = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_modelMax = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	m_motionTypeOld = m_motionType;

	Set(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

//------------------------------------
// 終了
//------------------------------------
void CPlayer::Uninit(void)
{
	for (int i = 0; i < MAX_MODELPARTS; i++)
	{
		// 頂点バッファーの解放
		if (m_parts[i].pBuffer != NULL)
		{
			m_parts[i].pBuffer->Release();
			m_parts[i].pBuffer = NULL;
		}
		if (m_parts[i].pMesh != NULL)
		{
			m_parts[i].pMesh->Release();
			m_parts[i].pMesh = NULL;
		}
	}
}

//------------------------------------
// 更新
//------------------------------------
void CPlayer::Update(void)
{
	// 現在のモーション番号の保管
	m_motionTypeOld = m_motionType;

	if (!m_bMotion)
	{// 使用してるモーションがない場合
		m_motionType = ANIME_NORMAL;
		m_isLoop = false;
	}

	Move();	//動きセット

	Collision();//床

	// //アニメーションや足音の設定
	if (!m_isLoop)
	{
		m_motionType = ANIME_NORMAL;
	}
	if (GetKeyboardPress(DIK_A) || GetKeyboardPress(DIK_W) || GetKeyboardPress(DIK_D) || GetKeyboardPress(DIK_S) ||
		GetJoypadPress(JOYKEY_UP, 0) || GetJoypadPress(JOYKEY_DOWN, 0) || GetJoypadPress(JOYKEY_LEFT, 0) || GetJoypadPress(JOYKEY_RIGHT, 0))
	{
		m_motionType = ANIME_RUN;//歩く

	}

	if (GetKeyboardPress(DIK_SPACE) || GetJoypadPress(JOYKEY_A, 0) || GetJoypadPress(JOYKEY_UP, 0))
	{//SPACEキーが押された
		m_pos.y = m_pos.y + 5.0f;
		m_pow++;
		m_motionType = ANIME_JUMP;
	}
	else
	{
		m_pow = 0;
	}
	if (m_pow >= 1 && m_pow <= 10)
	{//ジャンプシステム
		m_pow++;
		m_move.y = 1.00f* m_pow;
	}


	m_move.y -= 1.0f;
	if (GetKeyboardPress(DIK_B))
	{
		//cLnhale[0].Set(m_pos);
		m_motionType = ANIME_ATTACK;//攻撃

		m_bMotion = true;
	}
	if (GetKeyboardPress(DIK_N))
	{
		m_isLoop = true;
	}
	if (GetKeyboardPress(DIK_J))
	{

		m_copy = COPY_SWORD;
		// ファイルの読み込み
		SetCopy("Data/system/Gon/sword.txt", &m_partsFile[7], &m_parts[7], &m_motion[0], &m_nMaxModelParts);
	}
	if (GetKeyboardPress(DIK_H))
	{
		m_copy = COPY_FIRE;
		// ファイルの読み込み
		SetCopy("Data/system/Gon/flare.txt", &m_partsFile[7], &m_parts[7], &m_motion[0], &m_nMaxModelParts);
	}
	if (GetKeyboardPress(DIK_F))
	{
		m_copy = COPY_LASER;
		// ファイルの読み込み	
		SetCopy("Data/system/Gon/Laser.txt", &m_partsFile[7], &m_parts[7], &m_motion[0], &m_nMaxModelParts);
	}
	if (GetKeyboardPress(DIK_G))
	{
		m_copy = COPY_CUTTER;
		// ファイルの読み込み
		SetCopy("Data/system/Gon/Cutter.txt", &m_partsFile[7], &m_parts[7], &m_motion[0], &m_nMaxModelParts);
	}
	if (GetKeyboardPress(DIK_K))
	{
		m_copy = COPY_NORMAL;
		m_nMaxModelParts = 7;
	}

	if (m_motionTypeOld != m_motionType)
	{// モーションタイプが変更された時
		m_motion[m_motionTypeOld].nCntFrame = 0;
		m_motion[m_motionTypeOld].nCntKeySet = 0;
		m_bMotionBlend = true;
	}

	if (m_bMotionBlend)
	{// モーションブレンドを使用してる
		m_bMotionBlend = MotionBlend((int)(m_motionType),		// モーションの配列番号
			&m_parts[0],												// パーツ情報
			m_nMaxModelParts,										// パーツ数
			&m_motion[0]);											// モーション情報	
	}
	else if (!m_bMotionBlend)
	{// モーションブレンドを使用してない場合
		m_bMotion = PlayMotion(m_nMaxModelParts,				// パーツ数
			&m_parts[0],												// パーツ情報
			&m_motion[(int)(m_motionType)]);					// モーション情報
	}

}

//------------------------------------
// 描画
//------------------------------------
void CPlayer::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScale, mtxTrans, mtxRot;	// 計算用マトリックス
	D3DMATERIAL9 marDef;
	D3DXMATERIAL *pMat = {};
	D3DXVECTOR3 scale(1.8f, 1.8f, 1.8f);

	if (m_isUse)//使ってるやつ出す
	{
		// ワールドマトリックスの初期化
		// 行列初期化関数(第1引数の行列を単位行列に初期化)
		D3DXMatrixIdentity(&m_mtxWorld);

		// 拡縮を反映
		// 行列拡縮関数
		D3DXMatrixScaling(&mtxScale, scale.x, scale.y, scale.z);
		// 行列掛け算関数(第2引数×第3引数第を１引数に格納)
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

		// 向きを反映
		// 行列回転関数(第1引数にヨー(y)ピッチ(x)ロール(z)方向の回転行列を作成)
		D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
		// 行列掛け算関数(第2引数×第3引数第を１引数に格納)
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

		// 位置を反映
		// 行列移動関数(第１引数にX,Y,Z方向の移動行列を作成)
		D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
		// 行列掛け算関数(第2引数×第3引数第を１引数に格納)
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

		// テクスチャの設定
		pDevice->SetTexture(0, NULL);

		// パーツの描画設定
		SetParts(m_nMaxModelParts,	// パーツ数
			&m_parts[0],			// パーツ情報
			m_mtxWorld,				// ワールドマトリックス
			mtxRot,					// 計算用マトリックス
			mtxTrans,				// 計算用マトリックス
			&marDef,				// マテリアル保存変数
			pMat);					// マテリアルデータ

		// 行列掛け算関数(第2引数×第3引数第を１引数に格納)
		pDevice->SetMaterial(&marDef);
	}
}

//------------------------------------
// 設定
//------------------------------------
void CPlayer::Set(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// プレイヤー情報の初期化
	m_pos = pos;											// 位置の初期化
	m_posOld = m_pos;								// 過去位置の初期化
	m_posOld = m_pos;								// 過去位置の初期化
	m_rot = rot;											// 向きの初期化
	m_modelMin = D3DXVECTOR3(100.0f, 100.0f, 100.0f);	// 頂点座標の最小値
	m_modelMax = D3DXVECTOR3(-100.0f, -100.0f, -100.0f);	// 頂点座標の最大値
	m_mtxWorld = {};										// ワールドマトリックス
															//m_rotDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 目的の向き
	m_motionType = ANIME_NORMAL;							// ニュートラルモーション
	m_motionTypeOld = m_motionType;				// ニュートラルモーション
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量
	m_bMotionBlend = false;								// モーションブレンドの使用状況
	m_isUse = true;										// 使用状況

	// ファイルの読み込み
	LoodSetMotion("Data/system/Gon/Fox.txt", m_partsFile, m_parts, m_motion, &m_nMaxModelParts);

	for (int i = 0; i < m_nMaxModelParts; i++)
	{
		Parts* pParts = &m_parts[i];

		// 位置と向きの初期値を保存
		pParts->posOrigin = pParts->pos;
		pParts->rotOrigin = pParts->rot;

		// パーツ情報の初期化
		pParts->mtxWorld = {};										// ワールドマトリックス
		pParts->vtxMin = D3DXVECTOR3(1000.0f, 1000.0f, 1000.0f);		// 頂点座標の最小値
		pParts->vtxMax = D3DXVECTOR3(-1000.0f, -1000.0f, -1000.0f);	// 頂点座標の最大値

		// Xファイルの読み込み
		D3DXLoadMeshFromX(m_partsFile[pParts->nType].aName,
			D3DXMESH_SYSTEMMEM,
			GetDevice(),
			NULL,
			&pParts->pBuffer,
			NULL,
			&pParts->nNumMat,
			&pParts->pMesh);

		// 頂点座標の最小値・最大値の算出
		int nNumVtx;	// 頂点数
		DWORD sizeFVF;	// 頂点フォーマットのサイズ
		BYTE *pVtxBuff;	// 頂点バッファへのポインタ

		// 頂点数の取得
		nNumVtx = pParts->pMesh->GetNumVertices();

		// 頂点フォーマットのサイズの取得
		sizeFVF = D3DXGetFVFVertexSize(pParts->pMesh->GetFVF());

		// 頂点バッファのロック
		pParts->pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{
			// 頂点座標の代入
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

			if (vtx.x < pParts->vtxMin.x)
			{// 比較対象が現在の頂点座標(X)の最小値より小さい
				pParts->vtxMin.x = vtx.x;
			}
			if (vtx.y < pParts->vtxMin.y)
			{// 比較対象が現在の頂点座標(Y)の最小値より小さい
				pParts->vtxMin.y = vtx.y;
			}
			if (vtx.z < pParts->vtxMin.z)
			{// 比較対象が現在の頂点座標(Z)の最小値より小さい
				pParts->vtxMin.z = vtx.z;
			}

			if (vtx.x > pParts->vtxMax.x)
			{// 比較対象が現在の頂点座標(X)の最大値より大きい
				pParts->vtxMax.x = vtx.x;
			}
			if (vtx.y > pParts->vtxMax.y)
			{// 比較対象が現在の頂点座標(Y)の最大値より大きい
				pParts->vtxMax.y = vtx.y;
			}
			if (vtx.z > pParts->vtxMax.z)
			{// 比較対象が現在の頂点座標(Z)の最大値より大きい
				pParts->vtxMax.z = vtx.z;
			}

			// 頂点フォーマットのサイズ分ポインタを進める
			pVtxBuff += sizeFVF;
		}

		if (pParts->vtxMin.x < m_modelMin.x)
		{// 比較対象が現在の頂点座標(X)の最小値より小さい
			m_modelMin.x = pParts->vtxMin.x;
		}
		if (pParts->vtxMin.y < m_modelMin.y)
		{// 比較対象が現在の頂点座標(Y)の最小値より小さい
			m_modelMin.y = pParts->vtxMin.y;
		}
		if (pParts->vtxMin.z < m_modelMin.z)
		{// 比較対象が現在の頂点座標(Z)の最小値より小さい
			m_modelMin.z = pParts->vtxMin.z;
		}

		if (pParts->vtxMax.x > m_modelMax.x)
		{// 比較対象が現在の頂点座標(X)の最大値より大きい
			m_modelMax.x = pParts->vtxMax.x;
		}
		if (pParts->vtxMax.y > m_modelMax.y)
		{// 比較対象が現在の頂点座標(Y)の最大値より大きい
			m_modelMax.y = pParts->vtxMax.y;
		}
		if (pParts->vtxMax.z > m_modelMax.z)
		{// 比較対象が現在の頂点座標(Z)の最大値より大きい
			m_modelMax.z = pParts->vtxMax.z;
		}

		// 頂点バッファのアンロック
		pParts->pMesh->UnlockVertexBuffer();
	}
}

//------------------------------------
// 移動処理
//------------------------------------
void CPlayer::Move(void)
{
	// 移動係数の宣言
	int nDash = 1;
	if (GetKeyboardPress(DIK_LSHIFT) || GetKeyboardPress(DIK_RSHIFT) || GetJoypadPress(JOYKEY_A, 0))
	{//シフトダッシュ
		nDash = 3;
	}

	//ゲームの時の移動方法
	Camera *pCamera = GetCamera();
	m_consumption = 0.0f;

	if (GetJoypadStick(JOYKEY_LEFT_STICK, 0).x > 0.5f)
	{//右に半分より多く倒したら
		m_move.x += sinf(pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - m_rot.y + pCamera->rot.y;
	}
	else if (GetJoypadStick(JOYKEY_LEFT_STICK, 0).x < -0.5f)
	{//左に半分より多く倒したら
		m_move.x += sinf(pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - m_rot.y + pCamera->rot.y;
	}
	else if (GetJoypadStick(JOYKEY_LEFT_STICK, 0).y > 0.5f)
	{//上に半分より多く倒したら
		m_move.x += sinf(pCamera->rot.y - D3DX_PI) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y - D3DX_PI) * SPEED * nDash;

		m_consumption = (m_rotMove.x - D3DX_PI - m_rot.y + pCamera->rot.y);
	}
	else if (GetJoypadStick(JOYKEY_LEFT_STICK, 0).y < -0.5f)
	{//下に半分より多く倒したら
		m_move.x += sinf(pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - m_rot.y + pCamera->rot.y;
	}
	if (GetKeyboardPress(DIK_W) || GetJoypadPress(JOYKEY_UP, 0))
	{//W押したとき上加速

		m_move.x += sinf(pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - m_rot.y + pCamera->rot.y;
	}
	if (GetKeyboardPress(DIK_S) || GetJoypadPress(JOYKEY_DOWN, 0))
	{//S押したと下加速

		m_move.x += sinf(pCamera->rot.y - D3DX_PI) * SPEED * nDash;
		m_move.z += cosf(pCamera->rot.y - D3DX_PI) * SPEED * nDash;

		m_consumption = (m_rotMove.x - D3DX_PI - m_rot.y + pCamera->rot.y);
	}
	if (GetKeyboardPress(DIK_D) || GetJoypadPress(JOYKEY_RIGHT, 0))
	{//D押したとき右加速

		m_move.x += sinf(D3DX_PI * 0.5f + pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(D3DX_PI * 0.5f + pCamera->rot.y) * SPEED * nDash;


		m_consumption = m_rotMove.x + (D3DX_PI*0.5f) - m_rot.y + pCamera->rot.y;
	}

	if (GetKeyboardPress(DIK_A) || GetJoypadPress(JOYKEY_LEFT, 0))
	{//A押したと左加速

		m_move.x += sinf(-D3DX_PI * 0.5f + pCamera->rot.y) * SPEED * nDash;
		m_move.z += cosf(-D3DX_PI * 0.5f + pCamera->rot.y) * SPEED * nDash;

		m_consumption = m_rotMove.x - (D3DX_PI*0.5f) - m_rot.y + pCamera->rot.y;

	}
	m_posOld = m_pos;//過去の移動量を保存

	if (m_invincible <= 0)
	{//無敵時間がゼロになったらダメージくらうようにする
		m_damege = DAMEGE_NORMAL;
	}

	//（目的の値-現在の値）＊減衰係数
	m_move.x += (0.0f - m_move.x) * 0.5f;
	m_move.z += (0.0f - m_move.z) * 0.5f;

	m_pos += m_move;	// 移動を加算

	// 正規化
	if (m_consumption > D3DX_PI)
	{
		m_consumption += D3DX_PI * 2;
	}
	if (m_consumption < -D3DX_PI)
	{
		m_consumption += -D3DX_PI * 2;
	}

	//減算設定（慣性）
	m_rot.y += m_consumption * ATTENUATION;	//目的の値-現在の値）＊減衰係数

	//正規化
	if (m_rot.y > D3DX_PI)
	{
		m_rot.y += -D3DX_PI * 2;
	}
	if (m_rot.y <= -D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2;
	}
}

//------------------------------------
// 当たり判定
//------------------------------------
void CPlayer::Collision(void)
{
	if (m_pos.y <= 0.0f)
	{
		m_pos.y = 0.0f;
	}
}

//------------------------------------
//コピーしたときモーションをロードする処理
//------------------------------------
void CPlayer::SetCopy(char * pFileName, PartsFile * partsFile, Parts * parts, MyMotion * Motion, int * nMaxParts)
{
	// 変数宣言
	char aString[128] = {};			// 文字列比較用の変数
	char g_aEqual[128] = {};		// ＝読み込み用変数
	int	nCntKeySet = 0;				// KeySetカウント
	int	nCntKey = 0;				// Keyカウント

	//-------------------------------
	//コピーを処理
	//-------------------------------
	if (m_nMaxModelParts >= 7)
	{
		m_nMaxModelParts = 7;
	}

	// ファイルポインタの宣言
	FILE* pFile = fopen(pFileName, "r");

	if (pFile != NULL)
	{
		fscanf(pFile, "%s", &aString);

		while (strncmp(&aString[0], "SCRIPT", 6) != 0)
		{// 文字列が一致した場合
			aString[0] = {};
			// 文字列の読み込み
			fscanf(pFile, "%s", &aString[0]);
		}
		while (strncmp(&aString[0], "END_SCRIPT", 10) != 0)
		{
			fscanf(pFile, "%s", &aString[0]);

			if (strncmp(&aString[0], "#", 1) == 0)
			{// 一列読み込む
				fgets(&aString[0], sizeof(aString), pFile);
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// ファイル名の読み込み
				fscanf(pFile, "%s", &g_aEqual[0]);
				fscanf(pFile, "%s", &partsFile->aName[0]);
			}

			if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{// キャラクターの読み込み
				while (strcmp(&aString[0], "END_CHARACTERSET") != 0)
				{
					fscanf(pFile, "%s", &aString[0]);

					if (strncmp(&aString[0], "#", 1) == 0)
					{// 一列読み込む
						fgets(&aString[0], sizeof(aString), pFile);
					}
					if (strcmp(&aString[0], "PARTSSET") == 0)
					{// パーツの読み込み
						while (strcmp(&aString[0], "END_PARTSSET") != 0)
						{
							fscanf(pFile, "%s", &aString[0]);

							if (strncmp(&aString[0], "#", 1) == 0)
							{// 一列読み込む
								fgets(&aString[0], sizeof(aString), pFile);
							}

							if (strcmp(&aString[0], "INDEX") == 0)
							{// タイプの読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%d", &parts->nType);
							}
							if (strcmp(&aString[0], "PARENT") == 0)
							{// 親の読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%d", &parts->nIdxModelParent);
							}
							if (strcmp(&aString[0], "POS") == 0)
							{// 位置の読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%f", &parts->pos.x);
								fscanf(pFile, "%f", &parts->pos.y);
								fscanf(pFile, "%f", &parts->pos.z);
							}
							if (strcmp(&aString[0], "ROT") == 0)
							{// 向きの読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%f", &parts->rot.x);
								fscanf(pFile, "%f", &parts->rot.y);
								fscanf(pFile, "%f", &parts->rot.z);
							}
						}
					}
				}
			}
			if (strcmp(&aString[0], "MOTIONSET") == 0)
			{// モーションの読み込み
				while (strcmp(&aString[0], "END_MOTIONSET") != 0)
				{
					fscanf(pFile, "%s", &aString[0]);

					if (strncmp(&aString[0], "#", 1) == 0)
					{// 一列読み込む
						fgets(&aString[0], sizeof(aString), pFile);
					}

					if (strcmp(&aString[0], "LOOP") == 0)
					{// ループ有無の読み込み
						fscanf(pFile, "%s", &g_aEqual[0]);
						fscanf(pFile, "%d", (int*)(&Motion->bLoop));
					}
					if (strcmp(&aString[0], "NUM_KEY") == 0)
					{// キー数の読み込み
						fscanf(pFile, "%s", &g_aEqual[0]);
						fscanf(pFile, "%d", &Motion->nNumKey);
					}
					if (strcmp(&aString[0], "KEYSET") == 0)
					{// キーセットの読み込み
						while (strcmp(&aString[0], "END_KEYSET") != 0)
						{
							fscanf(pFile, "%s", &aString[0]);

							if (strncmp(&aString[0], "#", 1) == 0)
							{// 一列読み込む
								fgets(&aString[0], sizeof(aString), pFile);
							}

							if (strcmp(&aString[0], "FRAME") == 0)
							{// フレーム数の読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%d", &Motion->keySet[nCntKeySet].nFrame);
							}
							if (strcmp(&aString[0], "KEY") == 0)
							{// キーの読み込み
								while (strcmp(&aString[0], "END_KEY") != 0)
								{
									fscanf(pFile, "%s", &aString[0]);

									if (strncmp(&aString[0], "#", 1) == 0)
									{// 一列読み込む
										fgets(&aString[0], sizeof(aString), pFile);
									}

									if (strcmp(&aString[0], "POS") == 0)
									{// 位置の読み込み
										fscanf(pFile, "%s", &g_aEqual[0]);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].pos.x);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].pos.y);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].pos.z);
									}
									if (strcmp(&aString[0], "ROT") == 0)
									{// 向きの読み込み
										fscanf(pFile, "%s", &g_aEqual[0]);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].rot.x);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].rot.y);
										fscanf(pFile, "%f", &Motion->keySet[nCntKeySet].key[nCntKey].rot.z);
									}
								}

								// キーカウントのインクリメント
								nCntKey++;
							}
						}

						// キーカウントの初期化
						nCntKey = 0;

						// キーセットカウントのインクリメント
						nCntKeySet++;
					}
				}
				// キーセットカウントの初期化
				nCntKeySet = 0;

				// パーツ情報のインクリメント
				Motion++;
			}

		}

		//ファイルを閉じる
		fclose(pFile);
	}
	else
	{//ファイルが開けない場合
		printf("\n * * * ファイルが開けません * * * \n");
	}

	// 位置と向きの初期値を保存
	m_parts[m_nMaxModelParts].posOrigin = m_parts[m_nMaxModelParts].pos;
	m_parts[m_nMaxModelParts].rotOrigin = m_parts[m_nMaxModelParts].rot;

	// Xファイルの読み込み
	D3DXLoadMeshFromX(&m_partsFile[m_parts[m_nMaxModelParts].nType].aName[0],
		D3DXMESH_SYSTEMMEM,
		GetDevice(),
		NULL,
		&m_parts[m_nMaxModelParts].pBuffer,
		NULL,
		&m_parts[m_nMaxModelParts].nNumMat,
		&m_parts[m_nMaxModelParts].pMesh);

	m_nMaxModelParts++;
}
