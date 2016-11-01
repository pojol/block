#include <stdio.h>

#ifdef WIN32
	#include <windows.h>
#endif

#include <string>
#include <iostream>

int main(void)
{
#ifdef WIN32
	HANDLE hPipe;
	DWORD dwWritten;


	hPipe = CreateFile(TEXT("\\\\.\\pipe\\Pipe1"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hPipe != INVALID_HANDLE_VALUE)
	{
		WriteFile(hPipe,
			"Hello Pipe\n",
			12,   // = length of string + terminating '\0' !!!
			&dwWritten,
			NULL);

		CloseHandle(hPipe);
	}
#endif
	return (0);
}