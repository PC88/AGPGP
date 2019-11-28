#include "rt3d.h"
#include <map>

using namespace std;

namespace rt3d {

// struct vaoBuffers will be used inside the rt3d library
// clients should not need to know about this data structure
struct vaoBuffers {
	GLuint vertex_buffer;
	GLuint colour_buffer;
	GLuint normal_buffer;
	GLuint texcoord_buffer;
	GLuint index_buffer;
};

static map<GLuint, GLuint *> vertexArrayMap;

// Something went wrong - print error message and quit
void exitFatalError(const char *message) {
    cout << message << " ";
    exit(1);
}

// loadFile - loads text file from file fname as a char* 
// Allocates memory - so remember to delete after use
// size of file returned in fSize
char* loadFile(const char *fname, GLint &fSize) {
	int size;
	char * memblock;

	// file read based on example in cplusplus.com tutorial
	// ios::ate opens file at the end
	ifstream file (fname, ios::in|ios::binary|ios::ate);
	if (file.is_open()) {
		size = (int) file.tellg(); // get location of file pointer i.e. file size
		fSize = (GLint) size;
		memblock = new char [size];
		file.seekg (0, ios::beg);
		file.read (memblock, size);
		file.close();
		cout << "file " << fname << " loaded" << endl;
	}
	else {
		cout << "Unable to open file " << fname << endl;
		fSize = 0;
		// should ideally set a flag or use exception handling
		// so that calling function can decide what to do now
		return nullptr;
	}
	return memblock;
}

// printShaderError
// Display (hopefully) useful error messages if shader fails to compile or link
void printShaderError(const GLint shader) {
	int maxLength = 0;
	int logLength = 0;
	GLchar *logMessage;

	// Find out how long the error message is
	if (!glIsShader(shader))
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
	else
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	if (maxLength > 0) { // If message has some contents
		logMessage = new GLchar[maxLength];
		if (!glIsShader(shader))
			glGetProgramInfoLog(shader, maxLength, &logLength, logMessage);
		else
			glGetShaderInfoLog(shader,maxLength, &logLength, logMessage);
		cout << "Shader Info Log:" << endl << logMessage << endl;
		delete [] logMessage;
	}
	// should additionally check for OpenGL errors here
}


GLuint initShaders(const char *vertFile, const char *fragFile) {
	GLuint p, f, v;

	char *vs,*fs;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);	

	// load shaders & get length of each
	GLint vlen;
	GLint flen;
	vs = loadFile(vertFile,vlen);
	fs = loadFile(fragFile,flen);
	
	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv,&vlen);
	glShaderSource(f, 1, &ff,&flen);
	
	GLint compiled;

	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		cout << "Vertex shader not compiled." << endl;
		rt3d::printShaderError(v);
	} 

	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		cout << "Fragment shader not compiled." << endl;
		rt3d::printShaderError(f);
	} 
	
	p = glCreateProgram();
		
	glAttachShader(p,v);
	glAttachShader(p,f);

	glBindAttribLocation(p,RT3D_VERTEX,"in_Position");
	glBindAttribLocation(p,RT3D_COLOUR,"in_Color");
	glBindAttribLocation(p,RT3D_NORMAL,"in_Normal");
	glBindAttribLocation(p,RT3D_TEXCOORD,"in_TexCoord");

	glLinkProgram(p);
	glUseProgram(p);

	delete [] vs; // dont forget to free allocated memory
	delete [] fs; // we allocated this in the loadFile function...

	return p;
}

