#pragma once
#include <vector>
#include <thread>

using namespace std;

class ThreadsManager
{
private:
	int threadsCount_;
	vector<thread*> threadStorage_;
public:
	ThreadsManager(int threadsCount, void(*func)(void));
	~ThreadsManager();
	int getCurrentThreadsCount();
	void runThreads();
};

