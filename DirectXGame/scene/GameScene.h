#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "DebugCamera.h"
#include "Skydome.h"
#include "MapChipField.h"
#include "CameraControl.h"
#include "Enemy.h"
#include "DeathParticles.h"
#include <vector>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	bool IsFinished() const { return finished_; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	// テクスチャハンドル

	uint32_t flagHandle_ = 0;
	Sprite* sprite_ = nullptr;

	// 3Dモデル

	Model* model_ = nullptr;
	Model* modelBlock_ = nullptr;
	

	// ワールドトランスフォーム

	WorldTransform worldTransform_;

	// ビュープロジェクション

	ViewProjection viewProjection_;

	// 自キャラ
	Player* player_;

	// 縦横ブロック配列

	std::vector<std::vector<WorldTransform*>> worldTransformBlock_;

	// デバッグカメラ

	bool isDebugCameraActive_ = false;

	DebugCamera* debugCamera_ = nullptr;

	// 天球

	Skydome* skydome_ = nullptr;

	Model* modelSkydome_ = nullptr;	

	CameraControl* cameraControl_ = nullptr;

	// マップチップ

	MapChipField* mapChipField_ = nullptr;

	//敵

	Model* modelEnemy_ = nullptr;

	std::list<Enemy*> enemies_;
	static inline const uint32_t enemyCount = 3;

	//パーティクル

	Model* modelParticle_ = nullptr;

	DeathParticles* deathParticles_ = nullptr;

	//フェーズ

	enum class Phase {
		kPlay,
		kDeath,
	};

	Phase phase_;

	bool finished_ = false;

};
