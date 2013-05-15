
#include <Windows.h>
#include <stdio.h>
#include "../smbexec/utilities.h"
#include "../smbexec/connection.h"

int ExecuteCmd( HANDLE hPipeStdin, HANDLE hPipeStdout, char *szUser, char *szPassword, char *szCmd  )
{
    PROCESS_INFORMATION     pi = {0};
    STARTUPINFO             si = {0};

    HANDLE                  hToken;

    debug( szCmd );
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
        WaitForSingleObject( pi.hProcess, INFINITE );
        debug( "I'm out!!!:%d\n", GetLastError() );
    }

    return 0;
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


    LOGINFO LogInfo = {0};
    r = PipeRecv( hPipeStdin, (char *)&LogInfo, sizeof(LOGINFO) );
    if ( r < 0 )
    {
        return -1;
    }
    debug( "user: %s password: %s cmd: %s\n", LogInfo.szUserName, LogInfo.szPassword, LogInfo.szExcuteCmd );


    ExecuteCmd( hPipeStdin, hPipeStdout, LogInfo.szUserName, LogInfo.szPassword, LogInfo.szExcuteCmd );

    ClosePipe( hPipeStdin );
    ClosePipe( hPipeStdout );

    return 0;
}