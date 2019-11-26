#include "ChromFresnelDemo.h"



ChromFresnelDemo::ChromFresnelDemo()
{

	shaderProgram = rt3d::initShaders("Res\\Shaders\\phong-tex.vert", "Res\\Shaders\\phong-tex.frag");
	rt3d::setLight(shaderProgram, light0);
	rt3d::setMaterial(shaderProgram, currentMaterial);
	// set light attenuation shader uniforms
	GLuint uniformIndex = glGetUniformLocation(shaderProgram, "attConst");
	glUniform1f(uniformIndex, attConstant);
	uniformIndex = glGetUniformLocation(shaderProgram, "attLinear");
	glUniform1f(uniformIndex, attLinear);
	uniformIndex = glGetUniformLocation(shaderProgram, "attQuadratic");
	glUniform1f(uniformIndex, attQuadratic);

	const char *cubeTexFiles[6] = 
	{
			"Res\\images\\Umar-SB\\back.bmp", "Res\\images\\Umar-SB\\front.bmp", "Res\\images\\Umar-SB\\right.bmp", "Res\\images\\Umar-SB\\left.bmp", "Res\\images\\Umar-SB\\top.bmp", "Res\\images\\Umar-SB\\bottom.bmp"
	};
	loadCubeMap(cubeTexFiles, &skybox[0]);
	reflectProgram = rt3d::initShaders("Res\\Shaders\\reflect.vert", "Res\\Shaders\\reflect.frag");
	rt3d::setLight(reflectProgram, light0);
	rt3d::setMaterial(reflectProgram, material0_matt);

	textures[0] = loadBitmap("Res\\images\\fabric.bmp");
	textures[1] = loadBitmap("Res\\images\\studdedmetal.bmp");

	uniformIndex = glGetUniformLocation(reflectProgram, "cubeMap");
	glUniform1i(uniformIndex, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	textureProgram = rt3d::initShaders("Res\\Shaders\\textured.vert", "Res\\Shaders\\textured.frag");
	skyboxProgram = rt3d::initShaders("Res\\Shaders\\cubeMap.vert", "Res\\Shaders\\cubeMap.frag");


	vector<GLfloat> verts;
	vector<GLfloat> norms;
	vector<GLfloat> tex_coords;
	vector<GLuint> indices;
	rt3d::loadObj("Res\\Models\\cube.obj", verts, norms, tex_coords, indices);
	meshIndexCount = indices.size();
	meshObjects[0] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), tex_coords.data(), meshIndexCount, indices.data());


	verts.clear(); norms.clear(); tex_coords.clear(); indices.clear();
	rt3d::loadObj("Res\\Models\\bunny-5000.obj", verts, norms, tex_coords, indices);
	toonIndexCount = indices.size();
	meshObjects[2] = rt3d::createMesh(verts.size() / 3, verts.data(), nullptr, norms.data(), nullptr, toonIndexCount, indices.data());

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

ChromFresnelDemo::~ChromFresnelDemo()
{
}

void ChromFresnelDemo::ImGuiRender()
{

}

void ChromFresnelDemo::Update(double interval)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_W]) eye = moveForward(eye, r, -0.1f);
	if (keys[SDL_SCANCODE_S]) eye = moveForward(eye, r, 0.1f);
	if (keys[SDL_SCANCODE_A]) eye = moveRight(eye, r, 0.1f);
	if (keys[SDL_SCANCODE_D]) eye = moveRight(eye, r, -0.1f);
	if (keys[SDL_SCANCODE_R]) eye.y -= 0.1;
	if (keys[SDL_SCANCODE_F]) eye.y += 0.1;

	if (keys[SDL_SCANCODE_I]) lightPos[2] -= 0.1;
	if (keys[SDL_SCANCODE_J]) lightPos[0] -= 0.1;
	if (keys[SDL_SCANCODE_K]) lightPos[2] += 0.1;
	if (keys[SDL_SCANCODE_L]) lightPos[0] += 0.1;
	if (keys[SDL_SCANCODE_U]) lightPos[1] += 0.1;
	if (keys[SDL_SCANCODE_H]) lightPos[1] -= 0.1;

	if (keys[SDL_SCANCODE_COMMA]) r -= 2.0f;
	if (keys[SDL_SCANCODE_PERIOD]) r += 2.0f;

	if (keys[SDL_SCANCODE_1]) ratioR += 0.01f;
	if (keys[SDL_SCANCODE_2]) ratioR -= 0.01f;

	if (keys[SDL_SCANCODE_3]) ratioG += 0.01f;
	if (keys[SDL_SCANCODE_4]) ratioG -= 0.01f;

	if (keys[SDL_SCANCODE_5]) ratioB += 0.01f;
	if (keys[SDL_SCANCODE_6]) ratioB -= 0.01f;


	if (keys[SDL_SCANCODE_B]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	if (keys[SDL_SCANCODE_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
}

void ChromFresnelDemo::Render()
{

	glEnable(GL_CULL_FACE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 150.0f);


	GLfloat scale(1.0f); // just to allow easy scaling of complete scene

	glm::mat4 modelview(1.0); // set base position for scene
	mvStack.push(modelview);

	at = moveForward(eye, r, 1.0f);
	mvStack.top() = glm::lookAt(eye, at, up);

	// skybox as single cube using cube map
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


	// back to remainder of rendering
	glDepthMask(GL_TRUE); // make sure depth test is on

	cubeDraw(projection);
		// remember to use at least one pop operation per push...
	mvStack.pop(); // initial matrix

	// draw a cube for ground plane
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), glm::vec3(-10.0f, -0.1f, -10.0f));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(20.0f, 20.0f, 20.0f));
	rt3d::setUniformMatrix4fv(shaderProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(shaderProgram, material0);
	rt3d::drawIndexedMesh(meshObjects[2], meshIndexCount, GL_TRIANGLES);
	mvStack.pop();
}

