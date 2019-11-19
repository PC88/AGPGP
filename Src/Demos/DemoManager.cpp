#include "DemoManager.h"
#include "imgui.h"
#include <GL/glew.h>

DemoManager::DemoManager(Demo*& currentDemoPointer)
	: m_CurrentDemos(currentDemoPointer)
{

}


DemoManager::~DemoManager()
{

}

void DemoManager::ImGuiRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto& demo : m_Demos)
	{
		if (ImGui::Button(demo.first.c_str()))
		{
			m_CurrentDemos = demo.second();
		}
	}
}
