#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	//delete player_;
	for (std::vector<WorldTransform*>& worldTransforBlockLine : worldTransformBlock_) {
		for (WorldTransform* worldTransformBlock : worldTransforBlockLine) {
			delete worldTransformBlock;
		}
		worldTransformBlock_.clear();
	}
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ワールド、ビューの初期化

	/*worldTransform_.Initialize();*/

	viewProjection_.Initialize();

	//要素数

	const uint32_t kNumBlockVirtical = 10;

	const uint32_t kNumBlockHorizontal = 20;

	//ブロック1個分の横幅

	const float kBlockWidth = 2.0f;

	const float kBlockHeight = 2.0f;

	//マップチップ

	int map[kNumBlockVirtical][kNumBlockHorizontal] = {
	    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	    {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	    {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
	};

	//要素数を変更する

	worldTransformBlock_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		worldTransformBlock_[i].resize(kNumBlockHorizontal);
	}


	//キューブの生成

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (map[i][j] == 1) {
				worldTransformBlock_[i][j] = new WorldTransform();
				worldTransformBlock_[i][j]->Initialize();
				worldTransformBlock_[i][j]->translation_.x = kBlockWidth * j;
				worldTransformBlock_[i][j]->translation_.y = kBlockHeight * i;
			}
		}
	}

	//デバッグカメラ

	debugCamera_ = new DebugCamera(1280, 720);

	//ファイル名を指定してテクスチャを読み込む

	textureHandle_ = TextureManager::Load("mario.png");

	// 3Dモデルの生成

	model_ = Model::Create();
	modelBlock_ = Model::Create();

	// ワールドトランスフォームの初期化

	worldTransform_.Initialize();

	// ビュープロジェクションの初期化

	viewProjection_.Initialize();

	// 自キャラの生成
	//player_ = new Player();
	// 自キャラの初期化
	//player_->Initialise(model_, textureHandle_, &viewProjection_);
}

void GameScene::Update() {

	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

	if (isDebugCameraActive_ == true) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		viewProjection_.UpdateMatrix();
	}

	for (std::vector<WorldTransform*>& worldTransforBlockLine : worldTransformBlock_) {
		for (WorldTransform* worldTransformBlock : worldTransforBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->UpdateMatrix();
		}
	}

	debugCamera_->Update();


	// 自キャラの更新
	//player_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 自キャラの描画
	//player_->Draw();

	//ブロックの描画
	for (std::vector<WorldTransform*>& worldTransforBlockLine : worldTransformBlock_) {
		for (WorldTransform* worldTransformBlock : worldTransforBlockLine) {
			if (!worldTransformBlock)
				continue;
			model_->Draw(*worldTransformBlock, viewProjection_);
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