GLuint createMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours, 
	const GLfloat* normals, const GLfloat* texcoords, const GLuint indexCount, const GLuint* indices) {
	GLuint VAO;
	// generate and set up a VAO for the mesh
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint *pMeshBuffers = new GLuint[5];


	if (vertices == nullptr) {
		// cant create a mesh without vertices... oops
		exitFatalError("Attempt to create a mesh with no vertices");
	}

	// generate and set up the VBOs for the data
	GLuint VBO;
	glGenBuffers(1, &VBO);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3*numVerts*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)RT3D_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(RT3D_VERTEX);
	pMeshBuffers[RT3D_VERTEX] = VBO;


	// VBO for colour data
	if (colours != nullptr) {
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 3*numVerts*sizeof(GLfloat), colours, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)RT3D_COLOUR, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(RT3D_COLOUR);
		pMeshBuffers[RT3D_COLOUR] = VBO;
	}

	// VBO for normal data
	if (normals != nullptr) {
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 3*numVerts*sizeof(GLfloat), normals, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)RT3D_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(RT3D_NORMAL);
		pMeshBuffers[RT3D_NORMAL] = VBO;
	}

	// VBO for tex-coord data
	if (texcoords != nullptr) {
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 2*numVerts*sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)RT3D_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(RT3D_TEXCOORD);
		pMeshBuffers[RT3D_TEXCOORD] = VBO;
	}

	if (indices != nullptr && indexCount > 0) {
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);
		pMeshBuffers[RT3D_INDEX] = VBO;
	}
	// unbind vertex array
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// return the identifier needed to draw this mesh

	vertexArrayMap.insert( pair<GLuint, GLuint *>(VAO, pMeshBuffers) );

	return VAO;
}

GLuint createMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours, 
	const GLfloat* normals, const GLfloat* texcoords) {
	return createMesh(numVerts, vertices, colours, normals, texcoords, 0, nullptr);
}

GLuint createMesh(const GLuint numVerts, const GLfloat* vertices) {
	return createMesh(numVerts, vertices, nullptr, nullptr, nullptr);
}

GLuint createColourMesh(const GLuint numVerts, const GLfloat* vertices, const GLfloat* colours) {
	return createMesh(numVerts, vertices, colours, nullptr, nullptr);
}

void setUniformMatrix4fv(const GLuint program, const char* uniformName, const GLfloat *data) {
	int uniformIndex = glGetUniformLocation(program, uniformName);
	glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, data); 
}


void setLightPos(const GLuint program, const GLfloat *lightPos) {
	int uniformIndex = glGetUniformLocation(program, "lightPosition");
	glUniform4fv(uniformIndex, 1, lightPos);
}

void setProjection(const GLuint program, const GLfloat *data) {
	int uniformIndex = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, data); 
}

void setLight(const GLuint program, const lightStruct light) {
	// pass in light data to shader
	int uniformIndex = glGetUniformLocation(program, "light.ambient");
	glUniform4fv(uniformIndex, 1, light.ambient);
	uniformIndex = glGetUniformLocation(program, "light.diffuse");
	glUniform4fv(uniformIndex, 1, light.diffuse);
	uniformIndex = glGetUniformLocation(program, "light.specular");
	glUniform4fv(uniformIndex, 1, light.specular);
	uniformIndex = glGetUniformLocation(program, "lightPosition");
	glUniform4fv(uniformIndex, 1, light.position);
}

void setCutOffLight(const GLuint program, const lightStructCutOff light) {
	// pass in light data to shader
	int uniformIndex = glGetUniformLocation(program, "light.ambient");
	glUniform4fv(uniformIndex, 1, light.ambient);
	uniformIndex = glGetUniformLocation(program, "light.diffuse");
	glUniform4fv(uniformIndex, 1, light.diffuse);
	uniformIndex = glGetUniformLocation(program, "light.specular");
	glUniform4fv(uniformIndex, 1, light.specular);
	uniformIndex = glGetUniformLocation(program, "lightPosition");
	glUniform1f(uniformIndex, light.cutOff);
	uniformIndex = glGetUniformLocation(program, "cutOff");
}

