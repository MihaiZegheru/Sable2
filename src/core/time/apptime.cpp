#include <core/time/apptime.h>

void Time::Init(double currTime)
{
	m_pastTime = currTime;
}

void Time::ComputeDeltaTime(double currTime)
{
	m_deltaTime = currTime - m_pastTime;
	m_pastTime = currTime;
}