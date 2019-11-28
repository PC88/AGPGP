#include "MonoChromDemo.h"
#include "vector"
#include "rt3dObjLoader.h"
#include "imgui.h"



MonoChromDemo::MonoChromDemo()
{
	shaderProgram_phong_monochrome = rt3d::initShaders("Res\\Shaders\\phong-tex-monochrome.vert", "Res\\Shaders\\phong-tex-monochrome.frag");
	rt3d::setCutOffLight(shaderProgram_phong_monochrome, light1);
	rt3d::setMaterial(shaderProgram_phong_monochrome, material0);
	rt3d::setMaterial(shaderProgram_phong_monochrome, material1);


	// start load bunny
	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;

	rt3d::loadObj("Res\\Models\\bunny-5000.obj", verts, norms, tex_coords, indices);
	toonIndexCount = indices.size();
	meshObjects[1] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), nullptr, toonIndexCount, indices.data());

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();
	// end load bunny

	// start load ground
	textures[0] = rt3d::loadBitmap("Res\\images\\fabric.bmp");
	rt3d::loadObj("Res\\Models\\BasicCube5.obj", verts, norms, tex_coords, indices);
	GLuint size = indices.size();
	meshIndexCount = size;
	meshObjects[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());

	verts.clear();
	norms.clear();
	tex_coords.clear();
	indices.clear();
	// end load ground


	// start load skybox
	skyboxProgram = rt3d::initShaders("Res\\Shaders\\textured.vert", "Res\\Shaders\\textured.frag");
	rt3d::loadObj("Res\\Models\\cube.obj", verts, norms, tex_coords, indices);
	size = indices.size();
	meshIndexCount2 = size;
	meshObjects[2] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), size, indices.data());
	skybox[0] = rt3d::loadBitmap("Res\\images\\moon_skybox\\moondust_rt.bmp");
	skybox[1] = rt3d::loadBitmap("Res\\images\\moon_skybox\\moondust_lf.bmp");
	skybox[2] = rt3d::loadBitmap("Res\\images\\moon_skybox\\moondust_ft.bmp");
	skybox[3] = rt3d::loadBitmap("Res\\images\\moon_skybox\\moondust_bk.bmp");
	// end load skybox

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


MonoChromDemo::~MonoChromDemo()
{
}

void MonoChromDemo::ImGuiRender()
{

}

