#include "ThreadWorker.h"
#include <iostream>

ThreadWorker::ThreadWorker(ThreadPool* pool, const int id) : m_pool(pool), m_id(id) 
{
	isWorkingFlag = true;
}

void ThreadWorker::operator()()
{
	function<void()> task;
	bool dequeued;
	while (!m_pool->getShutdownFlag() && isWorkingFlag) {
		{
			unique_lock<mutex> lock(m_pool->getConditionalMutex());
			if (m_pool->getQueue().empty()) {
				m_pool->getConditionalLock().wait(lock);
			}

			if (m_pool->getCurrentThreadCount() < m_pool->getThreadsCount()) {
				isWorkingFlag = false;
				m_pool->setDeleteThreadId(m_id);
				m_pool->setDeleteFlag(true);
			}

			dequeued = m_pool->getQueue().dequeue(task);
		}

		if (dequeued) {
			task();
		}
	}
}
