/*=========================================================================
/ James McCormick - 3DMath.h
/ Contains the class definitions for the vector3, vector4, and matrix classes.
/ These classes inherent from the DirectX vector structures.  Basically
/ performing a C++ wrapper around the DirectX D3DXVECTOR3/4 and
/ D3DXMATRIX structures.  All of them are incased in the namespace Math so
/ the constants dont clash with anything as well as the word vector since
/ its a commonly used word
/==========================================================================*/
#ifndef _3DMATH_
#define _3DMATH_

#include <d3dx10.h>

namespace Math {
	const float INFINITY = FLT_MAX;
	const float PI       = 3.14159265358979323f;
	const float MATH_EPS = 0.0001f;
#define	DegToRadian(degree) ((degree) * (Math::PI / 180.0))
#define	RadianToDeg(radian) ((radian) * (180.0 / Math::PI))

class vector4;
class quaternion;

class vector3 : public D3DXVECTOR3 {
public:
	float length() { return D3DXVec3Length(this); }
	vector3* normalize() { return static_cast<vector3*>(D3DXVec3Normalize(this, this)); }
	float dot(const vector3& b) { return D3DXVec3Dot(this, &b); }
	vector3 cross(const vector3& b) const { 
		vector3 out;
		D3DXVec3Cross(&out, this, &b);
		return out;
	}

	// Constructors
	vector3() : D3DXVECTOR3() {}
	vector3(D3DXVECTOR3 &v) { x = v.x; y = v.y; z = v.z; }
	vector3(const float _x, const float _y, const float _z) { x = _x; y = _y; z = _z; }
	vector3(const vector4 &v4);
};


class vector4 : public D3DXVECTOR4 {
public:
	float length() { return D3DXVec4Length(this); }
	vector4* normalize() { return static_cast<vector4*>(D3DXVec4Normalize(this, this)); }
	float dot(const vector4& b) { return D3DXVec4Dot(this, &b); }

	// For cross product use C_Vector3::Cross

	// Constructors
	vector4() : D3DXVECTOR4() {}
	vector4(D3DXVECTOR4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	vector4(const float _x, const float _y, const float _z, const float _w) { x = _x; y = _y; z = _z; w = _w; }
	vector4(const vector3& v) { x = v.x; y = v.y; z = v.z; w = 1.0f; }
};

inline vector3::vector3(const vector4 &v4) { x = v4.x; y = v4.y; z = v4.z; }


class matrix4x4 : public D3DXMATRIX {
public:
	void setPosition(vector3 const &pos) {
		m[3][0] = pos.x;
		m[3][1] = pos.y;
		m[3][2] = pos.z;
		m[3][3] = 1.0f;
	}

	void setPosition(vector4 const &pos) {
		m[3][0] = pos.x;
		m[3][1] = pos.y;
		m[3][2] = pos.z;
		m[3][3] = pos.w;
	}

	vector3 getPosition() const { return vector3(m[3][0], m[3][1], m[3][2]); }

	vector4 transform(vector4 &v) const {
		vector4 temp;
		D3DXVec4Transform(&temp, &v, this);
		return temp;
	}

	vector3 transform(vector3 &v) const {
		vector4 temp(v), out;
		D3DXVec4Transform(&out, &temp, this);
		return vector3(out);
	}

	matrix4x4 inverse() const {
		matrix4x4 out;
		D3DXMatrixInverse(&out, NULL, this);
		return out;
	}

	void buildTranslation(const vector3 &pos) {
		*this = matrix4x4::G_Identity;
		m[3][0] = pos.x;
		m[3][1] = pos.y;
		m[3][2] = pos.z;
	}

	void buildTranslation(const float x, const float y, const float z) {
		*this = matrix4x4::G_Identity;
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
	}

	void buildRotationX(const float radians) { D3DXMatrixRotationX(this, radians); }
	void buildRotationY(const float radians) { D3DXMatrixRotationY(this, radians); }
	void buildRotationZ(const float radians) { D3DXMatrixRotationZ(this, radians); }

	void buildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians)
		{ D3DXMatrixRotationYawPitchRoll(this, yawRadians, pitchRadians, rollRadians); }

	void buildRotationQuat(const quaternion &q);

	// Constructor
	matrix4x4() : D3DXMATRIX() { }
	matrix4x4(D3DXMATRIX &mat) { memcpy(&m, &mat.m, sizeof(mat.m)); }

		// Global Identity matrix
	static matrix4x4 G_Identity;
};


inline matrix4x4 operator * (const matrix4x4 &a, const matrix4x4 &b) {
	matrix4x4 out;
	D3DXMatrixMultiply(&out, &a, &b);
	return out;
}

inline vector4 operator * (const vector4& v, const matrix4x4& m) {
	vector4 out;
	D3DXVec4Transform(&out, &v, &m);
	return out;
}

class quaternion : public D3DXQUATERNION {
public:
	void normalize() { D3DXQuaternionNormalize(this, this); }
	
	// Set delta between 0.0f-1.0f
	// Performs spherical linear interpolation between begin and end and
	// assigns it to the current quaternion.
	void slerp(const quaternion &begin, const quaternion &end, float delta)
	{ D3DXQuaternionSlerp(this, &begin, &end, delta); }


	void getAxisAngle(vector3 &axis, float &angle) const
	{ D3DXQuaternionToAxisAngle(this, &axis, &angle); }

	void buildRotYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians)
	{ D3DXQuaternionRotationYawPitchRoll(this, yawRadians, pitchRadians, rollRadians); }

	void buildAxisAngle(const vector3 &axis, const float radians)
	{ D3DXQuaternionRotationAxis(this, &axis, radians); }

	void buildFromMatrix(const matrix4x4 &mat)
	{ D3DXQuaternionRotationMatrix(this, &mat); }

	quaternion(D3DXQUATERNION &q) : D3DXQUATERNION(q) { }
	quaternion() : D3DXQUATERNION() { }

	static quaternion G_Identity;
};

// For concatenating quaternions - the new quat is rotation a 
// followed by rotation b.
inline quaternion operator * (const quaternion &a, const quaternion &b) {
	quaternion out;
	D3DXQuaternionMultiply(&out, &a, &b);
	return out;
}


inline void matrix4x4::buildRotationQuat(const quaternion &q) { D3DXMatrixRotationQuaternion(this, &q); }

} // namespace Math
#endif // _3DMATH_
