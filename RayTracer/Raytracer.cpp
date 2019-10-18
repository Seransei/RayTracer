#pragma once
#include "Raytracer.h"

void Raytracer::initScene() 
{
	Light
		camLight,
		roomLight,
		lightHautGauche,
		lightHautDroite,
		lightBas;

	Sphere
		sphere1,
		sphere2,
		sphere3,
		backWall,
		ceiling,
		ground,
		leftWall,
		rightWall;

	//----LIGHTCOLORS----     intensité lumineuse
	whiteLightColor = Color{
		Vector3<float>(500, 500, 500)
	};

	redLightColor = Color{
		Vector3<float>(1000, 0, 0)
	};

	greenLightColor = Color{
		Vector3<float>(10, 1000, 10)
	};

	blueLightColor = Color{
		Vector3<float>(0, 0, 1000)
	};
	//----OBJECT COLORS----   taux de reflexion
	white = Color{
		Vector3<float>(1, 1, 1)
	};

	red = Color{
		Vector3<float>(1, 0.1f, 0.1f)
	};

	green = Color{
		Vector3<float>(0.1f, 1, 0.1f)
	};

	blue = Color{
		Vector3<float>(0.1f, 0.1f, 1)
	};
	//----

	//----LIGHTS----
	camLight = Light{
		Vector3<float>(500,500,-500),
		whiteLightColor,
		100000.f
	};

	roomLight = Light{
		Vector3<float>(500, 10, 100),
		whiteLightColor,
		100000.f
	};

	lightHautGauche = Light{
		Vector3<float>(100, 100, 0),//pos
		redLightColor,//color		
		100000.f//intensity
	};

	lightHautDroite = Light{
		Vector3<float>(100, 900, 0),//pos
		greenLightColor,//color
		100000.f//intensity
	};

	lightBas = Light{
		Vector3<float>(900, W / 2, 0),//pos
		blueLightColor,//color		
		100000.f//intensity
	};

	lights.push_back(lightHautGauche);
	lights.push_back(lightHautDroite);
	lights.push_back(lightBas);
	 lights.push_back(camLight);
	// lights.push_back(roomLight);
	//----

	//----SPHERES----
	sphere1 = Sphere{
		Vector3<float>(W / 2, 800, 100),//pos
		100,//radius
		white,//color
		0.2f
	};

	sphere2 = Sphere{
		Vector3<float>(100, 100, 200),//pos
		50,//radius
		white,//color
		0.3f
	};

	sphere3 = Sphere{
		Vector3<float>(800, 200, 500),//pos
		180,//radius
		white,//color
		0.8f
	};

	scene.push_back(sphere1);
	scene.push_back(sphere2);
	scene.push_back(sphere3);

	//----WALLS----

	backWall = Sphere{
		Vector3<float>(0, 0, wallRadius + 800),
		wallRadius,
		red, 
		0.05f
	};

	ceiling = Sphere{
		Vector3<float>(-wallRadius - wallOffset, 500, zDepth),
		wallRadius,
		blue,
		0.05f
	};

	ground = Sphere{
		Vector3<float>(wallRadius + (float)H + wallOffset, 500, zDepth),
		wallRadius,
		white,
		0.05f
	};

	leftWall = Sphere{
		Vector3<float>(500, -wallRadius - wallOffset, zDepth),
		wallRadius,
		green,
		0.8f
	};

	rightWall = Sphere{
		Vector3<float>(500, wallRadius + (float)W + wallOffset, zDepth),
		wallRadius,
		green,
		0.05f
	};

	scene.push_back(backWall);
	scene.push_back(ceiling);
	scene.push_back(ground);
	scene.push_back(leftWall);
	scene.push_back(rightWall);
	//----

	//bvh = createBVH(scene);
}

void Raytracer::createScene() {
	srand(time(NULL));//seed pour la generation de nombre aleatoire

	//----Parcours des pixels----
	PPM ppm(H, W, 255);
	Vector3<float> perspectivePos(((float)H) / 2, ((float)W) / 2, -1000);//point pour la camera en perspective
	#pragma omp parallel for
	for (int y = 0; y < W; y++)
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
	ppm.save("./fileOut.ppm");
}

void Raytracer::sortScene()
{
	//TODO
}

Vector3<float> Raytracer::castRay(Ray ray, int iteration)
{
	Vector3<float> newColor(0, 0, 0);//nouvelle couleur a calculer
	Vector3<float> objColor(0, 0, 0);

	int sphIndex = -1;
	std::optional<float> min_t;//va recuperer le t du premier objet touché
	//----Parcours des spheres de la scene----
	for (unsigned int i = 0; i < scene.size(); i++)
	{
		//----Impact a la sphere----
		std::optional<float> current_t = scene[i].intersect(ray);
		if (current_t.has_value() && (!min_t.has_value() || (min_t.has_value() && current_t.value() < min_t.value())))
		{
			min_t = current_t;//premier impact courant du rayon
			sphIndex = i;//indice de la sphere la plus proche
		}
	}

	//----Sphere trouvee----
	if (min_t.has_value())
	{
		Sphere hitSphere = scene[sphIndex];
		Vector3<float> hitPos(ray.source + min_t.value() * ray.dir);//point d'impact à la sphere
		Vector3<float> hitNormal((hitPos - hitSphere.center).normalize());//normale a l'impact, normalisé

		//----Parcours des lumieres----
		for (auto& li : lights)
			for (int nbL = 0; nbL < nbLightPoints; nbL++)
			{
				Vector3<float> hitToLight((li.pos - hitPos + Vector3<float>((float)(rand() % (lightSize / 2) - lightSize),
					(float)(rand() % (lightSize / 2) - lightSize),
					(float)(rand() % (lightSize / 2) - lightSize))));//vecteur impact vers lumiere
				
				//----Lancer de rayon du point d'impact a la sphere vers la lumiere ----
				Ray toLight{
					hitPos + 0.01f * hitToLight.normalize(),//point
					hitToLight.normalize()//dir
				};
				
				//----Parcours des obstacles potentiels----
				std::optional<float> min_t_sphToLi;
				for (unsigned int i = 0; i < scene.size(); i++)
				{
					std::optional<float> t_sphToLi = scene[i].intersect(toLight);//impact rayon-sphere
					if (t_sphToLi.has_value() && (!min_t_sphToLi.has_value() || (min_t_sphToLi.has_value() && t_sphToLi.value() < min_t_sphToLi.value())))
						min_t_sphToLi = t_sphToLi;//premier impact courant du rayon
				}

				if (!min_t_sphToLi.has_value() || min_t_sphToLi.value() > hitToLight.norm() + 0.01f)//s'il n'ya pas d'obstacles jusque la lumiere
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
		std::vector<Sphere> splittedList1;
		splittedList1.insert(splittedList1.begin(), spheres.begin(), spheres.begin() + std::floor(spheres.size() / 2));

		std::vector<Sphere> splittedList2;
		splittedList2.insert(splittedList2.begin(), spheres.begin() + std::floor(spheres.size() / 2) + 1, spheres.end());

		std::variant<Node*, Leaf> left = createBVH(splittedList1);
		std::variant<Node*, Leaf> right = createBVH(splittedList2);

		return new Node(left, right, createBoundingBox(spheres));
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