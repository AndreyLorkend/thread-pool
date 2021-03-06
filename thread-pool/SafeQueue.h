#pragma once
#include <mutex>
#include <queue>

using namespace std;

// Thread safe implementation of a Queue using an queue
template <typename T>
class SafeQueue {
private:
	queue<T> m_queue;
	mutex m_mutex;
public:
	SafeQueue() { }
	SafeQueue(SafeQueue& other) { }
	~SafeQueue() { }

	bool empty() {
		unique_lock<mutex> lock(m_mutex);
		return m_queue.empty();
	}

	int size() {
		unique_lock<mutex> lock(m_mutex);
		return m_queue.size();
	}

	void enqueue(T& t) {
		unique_lock<mutex> lock(m_mutex);
		m_queue.push(t);
	}

	bool dequeue(T& t) {
		unique_lock<mutex> lock(m_mutex);

		if (m_queue.empty()) {
			return false;
		}
		t = move(m_queue.front());
		m_queue.pop();
		return true;
	}
};