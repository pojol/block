#include <stdio.h>

#ifdef WIN32
	#include <windows.h>
#endif

#include <iostream>
#include <string>


int main(void)
{
#ifdef WIN32
	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;


	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe1"),
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		PIPE_WAIT,
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer)-1, &dwRead, NULL) != FALSE)
			{
				/* add terminating zero */
				buffer[dwRead] = '\0';

				/* do something with data in buffer */
				printf("%s", buffer);
			}
		}

		DisconnectNamedPipe(hPipe);
	}
#endif

	return 0;
}