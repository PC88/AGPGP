#include "Instrumentor.h"



Instrumentor::Instrumentor()
	: m_CurrentSession(nullptr), m_ProfileCount(0)
{

}

Instrumentor::~Instrumentor()
{

}

void Instrumentor::BeginSession(const std::string& name, const std::string& filepath = "results.json")
{
	m_OutputStream.open(filepath);
	WriteHeader();
	m_CurrentSession = new InstrumentationSession{ name };
}

void Instrumentor::EndSession()
{
	WriteFooter();
	m_OutputStream.close();
	delete m_CurrentSession;
	m_CurrentSession = nullptr;
	m_ProfileCount = 0;
}

// TODO: make thread safe with mutex, and lock it at the start of function.
void Instrumentor::WriteProfile(const ProfileResult& result)
{
	if (m_ProfileCount++ > 0)
	{
		m_OutputStream << ",";
	}

	std::string name = result.Name;
	std::replace(name.begin(), name.end(), '"', '\'');

	m_OutputStream << "{";
	m_OutputStream << "\"cat\":\"function\",";
	m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
	m_OutputStream << "\"name\":\"" << name << "\",";
	m_OutputStream << "\"ph\":\"X\",";
	m_OutputStream << "\"pid\":0,";
	m_OutputStream << "\"tid\":" << result.ThreadID << ","; // support for multithreading: TID = thread ID
	m_OutputStream << "\"ts\":" << result.Start; // ts = time stamp
	m_OutputStream << "}";

	// stream peice meal for no data loss
	m_OutputStream.flush();

}

void Instrumentor::WriteFooter()
{
	m_OutputStream << "]}";
	m_OutputStream.flush();
}

void Instrumentor::WriteHeader()
{
	m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
	m_OutputStream.flush();
}