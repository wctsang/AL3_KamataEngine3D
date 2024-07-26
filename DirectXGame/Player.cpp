#define NOMINMAX
#include "Player.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "MapChipField.h"
#include "MathUtilityForText.h"
#include "myMath.h"
#include <algorithm>
#include <cassert>
#include <numbers>

Player::Player(){};

Player::~Player(){};

void Player::Initialize(const Vector3& position, ViewProjection* viewProjection) {

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	viewProjection_ = viewProjection;

	// 右を向かせる
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// 引数の内容をメンバ変数に記録
	model_ = Model::CreateFromOBJ("player", true);
	// textureHandle_ = textureHandle;
}

/* void Player ::playerMove() {
    if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
        Vector3 acceleration = {};
        if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
            if (velocity_.x < 0.0f) {
                velocity_.x *= (1.0f - kAttenuation);
            }
            acceleration.x += kAcceleration;
            if (lrDirection_ != LRDirection::kRight) {
                lrDirection_ = LRDirection::kRight;
                turnFirstRotationY_ = worldTransform_.rotation_.y;
                turnTimer_ = 0.2f;
            }
        } else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
            if (velocity_.x > 0.0f) {
                velocity_.x *= (1.0f - kAttenuation);
            }
            acceleration.x -= kAcceleration;
            if (lrDirection_ != LRDirection::kLeft) {
                lrDirection_ = LRDirection::kLeft;
                turnFirstRotationY_ = worldTransform_.rotation_.y;
                turnTimer_ = 0.2f;
            }
        }

        // 加減速

        velocity_ = Add(velocity_, acceleration);

        // 制限速度

        velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
    } else {
        velocity_.x *= (1.0f - kAttenuation);
    }
}*/

void Player::InputMove() {
	// 移動入力
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					// 旋回開始時の角度
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマー
					turnTimer_ = 0.3f;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					// 旋回開始時の角度
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマー
					turnTimer_ = 0.3f;
				}
			}
			// 加速/減速
			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;
			if (acceleration.x >= 0.01f || acceleration.x <= -0.01f) {
				acceleration.x = 0;
			}
			
		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
		}

			// 最大速度制限
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	} else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		// 着地フラグ
		landing = false;
		// 地面との当たり判定
		// 下降中？
		if (velocity_.y < 0) {
			// Y座標が地面以下になったら着地
			if (worldTransform_.translation_.y <= 1.0f) {
				landing = true;
			}
		}
	}
	// 移動
	//worldTransform_.translation_ += velocity_;
	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}
}

void Player::Turn() {
	if (turnTimer_ > 0.0f) {
		// タイマーのカウントダウン
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
//		worldTransform_.rotation_.y = (turnFirstRotationY_ * (1 - turnTimer_)) + (destinationRotationY * (turnTimer_));
		// Liner(destinationRotationY, turnFirstRotationY_, EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_));
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_/kTimeTurn);
	}
}

void Player::Update() {

	InputMove();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;
	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 移動
	worldTransform_.translation_ += collisionMapInfo.move;
	// 天井接触による落下開始
	if (collisionMapInfo.ceiling) {
		velocity_.y = 0;
	}
	// 壁接触による減速
	if (collisionMapInfo.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}

	// 接地判定
	//  UpdateOnGround(collisionMapInfo);
	//

    Turn();
	
	//  行列計算
	worldTransform_.UpdateMatrix();
	// 行列を定数バッファに転送
//	worldTransform_.TransferMatrix();
}

/* void Player::Update() {
    bool landing = false; // 着地フラグ

    if (velocity_.y < 0.0f) {
        if (worldTransform_.translation_.y <= 2.0f) {
            landing = true;
        }
    }

    if (onGround_) {
        if (velocity_.y > 0.0f) {
            onGround_ = false;
        }
    } else {
        // 着地
        if (landing) {
            // めり込み排斥
            worldTransform_.translation_.y = 2.0f;
            // 摩擦で横方向速度が減衰する
            velocity_.x *= (1.0f - kAttenuation);
            // 下方向速度をリセット
            velocity_.y = 0.0f;
            // 接地状態に移行
            onGround_ = true;
        }
    }
    if (onGround_) {
        if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

            Vector3 acceleration = {};
            if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
                if (velocity_.x < 0.0f) {
                    velocity_.x *= (1.0f - kAttenuation);
                }
                acceleration.x += kAcceleration;
                if (lrDirection_ != LRDirection::kRight) {
                    lrDirection_ = LRDirection::kRight;
                    turnFirstRotationY_ = worldTransform_.rotation_.y;
                    turnTimer_ = 0.2f;
                }
            } else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
                if (velocity_.x > 0.0f) {
                    velocity_.x *= (1.0f - kAttenuation);
                }
                acceleration.x -= kAcceleration;
                if (lrDirection_ != LRDirection::kLeft) {
                    lrDirection_ = LRDirection::kLeft;
                    turnFirstRotationY_ = worldTransform_.rotation_.y;
                    turnTimer_ = 0.2f;
                }
            }

            // 加減速

            velocity_ = Add(velocity_, acceleration);

            // 制限速度

            velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
            if (acceleration.x >= 0.01f || acceleration.x <= -0.01f) {
                acceleration.x = 0;
            }
            if (turnTimer_ > 0.0f) {
                // タイマーのカウントダウン
                turnTimer_ -= 1.0f / 60.0f;
                // 左右の自キャラ角度テーブル
                float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
                // 状態に応じた角度を取得する
                float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
                // 自キャラの角度を設定する
                worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
            }
        } else {
            velocity_.x *= (1.0f - kAttenuation);
        }

        if (Input::GetInstance()->PushKey(DIK_UP)) {
            velocity_ = Add(velocity_, {0.0f, kJumpAcceleration, 0.0f});
        }
    } else // 空中
    {
        velocity_ = Add(velocity_, Vector3(0, -kGravityAcceleration, 0));
        velocity_.y = std::max(velocity_.y, -1 * kLimitFallSpeed);
        landing = false;
        if (velocity_.y < 0) {
            if (worldTransform_.translation_.y <= 1.0f) {
                worldTransform_.translation_.y = 1.0f;
                landing = true;
            }
        }
    }

    worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

    // 旋回制御

    if (turnTimer_ > 0.0f) {
        turnTimer_ -= 1.0f / 60;
        float destinationRotationYTable[] = {
            std::numbers::pi_v<float> / 2.0f,
            std::numbers::pi_v<float> * 3.0f / 2.0f,
        };

        // 角度取得

        float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
        float timeRatio = 1 - turnTimer_ / kTimeTurn;
        float easing = timeRatio;
        float nowRotationY = std::lerp(turnFirstRotationY_, destinationRotationY, easing);
        worldTransform_.rotation_.y = nowRotationY;
    }

    /// 衝突

    CollisionMapInfo collisionMapInfo;
    collisionMapInfo.move = velocity_;
    MapCollision(collisionMapInfo);

    // 畫面制限

    if (worldTransform_.translation_.x > 16) {
        worldTransform_.translation_.x = 16;
    }

    if (worldTransform_.translation_.x < 2) {
        worldTransform_.translation_.x = 2;
    }

    worldTransform_.UpdateMatrix();
    worldTransform_.TransferMatrix();
}*/

