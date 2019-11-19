#include "SobelDemo.h"
#include "vector"
#include "rt3dObjLoader.h"
#include "imgui.h"



SobelDemo::SobelDemo()
{
	// initialize the shaders
	shaderProgram = rt3d::initShaders("Res\\Shaders\\phong-tex.vert", "Res\\Shaders\\phong-tex.frag");
	rt3d::setLight(shaderProgram, light0);
	rt3d::setMaterial(shaderProgram, material0);
	// set light attenuation shader uniforms
	GLuint uniformIndex = glGetUniformLocation(shaderProgram, "attConst");
	glUniform1f(uniformIndex, attConstant);
	uniformIndex = glGetUniformLocation(shaderProgram, "attLinear");
	glUniform1f(uniformIndex, attLinear);
	uniformIndex = glGetUniformLocation(shaderProgram, "attQuadratic");
	glUniform1f(uniformIndex, attQuadratic);

	// init Sobel shader
	SobelShaderProgram = rt3d::initShaders("Res\\Shaders\\Sobel.vert", "Res\\Shaders\\Sobel.frag");
	rt3d::setLight(SobelShaderProgram, light0);
	rt3d::setMaterial(SobelShaderProgram, material0);

	// iniit the quad shader
	screenTexShaderProgram = rt3d::initShaders("Res\\Shaders\\FBOtest.vert", "Res\\Shaders\\FBOtest.frag");
	// load the cube model
	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj("Res\\Models\\cube.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();
	textures[0] = rt3d::loadBitmap("Res\\images\\fabric.bmp");
	meshObjects[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());

	// load another texture
	textures[2] = rt3d::loadBitmap("Res\\images\\studdedmetal.bmp");

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	////////////////////
	// Initialize FBO //
	////////////////////

	// Generate FBO, RBO & Texture handles
	glGenFramebuffers(1, &fboID);
	glGenRenderbuffers(1, &depthStencilBufID);
	glGenTextures(1, &screenTex);

	// Bind FBO, RBO & Texture & init storage and params 
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBufID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindTexture(GL_TEXTURE_2D, screenTex);
	// Need to set mag and min params 
	// otherwise mipmaps are assumed 
	// This fixes problems with NVIDIA cards 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL); // null passes as data will be filled later

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Map COLOR_ATTACHMENT0 to texture & DEPTH_ATTACHMENT to depth buffer RBO
	// this actually attached the values, RBO`s and textures to the FBO`s
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilBufID);

	//////// Check for errors ////////
	GLenum valid = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (valid != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Frame buffer Object not complete" << std::endl;
	}
	if (valid == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
	{
		std::cout << "Frame buffer incomplete attachment" << std::endl;
	}
	if (valid == GL_FRAMEBUFFER_UNSUPPORTED)
	{
		std::cout << "FBO attachments unsupported" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // now unbind post checking
	////////////////////
	// Initialize FBO //
	////////////////////
}


SobelDemo::~SobelDemo()
{
	glDeleteFramebuffers(1, &fboID);
}

void SobelDemo::Update(double interval)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_W]) eye = moveForward(eye, rotation, 0.1f);
	if (keys[SDL_SCANCODE_S]) eye = moveForward(eye, rotation, -0.1f);
	if (keys[SDL_SCANCODE_A]) eye = moveRight(eye, rotation, -0.1f);
	if (keys[SDL_SCANCODE_D]) eye = moveRight(eye, rotation, 0.1f);
	if (keys[SDL_SCANCODE_R]) eye.y += 0.1f;
	if (keys[SDL_SCANCODE_F]) eye.y -= 0.1f;

	if (keys[SDL_SCANCODE_I]) lightPos[2] -= 0.1f;
	if (keys[SDL_SCANCODE_J]) lightPos[0] -= 0.1f;
	if (keys[SDL_SCANCODE_K]) lightPos[2] += 0.1f;
	if (keys[SDL_SCANCODE_L]) lightPos[0] += 0.1f;
	if (keys[SDL_SCANCODE_U]) lightPos[1] += 0.1f;
	if (keys[SDL_SCANCODE_H]) lightPos[1] -= 0.1f;

	if (keys[SDL_SCANCODE_COMMA]) rotation -= 1.0f;
	if (keys[SDL_SCANCODE_PERIOD]) rotation += 1.0f;

	if (keys[SDL_SCANCODE_1]) 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	if (keys[SDL_SCANCODE_2]) 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
}

