#include <Windows.h>
#include <stdio.h>

#include <assert.h>

#include <windef.h>
#include <stdint.h>

#include <cmath>
#include <cfloat>

#pragma region manth stuff

inline float clamp(float X, float Min, float Max)
{
	X = (X + Max - fabsf(X - Max)) * 0.5f;
	X = (X + Min + fabsf(X - Min)) * 0.5f;

	return X;
}

inline float isqrt_tpl(float op) { return 1.0f / sqrt(op); }
inline float fabs_tpl(float op) { return fabs(op); }
inline void cry_sincos(float angle, float* pSin, float* pCos) { *pSin = (sin(angle));  *pCos = (cos(angle)); }
inline void sincos(float angle, float* pSin, float* pCos) { cry_sincos(angle, pSin, pCos); }
inline float isqrt_safe_tpl(float op) { return 1.0f / sqrt(op + (float)DBL_MIN); }

inline float asin_tpl(float op) { return asin(clamp(op, -1.0f, +1.0f)); }
static float g_PI = 3.14159265358979323846264338327950288419716939937510f;
inline float atan2_tpl(float op1, float op2) { return atan2(op1, op2); }

#define FloatU32ExpMask (0xFF << 23)
inline int FloatU32(const float x)
{
	union { int ui; float f; } cvt;
	cvt.f = x;

	return cvt.ui;
}

inline bool NumberValid(const float& x)
{
	int i = FloatU32(x);
	int expmask = (0xFF << 23);
	int iexp = i & expmask;
	bool invalid = (iexp == expmask);

	if (invalid)
	{
		int i = 0x7F800001;
		float fpe = *(float*)(&i);
	}

	return !invalid;
}

class Vector {
public:
	Vector() {
		x = 0;
		y = 0;
		z = 0;
	}
	Vector(float x_, float y_, float z_) {
		x = x_;
		y = y_;
		z = z_;
	}
	bool IsValid() const
	{
		if (!NumberValid(x)) return false;
		if (!NumberValid(y)) return false;
		if (!NumberValid(z)) return false;

		return true;
	}
	Vector operator-(const Vector& v) const {
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	Vector operator+(const float& v) const {
		return Vector(x + v, y + v, z + v);
	}
	Vector operator+(const Vector& v) const {
		return Vector(x + v.x, y + v.y, z + v.z);
	}

	Vector operator-(const float& v) const {
		return Vector(x - v, y - v, z - v);
	}
	bool operator!=(const Vector& v) const {
		return (v.x != x || v.y != y || v.z != z);
	}
	bool operator==(const Vector& v) const {
		return (v.x == x && v.y == y && v.z == z);
	}


	inline float fl_distance(const Vector& vec) {
		return sqrt((pow((x - vec.x), 2.0f)) + pow((y - vec.y), 2.0f) + pow((z - vec.z), 2.0f));
	}
	float x, y, z;
};





class Matrix33
{
public:
	float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;

	Matrix33() {
		m00 = 1;
		m01 = 0;
		m02 = 0;
		m10 = 0;
		m11 = 1;
		m12 = 0;
		m20 = 0;
		m21 = 0;
		m22 = 1;
	}
	bool IsValid() const
	{
		if (!NumberValid(m00)) return false;
		if (!NumberValid(m01)) return false;
		if (!NumberValid(m02)) return false;

		if (!NumberValid(m10)) return false;
		if (!NumberValid(m11)) return false;
		if (!NumberValid(m12)) return false;

		if (!NumberValid(m20)) return false;
		if (!NumberValid(m21)) return false;
		if (!NumberValid(m22)) return false;

		return true;
	}
	
	void SetRotationVDir(const Vector& vdir)
	{
		m00 = 1;
		m01 = 0;
		m02 = 0;

		m10 = 0;
		m11 = 0;
		m12 = -vdir.z;

		m20 = 0;
		m21 = vdir.z;
		m22 = 0;

		float l = sqrt(vdir.x * vdir.x + vdir.y * vdir.y);

		if (l > 0.0001)
		{
			float xl = -vdir.x / l; float yl = vdir.y / l;

			m00 = (yl);
			m01 = (vdir.x);
			m02 = (xl * vdir.z);

			m10 = (xl);
			m11 = (vdir.y);
			m12 = (-vdir.z * yl);

			m20 = 0;
			m21 = (vdir.z);
			m22 = (l);
		}
	}

	Matrix33(const Vector& vx, const Vector& vy, const Vector& vz)
	{
		m00 = (vx.x);
		m01 = (vy.x);
		m02 = (vz.x);

		m10 = (vx.y);
		m11 = (vy.y);
		m12 = (vz.y);

		m20 = (vx.z);
		m21 = (vy.z);
		m22 = (vz.z);
	}
};



class Ang3
{
public:
	float x;
	float y;
	float z;

