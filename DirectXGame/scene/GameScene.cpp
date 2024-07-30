#include "GameScene.h"
#include "TextureManager.h"
#include "myMath.h"
//#include "CameraControl.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;

	//delete modelPlayer_;

	//delete blocks

	for (std::vector<WorldTransform*>& worldTransforBlockLine : worldTransformBlock_) {
		for (WorldTransform* worldTransformBlock : worldTransforBlockLine) {
			delete worldTransformBlock;
		}
	}

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	worldTransformBlock_.clear();

	delete debugCamera_;

	delete mapChipField_;

	delete cameraControl_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//ファイル名を指定してテクスチャを読み込む

//	textureHandle_ = TextureManager::Load("block.png");

	// 3Dモデルの生成

	model_ = Model::Create();
	modelBlock_ = Model::CreateFromOBJ("block");

	// ワールドトランスフォームの初期化

	worldTransform_.Initialize();

	//ビュープロジェクションの初期化

	viewProjection_.Initialize();

	// 自キャラの生成

	player_ = new Player();

	// 自キャラの初期化

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);

	player_->Initialize(playerPosition, &viewProjection_);

	//敵生成

	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

	Vector3 enemyPosition[enemyCount];

	for (uint32_t i = 0; i < enemyCount; ++i) {
		Enemy* newEnemy = new Enemy();
		enemyPosition[i] = mapChipField_->GetMapChipPositionByIndex(10 - i * 3, 18);
		newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPosition[i]);
		enemies_.push_back(newEnemy);
	}

	//天球3Dモデル生成

	skydome_ = new Skydome;

	modelSkydome_ = Model::CreateFromOBJ("sphere", true);

	//天球のの初期化

	skydome_->Initialize(modelSkydome_, &viewProjection_);

	// デバッグカメラ

	debugCamera_ = new DebugCamera(1280, 720);

	//マップチップ

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");

	player_->SetMapChipField(mapChipField_);

	GenerateBlocks();

	// カメラ

	cameraControl_ = new CameraControl();
	cameraControl_->Initialize();
	cameraControl_->SetTarget(player_);
	cameraControl_->Reset();

	CameraControl::Rect cameraArea_ = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraControl_->SetMovableArea(cameraArea_);
}

void GameScene::Update() {

	#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		if (isDebugCameraActive_ == true)
			isDebugCameraActive_ = false;
		else
			isDebugCameraActive_ = true;
	}
#endif // _DEBUG

	// カメラの更新

	cameraControl_->Update();
	
	//カメラ処理

	if (isDebugCameraActive_ == true) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		viewProjection_.UpdateMatrix();
		viewProjection_.matView = cameraControl_->GetViewProjection().matView;
		viewProjection_.matProjection = cameraControl_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	}

	// 自キャラの更新

	player_->Update();

	//敵更新

	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	//衝突判定更新

	CheckAllCollisions();

	//天球の更新
	skydome_->Update();

	//縦横ブロック更新

	for (std::vector<WorldTransform*>& worldTransformBlockVertical : worldTransformBlock_) {
		for (WorldTransform* worldTransformBlockHorizontal : worldTransformBlockVertical) {
			if (!worldTransformBlockHorizontal)
				continue;

			//アフィン変換行列の作成
			worldTransformBlockHorizontal->matWorld_ = MakeAffineMatrix(worldTransformBlockHorizontal->scale_, worldTransformBlockHorizontal->rotation_, worldTransformBlockHorizontal->translation_);

			//定数バッファに転送

			worldTransformBlockHorizontal->UpdateMatrix();

		}
	}
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

void GameScene::CheckAllCollisions() {

	// プレイヤーと敵の衝突判定

	AABB aabb1, aabb2;

	aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();
		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
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

	//天球描画
	skydome_->Draw();

	// 自キャラの描画
	player_->Draw();

	for (Enemy* enemy : enemies_) {
		if (enemy != nullptr) {
			enemy->Draw();
		}
	}

	//ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockVertical : worldTransformBlock_) {
		for (WorldTransform* worldTransformBlockHorizontal : worldTransformBlockVertical) {
			if (!worldTransformBlockHorizontal)
				continue;
			modelBlock_->Draw(*worldTransformBlockHorizontal, viewProjection_);
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
