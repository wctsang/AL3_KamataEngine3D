#include "MathUtilityForText.h"

//2項演算子オーバーロード
//Vector3の足し算

const Vector3 operator+=(const Vector3 &v1, const Vector3 &v2){
	Vector3 temp(v1);
	return temp += v2;
}

//代入演算子オーバーロード
//Vector3の掛け算

Vector3& operator*=(Vector3& v, float s) { 
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

//2項演算子オーバーロード
//Vector3の掛け算

const Vector3 operator*(const Vector3& v, float s) {
	Vector3 temp(v);
	return temp *= s;
}
