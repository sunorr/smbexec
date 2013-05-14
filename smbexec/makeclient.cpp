#include <Windows.h>
#include <stdio.h>
#include "utilities.h"
#include "connection.h"


int ExecuteCmd( HANDLE hPipe, char *szUser, char *szPassword, char *szCmd  )
{
    PROCESS_INFORMATION     pi = {0};
    STARTUPINFO             si = {0};
    SECURITY_ATTRIBUTES     psa={sizeof(psa),NULL,TRUE};

    HANDLE                  hToken;

    if ( !LogonUser( szUser,
        NULL,
        szPassword,
        LOGON32_LOGON_INTERACTIVE,
        LOGON32_PROVIDER_DEFAULT,
        &hToken ) )
    {
        debug( "LogonUser error:%d\n", GetLastError() );
        return -1;
    }

    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdError = hPipe;
    si.hStdInput = hPipe;
    si.hStdOutput = hPipe;

    if ( !CreateProcessAsUser( hToken, 
        NULL,
        szCmd,
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi ) )
    {
        debug( "CreateProcessAsUser error:%d\n", GetLastError() );
        return -1;
    }
    else
    {
        WaitForSingleObject( pi.hProcess, INFINITE );
    }

    return 0;
}



int Client( char * szRemoteComp )
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

    int r = 0;
    while ( 1 )
    {
        r = PipeSend( hPipeStdin, "pwd", MAX_PATH );
        if ( r < 0 )
            break;
       

        r = PipeRecv( hPipeStdout, szBuffer, BUFFERSIZE );
        if ( r < 0 )
            break;
        else
            debug( szBuffer );
    }
        

    return 0;
}