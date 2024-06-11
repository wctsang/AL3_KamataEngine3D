#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete skydome_;

	//delete blocks

	for (std::vector<WorldTransform*>& worldTransforBlockLine : worldTransformBlock_) {
		for (WorldTransform* worldTransformBlock : worldTransforBlockLine) {
			delete worldTransformBlock;
		}
		worldTransformBlock_.clear();
	}

	delete debugCamera_;

	delete mapChipField_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//マップチップ

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");
	GenerateBlocks();

	// テクスチャを読み込む

	textureHandle_ = TextureManager::Load("cube/cube.jpg");

	//デバッグカメラ

	debugCamera_ = new DebugCamera(1280, 720);

	//ファイル名を指定してテクスチャを読み込む

	textureHandle_ = TextureManager::Load("mario.png");

	// 3Dモデルの生成

	model_ = Model::Create();
	modelBlock_ = Model::Create();
	modelSkydome_ = Model::CreateFromOBJ("sphere", true);

	// ビュープロジェクションの初期化

	viewProjection_.farZ = 5000;
	viewProjection_.Initialize();

	// 自キャラの生成
	//player_ = new Player();
	// 自キャラの初期化
	//player_->Initialise(model_, textureHandle_, &viewProjection_);

	//天球生成
	skydome_ = new Skydome;
	skydome_->Initialize(modelSkydome_, &viewProjection_);
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

void GameScene::GenerateBlocks() {
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	worldTransformBlock_.resize(numBlockVertical);
	for (uint32_t i = 0; i < numBlockVertical; i++) {
		worldTransformBlock_[i].resize(numBlockHorizontal);
	}
	for (uint32_t i = 0; i < numBlockVertical; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlock_[i][j] = worldTransform;
				worldTransformBlock_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
};

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

	//天球描画

	skydome_->Draw();

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
