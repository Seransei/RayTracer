#pragma once
#include "Raytracer.h"

Raytracer rt;

Color
	whiteLightColor,
	redLightColor,
	greenLightColor,
	blueLightColor,
	white,
	red,
	green,
	blue;

void initColors();
void initLights();
void initSpheres();
void initScene();

int main()
{
	initColors();
	initScene();
	rt.createSceneBVH();
}



void initColors()
{
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
}

void initLights()
{
	Light
		camLight,
		roomLight,
		lightHautGauche,
		lightHautDroite,
		lightBas;

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
		Vector3<float>(900, rt.W / 2, 0),//pos
		blueLightColor,//color		
		100000.f//intensity
	};

	rt.lights.push_back(lightHautGauche);
	rt.lights.push_back(lightHautDroite);
	rt.lights.push_back(lightBas);
	rt.lights.push_back(camLight);
	// rt.lights.push_back(roomLight);
	//----
}

void initSpheres() 
{
	Sphere
		sphere1,
		sphere2,
		sphere3;

	//----SPHERES----
	sphere1 = Sphere{
		Vector3<float>(rt.W / 2, 800, 100),//pos
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

	rt.scene.push_back(sphere1);
	rt.scene.push_back(sphere2);
	rt.scene.push_back(sphere3);
}

void initScene()
{
	initLights();
	initSpheres();

	Sphere
		backWall,
		ceiling,
		ground,
		leftWall,
		rightWall;

	//----WALLS----
	backWall = Sphere{
		Vector3<float>(0, 0, rt.wallRadius + 800),
		rt.wallRadius,
		red,
		0.05f
	};

	ceiling = Sphere{
		Vector3<float>(-rt.wallRadius - rt.wallOffset, 500, rt.zDepth),
		rt.wallRadius,
		blue,
		0.05f
	};

	ground = Sphere{
		Vector3<float>(rt.wallRadius + (float)rt.H + rt.wallOffset, 500, rt.zDepth),
		rt.wallRadius,
		white,
		0.05f
	};

	leftWall = Sphere{
		Vector3<float>(500, -rt.wallRadius - rt.wallOffset, rt.zDepth),
		rt.wallRadius,
		green,
		0.8f
	};

	rightWall = Sphere{
		Vector3<float>(500, rt.wallRadius + (float)rt.W + rt.wallOffset, rt.zDepth),
		rt.wallRadius,
		green,
		0.05f
	};

	rt.scene.push_back(backWall);
	rt.scene.push_back(ceiling);
	rt.scene.push_back(ground);
	rt.scene.push_back(leftWall);
	rt.scene.push_back(rightWall);
	//----
}