#ifndef THREADTEST_H
#define THREADTEST_H

#include <QObject>

class CThreadTest
{
public:
	CThreadTest();
	~CThreadTest();
};

void runThread();

void runThread2();

class Counter
{
public:
	Counter(int id, int numIterations)
		:mId(id), mNumIterations(numIterations)
	{

	}
	void operator()() const;
protected:
	int mId;
	int mNumIterations;
};

#endif // THREADTEST_H
