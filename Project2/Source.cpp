#include <iostream>
#include <process.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include "windows.h"

HANDLE hMutex;
HANDLE empty;
HANDLE full;
const int n = 10;
int i = 0;
int buf[n] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int* p;
int* c;
int cons = 150;
int prod = 100;
bool end = 1;

void Producer(void* pParams)
{
	unsigned int num = 1;
	unsigned int count = 1;
	while (end)
	{
		WaitForSingleObject(empty, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);
		*p = count++;
		p++;
		if (p == buf + n)p = buf;
		ReleaseMutex(hMutex);
		ReleaseSemaphore(full, 1, NULL);
		Sleep(prod);
	}
}

void Consumer(void* pParams)
{
	unsigned int num = 1;
	while (end)

	{
		WaitForSingleObject(full, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);
		*c = 0;
		c++;
		if (c == buf + n) c = buf;
		ReleaseMutex(hMutex);
		ReleaseSemaphore(empty, 1, NULL);
		Sleep(cons);
	}
}

int main(int argc, CHAR* argv[])
{
	p = buf;
	c = buf;
	hMutex = CreateMutex(NULL, FALSE, NULL);
	empty = CreateSemaphore(NULL, n, n, NULL);
	full = CreateSemaphore(NULL, 0, n, NULL);
	_beginthread(Producer, 0, NULL);
	_beginthread(Consumer, 0, NULL);
	while (end)
	{
		for (int i = 0; i < n; i++)
			std::cout << "\t" << buf[i] << ' ';
		std::cout << "\t" << "   recording - " << prod << ' ';
		std::cout << "\t" << "   reading - " << cons;
		std::cout << std::endl;
		if (GetKeyState(VK_LEFT) & 0x80000) prod += 10;
		if (GetKeyState(VK_RIGHT) & 0x80000) prod -= prod - 10 >= 0 ? 10 : 0;
		if (GetKeyState(VK_UP) & 0x80000) cons -= cons - 10 >= 0 ? 10 : 0;
		if (GetKeyState(VK_DOWN) & 0x80000) cons += 10;
		if (GetKeyState(VK_ESCAPE) & 0x80000) end = 0;
		Sleep(500);

	}
	WaitForSingleObject(Producer, INFINITE);
	WaitForSingleObject(Consumer, INFINITE);
	return 0;
}