#include "SDLContainer.h"


SDLContainer& SDLContainer::instance()
{
	static SDLContainer _self;
	return _self;
}

SDLContainer::SDLContainer()
{

}

SDLContainer::~SDLContainer()
{

}
