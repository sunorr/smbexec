#include <Windows.h>
#include <stdio.h>
#include "utilities.h"
#include "connection.h"


int Client( char * szRemoteComp )
{
    char szPipeName[MAX_PATH] = {0};

    sprintf( szPipeName, "\\\\%s\\%s", szRemoteComp, PIPE_NAME );


    HANDLE hPipe = NULL;
    hPipe = PipeConnect( szPipeName );
    if ( !hPipe )
    {
        return -1;
    }


    char szBuffer[BUFFERSIZE] = {0};

    int r = 0;
    while ( 1 )
    {
        r = PipeSend( hPipe, szPipeName, MAX_PATH );
        if ( r < 0 )
            break;
       

        r = PipeRecv( hPipe, szBuffer, BUFFERSIZE );
        if ( r < 0 )
            break;
    }
        

    return 0;
}