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


	virtual void ImGuiRender() override;
	virtual void Update(double interval) override;
	virtual void Render() override;

	glm::vec3 moveForwardBack(glm::vec3 pos, GLfloat angle, GLfloat d);
	glm::vec3 moveRightLeft(glm::vec3 pos, GLfloat angle, GLfloat d);

private:

	bool lightCubeAttenuation = false;

	stack<glm::mat4> mvStack;

	GLuint shaderProgram;
	GLuint shaderProgram1stBunny;
	GLuint shaderProgram2ndBunny;
	GLuint shaderProgram_phong_normal;
	GLuint shaderProgram_phong_monochrome;
	GLuint skyboxProgram;

	GLfloat r = 180.0f;

	glm::vec3 eye = glm::vec3(0.0f, 20.0f, -40.0f);
	glm::vec3 at = glm::vec3(0.0f, 1, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	// TEXTURE STUFF
	GLuint textures[2];
	GLuint skybox[5];

	GLuint toonIndexCount = 0;
	GLuint meshIndexCount = 0;
	GLuint meshIndexCount2 = 0;
	GLuint meshObjects[3];

	float attConstant;
	float attLinear;
	float attQuadratric;

	rt3d::lightStructCutOff light1 = {
	{0.3f, 0.3f, 0.3f, 0.0f}, // ambient
	{1.0f, 1.0f, 1.0f, 0.0f}, // diffuse
	{1.0f, 1.0f, 1.0f, 0.0f}, // specular
	{-10.0f, 100.0f, 10.0f, 1.0f},  // position
	{0.8f} //Cutoff Angle
	};
	float cutoffAngle = 0.8f;
	glm::vec4 movingLightPos = glm::vec4(-10.0f, 100.0f, 10.0f, 1.0f); //light position


	rt3d::materialStruct material0 = {
		{0.2f, 0.4f, 0.2f, 1.0f}, // ambient
		{0.5f, 1.0f, 0.5f, 1.0f}, // diffuse
		{0.0f, 0.1f, 0.0f, 1.0f}, // specular
		2.0f  // shininess
	};
	rt3d::materialStruct material1 = {
		{0.4f, 0.4f, 1.0f, 1.0f}, // ambient
		{0.8f, 0.8f, 1.0f, 1.0f}, // diffuse
		{0.8f, 0.8f, 0.8f, 1.0f}, // specular
		1.0f  // shininess
	};
	rt3d::materialStruct materialGlossy = {
		{0.2f, 0.2f, 0.2f, 0.2f}, // ambient
		{3.0f, 3.0f, 3.0f, 3.0f}, // diffuse
		{4.0f, 4.0f, 4.0f, 1.0f}, // specular
		1.0f  // shininess
	};
};