	void Set(float xval, float yval, float zval) { x = xval; y = yval; z = zval; }
	void operator () (float vx, float vy, float vz) { x = vx; y = vy; z = vz; };
	Ang3(const Matrix33& m)
	{
		y = asin_tpl(max(-1.0f, min(1.0f, -m.m20)));
		if (fabs_tpl(fabs_tpl(y) - (g_PI * 0.5f)) < 0.01f)
		{
			x = 0;
			z = atan2_tpl(-m.m01, m.m11);
		}
		else {
			x = atan2_tpl(m.m21, m.m22);
			z = atan2_tpl(m.m10, m.m00);
		}
	}
	bool IsValid() const
	{
		if (!NumberValid(x)) return false;
		if (!NumberValid(y)) return false;
		if (!NumberValid(z)) return false;

		return true;
	}
};

class Quat
{
public:
	Vector v;
	float w;

	bool IsValid() const
	{
		if (!NumberValid(v.x)) return false;
		if (!NumberValid(v.y)) return false;
		if (!NumberValid(v.z)) return false;
		if (!NumberValid(w)) return false;

		return true;
	}

	friend float operator | (const Quat& q, const Quat& p)
	{
		assert(q.v.IsValid());
		assert(p.v.IsValid());
		return (q.v.x * p.v.x + q.v.y * p.v.y + q.v.z * p.v.z + q.w * p.w);
	}

	friend Quat operator - (const Quat& q, const Quat& p)
	{
		Quat ret;
		ret.w = q.w - p.w;

		ret.v.x = q.v.x - p.v.x;
		ret.v.y = q.v.y - p.v.y;
		ret.v.z = q.v.z - p.v.z;

		return ret;
	}

	void Normalize(void)
	{
		float d = isqrt_tpl(w * w + v.x * v.x + v.y * v.y + v.z * v.z);

		w *= d;

		v.x *= d;
		v.y *= d;
		v.z *= d;
	}

	void SetNlerp(const Quat& p, const Quat& tq, float t)
	{
		Quat q = tq;

		assert(p.IsValid());
		assert(q.IsValid());

		if ((p | q) < 0)
		{
			float qx = -q.v.x;
			float qy = -q.v.y;
			float qz = -q.v.z;

			q.v.x = qx;
			q.v.y = qy;
			q.v.z = qz;
		}

		v.x = p.v.x * (1.0f - t) + q.v.x * t;
		v.y = p.v.y * (1.0f - t) + q.v.y * t;
		v.z = p.v.z * (1.0f - t) + q.v.z * t;

		w = p.w * (1.0f - t) + q.w * t;

		Normalize();
	}

	void SetSlerp(const Quat& tp, const Quat& tq, float t)
	{
		assert(tp.IsValid());

		Quat p, q;
		p = tp;
		q = tq;
		Quat q2;

		float cosine = (p | q);

		if (cosine < 0.0f)
		{
			float qx = -q.v.x;
			float qy = -q.v.y;
			float qz = -q.v.z;

			cosine = -cosine;

			q.v.x = qx;
			q.v.y = qy;
			q.v.z = qz;
		}

		if (cosine > 0.9999f)
		{
			SetNlerp(p, q, t);
			return;
		}

		q2.w = q.w - p.w * cosine;
		q2.v.x = q.v.x - p.v.x * cosine;
		q2.v.y = q.v.y - p.v.y * cosine;
		q2.v.z = q.v.z - p.v.z * cosine;

		float sine = sqrt(q2 | q2);
		float s, c;

		sincos(atan2(sine, cosine) * t, &s, &c);

		w = (p.w * c + q2.w * s / sine);
		v.x = (p.v.x * c + q2.v.x * s / sine);
		v.y = (p.v.y * c + q2.v.y * s / sine);
		v.z = (p.v.z * c + q2.v.z * s / sine);
	}

	static Quat CreateSlerp(const Quat& p, const Quat& tq, float t)
	{
		Quat d;
		d.SetSlerp(p, tq, t);
		return d;
	}


	void SetRotationVDir(const Vector& vdir)
	{
		w = (0.70710676908493042f);
		v.x = (vdir.z * 0.70710676908493042f);
		v.y = (0.0f);
		v.z = (0.0f);

		float l = sqrt(vdir.x * vdir.x + vdir.y * vdir.y);

		if (l > (0.00001))
		{
			Vector hv;

			hv.x = vdir.x / l;
			hv.y = vdir.y / l + 1.0f;
			hv.z = l + 1.0f;

			float r = sqrt(hv.x * hv.x + hv.y * hv.y);
			float s = sqrt(hv.z * hv.z + vdir.z * vdir.z);
			float hacos0 = 0.0;
			float hasin0 = -1.0;

			if (r > (0.00001)) { hacos0 = hv.y / r; hasin0 = -hv.x / r; }

			float hacos1 = hv.z / s;
			float hasin1 = vdir.z / s;

			w = (hacos0 * hacos1);
			v.x = (hacos0 * hasin1);
			v.y = (hasin0 * hasin1);
			v.z = (hasin0 * hacos1);
		}
	}

