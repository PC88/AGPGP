#include "TestDemo.h"
#include "vector"
#include "rt3dObjLoader.h"
#include "imgui.h"



TestDemo::TestDemo()
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

	const char *cubeTexFiles[6] =
	{
		"Res\\images\\hw_nightsky\\nightsky_bk.bmp", "Res\\images\\hw_nightsky\\nightsky_ft.bmp", "Res\\images\\hw_nightsky\\nightsky_rt.bmp",
		"Res\\images\\hw_nightsky\\nightsky_lf.bmp", "Res\\images\\hw_nightsky\\nightsky_up.bmp", "Res\\images\\hw_nightsky\\nightsky_dn.bmp"
	};
	rt3d::loadCubeMap(cubeTexFiles, &skybox[0]);

	// init shader

	skyboxProgram = rt3d::initShaders("Res\\Shaders\\cubeMap.vert", "Res\\Shaders\\cubeMap.frag");
	ReflectRefractProg = rt3d::initShaders("Res\\Shaders\\reflect.vert", "Res\\Shaders\\reflect.frag");
	rt3d::setLight(ReflectRefractProg, light0);
	rt3d::setMaterial(ReflectRefractProg, material0);

	uniformIndex = glGetUniformLocation(ReflectRefractProg, "cubeMap");
	glUniform1i(uniformIndex, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[1]);


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
}


TestDemo::~TestDemo()
{
}

void TestDemo::Update(double interval)
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

	if (keys[SDL_SCANCODE_2]) ratioR += 0.01f;
	if (keys[SDL_SCANCODE_3]) ratioR -= 0.01f;

	if (keys[SDL_SCANCODE_4]) ratioG += 0.01f;
	if (keys[SDL_SCANCODE_5]) ratioG -= 0.01f;

	if (keys[SDL_SCANCODE_6]) ratioB += 0.01f;
	if (keys[SDL_SCANCODE_7]) ratioB -= 0.01f;

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

void TestDemo::ImGuiRender()
{
}

void TestDemo::Render()
{
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

	//skybox
	glUseProgram(skyboxProgram);
	rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));

	glDepthMask(GL_FALSE); // make sure writing to update depth test is off
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	mvStack.push(glm::mat4(mvRotOnlyMat3));

	glCullFace(GL_FRONT); // drawing inside of cube!
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));
	rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK); // drawing inside of cube!


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

	/// draw cube to be MAPPED
	/*glUseProgram(ReflectRefractProg);
	rt3d::setUniformMatrix4fv(ReflectRefractProg, "projection", glm::value_ptr(projection));
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -1.0f, -20.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(10.0f, 10.0f, 10.0f));
	rt3d::setUniformMatrix4fv(ReflectRefractProg, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(ReflectRefractProg, material0);
	int ID = glGetUniformLocation(ReflectRefractProg, "cameraPos");
	glUniform3f(ID, eye.x, eye.y, eye.z);
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
*/

	glUseProgram(ReflectRefractProg);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(10.0f, 10.0f, 10.0f));
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-2.0f, 1.0f, -3.0f));

	int uniformIndex = glGetUniformLocation(ReflectRefractProg, "modelMatrix");
	glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniform3fv(uniformIndex, 1, glm::value_ptr(mvStack.top()));
	rt3d::setUniformMatrix4fv(ReflectRefractProg, "projection", glm::value_ptr(projection));

	uniformIndex = glGetUniformLocation(ReflectRefractProg, "cameraPos");
	glUniform3fv(uniformIndex, 1, glm::value_ptr(eye));

	uniformIndex = glGetUniformLocation(ReflectRefractProg, "ratioR");
	glUniform1f(uniformIndex, ratioR);
	uniformIndex = glGetUniformLocation(ReflectRefractProg, "ratioG");
	glUniform1f(uniformIndex, ratioG);
	uniformIndex = glGetUniformLocation(ReflectRefractProg, "ratioB");
	glUniform1f(uniformIndex, ratioB);
	rt3d::setUniformMatrix4fv(ReflectRefractProg, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(ReflectRefractProg, material1);
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
}

glm::vec3 TestDemo::moveForward(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d * std::sin(rotation * DEG_TO_RADIAN), pos.y,
		pos.z - d * std::cos(rotation * DEG_TO_RADIAN));
}

glm::vec3 TestDemo::moveRight(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d * std::cos(rotation * DEG_TO_RADIAN), pos.y,
		pos.z + d * std::sin(rotation * DEG_TO_RADIAN));

}
