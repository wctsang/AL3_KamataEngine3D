#pragma once

#include <cmath>
#include <math.h>
#include "Vector3.h"

//2項演算子オーバーロード
//Vector3の足し算

const Vector3 operator+=(const Vector3 &v1, const Vector3 &v2);

//代入演算子オーバーロード
//Vector3の掛け算

Vector3& operator*=(Vector3& v, float s);

// 2項演算子オーバーロード
// Vector3の掛け算

const Vector3 operator*(const Vector3& v, float s);