glm::vec3 ChromFresnelDemo::moveRight(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d * std::cos(r*DEG_TO_RADIAN), pos.y, pos.z + d * std::sin(r*DEG_TO_RADIAN));
}

glm::vec3 ChromFresnelDemo::moveForward(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d * std::sin(r*DEG_TO_RADIAN), pos.y, pos.z - d * std::cos(r*DEG_TO_RADIAN));
}

GLuint ChromFresnelDemo::loadBitmap(const char * fname)
{
	GLuint texID;
	glGenTextures(1, &texID); // generate texture ID

	// load file - using core SDL library
	SDL_Surface *tmpSurface;
	tmpSurface = SDL_LoadBMP(fname);
	if (!tmpSurface) {
		std::cout << "Error loading bitmap" << std::endl;
	}

	// bind texture and set parameters
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	SDL_PixelFormat *format = tmpSurface->format;

	GLuint externalFormat, internalFormat;
	if (format->Amask) {
		internalFormat = GL_RGBA;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGBA : GL_BGRA;
	}
	else {
		internalFormat = GL_RGB;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tmpSurface->w, tmpSurface->h, 0,
		externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer
	return texID;	// return value of texture ID
}

GLuint ChromFresnelDemo::loadCubeMap(const char * fname[6], GLuint * texID)
{
	glGenTextures(1, texID); // generate texture ID
	GLenum sides[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
						GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
						GL_TEXTURE_CUBE_MAP_POSITIVE_X,
						GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
						GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
						GL_TEXTURE_CUBE_MAP_NEGATIVE_Y };
	SDL_Surface *tmpSurface;

	glBindTexture(GL_TEXTURE_CUBE_MAP, *texID); // bind texture and set parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint externalFormat;
	for (int i = 0; i < 6; i++)
	{
		// load file - using core SDL library
		tmpSurface = SDL_LoadBMP(fname[i]);
		if (!tmpSurface)
		{
			std::cout << "Error loading bitmap" << std::endl;
			return *texID;
		}

		// skybox textures should not have alpha (assuming this is true!)
		SDL_PixelFormat *format = tmpSurface->format;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;

		glTexImage2D(sides[i], 0, GL_RGB, tmpSurface->w, tmpSurface->h, 0,
			externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
		// texture loaded, free the temporary buffer
		SDL_FreeSurface(tmpSurface);
	}
	return *texID;	// return value of texure ID, redundant really
}

void ChromFresnelDemo::cubeDraw(glm::mat4 & projection)
{
	glUseProgram(reflectProgram);
	glm::mat4 modelMatrix(1.0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	mvStack.push(mvStack.top());
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f, 1.0f, -3.0f));
	modelMatrix = glm::rotate(modelMatrix, float(theta*DEG_TO_RADIAN), glm::vec3(1.0f, 1.0f, 1.0f));
	mvStack.top() = mvStack.top() * modelMatrix;

	int uniformIndex = glGetUniformLocation(reflectProgram, "modelMatrix");
	glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glUniform3fv(uniformIndex, 1, glm::value_ptr(modelMatrix));
	rt3d::setUniformMatrix4fv(reflectProgram, "projection", glm::value_ptr(projection));

	uniformIndex = glGetUniformLocation(reflectProgram, "cameraPos");
	glUniform3fv(uniformIndex, 1, glm::value_ptr(eye));

	uniformIndex = glGetUniformLocation(reflectProgram, "ratioR");
	glUniform1f(uniformIndex, ratioR);
	uniformIndex = glGetUniformLocation(reflectProgram, "ratioG");
	glUniform1f(uniformIndex, ratioG);
	uniformIndex = glGetUniformLocation(reflectProgram, "ratioB");
	glUniform1f(uniformIndex, ratioB);
	rt3d::setUniformMatrix4fv(reflectProgram, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::setMaterial(reflectProgram, material1);
	rt3d::drawIndexedMesh(meshObjects[0], meshIndexCount, GL_TRIANGLES);
}
