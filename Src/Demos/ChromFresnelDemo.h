#pragma once
#include "Demo.h"


#include "rt3d.h"
#include "rt3dObjLoader.h"
#include <stack>
#include "md2model.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define DEG_TO_RADIAN 0.017453293

class ChromFresnelDemo :
	public Demo
{
public:
	ChromFresnelDemo();
	virtual ~ChromFresnelDemo();


	virtual void ImGuiRender() override;
	virtual void Update(double interval) override;
	virtual void Render() override;

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);
	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);

	GLuint loadBitmap(const char *fname);
	GLuint loadCubeMap(const char *fname[6], GLuint *texID);
	void cubeDraw(glm::mat4 &projection);

private:
	GLfloat offset;

	GLuint meshIndexCount = 0;
	GLuint toonIndexCount = 0;
	GLuint md2VertCount = 0;
	GLuint meshObjects[3];

	GLuint textureProgram;
	GLuint shaderProgram;
	GLuint skyboxProgram;

	rt3d::materialStruct material0 = {
		{0.2f, 0.4f, 0.2f, 1.0f}, // ambient
		{0.5f, 1.0f, 0.5f, 1.0f}, // diffuse
		{0.0f, 0.1f, 0.0f, 1.0f}, // specular
		2.0f  // shininess
	};
	GLuint reflectProgram;

	GLuint gouraud;
	bool shiftDown = false;

	GLfloat specular2[4] = { 4.0f, 4.0f, 4.0f, 1.0f };

	GLfloat r = 0.0f;

	glm::vec3 eye = glm::vec3(-2.0f, 1.0f, 8.0f);
	glm::vec3 at = glm::vec3(0.0f, 1.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	stack<glm::mat4> mvStack;

	GLuint textures[3];
	GLuint skybox[5];
	GLuint labels[5];


	GLfloat ratioR = 1.35;

	GLfloat ratioG = 1.36;
	GLfloat ratioB = 1.37;

	rt3d::lightStruct light0 = {
		{0.4f, 0.4f, 0.4f, 1.0f}, // ambient
		{1.0f, 1.0f, 1.0f, 1.0f}, // diffuse
		{1.0f, 1.0f, 1.0f, 1.0f}, // specular
		{-5.0f, 2.0f, 2.0f, 1.0f}  // position
	};
	glm::vec4 lightPos = glm::vec4(-5.0f, 2.0f, 2.0f, 1.0f); //light position

	rt3d::materialStruct material0_matt = {
		{0.2f, 0.4f, 0.2f, 1.0f}, // ambient
		{0.5f, 1.0f, 0.5f, 1.0f}, // diffuse
		{0.0f, 0.1f, 0.0f, 1.0f}, // specular
		0.25f  // specular weight
	};
	rt3d::materialStruct material0_gloss = {
		{0.2f, 0.4f, 0.2f, 1.0f}, // ambient
		{0.5f, 0.2f, 0.5f, 1.0f}, // diffuse
		{1.0f, 1.1f, 0.0f, 1.0f}, // specular
	1.0f  // specular weight
	};
	rt3d::materialStruct material1 = {
		{0.4f, 0.4f, 1.0f, 1.0f}, // ambient
		{0.8f, 0.8f, 1.0f, 1.0f}, // diffuse
		{0.8f, 0.8f, 0.8f, 1.0f}, // specular
		1.0f  // shininess
	};


	rt3d::materialStruct currentMaterial = material0_matt;
	rt3d::materialStruct currentMaterial2 = material0_matt;
	// light attenuation
	float attConstant = 1.0f;
	float attLinear = 0.01f;
	float attQuadratic = 0.01f;


	float theta = 0.0f;

};

