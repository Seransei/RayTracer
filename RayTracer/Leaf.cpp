#pragma once
#include "Leaf.h"

Leaf::Leaf(Sphere s) {
	sphere = s;
}

std::optional<float> Leaf::intersect(Ray r) {
	return sphere.intersect(r);
}

Vector3<float> Leaf::minPoint()
{
	float minX = sphere.center.x - sphere.radius;
	float minY = sphere.center.y - sphere.radius;
	float minZ = sphere.center.z - sphere.radius;
	return Vector3<float>(minX, minY, minZ);
}

Vector3<float> Leaf::maxPoint()
{
	float maxX = sphere.center.x + sphere.radius;
	float maxY = sphere.center.y + sphere.radius;
	float maxZ = sphere.center.z + sphere.radius;
	return Vector3<float>(maxX, maxY, maxZ); 
}
