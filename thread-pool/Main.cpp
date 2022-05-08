#include <iostream>
#include <random>
#include "ThreadPool.h"
#include <conio.h>
#include <windows.h>

#define BUFFER_SIZE 128
#define INPUT_RECORD_BUFFER 128

using namespace std;

void simulate_hard_computation();
void fillBuffer(int& num, int& currentFillIndex, int* buffer);
void handleConsole();
void KeyEventProc(KEY_EVENT_RECORD ker);
bool setConsoleParams();

bool isExitFlagSet = false;
HANDLE hStdin;
HANDLE hStdout;
DWORD fdwSaveOldMode;
DWORD fdwMode;
INPUT_RECORD irInBuf[INPUT_RECORD_BUFFER];

ThreadPool fillThreads(1);
ThreadPool keyboardThread(1);

enum keyButtons {
	ARROW_UP = 38,
	ARROW_DOWN = 40,
	ESC = 27,
	KEY_I = 73
};

int main()
{
	fillThreads.init();
	keyboardThread.init();
	srand(time(NULL));

	int num = 0;
	int currentFillIndex = 0;
	int buffer[BUFFER_SIZE];

	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	isExitFlagSet = false;
	fdwMode = 0;

	setConsoleParams();
	keyboardThread.submit(handleConsole);
	

	for (int i = 0; i < 4096; i++) {
		num = rand() % 64 + 1;
		if (currentFillIndex < BUFFER_SIZE) {
			fillThreads.submit(fillBuffer, num, currentFillIndex, buffer);
			currentFillIndex++;
		} else {
			currentFillIndex = 0;
		}
	}

	keyboardThread.shutdown();
	fillThreads.shutdown();
	return 0;
}


void simulate_hard_computation() {
	this_thread::sleep_for(chrono::milliseconds(2000));
}

void fillBuffer(int& num, int& currentFillIndex, int* buffer) {
	simulate_hard_computation();
	buffer[currentFillIndex] = currentFillIndex * num;
	cout << currentFillIndex << " : " << buffer[currentFillIndex] << "\n";
}

void handleConsole()
{
	DWORD cNumRead = 0;
	while (!isExitFlagSet) {
		if (!ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead)) {
			cout << "Error: ReadConsoleInput\n";
		}

		for (int i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT: // keyboard input
				KeyEventProc(irInBuf[i].Event.KeyEvent);
				break;
			default:
				break;
			}
		}
		cNumRead = 0;
	}
	cout << "Keyboard handler has closed!" << endl;
}

void KeyEventProc(KEY_EVENT_RECORD ker)
{
	if (ker.bKeyDown) {
		switch (ker.wVirtualKeyCode)
		{
		case ARROW_UP:
			fillThreads.addThread();
			break;
		case ARROW_DOWN:
			fillThreads.removeThread();
			break;
		case KEY_I:
			cout << "Threads are working: " << fillThreads.getThreadsCount() << "\n";
			break;
		case ESC:
			isExitFlagSet = true;
			break;
		default:
			break;
		}
	}
}

bool setConsoleParams()
{
	bool errorFlag = false;

	if (hStdin == INVALID_HANDLE_VALUE) {
		cout << "Error: GetStdHandle\n";
		errorFlag = true;
	}

	if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) {
		cout << "Error: GetConsoleMode\n";
		errorFlag = true;
	}

	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT;
	if (!SetConsoleMode(hStdin, fdwMode)) {
		cout << "Error: SetConsoleMode\n";
		errorFlag = true;
	
	}
	return errorFlag;
}
