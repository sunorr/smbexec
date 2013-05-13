
#include <Windows.h>
#include <stdio.h>
#include "../smbexec/utilities.h"
#include "../smbexec/connection.h"

int Server()
{
    char szPipeName[MAX_PATH] = {0};
    char szBuffer[BUFFERSIZE] = {0};
    HANDLE hPipe = NULL;

    int r = 0;

    sprintf( szPipeName, "\\\\.\\%s", PIPE_NAME );
    hPipe = CreateXPipe( szPipeName );
    if ( !hPipe )
        return -1;

    while ( 1 )
    {
        r = PipeRecv( hPipe, szBuffer, BUFFERSIZE );
        if ( r > 0 )
            debug( szBuffer );
        else
            break;

        if ( strcmp( szBuffer, "exit" ) == 0 )
            break;

        r = PipeSend( hPipe, szBuffer, BUFFERSIZE );
        if ( r > 0 )
            debug( "ok\n" );
        else
            break;

    }

    ClosePipe( hPipe );

    return 0;
}