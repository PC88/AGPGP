// MD2 animation renderer
// This demo will load and render an animated MD2 model, an OBJ model and a skybox
// Most of the OpenGL code for dealing with buffer objects, etc has been moved to a 
// utility library, to make creation and display of mesh objects as simple as possible

// Windows specific: Uncomment the following line to open a console window for debug output
#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

// imgui
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// demos
#include "Demo.h"
#include "DemoManager.h"
// Peter Demo
#include "ThreadedKernelDemo.h"
// Umar Demo
#include "MonoChromDemo.h"
#include "TestDemo.h"
// Matt Demo
#include "ChromFresnelDemo.h"

// Combined Demo
#include "CombinedDemo.h"

// general timer
#include "Timer.h"
#include "HPTimer.h"
#include "LPTimer.h"

//instrumentor and its timer
#include "Instrumentor.h"
#include "InstrumentationTimer.h"

// Program entry point - SDL manages the actual WinMain entry point for us
int main(int argc, char *argv[]) 
{
    SDL_Window* hWindow;						// window handle
    SDL_GLContext glContext;					// OpenGL context handle
	hWindow = rt3d::setupRC(glContext);         // Create window and render context 

	// Required on Windows *only* init GLEW to access OpenGL beyond 1.1
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (GLEW_OK != err) 
	{   
		// glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << endl;
		exit (1);
	}

	cout << glGetString(GL_VERSION) << endl;

	ImGui::CreateContext();
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui_ImplSDL2_InitForOpenGL(hWindow, glContext);
	ImGui::StyleColorsDark();

	Demo* currentDemo = nullptr;
	DemoManager* demoManager = new DemoManager(currentDemo);
	currentDemo = demoManager;

	/// group project Demos ///
	demoManager->RegisterDemo<ThreadedKernelDemo>("Sobel Threaded Demo");
	demoManager->RegisterDemo<MonoChromDemo>("Mono Chrom Demo");
	demoManager->RegisterDemo<ChromFresnelDemo>("Chrom Fresnel Demo");
	demoManager->RegisterDemo<TestDemo>("Test Demo");
	demoManager->RegisterDemo<CombinedDemo>("Combined Demo");


	// Timer - PC
	LPTimer timer;
	const double MS_PER_UPDATE = 0.008;
	double previousTime = timer.getTime();
	double lag = 0;

	bool running = true;                   // set running to true
	SDL_Event sdlEvent;                    // variable to detect SDL events

	while (running)	
	{
		
		// game loop pattern, times -PC
		double currentTime = timer.getTime();
		double elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		lag += elapsedTime;

		
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		while (SDL_PollEvent(&sdlEvent)) // the event loop
		{
			if (sdlEvent.type == SDL_QUIT)
			{
				running = false;
			}
		}
       // call the draw function

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(hWindow);
		ImGui::NewFrame();
		if (currentDemo)
		{
			while (lag >= MS_PER_UPDATE)
			{
				currentDemo->Update(MS_PER_UPDATE);
				lag -= MS_PER_UPDATE;
			}

			// Game Loop, Pattern used for decoupling -PC, -Robert Nystrom GPP
			currentDemo->Render();
			ImGui::Begin("Test Demos");
			if (currentDemo != demoManager && ImGui::Button("<-"))
			{
				delete currentDemo;
				currentDemo = demoManager;
			}
			currentDemo->ImGuiRender();
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(hWindow); // swap buffers, really should not have been in draw in the first place.
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(hWindow);
    SDL_Quit();
    return 0;
}