void SobelDemo::ImGuiRender()
{
	ImGui::Begin("Demos");
	ImGui::SliderFloat("Hermite interpolation 1", &Herp1, 0.0f, 1.0f);
	ImGui::SliderFloat("Hermite interpolation 2", &Herp2, 0.0f, 1.0f);
	ImGui::SliderFloat3("Edge Colour values", edgeColor, 0.0f, 1.0f);
	ImGui::End();
}

void SobelDemo::Render()
{

	// bind to framebuffer and draw scene as we normally would to color texture 
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	glEnable(GL_CULL_FACE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 150.0f);


	GLfloat scale(1.0f); // just to allow easy scaling of complete scene

	glm::mat4 modelview(1.0); // set base position for scene
	mvStack.push(modelview);

	at = moveForward(eye, rotation, 1.0f);
	mvStack.top() = glm::lookAt(eye, at, up);

	// draw 
	glUseProgram(shaderProgram);
	rt3d::setUniformMatrix4fv(shaderProgram, "projection", glm::value_ptr(projection));

	/// draw light
	glm::vec4 tmp = mvStack.top() * lightPos;
	light0.position[0] = tmp.x;
	light0.position[1] = tmp.y;
	light0.position[2] = tmp.z;
	rt3d::setLightPos(shaderProgram, glm::value_ptr(tmp));

	// draw a small cube block at lightPos
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(0.25f, 0.25f, 0.25f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(shaderProgram, material0);
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();

	/// draw a cube for ground plane
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(20.0f, 0.1f, 20.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(shaderProgram, material0);
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();

	/// draw cube to be Sobel filtered
	glUseProgram(SobelShaderProgram);
	rt3d::setUniformMatrix4fv(SobelShaderProgram, "projection", glm::value_ptr(projection));
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-1.0f, 1.1f, -1.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.0f, 1.0f, 1.0f));
	rt3d::setUniformMatrix4fv(SobelShaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(SobelShaderProgram, material0);

	// calculate normal mat CPU side
	glm::mat3 normMat = glm::transpose(glm::inverse(mvStack.top()));
	rt3d::setUniformMatrix4fv(SobelShaderProgram, "normalMatrix", glm::value_ptr(normMat));

	glm::mat4 MVP = mvStack.top() * projection; // get the MVP directly
	rt3d::setUniformMatrix4fv(SobelShaderProgram, "modelviewproj", glm::value_ptr(MVP));

	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();

	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(screenTexShaderProgram);
	int uniformIndex = glGetUniformLocation(screenTexShaderProgram, "edgeColor");
	glUniform3f(uniformIndex, edgeColor[0], edgeColor[1], edgeColor[2]);

	uniformIndex = glGetUniformLocation(screenTexShaderProgram, "herp1");
	glUniform1f(uniformIndex, Herp1);

	uniformIndex = glGetUniformLocation(screenTexShaderProgram, "herp2");
	glUniform1f(uniformIndex, Herp2);

	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, screenTex);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

glm::vec3 SobelDemo::moveForward(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d * std::sin(rotation * DEG_TO_RADIAN), pos.y,
		pos.z - d * std::cos(rotation * DEG_TO_RADIAN));
}

glm::vec3 SobelDemo::moveRight(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d * std::cos(rotation * DEG_TO_RADIAN), pos.y,
		pos.z + d * std::sin(rotation * DEG_TO_RADIAN));

}
