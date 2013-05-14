
#include <Windows.h>
#include <stdio.h>
#include "../smbexec/utilities.h"
#include "../smbexec/connection.h"

int ExecuteCmd( HANDLE hPipeStdin, HANDLE hPipeStdout, char *szUser, char *szPassword, char *szCmd  )
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
    si.hStdError = hPipeStdout;
    si.hStdOutput = hPipeStdout;
    si.hStdInput = hPipeStdin;

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
        debug( "I'm out!!!:%d\n", GetLastError() );
        WaitForSingleObject( pi.hProcess, INFINITE );
    }

    int ret = 0;
    char szBuffer[BUFFERSIZE] = {0};
    DWORD len = 0;
    while ( 1)
    {
        ret = PeekNamedPipe( hPipeStdout, szBuffer, BUFFERSIZE, &len, 0, 0 );
        if ( ret )
            debug( szBuffer );


    }
}

int Server()
{
    char szPipeName[MAX_PATH] = {0};
    char szBuffer[BUFFERSIZE] = {0};
    HANDLE hPipeStdin = NULL;
    HANDLE hPipeStdout = NULL;

    int r = 0;

    sprintf( szPipeName, "\\\\.\\%s", PIPE_NAME_STDIN );
    hPipeStdin = CreateXPipe( szPipeName );
    if ( !hPipeStdin )
        return -1;

    sprintf( szPipeName, "\\\\.\\%s", PIPE_NAME_STDOUT );
    hPipeStdout = CreateXPipe( szPipeName );
    if ( !hPipeStdout )
        return -1;

    ExecuteCmd( hPipeStdin, hPipeStdout, "administrator", "123qwe!@#", "cmd.exe" );
    /*
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
    */

    ClosePipe( hPipeStdin );
    ClosePipe( hPipeStdout );

    return 0;
}