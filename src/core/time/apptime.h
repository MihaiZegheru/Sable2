#ifndef SRC_APPTIME_H__
#define SRC_APPTIME_H__


class Time
{
public:
	static Time& GetInstance()
	{
		static Time instance;
		return instance;
	}

	void Init(double currTime);
	void ComputeDeltaTime(double currTime);

	inline double GetDeltaTime() { return m_deltaTime; }

private:
	Time() {};
	Time(Time const&) = delete;
	void operator= (Time const&) = delete;

	double m_pastTime;
	double m_deltaTime;
};



#endif  // SRC_APPTIME_H__