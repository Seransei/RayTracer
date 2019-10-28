#pragma once
#include "Tools.h"

class Leaf
{
public:
	Sphere sphere;

	Leaf(Sphere s);

	std::optional<float> intersect(Ray r);
	Vector3<float> minPoint();
	Vector3<float> maxPoint();
};

