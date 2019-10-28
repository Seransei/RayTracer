#pragma once
#include "Vector3.hpp"
#include <optional>
#include <cstdlib>
#include <cstdio>
#include <vector>

struct Color {
	Vector3<float> value;
};

struct Ray {
	Vector3<float> source;
	Vector3<float> dir;
};

struct Sphere {
	Vector3<float> center;
	float radius;
	Color color;
	float albedo;

	std::optional<float> intersect(Ray r) {
		float a = r.dir.dot2(); //la direction est normalisé d'ou dot2(dir) = norm_squared(dir) = 1
		float b = 2 * (r.source.dot(r.dir) - center.dot(r.dir));
		float c = r.source.dot2() + center.dot2() - (2 * center.dot(r.source)) - (radius * radius);

		float delta = (b * b) - (4 * a * c);

		if (delta >= 0)
		{
			float x1 = (float)((-b - sqrt(delta)) / (2 * a));
			float x2 = (float)((-b + sqrt(delta)) / (2 * a));//x1 <= x2

			if (x1 >= 0)
				return x1;
			else if (x2 >= 0)
				return x2;
		}

		return std::nullopt;
	}
};

struct Light {
	Vector3<float> pos;
	Color color;
	float intensity;
};

struct BoundingBox
{
	Vector3<float>
		minPos,
		maxPos;

	BoundingBox(Vector3<float> min, Vector3<float> max) : minPos(min), maxPos(max) {};

	std::optional<float> intersect(Ray r)
	{
		float
			t_min, t_max,
			ty_min, ty_max,
			tz_min, tz_max;
		Vector3<float> invDir = 1.0f / r.dir;//inverse de la direction

		//impact aux droites paralleles à x = 0
		if (invDir.x >= 0)
		{
			t_min = (minPos.x - r.source.x) * invDir.x;
			t_max = (maxPos.x - r.source.x) * invDir.x;
		}
		else {
			t_min = (maxPos.x - r.source.x) * invDir.x;
			t_max = (minPos.x - r.source.x) * invDir.x;
		}

		//impact aux droites paralleles à y = 0
		if (invDir.y >= 0)
		{
			ty_min = (minPos.y - r.source.y) * invDir.y;
			ty_max = (maxPos.y - r.source.y) * invDir.y;
		}
		else {
			ty_min = (maxPos.y - r.source.y) * invDir.y;
			ty_max = (minPos.y - r.source.y) * invDir.y;
		}

		//impact aux droites paralleles à z = 0
		if (invDir.z >= 0)
		{
			tz_min = (minPos.z - r.source.z) * invDir.z;
			tz_max = (maxPos.z - r.source.z) * invDir.z;
		}
		else {
			tz_min = (maxPos.z - r.source.z) * invDir.z;
			tz_max = (minPos.z - r.source.z) * invDir.z;
		}

		//--l'impact aux droites est-il dans ou en dehors de la boites--
		if ((t_min > ty_max) || (ty_min > t_max))
			return std::nullopt;
		if (ty_min > t_min)
			t_min = ty_min;
		if (ty_max < t_max)
			t_max = ty_max;

		if ((t_min > tz_max) || (tz_min > t_max))
			return std::nullopt;
		if (tz_min > t_min)
			t_min = tz_min;
		if (tz_max < t_max)
			t_max = tz_max;
		//----

		if (t_min >= 0)
			return t_min;
		else if (t_max >= 0)
			return t_max;
		else
			return std::nullopt;
	}
};

struct IntersectedObject {
	std::optional<float> t;
	Sphere sphere;

	IntersectedObject(std::optional<float> tt, Sphere s) {
		t = tt;
		sphere = s;
	}
};