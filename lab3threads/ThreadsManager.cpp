#include "ThreadsManager.h"

ThreadsManager::ThreadsManager(int threadsCount, void(*func)(void))
{
	threadsCount_ = threadsCount;
	for (int i = 0; i < threadsCount; i++) {
		threadStorage_.push_back(new thread(func));
	}
}

ThreadsManager::~ThreadsManager()
{
	if (!threadStorage_.empty()) {
		for (auto el : threadStorage_) {
			if (el->joinable()) {
				el->detach();
			}
		}
		std::vector<thread*>::iterator itt;
		for (itt = threadStorage_.begin(); itt != threadStorage_.end();) {
			delete* itt;
			itt = threadStorage_.erase(itt);
		}
	}
}

int ThreadsManager::getCurrentThreadsCount()
{
	return 0;
}

void ThreadsManager::runThreads()
{
	for (auto th : threadStorage_) {
		th->join();
	}
}
