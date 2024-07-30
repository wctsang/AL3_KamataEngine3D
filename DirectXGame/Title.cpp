#include "Title.h"

void Title::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {10.0f, 10.0f, 10.0f};
	worldTransform_.rotation_.x = -1 * std::numbers::pi_v<float> / 2.0f;
	viewProjection_ = viewProjection;
}

void Title::Update() {
	if (deltaTime > 2.0f) {
		deltaTime = 0.0f;
	} else {
		deltaTime += 1.0f / 60.0f;
	}
	worldTransform_.translation_.y = 2.0f * std::sin(deltaTime * std::numbers::pi_v<float> / 2.0f);
	worldTransform_.UpdateMatrix();
}

void Title::Draw() { model_->Draw(worldTransform_, *viewProjection_); }