#include "GameScene.h"

// 関数宣言

// 積

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// 移動

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 拡大縮小

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// 回転

Matrix4x4 MakeRotateXMatrix(float radian);

Matrix4x4 MakeRotateYMatrix(float radian);

Matrix4x4 MakeRotateZMatrix(float radian);

// アフィン変換

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);