	static Quat CreateRotationVDir(const Vector& vdir) { Quat q; q.SetRotationVDir(vdir); return q; }
	Quat CreateRotationVDir_(const Vector& vdir, float roll) { Quat q; q.SetRotationVDir_2(vdir, roll); return q; }

	void SetRotationVDir_2(const Vector& vdir, float r)
	{
		SetRotationVDir(vdir);
		float sy, cy;  sincos(r * 0.5f, &sy, &cy);
		float vx = v.x, vy = v.y;
		v.x = (vx * cy - v.z * sy); v.y = (w * sy + vy * cy); v.z = (v.z * cy + vx * sy); w = (w * cy - vy * sy);
	}

	Quat CreateRotationXYZ(const Ang3& a)
	{
		assert(a.IsValid());
		Quat q;
		q.SetRotationXYZ(a);

		return q;
	}

	void SetRotationXYZ(const Ang3& a)
	{
		assert(a.IsValid());
		float sx, cx;  sincos((a.x * 0.5f), &sx, &cx);
		float sy, cy;  sincos((a.y * 0.5f), &sy, &cy);
		float sz, cz;  sincos((a.z * 0.5f), &sz, &cz);
		w = cx * cy * cz + sx * sy * sz;
		v.x = cz * cy * sx - sz * sy * cx;
		v.y = cz * sy * cx + sz * cy * sx;
		v.z = sz * cy * cx - cz * sy * sx;
	}

	void SetRotationZ(float r)
	{
		float s, c;
		sincos((r * 0.5f), &s, &c);
		w = c;
		v.x = 0;
		v.y = 0;
		v.z = s;
	}

	Quat CreateRotationZ(float r)
	{
		Quat q;
		q.SetRotationZ(r);
		return q;
	}

	Quat() {}
};


Matrix33 quat_to_matrix33(Quat q);

class QuatT
{
public:
	Quat q;
	Vector t;

	bool IsValid() const
	{
		if (!t.IsValid()) return false;
		if (!q.IsValid()) return false;
		return true;
	}

	QuatT() {}
};

struct Matrix34
{
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	Vector GetTranslation() { Vector mf; mf.x = m03; mf.y = m13; mf.z = m23; return mf; }
	Matrix34(const QuatT& q)
	{
		assert(q.q.IsValid());

		Vector v2 = { 0, 0, 0 };
		v2.x = (q.q.v.x) + (q.q.v.x);

		float xx = 1 - v2.x * q.q.v.x; float yy = v2.y * q.q.v.y; float xw = v2.x * q.q.w;
		float xy = v2.y * q.q.v.x;   float yz = v2.z * q.q.v.y; float  yw = v2.y * q.q.w;
		float xz = v2.z * q.q.v.x;   float zz = v2.z * q.q.v.z; float  zw = v2.z * q.q.w;

		m00 = float(1 - yy - zz);     m01 = float(xy - zw);     m02 = float(xz + yw);   m03 = float(q.t.x);
		m10 = float(xy + zw);      m11 = float(xx - zz);     m12 = float(yz - xw);   m13 = float(q.t.y);
		m20 = float(xz - yw);      m21 = float(yz + xw);     m22 = float(xx - yy);   m23 = float(q.t.z);
	}

	Matrix34() {
		m00 = 1;
		m01 = 0;
		m02 = 0;
		m03 = 0;
		m10 = 0;
		m11 = 1;
		m12 = 0;
		m13 = 0;
		m20 = 0;
		m21 = 0;
		m22 = 1;
		m23 = 0;
	}
};


inline Matrix34 operator*(const Matrix34& l, const Matrix34& r)
{
	Matrix34 m;
	m.m00 = l.m00 * r.m00 + l.m01 * r.m10 + l.m02 * r.m20;
	m.m10 = l.m10 * r.m00 + l.m11 * r.m10 + l.m12 * r.m20;
	m.m20 = l.m20 * r.m00 + l.m21 * r.m10 + l.m22 * r.m20;
	m.m01 = l.m00 * r.m01 + l.m01 * r.m11 + l.m02 * r.m21;
	m.m11 = l.m10 * r.m01 + l.m11 * r.m11 + l.m12 * r.m21;
	m.m21 = l.m20 * r.m01 + l.m21 * r.m11 + l.m22 * r.m21;
	m.m02 = l.m00 * r.m02 + l.m01 * r.m12 + l.m02 * r.m22;
	m.m12 = l.m10 * r.m02 + l.m11 * r.m12 + l.m12 * r.m22;
	m.m22 = l.m20 * r.m02 + l.m21 * r.m12 + l.m22 * r.m22;
	m.m03 = l.m00 * r.m03 + l.m01 * r.m13 + l.m02 * r.m23 + l.m03;
	m.m13 = l.m10 * r.m03 + l.m11 * r.m13 + l.m12 * r.m23 + l.m13;
	m.m23 = l.m20 * r.m03 + l.m21 * r.m13 + l.m22 * r.m23 + l.m23;
	return m;
}

struct AABB
{
	__inline AABB()
	{}

	Vector min;
	Vector max;
};

#pragma endregion

