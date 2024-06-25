#include "myMath.h"

// 関数宣言

// V3加算

Vector3 Add(const Vector3& v1, const Vector3& v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; };

// V3減算

Vector3 Subtract(const Vector3& v1, const Vector3& v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; };

//V3乗算

Vector3 Multiply(float scaler, const Vector3& v) {
	Vector3 result = {};
	result.x = scaler * v.x;
	result.y = scaler * v.y;
	result.z = scaler * v.z;
	return result;
}

// 積

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
};

// 移動

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result = {};

	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
};

// 拡大縮小

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result = {};

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;

	return result;
};

// 回転

Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result = {};
	result.m[0][0] = 1;
	result.m[3][3] = 1;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -1 * std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
};

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result = {};
	result.m[1][1] = 1;
	result.m[3][3] = 1;
	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -1 * std::sin(radian);
	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
};

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result = {};
	result.m[2][2] = 1;
	result.m[3][3] = 1;
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -1 * std::sin(radian);
	result.m[1][1] = std::cos(radian);

	return result;
};

// アフィン変換

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result = {};
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);

	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	result = Multiply(Multiply(MakeScaleMatrix(scale), rotateXYZMatrix), MakeTranslateMatrix(translate));

	return result;
};