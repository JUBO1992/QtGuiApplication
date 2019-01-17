#include "threadtest.h"
#include <vector>
#include <thread>
#include <functional>
#include "commonoperate.h"

extern CommonOperate* g_COperate;

CThreadTest::CThreadTest()
{
}


CThreadTest::~CThreadTest()
{
}

void func(int& counter)
{
	for (int i = 0; i < 100000; ++i)
	{
		++counter;
	}
}

void runThread()
{
	int counter = 0;
	std::vector<std::thread> threads;
	for (int i = 0; i < 10; ++i)
	{
		threads.push_back(std::thread{ func, std::ref(counter) });
	}
	for (auto& t : threads)
	{
		t.join();
	}
	g_COperate->MsgPrint(QString("Result = %1").arg(counter));
}

void runThread2()
{
	std::thread t1{ Counter(1, 20) };
	Counter c(2, 12);
	std::thread t2(c);
	std::thread t3(Counter(3, 10));
	t1.join();
	t2.join();
	t3.join();
}

void Counter::operator()() const
{
	for (int i = 0; i < mNumIterations; ++i)
	{
		g_COperate->MsgPrint(QString("Counter %1 has value %2").arg(mId).arg(i));
	}
}


void counter(int id, int numIter)
{
	for (int i = 0; i < numIter; ++i)
	{
		g_COperate->MsgPrint(QString("Counter %1 has value %2").arg(id).arg(i));
	}
}
