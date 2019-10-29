#pragma once
#include "Raytracer.h"

bool compareX(Sphere a, Sphere b) { return a.center.x < b.center.x; }
bool compareY(Sphere a, Sphere b) { return a.center.y < b.center.y; }
bool compareZ(Sphere a, Sphere b) { return a.center.z < b.center.z; }

void Raytracer::createScene() {
	srand(time(NULL));//seed pour la generation de nombre aleatoire

	//----Parcours des pixels----
	PPM ppm(H, W, 255);

	#pragma omp parallel for
	for (int y = 0; y < W; y++)
	{
		for (int x = 0; x < H; x++)
		{
			ppm(x, y).color = Vector3<float>(0, 0, 0);
			//----Lancer de rayon perspective----
			Vector3<float> cameraDirection = Vector3<float>((float)x, (float)y, 0) - perspectivePos;
			Ray r{
				Vector3<float>((float)x, (float)y, 0),//point
				cameraDirection.normalize()//direction
			};
			ppm(x, y).color = castRay(r, 2).clamp255();
		}
	}

	ppm.save("./fileOut.ppm");
}

void Raytracer::createSceneBVH() {
	srand(time(NULL));//seed pour la generation de nombre aleatoire

	createBVH(scene);

	//----Parcours des pixels----
	PPM ppm(H, W, 255);
	
	#pragma omp parallel for
	for (int y = 0; y < W; y++) 
	{
		for (int x = 0; x < H; x++)
		{
			ppm(x, y).color = Vector3<float>(0, 0, 0);
			//----Lancer de rayon perspective----
			Vector3<float> cameraDirection = Vector3<float>((float)x, (float)y, 0) - perspectivePos;
			Ray r{
				Vector3<float>((float)x, (float)y, 0),//point
				cameraDirection.normalize()//direction
			};
			ppm(x, y).color = castRayToBVH(r, 2).clamp255();
		}
	}

	ppm.save("./fileOut.ppm");
}

std::optional<IntersectedObject> Raytracer::rayObstacles(Ray ray, std::vector<Sphere> spheres)
{
	int sphIndex = -1;//recupere l'indice de la sphere touchée
	std::optional<float> min_t;//recupere le t du premier objet touché
	for (unsigned int i = 0; i < spheres.size(); i++)
	{
		//----Impact a la sphere----
		std::optional<float> current_t = spheres[i].intersect(ray);//impact rayon-sphere
		if (current_t.has_value() && (!min_t.has_value() || (min_t.has_value() && current_t.value() < min_t.value())))
		{
			min_t = current_t;//premier impact courant du rayon
			sphIndex = i;//indice de la sphere la plus proche
		}
	}
	if (min_t.has_value())
		return IntersectedObject(min_t, spheres[sphIndex]);
	else
		return std::nullopt;
}

std::optional<IntersectedObject> Raytracer::rayObstaclesBVH(Ray ray, std::variant<Node*, Leaf> currBVH) 
{
	if (auto leaf = std::get_if<Leaf>(&currBVH)) 
	{
		return IntersectedObject(leaf->intersect(ray), leaf->sphere);
	}

	if (auto node = std::get_if<Node*>(&currBVH)) 
	{
		auto t = (**node).boundingBox.intersect(ray);
		
		auto t_left = rayObstaclesBVH(ray, (**node).left);		
		auto t_right = rayObstaclesBVH(ray, (**node).right);

		return min(t_left, t_right);
	}

	return std::nullopt;
}

Vector3<float> Raytracer::castRayToBVH(Ray ray, int iteration) 
{
	Vector3<float> newColor(0, 0, 0);//nouvelle couleur a renvoyer
	Vector3<float> objColor(0, 0, 0);

	Node* *node = std::get_if<Node*>(&bvh);
	Leaf *leaf = std::get_if<Leaf>(&bvh);

	std::optional<IntersectedObject> intSphere = rayObstaclesBVH(ray, bvh);

}

