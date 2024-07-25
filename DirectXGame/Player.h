#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class MapChipField;

class Player {
public:

	enum class LRDirection {
		kRight,
		kLeft,
	};

	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorners // 要素数
	};
	/// <summary>
	/// 初期化
	/// </summary>
	Player();
	~Player();
	void Initialize(const Vector3& position, ViewProjection* viewProjection);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void InputMove();

	WorldTransform& GetWorldTransform() { return worldTransform_; }

	Vector3 CornerPostion(const Vector3& centre, Corner corner);
	void Turn();

	private:
	
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};
	
	// ワールド変換データ

	WorldTransform worldTransform_;

	//モデル

	Model* model_ = nullptr;

	// テクスチャハンドル

	uint32_t textureHandle_ = 0u;

	ViewProjection* viewProjection_ = nullptr;

	// 移動

	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.01f;
	static inline const float kLimitRunSpeed = 0.5f;
	static inline const float kTimeTurn_ = 0.3f;
	LRDirection lrDirection_ = LRDirection::kRight;

	//旋回開始時の角度

	float turnFirstRotationY_ = 0.0f;

	//旋回タイマー

	float turnTimer_ = 0.0f;

	//旋回時間<秒>

	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	bool landing = false;

	static inline const float kGravityAcceleration = 0.98f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 20.0f;

	// マップチップフィールド

	MapChipField* mapChipField_ = nullptr;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.04f;
	static inline const float kAttenuationWall = 0.2f;

	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionTop(CollisionMapInfo& info);
	void CheckMapCollisionBottom(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
};
