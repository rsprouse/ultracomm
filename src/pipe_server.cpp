#include "StdAfx.h"

/*
   Simple server that blocks while waiting to read from a named pipe.
*/

void block_on_named_pipe()
{
    HANDLE mypipe;
    char buffer[1024];
    DWORD dwRead;

    /*
       As created the pipe is bidirectional, but in use we only wait to read from it.
    */
    mypipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\ultracomm"),
                            PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
                            PIPE_WAIT,
                            1,
                            1024 * 16,
                            1024 * 16,
                            NMPWAIT_USE_DEFAULT_WAIT,
                            NULL);

    // Wait for a client to connect to the pipe.
    if (ConnectNamedPipe(mypipe, NULL) != FALSE)
    {
        // Wait on a read.
        ReadFile(mypipe, buffer, sizeof(buffer), &dwRead, NULL);
    }

    // We read something, so stop.
    DisconnectNamedPipe(mypipe);
}
