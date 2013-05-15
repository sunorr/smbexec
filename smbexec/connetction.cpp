#include <Windows.h>
#include "connection.h"
#include <stdio.h>
#include "utilities.h"

#define __DEBUG__



int PipeRecv( HANDLE hPipe, char * szBuffer, DWORD dwSize )
{
    BOOL fSuccess = FALSE;
    DWORD dwRead = 0;

    fSuccess = ReadFile( hPipe, 
                         szBuffer,
                         dwSize,
                         &dwRead,
                         NULL );

    if ( !fSuccess )
    {
        debug( "Read from pipe error:%d\n", GetLastError() );
        return -1;
    }

    return dwRead;
}

int PipeSend( HANDLE hPipe, char * szBuffer, DWORD dwSize )
{
    BOOL fSuccess = FALSE;
    DWORD dwWrite = 0;

    int i = 0;
    if ( !dwSize )
    {
        for( i = 0; i < strlen(szBuffer); i++ )
        {

            fSuccess = WriteFile( hPipe, 
                                  szBuffer + i ,
                                  1,
                                  &dwWrite,
                                  NULL );

            if ( !fSuccess )
            {
                debug( "write pipe error:%d\n", GetLastError() );
                return -1;
            }
        }
    }
    else
    {
        fSuccess = WriteFile( hPipe, 
                              szBuffer,
                              dwSize,
                              &dwWrite,
                              NULL );

        if ( !fSuccess )
        {
            debug( "write pipe error:%d\n", GetLastError() );
            return -1;
        }
    }

    return dwWrite;
}



void ClosePipe( HANDLE hPipe )
{
    FlushFileBuffers( hPipe );
    DisconnectNamedPipe( hPipe );
    CloseHandle( hPipe );
}

HANDLE CreateXPipe( char * szPipeName )
{
    HANDLE hPipe = NULL;
    // inheritance 必须为true 才可以使用 STARTF_USESTDHANDLES
    SECURITY_ATTRIBUTES sa =  { sizeof(sa), NULL, TRUE };
    debug( "hPipeName:%s\n", szPipeName );

    hPipe = CreateNamedPipe( szPipeName, 
                             PIPE_ACCESS_DUPLEX,
                             PIPE_TYPE_MESSAGE | 
                             PIPE_READMODE_MESSAGE | 
                             PIPE_WAIT, // 阻塞？or非阻塞？
                             PIPE_UNLIMITED_INSTANCES, 
                             BUFFERSIZE,
                             BUFFERSIZE,
                             0,
                             &sa );
    if ( hPipe == INVALID_HANDLE_VALUE )
    {
        debug( "CreateNamePipe error:%d\n", GetLastError() );
        return NULL;
    }

    BOOL fconnect = ConnectNamedPipe( hPipe, NULL );
    if ( !fconnect )
    {
        debug( "ConnectNamePipe error:%d\n", GetLastError() );
        CloseHandle( hPipe );
        return NULL;
    }

    debug( "hPipe:0x%x\n", hPipe );
    return hPipe;
}


HANDLE PipeConnect( char * szPipeName )
{
    HANDLE hPipe = NULL; 

    while ( 1 )
    {
        hPipe = CreateFile( szPipeName,
                            GENERIC_ALL,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL );
        if ( hPipe != INVALID_HANDLE_VALUE )
            break;

        if ( GetLastError() != ERROR_PIPE_BUSY )
        {
            debug( "CreateFile error:%d\n", GetLastError() );
            return NULL;
        }

        if ( ! WaitNamedPipe( szPipeName, 2000 ) ) 
        {
            debug( "WaitNamePipe error:%d\n", GetLastError() );
            return NULL;
        }
    }

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    if ( ! SetNamedPipeHandleState( hPipe, &dwMode, NULL, NULL ) )
    {
        debug( "SetNamePipeHandleState error:%d\n", GetLastError() );
        CloseHandle( hPipe );
        return NULL;
    }
    
    return hPipe;
}