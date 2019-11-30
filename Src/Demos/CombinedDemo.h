#pragma once
#include "Demo.h"
#include "rt3d.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include "vector"
#include "rt3dObjLoader.h"
#include "imgui.h"
#include <thread>

#define DEG_TO_RADIAN 0.017453293

using namespace std;

class CombinedDemo :
	public Demo
{
public:
	CombinedDemo();
	virtual ~CombinedDemo();


	virtual void ImGuiRender() override;
	virtual void Update(double interval) override;
	virtual void Render() override;

	glm::vec3 moveRightLeft(glm::vec3 pos, GLfloat angle, GLfloat d);

	glm::vec3 moveForwardBack(glm::vec3 pos, GLfloat angle, GLfloat d);


	// functions to be threaded
	void LoadFBO();
	void SetUpQuad();
	void LoadShaders();
	// test
	void LoadShaders2(GLuint& BSP, rt3d::lightStruct& L0, rt3d::materialStruct& M0, GLuint& STSP, GLuint& LP, 
		GLuint& PMSP, rt3d::lightStruct& L1, rt3d::materialStruct& M1  );
	void LoadModels();
	void LoadSkybox();

private:
	//std::thread shaderWorker;
	//std::thread skyboxWorker;
	//std::thread modelWorker;
	//std::thread quadWorker;
	//std::thread fboWorker;

private:



	GLuint meshIndexCount = 0;
	GLuint toonIndexCount = 0;
	GLuint meshObjects[2];

	glm::vec3 eye = glm::vec3(-2.0f, 10.0f, 8.0f);
	glm::vec3 at = glm::vec3(0.0f, 1.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	stack<glm::mat4> mvStack;

	// shader handles
	GLuint basicShaderProgram;
	GLuint skyboxProgram;
	GLuint reflectRefractProgram;
	GLuint shaderProgram_phong_monochrome;
	GLuint lightProgram;

	GLfloat ratioR = 1.35;
	GLfloat ratioG = 1.36;
	GLfloat ratioB = 1.37;

	GLfloat rotation = 0.0f;

	// TEXTURE STUFF
	GLuint textures[3];
	GLuint skybox[5];
	GLuint labels[5];

	glm::vec4 AmbientLight = glm::vec4(4.0f, 3.0f, 0.3f, 1.0f);

	rt3d::lightStruct light0 = {
		{0.4f, 0.4f, 0.4f, 1.0f}, // ambient
		{1.0f, 1.0f, 1.0f, 1.0f}, // diffuse
		{1.0f, 1.0f, 1.0f, 1.0f}, // specular
		{-5.0f, 2.0f, 2.0f, 1.0f}  // position
	};
	glm::vec4 lightPos = glm::vec4(-5.0f, 2.0f, 2.0f, 1.0f); //light position

	rt3d::lightStruct light1 = {
	{0.3f, 0.3f, 0.3f, 0.0f}, // ambient
	{1.0f, 1.0f, 1.0f, 0.0f}, // diffuse
	{1.0f, 1.0f, 1.0f, 0.0f}, // specular
	{-10.0f, 10.0f, 10.0f, 1.0f},  // position
	{0.8f} //Cutoff Angle
	};
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
	const GLenum fboAttachments[2] = { GL_COLOR_ATTACHMENT0 }; // color buffer attachment
	const GLenum frameBuff[1] = { GL_BACK_LEFT }; // back buffer FBO attachment

	// data for the quad which is drawn as a lens
	GLuint screenTexShaderProgram;

	// uniform interactive variables
	float edgeColor[3] = { 0.0f, 0.0f, 0.0f };
	float Herp1 = 0.0f;
	float Herp2 = 1.0f;

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