void setMaterial(const GLuint program, const materialStruct material) {
	// pass in material data to shader 
	int uniformIndex = glGetUniformLocation(program, "material.ambient");
	glUniform4fv(uniformIndex, 1, material.ambient);
	uniformIndex = glGetUniformLocation(program, "material.diffuse");
	glUniform4fv(uniformIndex, 1, material.diffuse);
	uniformIndex = glGetUniformLocation(program, "material.specular");
	glUniform4fv(uniformIndex, 1, material.specular);
	uniformIndex = glGetUniformLocation(program, "material.shininess");
	glUniform1f(uniformIndex, material.shininess);
}

void setSpotLightPos(const GLuint program, const GLfloat* lightPos)
{
	int uniformIndex = glGetUniformLocation(program, "spotDir");
	glUniform4fv(uniformIndex, 1, lightPos);
}

void drawMesh(const GLuint mesh, const GLuint numVerts, const GLuint primitive) {
	glBindVertexArray(mesh);	// Bind mesh VAO
	glDrawArrays(primitive, 0, numVerts);	// draw first vertex array object
	glBindVertexArray(0);
}


void drawIndexedMesh(const GLuint mesh, const GLuint indexCount, const GLuint primitive) 
{
	glBindVertexArray(mesh);	// Bind mesh VAO
	glDrawElements(primitive, indexCount,  GL_UNSIGNED_INT, 0);	// draw VAO 
	glBindVertexArray(0);
}


void updateMesh(const GLuint mesh, const unsigned int bufferType, const GLfloat *data, const GLuint size) {
	GLuint * pMeshBuffers = vertexArrayMap[mesh];
	glBindVertexArray(mesh);

	// Delete the old buffer data
	glDeleteBuffers(1, &pMeshBuffers[bufferType]);

	// generate and set up the VBOs for the new data
	GLuint VBO;
	glGenBuffers(1, &VBO);
		// VBO for the data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLfloat), data, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)bufferType, 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(bufferType);
	pMeshBuffers[RT3D_VERTEX] = VBO;

	glBindVertexArray(0);

}

// textToTexture
// removed commented params: /*, TTF_Font *font, SDL_Color colour, GLuint &w,GLuint &h */
GLuint textToTexture(const char * str, GLuint textID, TTF_Font* textFont)
{
	TTF_Font* font = textFont;
	SDL_Color colour = { 255, 255, 255 };
	SDL_Color bg = { 0, 0, 0 };

	SDL_Surface *stringImage;
	stringImage = TTF_RenderText_Blended(font, str, colour);

	if (stringImage == NULL)
		//exitFatalError("String surface not created.");
		std::cout << "String surface not created." << std::endl;

	GLuint w = stringImage->w;
	GLuint h = stringImage->h;
	GLuint colours = stringImage->format->BytesPerPixel;

	GLuint format, internalFormat;
	if (colours == 4) {   // alpha
		if (stringImage->format->Rmask == 0x000000ff)
			format = GL_RGBA;
		else
			format = GL_BGRA;
	}
	else {             // no alpha
		if (stringImage->format->Rmask == 0x000000ff)
			format = GL_RGB;
		else
			format = GL_BGR;
	}
	internalFormat = (colours == 4) ? GL_RGBA : GL_RGB;

	GLuint texture = textID;

	if (texture == 0) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	} //Do this only when you initialize the texture to avoid memory leakage

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stringImage->w, stringImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, stringImage->pixels);
	glBindTexture(GL_TEXTURE_2D, NULL);

	SDL_FreeSurface(stringImage);
	return texture;
}

// Set up rendering context
SDL_Window* setupRC(SDL_GLContext &context)
{
	SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.0 context.

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

	// Create 800x600 window
	window = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate


	return window;
}

// A simple texture loading function
// lots of room for improvement - and better error checking!
GLuint loadBitmap(const char *fname)
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

// A simple cube map loading function
// lots of room for improvement - and better error checking!
GLuint loadCubeMap(const char *fname[6], GLuint *texID)
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
	return *texID;	// return value of texture ID, redundant really
}

} // namespace rt3d