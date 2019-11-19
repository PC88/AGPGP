#pragma once
#include "Demo.h"
#include "rt3d.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>

#define DEG_TO_RADIAN 0.017453293

using namespace std;

class SobelDemo :
	public Demo
{
public:
	SobelDemo();
	virtual ~SobelDemo();


	virtual void Update(double interval) override;
	virtual void ImGuiRender() override;
	virtual void Render() override;

	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);

private:

	GLuint meshIndexCount = 0;
	GLuint toonIndexCount = 0;
	GLuint meshObjects[2];

	glm::vec3 eye = glm::vec3(-2.0f, 1.0f, 8.0f);
	glm::vec3 at = glm::vec3(0.0f, 1.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	stack<glm::mat4> mvStack;

	// shader handles
	GLuint SobelShaderProgram;
	GLuint shaderProgram;

	GLfloat rotation = 0.0f;

	// TEXTURE STUFF
	GLuint textures[3];
	GLuint skybox[5];
	GLuint labels[5];

	rt3d::lightStruct light0 = {
		{0.4f, 0.4f, 0.4f, 1.0f}, // ambient
		{1.0f, 1.0f, 1.0f, 1.0f}, // diffuse
		{1.0f, 1.0f, 1.0f, 1.0f}, // specular
		{-5.0f, 2.0f, 2.0f, 1.0f}  // position
	};

	glm::vec4 lightPos = glm::vec4(-5.0f, 2.0f, 2.0f, 1.0f); //light position

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

	// light attenuation
	float attConstant = 1.0f;
	float attLinear = 0.0f;
	float attQuadratic = 0.0f;

	float theta = 0.0f;

	/// FBO details ///
	GLuint fboID;
	GLuint depthStencilBufID;
	GLuint screenTex;
	GLuint screenWidth = 800;
	GLuint screenHeight = 600;
	const GLenum fboAttachments[1] = { GL_COLOR_ATTACHMENT0 }; // color buffer attachment
	const GLenum frameBuff[1] = { GL_BACK_LEFT }; // back buffer FBO attachment

	// data for the quad which is drawn as a lens
	GLuint screenTexShaderProgram;

	// uniform interactive variables
	float edgeColor[3] = { 1.0f, 1.0f, 1.0f };
	float Herp1;
	float Herp2;

	unsigned int quadVAO, quadVBO;
	float quadVertices[32] = 
	{ // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};
};

