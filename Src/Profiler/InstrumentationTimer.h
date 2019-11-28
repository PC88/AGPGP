#pragma once
#include <chrono>


class InstrumentationTimer
{
public:
	InstrumentationTimer(const char* name);
	virtual ~InstrumentationTimer();

	void Stop();

private:

	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};

//	macros for effective usage:
//  the ability to profile scopes, and functions of any description

#define PROFILING 1
#if PROFILING
#define PROFILE_BEGIN_SESSION(name, filepath) Instrumentor::Get().BeginSession(name, filepath)
#define PROFILE_END_SESSION() Instrumentor::Get().EndSession()
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
#else
#define PROFILE_SCOPE(name)
#endif