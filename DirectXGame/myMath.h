#pragma once

#include <cmath>
#include <math.h>
#include "Vector3.h"
#include "Matrix4x4.h"

// 関数宣言

// V3加算
Vector3 Add(const Vector3& v1, const Vector3& v2);

// V3減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

//V3乗算

Vector3 Multiply(float scaler, const Vector3& v);

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