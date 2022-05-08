#include "ThreadPool.h"
#include "ThreadWorker.h"

ThreadPool::ThreadPool(const int n_threads)	: m_shutdown(false) 
{
	threadsCount = n_threads;
	deleteFlag = false;
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
	m_threads.push_back(new thread(ThreadWorker(this, m_threads.size())));
	threadsCount++;
}

void ThreadPool::removeThread()
{
	threadsCount--;
}

void ThreadPool::shutdown() {
	bool dequeued;
	function<void()> func;
	while (m_queue.size()) {
		{
			unique_lock<mutex> lock(m_conditional_mutex);
			dequeued = m_queue.dequeue(func);
		}

		if (dequeued) {
			func();
		}
	}

	m_shutdown = true;
	m_conditional_lock.notify_all();

	for (int i = 0; i < m_threads.size(); ++i) {
		if (m_threads[i]->joinable()) {
			m_threads[i]->join();
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

void ThreadPool::deleteThreadById(int threadId)
{
	std::vector<thread*>::iterator itt;
	if (m_threads[threadId]->joinable()) {
		m_threads[threadId]->join();
		for (itt = m_threads.begin(); itt != m_threads.end();) {
			if ((*itt)->get_id() == m_threads[threadId]->get_id()) {
				swap(*itt, m_threads.back());
				m_threads.pop_back();
			}
			else {
				itt++;
			}
		}
	}
}

void ThreadPool::setDeleteFlag(bool flag)
{
	deleteFlag = flag;
}

void ThreadPool::setDeleteThreadId(int id)
{
	deleteThreadId = id;
}
