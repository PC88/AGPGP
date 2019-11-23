#pragma once
#include "Demo.h"
class TestDemo :
	public Demo
{
public:
	TestDemo();
	virtual ~TestDemo();


	virtual void Update(double interval) override;
	virtual void ImGuiRender() override;
	virtual void Render() override;

	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);
};

