#include "ThreadPool.h"
#include "ThreadWorker.h"
#include <iostream>

ThreadPool::ThreadPool(const int n_threads)	: m_shutdown(false) 
{
	threadsCount = n_threads;
	deleteThreadId = -1;
	deleteFlag = false;
	lockedFlag = false;
	tasksCount = m_queue.size();
}

ThreadPool::~ThreadPool()
{
	if (!m_threads.empty()) {
		vector<thread*>::iterator itt;
		for (itt = m_threads.begin(); itt != m_threads.end();) {
			delete* itt;
			itt = m_threads.erase(itt);
		}
	}
}

void ThreadPool::init() {
	for (int i = 0; i < threadsCount; ++i) {
		m_threads.push_back(new thread(ThreadWorker(this, i)));
	}
}

void ThreadPool::addThread()
{
	if (m_queue.size()) {
		m_threads.push_back(new thread(ThreadWorker(this, m_threads.size())));
		threadsCount++;
	}
}

void ThreadPool::removeThread()
{
	if (threadsCount > 0 && !lockedFlag) {
		lockedFlag = true;
		threadsCount--;
	}
}

void ThreadPool::shutdown() {
	bool dequeued;
	function<void()> func;
	while (m_queue.size()) {
		{
			unique_lock<mutex> lock(m_conditional_mutex);
			if (m_threads.size()) {
				dequeued = m_queue.dequeue(func);
			}
		}

		if (dequeued && m_threads.size()) {
			func();
		}

		if (lockedFlag && m_threads.size()) {
			deleteThreadById(deleteThreadId);
		}
	}

	m_shutdown = true;
	m_conditional_lock.notify_all();

	for (int i = 0; i < m_threads.size(); ++i) {
		if (m_threads[i]->joinable()) {
			m_threads[i]->join();
			deleteThreadById(i);
		}
	}
}

bool ThreadPool::getShutdownFlag()
{
	return m_shutdown;
}

mutex& ThreadPool::getConditionalMutex()
{
	return m_conditional_mutex;
}

SafeQueue<function<void()>>& ThreadPool::getQueue()
{
	return m_queue;
}

condition_variable& ThreadPool::getConditionalLock()
{
	return m_conditional_lock;
}

int ThreadPool::getThreadsCount()
{
	return m_threads.size();
}

int ThreadPool::getCurrentThreadCount()
{
	return threadsCount;
}

int ThreadPool::getCurrentDeleteThreadId()
{
	return deleteThreadId;
}

int ThreadPool::getTasksCount()
{
	return tasksCount;
}

int ThreadPool::getCurrentTasksCount()
{
	return m_queue.size();
}

void ThreadPool::deleteThreadById(int threadId)
{
	std::vector<thread*>::iterator itt;
	if (threadsCount >= 0 && !m_threads.empty()) {
		for (itt = m_threads.begin(); itt != m_threads.end();) {
			if ((*itt)->get_id() == m_threads[threadId]->get_id()) {
				if ((*itt)->joinable()) {
					(*itt)->join();
				}
				cout << "=============================================\n";
				cout << "The thread: " << m_threads[threadId]->get_id() << " was deleted!\n";
				cout << "=============================================\n";
				delete* itt;
				itt = m_threads.erase(itt);
				lockedFlag = false;
				deleteThreadId = -1;
			} else {
				itt++;
			}
		}
	}
}

void ThreadPool::setDeleteFlag(bool flag)
{
	if (!lockedFlag) {
		deleteFlag = flag;
		cout << "======================\n";
		cout << "Deleting the thread...\n";
		cout << "======================\n";
	} else {
		cout << "=============================================\n";
		cout << "Please wait until the thread will be deleted!\n";
		cout << "=============================================\n";
	}
}

bool ThreadPool::getDeleteFlag()
{
	return deleteFlag;
}

void ThreadPool::setDeleteThreadId(int id)
{
	deleteThreadId = id;
}
