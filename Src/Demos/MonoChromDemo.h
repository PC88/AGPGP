#pragma once
#include "Demo.h"

#include "rt3d.h"
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>

#include <SDL_ttf.h>

using namespace std;

#define DEG_TO_RADIAN 0.017453293

class MonoChromDemo :
	public Demo
{
public:
	MonoChromDemo();
	virtual ~MonoChromDemo();

};

