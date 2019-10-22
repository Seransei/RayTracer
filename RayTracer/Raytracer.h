#pragma once
#include "PPM.h"
#include "Tools.h"
#include "Node.h"
#include "Leaf.h"
#include <cmath>
#include <algorithm>
#include <time.h>
#include <variant>

class Raytracer
{
public:	
	Color
		whiteLightColor,
		redLightColor,
		greenLightColor,
		blueLightColor,
		white,
		red,
		green,
		blue;

	int H = 1000, W = 1000;
	float wallRadius = 30000;
	float zDepth = 0;
	float wallOffset = 0;
	
	std::vector<Sphere> scene;
	std::variant<Node*, Leaf> bvh;
	std::vector<Light> lights;
	int nbLightPoints = 50;
	int lightSize = 50;

	void initScene();
	void createScene();
	void sortSphereList(std::vector<Sphere> spheres);
	IntersectedObject intersectRay(Ray ray, std::vector<Sphere> scene);
	Vector3<float> castRay(Ray, int);
	std::variant<Node*, Leaf> createBVH(const std::vector<Sphere> spheres);
	BoundingBox createBoundingBox(std::vector<Sphere> spheres);
	float clamp1(float);
	bool compareX(Sphere a, Sphere b);
	bool compareY(Sphere a, Sphere b);
	bool compareZ(Sphere a, Sphere b);
};

