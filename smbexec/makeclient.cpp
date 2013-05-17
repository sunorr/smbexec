#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include "utilities.h"
#include "connection.h"

int backspase = 0;

typedef struct __THREAD_ARGS__
{
    HANDLE hPipeStdin;
    HANDLE hPipeStdout;
}THREAD_ARGS, *PTHREAD_ARGS;

UINT __stdcall ThreadGets( void * argument )
{

    char szInteractive[CMD_LEN] = {0};
    int len = 0;
    int r = 0;
    PTHREAD_ARGS t = (PTHREAD_ARGS)argument;

    while ( 1 )
    {

        gets( szInteractive );
        len = strlen( szInteractive );
        szInteractive[len] = '\r';
        szInteractive[len + 1] = '\n';
        szInteractive[len + 2] = 0;


        r = PipeSend( t->hPipeStdin, szInteractive, 0  );
        if ( r < 0 )
            goto err;
    }
err:
    _endthreadex(0) ;
    return 0;
}



int Client( char * szRemoteComp, PLOGINFO pLogInfo )
{
    char szPipeName[MAX_PATH] = {0};

    HANDLE hPipeStdout = NULL;
    HANDLE hPipeStdin = NULL;


    sprintf( szPipeName, "\\\\%s\\%s", szRemoteComp, PIPE_NAME_STDIN );
    hPipeStdin = PipeConnect( szPipeName );
    if ( !hPipeStdin )
        return -1;

    sprintf( szPipeName, "\\\\%s\\%s", szRemoteComp, PIPE_NAME_STDOUT );
    hPipeStdout= PipeConnect( szPipeName );
    if ( !hPipeStdout )
        return -1;

    char szBuffer[BUFFERSIZE] = {0};

    DWORD len = BUFFERSIZE;
    int r = 0;

    // 发送指令让服务器运转
    r = PipeSend( hPipeStdin, (char *)pLogInfo, sizeof(LOGINFO) );
    if ( r < 0 )
        return -1;


    THREAD_ARGS t; 
    t.hPipeStdin = hPipeStdin;

    _beginthreadex( NULL, NULL, ThreadGets, &t, 0, NULL );

    while ( 1 )
    {
        memset( szBuffer, 0, BUFFERSIZE );
        r = PipeRecv( hPipeStdout, szBuffer, BUFFERSIZE );
        if ( r < 0 )
            break;
        else
            debug( szBuffer );
    }
        

    return 0;
}