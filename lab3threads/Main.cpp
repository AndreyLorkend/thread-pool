#include <iostream>
#include <thread>
#include <mutex>
#include "ThreadsManager.h"

#define BUFFER_SIZE 20

using namespace std;

void fillBuffer();
void printBuffer();

thread::id buffer[BUFFER_SIZE];
int currentIndex = 0;
mutex fillMutex;
mutex printMutex;

int main() {
	ThreadsManager fillBufferThreads(5, fillBuffer);
	ThreadsManager printBufferThreads(20, printBuffer);
	fillBufferThreads.runThreads();
	printBufferThreads.runThreads();
	return 0;
}

void fillBuffer()
{
	int num = 0;
	while (true) {
		fillMutex.lock();
		if (currentIndex < BUFFER_SIZE) {
			buffer[currentIndex] = this_thread::get_id();
			currentIndex++;
		}
		else {
			currentIndex = 0;
		}
		fillMutex.unlock();
	}
}

void printBuffer()
{
	while (true) {
		printMutex.lock();
		system("cls");
		for (int i = 0; i < BUFFER_SIZE; i++) {
			cout << buffer[i] << " ";
		}
		cout << "\n";
		printMutex.unlock();
	}
}
