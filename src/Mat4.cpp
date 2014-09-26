#include "../header/int/Mat4.h"
#include "../header/int/Vector4.h"
#include "../header/ext/Vector3.h"

Mat4x4::Mat4x4(const Mat4x4& mat){
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			m[i][j] = mat.m[i][j];
		}
	}
}

Mat4x4::Mat4x4(float scale){
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			m[i][j] = (i==j? scale : 0);
		}
	}
}


Mat4x4 Mat4x4::operator*(const Mat4x4& param) const{
	Mat4x4 result;
	
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			result.m[i][j] = DotProduct(GetRow(i), param.GetColumn(j));
		}
	}
	
	return result;
}

Vector3 Mat4x4::operator*(const Vector3& param) const{
	float vector[4] = {param.x, param.y, param.z, 1};

	return Vector3( DotProduct(vector, m[0], 4),
					DotProduct(vector, m[1], 4),
					DotProduct(vector, m[2], 4));
}

Vector4 Mat4x4::operator*(const Vector4& param) const{
	return Vector4( DotProduct(param, Vector4((float*)m[0])),
					DotProduct(param, Vector4((float*)m[1])),
					DotProduct(param, Vector4((float*)m[2])),
					DotProduct(param, Vector4((float*)m[3])));
}

Vector4 Mat4x4::GetColumn(int index) const{
	float col[4];
	
	for(int i = 0; i < 4; i++){
		col[i] = m[i][index];
	}
	
	return Vector4(col);
}

Vector4 Mat4x4::GetRow(int index) const{
	return Vector4( (float*)m[index]);
}

void Mat4x4::SetRow(int index, const Vector4& value){
	m[index][0] = value.x;
	m[index][1] = value.y;
	m[index][2] = value.z;
	m[index][3] = value.w;
}

float DotProduct(const Vector4& param1, const Vector4& param2){
	return param1.w * param2.w
		 + param1.x * param2.x
		 + param1.y * param2.y
		 + param1.z * param2.z;
}

float DotProduct(const float a[], const float b[], int length){
	float total = 0;
	for(int i = 0; i < length; i++){
		total += (a[i] * b[i]);
	}
	
	return total;
}