void MonoChromDemo::Update(double interval)
{
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_W]) eye = moveForwardBack(eye, r, 0.51f);
	if (keys[SDL_SCANCODE_S]) eye = moveForwardBack(eye, r, -0.51f);
	if (keys[SDL_SCANCODE_A]) eye = moveRightLeft(eye, r, -0.51f);
	if (keys[SDL_SCANCODE_D]) eye = moveRightLeft(eye, r, 0.51f);
	if (keys[SDL_SCANCODE_Q]) eye.y += 0.51;
	if (keys[SDL_SCANCODE_E]) eye.y -= 0.51;

	if (keys[SDL_SCANCODE_COMMA]) r -= 1.0f;
	if (keys[SDL_SCANCODE_PERIOD]) r += 1.0f;

	if (keys[SDL_SCANCODE_V]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	if (keys[SDL_SCANCODE_C]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	//Control the spotlight's movement
	if (keys[SDL_SCANCODE_L]) {
		if (keys[SDL_SCANCODE_LCTRL])
		{
			movingLightPos += glm::vec4(-1, 0, 0, 0);
		}
		else
		{
			movingLightPos += glm::vec4(1, 0, 0, 0);
		}
	}
	if (keys[SDL_SCANCODE_U]) {
		if (keys[SDL_SCANCODE_LCTRL])
		{
			movingLightPos += glm::vec4(0, -1, 0, 0);
		}
		else
		{
			movingLightPos += glm::vec4(0, 1, 0, 0);
		}
	}
	if (keys[SDL_SCANCODE_K]) {
		if (keys[SDL_SCANCODE_LCTRL])
		{
			movingLightPos += glm::vec4(0, 0, -1, 0);
		}
		else
		{
			movingLightPos += glm::vec4(0, 0, 1, 0);
		}
	}
	if (keys[SDL_SCANCODE_J]) {
		if (keys[SDL_SCANCODE_LCTRL])
		{
			movingLightPos -= glm::vec4(-1, 0, 0, 0);
		}
		else
		{
			movingLightPos -= glm::vec4(1, 0, 0, 0);
		}
	}
	if (keys[SDL_SCANCODE_O]) {
		if (keys[SDL_SCANCODE_LCTRL])
		{
			movingLightPos -= glm::vec4(0, -1, 0, 0);
		}
		else
		{
			movingLightPos -= glm::vec4(0, 1, 0, 0);
		}
	}
	if (keys[SDL_SCANCODE_I]) {
		if (keys[SDL_SCANCODE_LCTRL])
		{
			movingLightPos -= glm::vec4(0, 0, -1, 0);
		}
		else
		{
			movingLightPos -= glm::vec4(0, 0, 1, 0);
		}
	}
}

void MonoChromDemo::Render()
{
	// clear the screen
	glEnable(GL_CULL_FACE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 5000.0f);
	rt3d::setUniformMatrix4fv(shaderProgram_phong_monochrome, "projection", glm::value_ptr(projection));


	GLfloat scale(1.0f); // just to allow easy scaling of complete scene

	glm::mat4 modelview(1.0); // set base position for scene
	mvStack.push(modelview);

	at = moveForwardBack(eye, r, 1.0f);
	mvStack.top() = glm::lookAt(eye, at, up);


	/////SKYBOX/////
	glUseProgram(skyboxProgram);
	rt3d::setUniformMatrix4fv(skyboxProgram, "projection", glm::value_ptr(projection));
	glDepthMask(GL_FALSE); // make sure depth test is off
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	mvStack.push(glm::mat4(mvRotOnlyMat3));
	//Front
	mvStack.push(mvStack.top());
	glBindTexture(GL_TEXTURE_2D, skybox[0]);
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(2.0f, 2.0f, 2.0f));
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(0.0f, 0.0f, -2.0f));
	rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(meshObjects[2], meshIndexCount2, GL_TRIANGLES);
	mvStack.pop();
	//Back
	mvStack.push(mvStack.top());
	glBindTexture(GL_TEXTURE_2D, skybox[1]);
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(2.0f, 2.0f, 2.0f));
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(0.0f, 0.0f, 2.0f));
	rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(meshObjects[2], meshIndexCount2, GL_TRIANGLES);
	mvStack.pop();
	//Left
	mvStack.push(mvStack.top());
	glBindTexture(GL_TEXTURE_2D, skybox[2]);
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(2.0f, 2.0f, 2.0f));
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-2.0f, 0.0f, 0.0f));
	rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(meshObjects[2], meshIndexCount2, GL_TRIANGLES);
	mvStack.pop();
	//Right
	mvStack.push(mvStack.top());
	glBindTexture(GL_TEXTURE_2D, skybox[3]);
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(2.0f, 2.0f, 2.0f));
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(2.0f, 0.0f, 0.0f));
	rt3d::setUniformMatrix4fv(skyboxProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(meshObjects[2], meshIndexCount2, GL_TRIANGLES);
	mvStack.pop();


	mvStack.pop();


	// back to remainder of rendering
	glDepthMask(GL_TRUE); // make sure depth test is on


	glUseProgram(shaderProgram_phong_monochrome);
	//handle moving light
	glm::vec4 tmp = mvStack.top() * movingLightPos;
	light1.position[0] = tmp.x;
	light1.position[1] = tmp.y;
	light1.position[2] = tmp.z;
	rt3d::setLightPos(shaderProgram_phong_monochrome, glm::value_ptr(tmp));
	// draw a cube for the moving light
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(movingLightPos.x, movingLightPos.y, movingLightPos.z));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.0f, 1.0f, 1.0f));
	mvStack.top() = glm::rotate(mvStack.top(), 3.14f, glm::vec3(0.5f, 0.5f, 0.5f));
	rt3d::setUniformMatrix4fv(shaderProgram_phong_monochrome, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(shaderProgram_phong_monochrome, material1);
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();

	//glUseProgram(shaderProgram1stBunny);
	rt3d::setUniformMatrix4fv(shaderProgram_phong_monochrome, "projection", glm::value_ptr(projection));

	int uniformIndex = glGetUniformLocation(shaderProgram_phong_monochrome,	"attConst");
	glUniform1f(uniformIndex, attConstant);



	//Set-up
	glm::vec4 v_spotDir = glm::vec4(0.0, -1.0, 0.0, 1.0);
	glm::vec4 tmp_s = v_spotDir * mvStack.top();
	//rt3d::setLightPos(shaderProgram_phong_monochrome, glm::value_ptr(tmp_s));
	rt3d::setSpotLightPos(shaderProgram_phong_monochrome, glm::value_ptr(tmp_s));
	//Draw the first bunny
	glUseProgram(shaderProgram_phong_monochrome);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(0, 0, 0));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(80.0f, 80.0f, 80.0f));
	rt3d::setUniformMatrix4fv(shaderProgram_phong_monochrome, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(shaderProgram_phong_monochrome, material1);
	rt3d::drawIndexedMesh(meshObjects[1], toonIndexCount, GL_TRIANGLES);
	mvStack.pop();

	//Draw the second bunny
	glUseProgram(shaderProgram_phong_monochrome);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(100, 0, 0));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(80.0f, 80.0f, 80.0f));
	mvStack.top() = glm::rotate(mvStack.top(), 3.14f, glm::vec3(0, 0.3f, 0));
	rt3d::setUniformMatrix4fv(shaderProgram_phong_monochrome, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(shaderProgram_phong_monochrome, material1);
	rt3d::drawIndexedMesh(meshObjects[1], toonIndexCount, GL_TRIANGLES);
	mvStack.pop();


	//Draw the third bunny
	glUseProgram(shaderProgram_phong_monochrome);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(60, 0, 50));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(80.0f, 80.0f, 80.0f));
	mvStack.top() = glm::rotate(mvStack.top(), 3.14f, glm::vec3(0, 0.5f, 0));
	rt3d::setUniformMatrix4fv(shaderProgram_phong_monochrome, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(shaderProgram_phong_monochrome, material1);
	rt3d::drawIndexedMesh(meshObjects[1], toonIndexCount, GL_TRIANGLES);
	mvStack.pop();


	//Draw the ground
	glUseProgram(shaderProgram_phong_monochrome);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(0, 2, 0));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(300.0f, 1.0f, 300.0f));
	rt3d::setUniformMatrix4fv(shaderProgram_phong_monochrome, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(shaderProgram_phong_monochrome, material0);
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();

	// remember to use at least one pop operation per push...
	mvStack.pop(); // initial matrix
	glDepthMask(GL_TRUE);
}

glm::vec3 MonoChromDemo::moveForwardBack(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d * std::sin(r * DEG_TO_RADIAN), pos.y,
		pos.z - d * std::cos(r * DEG_TO_RADIAN));
}

glm::vec3 MonoChromDemo::moveRightLeft(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d * std::cos(r * DEG_TO_RADIAN), pos.y,
		pos.z + d * std::sin(r * DEG_TO_RADIAN));
}
