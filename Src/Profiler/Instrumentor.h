#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>


struct ProfileResult
{
	std::string Name;
	long long Start, End;
	uint32_t ThreadID;
};

struct InstrumentationSession
{
	std::string Name;
};

class Instrumentor
{
private:
	InstrumentationSession* m_CurrentSession;
	std::ofstream m_OutputStream;
	int m_ProfileCount;

public:
	Instrumentor();
	virtual ~Instrumentor();

	void BeginSession(const std::string& name, const std::string& filepath);
	void EndSession();
	void WriteProfile(const ProfileResult& result);
	void WriteFooter();
	void WriteHeader();

	static Instrumentor& Get()
	{
		static Instrumentor* instance = new Instrumentor();
		return *instance;
	}
};

