#include "InstrumentationTimer.h"
#include <iostream>
#include "Instrumentor.h"
#include <thread>


InstrumentationTimer::InstrumentationTimer(const char* name)
	: m_Name(name), m_Stopped(false)
{
	m_StartTimepoint = std::chrono::high_resolution_clock::now();
}

InstrumentationTimer::~InstrumentationTimer()
{
	if (!m_Stopped)
	{
		Stop();
	}
}


void InstrumentationTimer::Stop()
{
	auto endTimepoint = std::chrono::high_resolution_clock::now();

	// microseconds for compatibility with chrome tracing
	long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
	long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

	//std::cout << m_Name << ": " << (end - start) << "ms\n";
	// retrieve what thread this was run on
	uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
	Instrumentor::Get().WriteProfile({m_Name, start, end});
	m_Stopped = true;
}
