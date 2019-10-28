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
	int H = 1000, W = 1000;
	Vector3<float> perspectivePos = Vector3<float>((float)H / 2, (float)W / 2, -1000);//point pour la camera en perspective
	float wallRadius = 30000;
	float zDepth = 0;
	float wallOffset = 0;
	
	std::variant<Node*, Leaf> bvh;
	std::vector<Light> lights;
	std::vector<Sphere> scene;
	int nbLightPoints = 50;
	int lightSize = 10;

	void createScene();
	void createSceneBVH();
	std::optional<IntersectedObject> rayObstacles(Ray ray, std::vector<Sphere> scene);
	std::optional<IntersectedObject> rayObstaclesBVH(Ray ray, std::variant<Node*, Leaf> currBVH);
	Vector3<float> castRayToBVH(Ray ray, int iteration);
	Vector3<float> castRay(Ray, int);
	std::variant<Node*, Leaf> createBVH(const std::vector<Sphere> spheres);
	void sortSphereList(std::vector<Sphere> spheres);
	BoundingBox createBoundingBox(std::vector<Sphere> spheres);
	float clamp1(float);
};

