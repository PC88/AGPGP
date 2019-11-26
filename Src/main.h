#pragma once

void pbo(glm::mat4 &projection, glm::mat4 &modelview, int &uniformIndex);

void fbo(glm::mat4 &projection, int &uniformIndex);

void toonBunnyDraw(glm::vec4 &tmp, glm::mat4 &projection);

void pboDraw(glm::mat4 &projection, glm::mat4 &modelview);

void bunnyDraw(glm::vec4 &tmp, glm::mat4 &projection, GLfloat &offset);

void cubeDraw(glm::vec4 &tmp, glm::mat4 &projection);

void hobDraw(const GLfloat &scale);

void groundDraw();

void FBO(glm::mat4 &projection);

void initFBO();