void Player::Draw() {

	// 3Dモデルを描画

	model_->Draw(worldTransform_, *viewProjection_);
}

Vector3 Player::CornerPostion(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorners] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };
	return Add(center, offsetTable[static_cast<uint32_t>(corner)]);
};

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionTop(info);
	CheckMapCollisionBottom(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionTop(CollisionMapInfo& info) {

	if (info.move.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorners> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPostion(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	bool hit = false;

	// 左上点

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock /* && mapChipTypeNext != MapChipType::kBlock*/) {
		hit = true;
	}

	// 右上点

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock /* && mapChipTypeNext != MapChipType::kBlock*/) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));

		//めり込み先ブロックの範囲矩形
		
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		
		// 移動量Y=上にいるブロックの下端-プレイヤー-Y座標-(プレイヤーの高さ/2+ブランク)
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		info.ceiling = true;
	}
}


void Player::CheckMapCollisionBottom(CollisionMapInfo& info) {
	if (info.move.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorners> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPostion(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	// 左下点
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		cameraStop = true;
	}
	// 右下点
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		cameraStop = true;
	}

	if (hit) {
		Vector3 offset = {0.0f, kHeight / 2.0f, 0.0f};
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(Subtract(Add(worldTransform_.translation_, info.move), offset));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float moveY = (rect.top - worldTransform_.translation_.y) + (kHeight / 2.0f) + kBlank;

		info.move.y = std::min(0.0f, moveY);
		info.landing = true;
	} else {
		info.landing = false;
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.move.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorners> positionsNew;
	for (uint32_t i = 0; i < kNumCorners; ++i) {
		positionsNew[i] = CornerPostion(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	// 右上角
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下角
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		Vector3 offset = {kWidth / 2.0f, 0.0f, 0.0f};
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(Add(worldTransform_.translation_, info.move), offset));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float moveX = rect.left - worldTransform_.translation_.x - kWidth / 2.0f - kBlank;
		info.move.x = std::max(0.0f, moveX);
		info.hitWall = true;
	} else {
		info.hitWall = false;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.move.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorners> positionsNew;
	for (uint32_t i = 0; i < kNumCorners; ++i) {
		positionsNew[i] = CornerPostion(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	// 左上角
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下角
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		Vector3 offset = {kWidth / 2.0f, 0.0f, 0.0f};
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(Subtract(Add(worldTransform_.translation_, info.move), offset));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float moveX = rect.right - worldTransform_.translation_.x + kWidth / 2.0f + kBlank;
		info.move.x = std::min(0.0f, moveX);
		info.hitWall = true;
	} else {
		info.hitWall = false;
	}
}

void Player::collisionResult(CollisionMapInfo& info) { worldTransform_.translation_ = Add(worldTransform_.translation_, info.move); }

void Player::isCeilingCollision(CollisionMapInfo& info) {
	if (info.ceiling) {
		velocity_.y = 0.0f;
	}
	if (info.ceiling) {
		velocity_.y = 0.0f;
	}
}

void Player::isWallCollision(CollisionMapInfo& info) {
	if (info.hitWall) {
		velocity_.x *= (1 - kAttenuationWall);
	}
}

void Player::isLandingCollision(CollisionMapInfo& info) {
	velocity_ = Add(velocity_, {0.0f, -kGravityAcceleration, 0.0f});
	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			const float kSmallOffset = 0.01f;
			std::array<Vector3, kNumCorners> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPostion(Add(worldTransform_.translation_, info.move), static_cast<Corner>(i));
			}

			if (info.move.y <= 0) {
				return;
			}

			MapChipType mapChipType;
			bool hit = false;
			MapChipField::IndexSet indexSet;

			// 左下
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kLeftBottom], Vector3(0, kSmallOffset, 0)));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
				cameraStop = true;
			}
			// 右下
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kRightBottom], Vector3(0, kSmallOffset, 0)));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
				cameraStop = true;
			}

			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		if (info.landing) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}