Vector3<float> Raytracer::castRay(Ray ray, int iteration)
{
	Vector3<float> newColor(0, 0, 0);//nouvelle couleur a calculer
	Vector3<float> objColor(0, 0, 0);
	
	std::optional<IntersectedObject> intSphere = rayObstacles(ray, scene);

	//----Sphere trouvee----
	if (intSphere.has_value() && intSphere.value().t.has_value())
	{
		Sphere hitSphere = intSphere.value().sphere;
		Vector3<float> hitPos(ray.source + intSphere.value().t.value() * ray.dir);//point d'impact à la sphere
		Vector3<float> hitNormal((hitPos - hitSphere.center).normalize());//normale a l'impact, normalisé

		//----Parcours des lumieres----
		for (auto& li : lights)
			for (int nbL = 0; nbL < nbLightPoints; nbL++)
			{
				float
					randX = (float)(rand() % (lightSize / 2) - lightSize),
					randY = (float)(rand() % (lightSize / 2) - lightSize),
					randZ = (float)(rand() % (lightSize / 2) - lightSize);

				Vector3<float> hitToLight((li.pos - hitPos + Vector3<float>(randX, randY, randZ)));//vecteur impact vers lumiere
				
				//----Lancer de rayon du point d'impact a la sphere vers la lumiere ----
				Ray toLight{
					hitPos + 0.01f * hitToLight.normalize(),//point
					hitToLight.normalize()//dir
				};
				
				std::optional<IntersectedObject> intObstacle = rayObstacles(toLight, scene);

				if(intObstacle.has_value())
					if (!intObstacle.value().t.has_value() || intObstacle.value().t.value() > hitToLight.norm() + 0.01f)//s'il n'ya pas d'obstacles jusque la lumiere
					{
						objColor += 
							li.intensity *
							clamp1(hitToLight.normalize().dot(hitNormal)) *
							li.color.value *
							hitSphere.color.value
							/ pow(hitToLight.norm(),2)
							/ (float)nbLightPoints;
					}
			}
		
		//----Miroir----
		if (iteration > 0)
		{
			Vector3<float> refDir = 2 * ((-1) * ray.dir).dot(hitNormal) * hitNormal + ray.dir;//direction du rayon reflechi
			Ray ref{
				hitPos + 0.01f * refDir.normalize(),//position
				refDir.normalize()//direction
			};
			newColor = (1 - hitSphere.albedo) * objColor + hitSphere.albedo * castRay(ref, iteration - 1);
		}
		
		
		//manque la lumiere indirecte


	}

	return newColor;
}

std::variant<Node*, Leaf> Raytracer::createBVH(const std::vector<Sphere> spheres)
{
	if (spheres.size() == 1)
		return Leaf(spheres[0]);
	else {
		sortSphereList(spheres);

		std::vector<Sphere> splittedList1;
		splittedList1.insert(splittedList1.begin(), spheres.begin(), spheres.begin() + std::floor(spheres.size() / 2));

		std::vector<Sphere> splittedList2;
		splittedList2.insert(splittedList2.begin(), spheres.begin() + std::floor(spheres.size() / 2), spheres.end());

		std::variant<Node*, Leaf> left = createBVH(splittedList1);
		std::variant<Node*, Leaf> right = createBVH(splittedList2);

		return new Node(left, right, createBoundingBox(spheres));
	}
}

void Raytracer::sortSphereList(std::vector<Sphere> spheres)
{
	if (spheres.size() > 1) 
	{
		float
			minX = spheres[0].center.x, maxX = spheres[0].center.x,
			minY = spheres[0].center.y, maxY = spheres[0].center.y,
			minZ = spheres[0].center.z, maxZ = spheres[0].center.z;

		for (int i = 1; i < spheres.size(); i++) {
			minX = (spheres[i].center.x < minX) ? spheres[i].center.x : minX;
			maxX = (spheres[i].center.x > maxX) ? spheres[i].center.x : maxX;

			minY = (spheres[i].center.y < minY) ? spheres[i].center.y : minY;
			maxY = (spheres[i].center.y > maxY) ? spheres[i].center.y : maxY;

			minZ = (spheres[i].center.z < minZ) ? spheres[i].center.z : minZ;
			maxZ = (spheres[i].center.z > maxZ) ? spheres[i].center.z : maxZ;
		}

		float
			deltaX = abs(maxX - minX),
			deltaY = abs(maxY - minY),
			deltaZ = abs(maxZ - minZ);

		if (deltaX >= deltaY && deltaX >= deltaZ)
			std::sort(spheres.begin(), spheres.end(), compareX);
		else if (deltaY >= deltaX && deltaY >= deltaZ)
			std::sort(spheres.begin(), spheres.end(), compareY);
		else
			std::sort(spheres.begin(), spheres.end(), compareZ);
	}
}

BoundingBox Raytracer::createBoundingBox(std::vector<Sphere> spheres)
{
	float
		min_x = spheres[0].center.x - spheres[0].radius, 
		min_y = spheres[0].center.y - spheres[0].radius,
		min_z = spheres[0].center.z - spheres[0].radius,
		max_x = spheres[0].center.x + spheres[0].radius, 
		max_y = spheres[0].center.y + spheres[0].radius, 
		max_z = spheres[0].center.z + spheres[0].radius;

	for (unsigned int i = 1; i < spheres.size(); i++) 
	{
		min_x = std::min(spheres[i].center.x - spheres[i].radius, min_x);
		min_y = std::min(spheres[i].center.y - spheres[i].radius, min_y);
		min_z = std::min(spheres[i].center.z - spheres[i].radius, min_z);

		max_x = std::max(spheres[i].center.x + spheres[i].radius, max_x);
		max_y = std::max(spheres[i].center.y + spheres[i].radius, max_y);
		max_z = std::max(spheres[i].center.z + spheres[i].radius, max_z);
	}

	return BoundingBox(Vector3<float>(min_x, min_y, min_z), Vector3<float>(max_x, max_y, max_z));
}

float Raytracer::clamp1(float n)
{
	return (n < 0) ? 0 : (n > 1) ? 1 : n;
}