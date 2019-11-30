#pragma once
#include <SDL.h>

//struct Container
//{
//
//	Container(SDL_GLContext& mainCont, SDL_GLContext& threadCont, SDL_Window* window)
//		: Window(window), mainC(mainCont), threadC(threadCont)
//	{
//		
//	}
//
//
//	SDL_Window* Window;
//	SDL_GLContext& mainC;
//	SDL_GLContext& threadC;
//};

class SDLContainer
{
public:
	static SDLContainer& instance();
	//Container container;
	SDL_Window* window;
	SDL_GLContext* mainContext;
	SDL_GLContext* threadContext;
	/*
		static SDL_GLContext& GetThreadContext();
		static SDL_GLContext& GetMainContext();
		static SDL_Window* GetSDLWindow();
	*/
	SDLContainer(SDLContainer const&) = delete;
	void operator=(SDLContainer const&) = delete;
	// Note: Scott Meyers mentions in his Effective Modern
	//       C++ book, that deleted functions should generally
	//       be public as it results in better error messages
	//       due to the compilers behavior to check accessibility
	//       before deleted status
	//       this probably wont matter here but dose not hurt -PC


private:
	SDLContainer();
	virtual ~SDLContainer();

};

