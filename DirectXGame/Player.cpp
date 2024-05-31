#include "Player.h"
#include "cassert"

void Player::Initialise(Model *model, uint32_t textureHandle, ViewProjection *viewProjection) {
	//NULLポインタチェック
	assert(model);

	worldTransform_.Initialize();

	//引数の内容をメンバ変数に記録

	model_ = model;
	textureHandle_ = textureHandle;
	viewProjection_ = viewProjection;
}

void Player::Update() {

	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();

}

void Player::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);

}